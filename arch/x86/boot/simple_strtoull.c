/* SPDX-License-Identifier: GPL-2.0-or-later */

/* This file is included by 'setup/string.c' and 'compressed/string.c'. * */

static inline unsigned int digit_value(int ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';

    else if (ch >= 'A' && ch <= 'Z')
        return ch - 'A' + 10;

    else if (ch >= 'a' && ch <= 'z')
        return ch - 'a' + 10;

    else
        return -1;
}

unsigned long long simple_strtoull(const char *cptr, char **endptr,
    unsigned int base)
{
    unsigned long long result = 0;
    unsigned int value;

    if (base == 0) {
        if (cptr[0] == '0') {
            if (cptr[1] == 'x' || cptr[1] == 'X') {
                cptr += 2;
                base = 16;
            } else {
                cptr++;
                base = 8;
            }
        } else
            base = 10;

    } else if (base == 16) {
        if ((cptr[0] == '0') && (cptr[1] == 'x' || cptr[1] == 'X')) {
            cptr += 2;
        }
    }

    while ((value = digit_value(*cptr)) >= 0) {
        if (value > base) {

            /* '0 ..  7' for base 8.  * */
            /* '0 ..  9' for base 10. * */
            /* '0 .. 15' for base 16. * */
            break;
        }

        result = result * base + value;
        cptr++;
    }

    if (endptr != NULL)
        *endptr = (char *)(cptr);

    return result;
}
