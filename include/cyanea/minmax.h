/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_MINMAX_H__
#define __CYANEA_MINMAX_H__

#include <cyanea/const.h>

#define __cmp_const(a, b, op) ((a) op (b) ? (a) : (b))
#define __cmp_once(a, b, op) ({ \
        typeof(a) name_a = (a); \
        typeof(b) name_b = (b); \
        __cmp_const(name_a, name_b, op); \
    })

#define __careful_cmp(a, b, op) \
    __builtin_choose_expr( \
        (__builtin_constant_p(a) && \
            __builtin_constant_p(b)), \
        __cmp_const(a, b, op), __cmp_once(a, b, op))

#define min(a, b) __careful_cmp(a, b, <)
#define max(a, b) __careful_cmp(a, b, >)

#define clamp(a, low, high) min(max(a, low), high)

#endif /* __CYANEA_MINMAX_H__ */
