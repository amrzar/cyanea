/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/stdarg.h>
#include <cyanea/errno.h>

#include <ulib/ctype.h>
#include <ulib/string.h>

#include "../io_buffer.h"

#define FL_ZERO   (1 << 0)      /* Zero modifier. */
#define FL_MINUS  (1 << 1)      /* Minus modifier. */
#define FL_PLUS   (1 << 2)      /* Plus modifier. */
#define FL_SPACE  (1 << 3)      /* Space modifier. */
#define FL_HASH   (1 << 4)      /* '#' modifier. */
#define FL_SIGNED (1 << 5)      /* Number is signed. */
#define FL_UPPER  (1 << 6)      /* Upper case digits. */

enum ranks {
    RANK_CHAR = -2,
    RANK_SHORT = -1,
    RANK_INT = 0,
    RANK_LONG = 1,
    RANK_LLONG = 2
};

static void io_buffer_put_char(_IO_BUFFER io, char c)
{
    __iob_write(io, &c, 1);
}

static void put_padding(_IO_BUFFER io, int condition, char c, int width,
    int nchar)
{
    if ((condition) && (width > nchar)) {
        width -= nchar;

        while (width--)
            io_buffer_put_char(io, c);
    }
}

static const char *digits(unsigned long flags)
{
    static const char lcdigits[] = "0123456789abcdef";
    static const char ucdigits[] = "0123456789ABCDEF";
    return (flags & FL_UPPER) ? ucdigits : lcdigits;
}

static void format_int(_IO_BUFFER io, unsigned long long value,
    unsigned long flags, int base, int width, int precision)
{

    int minus = 0, ndigits = 0, nchar = 0;

    if (flags & FL_SIGNED) {
        if ((long long)(value) < 0) {
            value = (unsigned long long)(-(long long)(value));
            minus = 1;
        }
    }

    unsigned long long t_value = value;
    int t_ndigit;

    do {
        ndigits++;
        t_value /= base;
    } while (t_value != 0);

    if (ndigits < precision)
        ndigits = precision;

    /* 'ndigits' is number of digits to represent the 'value'.
     * 'nchar' is number of character including sign and preceding string, e.g. '0x'.
     * 'width' is minimum number of character for this field.
     */

    nchar = ndigits;

    if (minus || (flags & FL_PLUS) || (flags & FL_SPACE))
        nchar++;

    if (flags & FL_HASH) {
        if (base == 16)
            nchar += 2;
        else if (base == 8)
            nchar += 1;
    }

    put_padding(io, !(flags & FL_MINUS) &&
        !(flags & FL_ZERO), ' ', width, nchar);

    if (minus)
        io_buffer_put_char(io, '-');
    else if (flags & FL_PLUS)
        io_buffer_put_char(io, '+');
    else if (flags & FL_SPACE)
        io_buffer_put_char(io, ' ');

    if (flags & FL_HASH) {
        if (base == 16) {
            io_buffer_put_char(io, '0');
            io_buffer_put_char(io, (flags & FL_UPPER) ? 'X' : 'x');
        } else if (base == 8)
            io_buffer_put_char(io, '0');
    }

    put_padding(io, !(flags & FL_MINUS) &&
        (flags & FL_ZERO), '0', width, nchar);

    char t_str[64] = { '\0' };

    t_ndigit = ndigits;
    while (t_ndigit > 0) {
        t_str[--t_ndigit] = digits(flags)[value % base];
        value /= base;
    }

    __iob_write(io, t_str, ndigits);
    put_padding(io, (flags & FL_MINUS), ' ', width, nchar);
}

#define USTTOLL(x) ((unsigned long long)(x))
#define STTOLL(x)  ((unsigned long long)(long long)(x))

static void ap_format_int(_IO_BUFFER io, int base, unsigned long flags,
    int width, int precision, int rank, va_list *ap)
{
    unsigned long long value = 0;

    /* Do some type casting to extend signed value for 'long long'. */

    if (flags & FL_SIGNED) {
        switch (rank) {
        case RANK_CHAR:
            value = STTOLL((unsigned char)(va_arg(*ap, int)));
            break;

        case RANK_SHORT:
            value = STTOLL((unsigned short)(va_arg(*ap, int)));
            break;

        case RANK_INT:
            value = STTOLL(va_arg(*ap, int));
            break;

        case RANK_LONG:
            value = STTOLL(va_arg(*ap, long));
            break;

        case RANK_LLONG:
            value = STTOLL(va_arg(*ap, long long));
            break;
        }

    } else {
        switch (rank) {
        case RANK_CHAR:
            value = USTTOLL((unsigned char)(va_arg(*ap, int)));
            break;

        case RANK_SHORT:
            value = USTTOLL((unsigned short)(va_arg(*ap, int)));
            break;

        case RANK_INT:
            value = USTTOLL(va_arg(*ap, int));
            break;

        case RANK_LONG:
            value = USTTOLL(va_arg(*ap, long));
            break;

        case RANK_LLONG:
            value = USTTOLL(va_arg(*ap, long long));
            break;
        }
    }

    format_int(io, value, flags, base, width, precision);
}

static void format_str(_IO_BUFFER io, const char *str, int count,
    unsigned long flags, int width, int precision)
{

    if (precision != -1 && count > precision)
        count = precision;

    put_padding(io, !(flags & FL_MINUS), ' ', width, count);
    __iob_write(io, str, count);
    put_padding(io, (flags & FL_MINUS), ' ', width, count);
}

int iob_vsnprintf(_IO_BUFFER io, const char *format, va_list _ap)
{

    /* ''%[parameter][flags][width][.precision][length or modifier]type''. */

    enum {
        ST_NORMAL,
        ST_FLAGS,
        ST_WIDTH,
        ST_PRECISION,
        ST_MODIFIERS
    } state = ST_NORMAL;

    va_list ap;

    /* 'va_list' may be an array, 'va_copy' to pass 'va_list *'. */

    va_copy(ap, _ap);

    char ch;
    int p = 0;
    int width, precision, rank;
    unsigned long flags;

    while ((ch = format[p++])) {
        switch (state) {
        case ST_NORMAL:
            if (ch == '%') {
                if (format[p] != '%') {
                    state = ST_FLAGS;
                    flags = 0;
                    rank = RANK_INT;
                    width = 0;
                    precision = -1;

                } else {
                    io_buffer_put_char(io, ch);
                    /* Ignore next '%'. */
                    p++;
                }
            } else
                io_buffer_put_char(io, ch);

            break;

        case ST_FLAGS:
            switch (ch) {
            case '-':
                flags |= FL_MINUS;
                break;

            case '+':
                flags |= FL_PLUS;
                break;

            case ' ':
                flags |= FL_SPACE;
                break;

            case '#':
                flags |= FL_HASH;
                break;

            case '0':
                flags |= FL_ZERO;
                break;

            default:
                state = ST_WIDTH;
                goto st_width;
            }

            break;

        case ST_WIDTH:
st_width:

            if (isdigit(ch))
                width = width * 10 + (ch - '0');
            else if (ch == '*' && width == 0) {
                width = va_arg(ap, int);

                if (width < 0) {
                    width = -width;
                    flags |= FL_MINUS;
                }

                if (format[p] != '.')
                    state = ST_MODIFIERS;
            } else if (ch == '.') {
                precision = 0;
                state = ST_PRECISION;
            } else {
                state = ST_MODIFIERS;
                goto st_modifiers;
            }

            break;

        case ST_PRECISION:

            if (isdigit(ch))
                precision = precision * 10 + (ch - '0');
            else if (ch == '*' && precision == 0) {
                precision = va_arg(ap, int);

                if (precision < 0)
                    precision = -1;

                state = ST_MODIFIERS;
            } else {
                state = ST_MODIFIERS;
                goto st_modifiers;
            }

            break;

        case ST_MODIFIERS:
st_modifiers:

            if ((ch == 'h') && (rank > RANK_CHAR))
                rank--;

            else if ((ch == 'l') && (rank < RANK_LLONG))
                rank++;

            else {
                state = ST_NORMAL;

                switch (ch) {
                case 'P':
                    flags |= FL_UPPER;

                    fallthrough;
                case 'p':
                    flags |= FL_HASH;
                    format_int(io, USTTOLL(va_arg(ap, void *)), 16, flags,
                        width, sizeof(void *) >> 2);
                    break;

                case 'd':
                case 'i':
                    flags |= FL_SIGNED;

                    fallthrough;
                case 'u':
                    ap_format_int(io, 10, flags, width, precision, rank, &ap);
                    break;

                case 'o':
                    ap_format_int(io, 8, flags, width, precision, rank, &ap);
                    break;

                case 'X':
                    flags |= FL_UPPER;

                    fallthrough;
                case 'x':
                    ap_format_int(io, 16, flags, width, precision, rank, &ap);
                    break;

                case 'c': {
                    char char_arg = (signed char)(va_arg(ap, int));

                    format_str(io, &char_arg, 1, flags, width, precision);
                    break;
                }

                case 's': {
                    const char *str_arg = va_arg(ap, const char *);
                    str_arg = str_arg ? str_arg : "(null)";

                    format_str(io, str_arg, strlen(str_arg), flags, width,
                        precision);
                    break;
                }

                default:
                    return -1;
                }
            }
        }
    }

    return SUCCESS;
}
