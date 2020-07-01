/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_LIMITS_H__
#define __CYANEA_LIMITS_H__

#define USHRT_MAX ((unsigned short)~0U)
#define SHRT_MAX ((short)(USHRT_MAX >> 1))
#define SHRT_MIN ((short)(-SHRT_MAX - 1))

#define UINT_MAX (~0U)
#define INT_MAX ((int)(UINT_MAX >> 1))
#define INT_MIN (-INT_MAX - 1)

#define ULONG_MAX (~0UL)
#define LONG_MAX ((long)(ULONG_MAX >> 1))
#define LONG_MIN (-LONG_MAX - 1)

#define ULLONG_MAX (~0ULL)
#define LLONG_MAX ((long long)(ULLONG_MAX >> 1))
#define LLONG_MIN (-LLONG_MAX - 1)

#define UINTPTR_MAX ULONG_MAX

#endif /* __CYANEA_LIMITS_H__ */
