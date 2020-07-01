/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_TYPES_H__
#define __CYANEA_TYPES_H__

#ifndef __ASSEMBLY__

#include <asm-generic/int-ll64.h>
#include <uapi/cyanea/types.h>

typedef _Bool bool;

#ifndef __UINTPTR_TYPE__
#  error '__UINTPTR_TYPE__' is not defined.
#endif

typedef __UINTPTR_TYPE__ uintptr_t;

#ifndef __SIZE_TYPE__
#  error '__SIZE_TYPE__' is not defined.
#endif

typedef __SIZE_TYPE__ size_t;

typedef struct list_head {
    struct list_head *next, *prev;
} *LIST, *LIST_NODE;

typedef struct hlist_head {
    struct hlist_node *first;
} *HLIST;

typedef struct hlist_node {
    struct hlist_node *next, **pprev;
} *HLIST_NODE;

#endif /*  __ASSEMBLY__ */

#endif /* __CYANEA_TYPES_H__ */
