/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UAPI_CYANEA_SWAB_H__
#define __UAPI_CYANEA_SWAB_H__

#include <uapi/cyanea/types.h>
#include <uapi/asm/bitsperlong.h>

#define __swab16(x) ((__u16)__builtin_bswap16((__u16)(x)))
#define __swab32(x) ((__u32)__builtin_bswap32((__u32)(x)))
#define __swab64(x) ((__u64)__builtin_bswap64((__u64)(x)))

#endif /* __UAPI_CYANEA_SWAB_H__ */
