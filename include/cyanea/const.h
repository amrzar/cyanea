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
#define PTR_ALIGN(p, a) ((typeof(p))(ALIGN_KERNEL((unsigned long)(p), (a))))

/* Use 'ROUND_UP/DOWN' only if 'pow' is power of two. */
#define ROUND_UP(x, pow)   __ALIGN_KERNEL(x, pow)
#define ROUND_DOWN(x, pow) __ALIGN_KERNEL((x) - ((pow) - 1), (pow))

#endif /* __CYANEA_CONST_H__ */
