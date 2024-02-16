/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/errno.h>

#include <limits.h>
#include <ctype.h>

#include "../io_buffer.h"

static inline unsigned int digit_value(int ch)
{
    if (isdigit(ch)) {
        return ch - '0';
    } else if (isxdigit(ch)) {
        if (isupper(ch))
            return ch - 'A' + 10;
        else
            return ch - 'a' + 10;
    } else
        return -1;
}

static int __iob_strtoull(_IO_BUFFER io, int base,
    unsigned long long *result_ret)
{
    char c[2];
    int tmp;
    unsigned long long result = 0;

    if (base == 0) {
        if (__iob_read(io, &c[0], 1)) {
            if (c[0] == '0') {
                base = 8;

                if (__iob_read(io, &c[1], 1)) {
                    if ((c[1] == 'x') || (c[1] == 'X'))
                        base = 16;
                    else
                        __iob_ungetc(io, c[1]);
                }
            } else {
                base = 10;
                __iob_ungetc(io, c[0]);
            }
        }

    } else if (base == 16) {
        if ((tmp = __iob_read(io, c, 2)) == 2) {
            if ((c[0] != '0') || (c[1] != 'x' && c[1] != 'X')) {
                __iob_ungetc(io, c[1]);
                __iob_ungetc(io, c[0]);
            }
        } else if (tmp == 1)
            __iob_ungetc(io, c[0]);
    }

    while (__iob_read(io, &c[0], 1)) {
        if ((tmp = digit_value(c[0])) >= 0 && tmp < base) {
            if (result & (ULLONG_MAX << 60)) {
                if (result > (ULLONG_MAX - tmp) / base) {
                    __iob_ungetc(io, c[0]);
                    return -ERANGE;
                }
            }

            result = result * base + tmp;
        } else {
            __iob_ungetc(io, c[0]);
            break;
        }
    }

    *result_ret = result;
    return SUCCESS;
}

int iob_strtoull(_IO_BUFFER io, int base, unsigned long long *result_ret)
{
    return __iob_strtoull(io, base, result_ret);
}

int iob_strtoll(_IO_BUFFER io, int base, long long *result_ret)
{
    char c;
    int ret;
    unsigned long long result;

    if (__iob_read(io, &c, 1)) {
        if (c == '-') {
            if ((ret = iob_strtoull(io, base, &result)) < 0)
                return ret;

            if ((long long)(-result) > 0)
                return -ERANGE;

            *result_ret = -result;
        } else {
            if (c != '+')
                __iob_ungetc(io, c);

            if ((ret = iob_strtoull(io, base, &result)) < 0)
                return ret;

            if ((long long)(result) < 0)
                return -ERANGE;

            *result_ret = result;
        }
    }

    return SUCCESS;
}
