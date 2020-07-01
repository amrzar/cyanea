/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_STDDEF_H__
#define __CYANEA_STDDEF_H__

#  undef NULL
#define NULL ((void *)(0))

enum { false = 0, true = 1 };

#  undef offsetof
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)

#endif /* __CYANEA_STDDEF_H__ */
