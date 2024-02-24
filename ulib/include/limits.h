/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ULIB_LIMITS_H__
#define __ULIB_LIMITS_H__

#ifdef __SHRT_MAX__
#  define SHRT_MAX __SHRT_MAX__
#else
#  error '__SHRT_MAX__' is not defined.
#endif

#define SHRT_MIN (-SHRT_MAX - 1)

#ifdef __INT_MAX__
#  define INT_MAX __INT_MAX__
#else
#  error '__INT_MAX__' is not defined.
#endif

#define INT_MIN (-INT_MAX - 1)
#define UINT_MAX (INT_MAX * 2U + 1U)

#ifdef __LONG_MAX__
#  define LONG_MAX __LONG_MAX__
#else
#  error '__LONG_MAX__' is not defined.
#endif

# define LONG_MIN (-LONG_MAX - 1)
# define ULONG_MAX (LONG_MAX * 2UL + 1UL)

#ifdef __LONG_LONG_MAX__
#  define LLONG_MAX __LONG_LONG_MAX__
#else
#  error '__LONG_LONG_MAX__' is not defined.
#endif

# define LLONG_MIN (-LLONG_MAX - 1)
# define ULLONG_MAX (LLONG_MAX * 2ULL + 1ULL)

#define UINTPTR_MAX ULONG_MAX

#endif /* __ULIB_LIMITS_H__ */
