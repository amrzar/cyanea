/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm/io.h>

#include <ulib/stddef.h>
#include <ulib/stdarg.h>

#include "string.h"
#include "compressed.h"

#include "../../../../drivers/uart/uart_regs.h"
#include "../early_serial_console.c"

void putchar(int c)
{
    if (c == '\n')
        putchar('\r');

    early_serial_putchar(c);
}

void puts(const char *str)
{
    while (*str)
        putchar(*str++);
}

#define FL_ZERO   (1 << 0)
#define FL_HASH   (1 << 1)
#define FL_UPPER  (1 << 2)
#define FL_SIGNED (1 << 3)
#define FL_MINUS  (1 << 4)

enum ranks {
    RANK_CHAR = -2,
    RANK_SHORT = -1,
    RANK_INT = 0,
    RANK_LONG = 1
};

static void put_padding(int condition, char c, int width, int nchar)
{
    if ((condition) && (width > nchar)) {
        width -= nchar;

        while (width--)
            putchar(c);
    }
}

static void format_int(unsigned long value, int base,
    unsigned long flags, int width, int precision)
{

    int minus = 0, ndigits = 0, nchar = 0;

    if ((flags & FL_SIGNED) && (long)(value) < 0) {
        value = (unsigned long)(-(long)(value));
        minus = 1;
        nchar = 1;
    }

    unsigned long t_value = value;

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

    nchar += ndigits;

    if (flags & FL_HASH) {
        if (base == 16)
            nchar += 2;
        else if (base == 8)
            nchar += 1;
    }

    /* ... right align with space. */
    put_padding(!(flags & FL_MINUS) && !(flags & FL_ZERO), ' ', width, nchar);

    if (minus == 1)
        putchar('-');

    if (flags & FL_HASH) {
        if (base == 16) {
            putchar('0');
            putchar((flags & FL_UPPER) ? 'X' : 'x');
        } else if (base == 8)
            putchar('0');
    }

    /* ... right align with '0'. */
    put_padding(!(flags & FL_MINUS) && (flags & FL_ZERO), '0', width, nchar);

    char t_str[32] = { '\0' };

    while (ndigits > 0) {
        t_str[--ndigits] = ((flags & FL_UPPER) ?
                "0123456789ABCDEF" : "0123456789abcdef")
            [value % base];
        value /= base;
    }

    puts(t_str);

    /* ... left align with space. */
    put_padding((flags & FL_MINUS), ' ', width, nchar);
}

#define USTTOLL(x) ((unsigned long)(x))
#define STTOLL(x)  ((unsigned long)(long)(x))

static void ap_format_int(int base, unsigned long flags, int width,
    int precision, int rank, va_list *ap)
{
    unsigned long value = 0;

    /* Do series of type cast for correct data type. */

    if (flags & FL_SIGNED) {
        switch (rank) {
        case RANK_CHAR:
            value = STTOLL((unsigned char)(va_arg(*ap, int)));
            break;

        case RANK_SHORT:
            value = STTOLL((unsigned short)(va_arg(*ap, int)));
            break;

        case RANK_INT:
            value = STTOLL(va_arg(*ap, unsigned int));
            break;

        case RANK_LONG:
            value = STTOLL(va_arg(*ap, unsigned long));
            break;

            /* No 'rank_long_long' so that 'format_int' can use 'div' and '%'
             * without using gcc '__udiv' helping functions.
             */

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
            value = USTTOLL(va_arg(*ap, unsigned int));
            break;

        case RANK_LONG:
            value = USTTOLL(va_arg(*ap, unsigned long));
            break;

            /* No 'rank_long_long' so that 'format_int' can use 'div' and '%'
             * without using gcc '__udiv' helping functions.
             */

        }
    }

    format_int(value, base, flags, width, precision);
}

static void format_str(const char *str, int count,
    unsigned long flags, int width, int precision)
{
    int i = 0;

    if (precision != -1 && count > precision)
        count = precision;

    put_padding(!(flags & FL_MINUS), ' ', width, count);

    while (i < count)
        putchar(str[i++]);

    put_padding((flags & FL_MINUS), ' ', width, count);
}

int printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    /* * ''%[parameter][flags][width][.precision][length or modifier]type''. */

    int ret = 0;

    enum {
        ST_NORMAL,
        ST_FLAGS,
        ST_WIDTH,
        ST_PRECISION,
        ST_MODIFIERS
    } state = ST_NORMAL;

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
                    putchar(ch);
                    /* ... ignore next '%'. */
                    p++;
                }
            } else
                putchar(ch);

            break;

        case ST_FLAGS:
            switch (ch) {
            case '-':
                flags |= FL_MINUS;
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

            if (ch >= '0' && ch <= '9')
                width = width * 10 + (ch - '0');
            else if (ch == '.') {
                precision = 0;
                state = ST_PRECISION;
            } else {
                state = ST_MODIFIERS;
                goto st_modifiers;
            }

            break;

        case ST_PRECISION:

            if (ch >= '0' && ch <= '9')
                precision = precision * 10 + (ch - '0');
            else {
                state = ST_MODIFIERS;
                goto st_modifiers;
            }

            break;

        case ST_MODIFIERS:
st_modifiers:

            if ((ch == 'h') && (rank > RANK_CHAR))
                rank--;

            else if ((ch == 'l') && (rank < RANK_LONG))
                rank++;

            else {
                state = ST_NORMAL;

                switch (ch) {
                case 'P':
                    flags |= FL_UPPER;

                case 'p':
                    flags |= FL_HASH;
                    format_int(USTTOLL(va_arg(ap, void *)), 16, flags, width,
                        sizeof(void *) >> 2);
                    break;

                case 'd':
                case 'i':
                    flags |= FL_SIGNED;

                case 'u':
                    ap_format_int(10, flags, width, precision, rank, &ap);
                    break;

                case 'o':
                    ap_format_int(8, flags, width, precision, rank, &ap);
                    break;

                case 'X':
                    flags |= FL_UPPER;

                case 'x':
                    ap_format_int(16, flags, width, precision, rank, &ap);
                    break;

                case 'c': {
                    char char_arg = (signed char)(va_arg(ap, int));

                    format_str(&char_arg, 1, flags, width, precision);
                    break;
                }

                case 's': {
                    const char *str_arg = va_arg(ap, const char *);
                    str_arg = str_arg ? str_arg : "(null)";

                    format_str(str_arg, strlen(str_arg), flags, width,
                        precision);
                    break;
                }

                default:
                    ret = -1;

                    goto out;
                }
            }
        }
    }

out:
    va_end(ap);

    return ret;
}
