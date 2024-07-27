/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UAPI_CYANEA_BYTEORDER_H__
#define __UAPI_CYANEA_BYTEORDER_H__

#include <uapi/cyanea/swab.h>
#include <uapi/cyanea/types.h>

#ifdef CONFIG_CPU_LITTLE_ENDIAN
#define __cpu_to_le64(x) ((__le64)(__u64)(x))
#define __le64_to_cpu(x) ((__u64)(__le64)(x))
#define __cpu_to_le32(x) ((__le32)(__u32)(x))
#define __le32_to_cpu(x) ((__u32)(__le32)(x))
#define __cpu_to_le16(x) ((__le16)(__u16)(x))
#define __le16_to_cpu(x) ((__u16)(__le16)(x))
#define __cpu_to_be64(x) ((__be64)__swab64((x)))
#define __be64_to_cpu(x) __swab64((__u64)(__be64)(x))
#define __cpu_to_be32(x) ((__be32)__swab32((x)))
#define __be32_to_cpu(x) __swab32((__u32)(__be32)(x))
#define __cpu_to_be16(x) ((__be16)__swab16((x)))
#define __be16_to_cpu(x) __swab16((__u16)(__be16)(x))

#else
#  error 'CONFIG_CPU_LITTLE_ENDIAN' is not set.
#endif /* CONFIG_CPU_LITTLE_ENDIAN */

#endif /* __UAPI_CYANEA_BYTEORDER_H__ */
