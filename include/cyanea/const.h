/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_CONST_H__
#define __CYANEA_CONST_H__

#include <uapi/cyanea/const.h>

#define UL(x)  _UL(x)
#define ULL(x) _ULL(x)

#define BIT_UL(x)  _BITUL(x)
#define BIT_ULL(x) _BITULL(x)

#define IS_ALIGNED(x, a) (((x) & ((__typeof__(x))(a) - 1)) == 0)

#define ALIGN_KERNEL(x, a) __ALIGN_KERNEL(x, a)
#define PTR_ALIGN(p, a) ((typeof(p))ALIGN_KERNEL((unsigned long)(p), (a)))

/* Use 'ROUND_UP/DOWN' only if 'p' is power of two; otherwise, use '__KERNEL_DIV_ROUND_UP'. */

#define ROUND_UP(x, p)  __ALIGN_KERNEL(x, p)
#define ROUND_DOWN(x, p) ((x) & ~((__typeof__(x))((p) - 1)))

#endif /* __CYANEA_CONST_H__ */
