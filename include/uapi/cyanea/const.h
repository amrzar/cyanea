/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UAPI_CYANEA_CONST_H__
#define __UAPI_CYANEA_CONST_H__

#ifdef __ASSEMBLY__
#define _AC(X, Y) X
#define _AT(T, X) X
#else
#define __AC(X, Y) (X ## Y)
#define _AC(X, Y) __AC(X, Y)
#define _AT(T, X) ((T)(X))
#endif /*  __ASSEMBLY__ */

#define _UL(x) (_AC(x, UL))
#define _ULL(x) (_AC(x, ULL))

#define _BITUL(x) (_UL(1) << (x))
#define _BITULL(x) (_ULL(1) << (x))

#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define __ALIGN_KERNEL(x, a) __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)

#define __KERNEL_DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

#endif /* __UAPI_CYANEA_CONST_H__ */
