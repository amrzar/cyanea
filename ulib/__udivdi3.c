/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/stddef.h>

extern unsigned long long __udivmodti4(unsigned long long, unsigned long long,
    unsigned long long *);

unsigned long __udivdi3(unsigned long a, unsigned long b)
{
    return __udivmodti4(a, b, NULL);
}
