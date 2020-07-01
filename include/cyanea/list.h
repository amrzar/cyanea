/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_LIST_H__
#define __CYANEA_LIST_H__

#include <cyanea/types.h>
#include <cyanea/container_of.h>

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(LIST head)
{
    head->next = head;
    head->prev = head;
}

static inline void __list_add(LIST_NODE node, LIST_NODE prev, LIST_NODE next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static inline void list_add(LIST_NODE node, LIST head)
{
    __list_add(node, head, head->next);
}

static inline void list_add_tail(LIST_NODE node, LIST head)
{
    __list_add(node, head->prev, head);
}

static inline void __list_del(LIST_NODE prev, LIST_NODE next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(LIST_NODE node)
{
    __list_del(node->prev, node->next);
}

static inline void list_del_init(LIST_NODE node)
{
    __list_del(node);
    INIT_LIST_HEAD(node);
}

static inline int list_empty(LIST_NODE node)
{
    return node->next == node;
}

#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) \
    struct hlist_head name = HLIST_HEAD_INIT

#define INIT_HLIST_HEAD(head) ((head)->first = NULL)
static inline void INIT_HLIST_NODE(HLIST_NODE node)
{
    node->next = NULL;
    node->pprev = NULL;
}

#endif /* __CYANEA_LIST_H__ */
