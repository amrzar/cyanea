/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_BYTEORDER_H__
#define __CYANEA_BYTEORDER_H__

#include <uapi/cyanea/byteorder.h>

#define cpu_to_le64 __cpu_to_le64
#define le64_to_cpu __le64_to_cpu
#define cpu_to_le32 __cpu_to_le32
#define le32_to_cpu __le32_to_cpu
#define cpu_to_le16 __cpu_to_le16
#define le16_to_cpu __le16_to_cpu
#define cpu_to_be64 __cpu_to_be64
#define be64_to_cpu __be64_to_cpu
#define cpu_to_be32 __cpu_to_be32
#define be32_to_cpu __be32_to_cpu
#define cpu_to_be16 __cpu_to_be16
#define be16_to_cpu __be16_to_cpu

#endif /* __CYANEA_BYTEORDER_H__ */
