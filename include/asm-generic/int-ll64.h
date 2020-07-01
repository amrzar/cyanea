/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASM_GENERIC_INT_LL64_H__
#define __ASM_GENERIC_INT_LL64_H__

#ifndef __ASSEMBLY__
#include <uapi/asm-generic/int-ll64.h>

typedef __s8 s8;
typedef __u8 u8;
_Static_assert((sizeof(u8) == 1) && (sizeof(s8) == 1),
    "'u8' or 's8' mis-size.");

typedef __s16 s16;
typedef __u16 u16;
_Static_assert((sizeof(u16) == 2) && (sizeof(s16) == 2),
    "'u16' or 's16' mis-size.");

typedef __s32 s32;
typedef __u32 u32;
_Static_assert((sizeof(u32) == 4) && (sizeof(s32) == 4),
    "'u32' or 's32' mis-size.");

typedef __s64 s64;
typedef __u64 u64;
_Static_assert((sizeof(u64) == 8) && (sizeof(s64) == 8),
    "'u64' or 's64' mis-size.");

#endif /* __ASSEMBLY__ */

#endif /* __ASM_GENERIC_INT_LL64_H__ */
