/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/stdarg.h>

#include "../io_buffer.h"

enum types {
    UINT,
    ULONG,
    ULLONG,
    USHORT,
    UCHAR,
    UIPTR
};

enum ranks {
    RANK_CHAR = -2,
    RANK_SHORT = -1,
    RANK_INT = 0,
    RANK_LONG = 1,
    RANK_LLONG = 2,
    RANK_PTR = INT_MAX
};

static inline void push_arg(enum types type, uintmax_t value, va_list * ap)
{
#define __va_arg(type) *va_arg(*ap, type *) = (type)value

    switch (type) {
    case UCHAR:
        __va_arg(unsigned char);
        break;

    case USHORT:
        __va_arg(unsigned short);
        break;

    case UINT:
        __va_arg(unsigned int);
        break;

    case ULONG:
        __va_arg(unsigned long);
        break;

    case ULLONG:
        __va_arg(unsigned long long);
        break;

    case UIPTR:
        *va_arg(*ap, void **) = (void *)(uintptr_t) value;
        break;
    }

#  undef __va_arg
}

#define FL_SPLAT 0
#define FL_WIDTH 1

int io_buffer_vsscanf(_IO_BUFFER io, const char *format, va_list _ap)
{
    uintmax_t value = 0;
    int converted = 0;

    enum {
        ST_NORMAL,
        ST_FLAGS,
        ST_WIDTH,
        ST_MODIFIERS,
    } state = ST_NORMAL;

    va_list ap;
    /* ... in some 64-bit architectures 'va_list' is an array,
       'va_copy' allows passing 'va_list *' even if 'va_list' is an array. */
    va_copy(ap, _ap);

    char ch, q;
    int p = 0;

    /* ... flags and parameters extracted from 'format'. */
    unsigned int width;
    unsigned long flags;
    int rank, base;

#define BAIL_NONE 0
#define BAIL_EOF 1
#define BAIL_ERROR 2
    int bail = BAIL_NONE;

    while ((ch = format[p++]) && bail == BAIL_NONE) {
        switch (state) {
        case ST_NORMAL:{
                if (ch == '%') {
                    state = ST_FLAGS;
                    flags = 0;
                    rank = RANK_INT;
                    width = UINT_MAX;

                } else if (isspace(ch)) {
                    while (io_buffer_read(&q, 1, io)) {
                        if (!isspace(q)) {
                            io_buffer_ungetc(q, io);
                            break;
                        }
                    }
                } else {
                    if (io_buffer_read(&q, 1, io)) {
                        if (q != ch) {
                            io_buffer_ungetc(q, io);
                            bail = BAIL_ERROR;
                        }
                    } else
                        bail = BAIL_EOF;
                }
            }

            break;

        case ST_FLAGS:
            switch (ch) {
            case '*':
                BIT_SET(flags, FL_SPLAT);
                break;

            case '0' ... '9':
                width = (ch - '0');
                state = ST_WIDTH;
                BIT_SET(flags, FL_WIDTH);
                break;

            default:
                state = ST_MODIFIERS;
                goto st_modifiers;
            }

            break;

        case ST_WIDTH:
            if (isdigit(ch)) {
                width = width * 10 + (ch - '0');
            } else {
                state = ST_MODIFIERS;
                goto st_modifiers;
            }

            break;

        case ST_MODIFIERS:
 st_modifiers:

            switch (ch) {
            case 'h':
                rank--;
                break;

            case 'l':
                rank++;
                break;

            case 'j':
                rank = RANK_LLONG;
                break;

            case 'z':
                rank = RANK_LONG;
                break;

            case 't':
                rank = RANK_LONG;
                break;

            case 'L':
            case 'q':
                rank = RANK_LLONG;
                break;

            default:
                state = ST_NORMAL;

                rank = clamp(rank, RANK_CHAR, RANK_LLONG);

                switch (ch) {
                case 'P':
                case 'p':
                    rank = RANK_PTR;

                case 'i':
                    base = 0;
                    goto scan_int;

                case 'd':
                case 'u':
                    base = 10;
                    goto scan_int;

                case 'o':
                    base = 8;
                    goto scan_int;

                case 'x':
                case 'X':
                    base = 16;
                    goto scan_int;

 scan_int:         {
                        int success = 0;

                        bail = BAIL_EOF;        /* ... assume it is EOF. */
                        value = __strntoumax(io, base, &success, &bail);

                        if (success && !BIT_TEST(flags, FL_SPLAT)) {
                            switch (rank) {
                            case RANK_CHAR:
                                push_arg(UCHAR, value, &ap);
                                break;

                            case RANK_SHORT:
                                push_arg(USHORT, value, &ap);
                                break;

                            case RANK_INT:
                                push_arg(UINT, value, &ap);
                                break;

                            case RANK_LONG:
                                push_arg(ULONG, value, &ap);
                                break;

                            case RANK_LLONG:
                                push_arg(ULLONG, value, &ap);
                                break;

                            case RANK_PTR:
                                push_arg(UIPTR, value, &ap);
                                break;
                            }

                            converted++;
                        }
                    }

                    break;

                case 'c':
                    width = BIT_TEST(flags, FL_WIDTH) ? width : 1;

                    if (BIT_TEST(flags, FL_SPLAT)) {
                        while (width--) {
                            if (!io_buffer_read(&q, 1, io)) {
                                bail = BAIL_EOF;
                                break;
                            }
                        }
                    } else {
                        char *sarg = va_arg(ap, char *);

                        while (width--) {
                            if (!io_buffer_read(&q, 1, io)) {
                                bail = BAIL_EOF;
                                break;
                            }

                            *sarg++ = q;
                        }

                        if (bail == BAIL_NONE)
                            converted++;
                    }

                    break;

                case 's':
                    if (BIT_TEST(flags, FL_SPLAT)) {
                        while (width--) {
                            if (!io_buffer_read(&q, 1, io)) {
                                bail = BAIL_EOF;
                                break;

                            } else if (isspace(q))
                                break;
                        }
                    } else {
                        char *sarg = va_arg(ap, char *);

                        while (width--) {
                            if (!io_buffer_read(&q, 1, io)) {
                                bail = BAIL_EOF;
                                break;

                            } else if (!isspace(q))
                                *sarg++ = q;

                            else        /* ... it is space. */
                                break;
                        }

                        if (bail == BAIL_NONE) {
                            *sarg = '\0';
                            converted++;
                        }
                    }

                    break;

                case '%':
                    if (io_buffer_read(&q, 1, io))
                        if (q != '%')
                            bail = BAIL_ERROR;
                        else
                            bail = BAIL_EOF;

                    break;

                default:       /* ... invalid format. */
                    bail = BAIL_ERROR;
                }
            }
        }
    }

    if (bail == BAIL_EOF && !converted)
        converted = -1;

    return converted;
}
