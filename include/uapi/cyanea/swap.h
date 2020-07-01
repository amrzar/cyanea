/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UAPI_CYANEA_SWAP_H__
#define __UAPI_CYANEA_SWAP_H__

#include <uapi/cyanea/types.h>
#include <uapi/asm/bitsperlong.h>

#define __swab16(x) ((__u16)__builtin_bswap16((__u16)(x)))
#define __swab32(x) ((__u32)__builtin_bswap32((__u32)(x)))
#define __swab64(x) ((__u64)__builtin_bswap64((__u64)(x)))

#if __BITS_PER_LONG == 64
static inline __attribute__((__always_inline__))
unsigned long __swap(const unsigned long x)
{
    return __swab64(x);
}

#else /* __BITS_PER_LONG == 32 */
static inline __attribute__((__always_inline__))
unsigned long __swap(const unsigned long x)
{
    return __swab32(x);
}
#endif

#endif /* __UAPI_CYANEA_SWAP_H__ */
