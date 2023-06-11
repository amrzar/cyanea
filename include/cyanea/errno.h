/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_ERRNO_H__
#define __CYANEA_ERRNO_H__

#include <cyanea/types.h>
#include <cyanea/compiler.h>

#include <uapi/asm/errno.h>

#define SUCCESS 0

/* Pointers in the first page [0 .. 4095] have extra information. */

#define MAX_ERRNO 4095

#define IS_ERR_VALUE(x) \
    unlikely((unsigned long)(void *) (x) >= (unsigned long) -MAX_ERRNO)

/* Use function so we can enforce __must_check. */

static inline void *__must_check ERR_PTR(long error)
{
    return (void *) error;
}

static inline long __must_check PTR_ERR(const void *ptr)
{
    return (long) ptr;
}

static inline bool __must_check IS_ERR(const void *ptr)
{
    return IS_ERR_VALUE(ptr);
}

#endif /* __CYANEA_ERRNO_H__ */
