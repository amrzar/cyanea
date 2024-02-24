/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ULIB_STDDEF_H__
#define __ULIB_STDDEF_H__

#ifndef __UINTPTR_TYPE__
#  error '__UINTPTR_TYPE__' is not defined.
#endif

typedef __UINTPTR_TYPE__ uintptr_t;

#ifndef __SIZE_TYPE__
#  error '__SIZE_TYPE__' is not defined.
#endif

typedef __SIZE_TYPE__ size_t;

#  undef NULL
#define NULL ((void *)(0))

enum { false = 0, true = 1 };

#  undef offsetof
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)

#endif /* __ULIB_STDDEF_H__ */
