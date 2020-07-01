/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_ERRNO_H__
#define __CYANEA_ERRNO_H__

#include <cyanea/types.h>
#include <cyanea/compiler.h>

#include <uapi/asm/errno.h>

#define SUCCESS 0

enum {
    MAX_ERRNO = __errno_asm_end
};

static_assert((MAX_ERRNO < 4096), "error codes exceeded the first page.");

#define IS_ERR_VALUE(x) \
    unlikely((unsigned long)(void *)(x) >= (unsigned long)(-MAX_ERRNO))

static inline void *__must_check ERR_PTR(long error)
{
    return (void *)(error);
}

static inline long __must_check PTR_ERR(const void *ptr)
{
    return (long)(ptr);
}

static inline bool __must_check IS_ERR(const void *ptr)
{
    return IS_ERR_VALUE((unsigned long)(ptr));
}

static inline bool __must_check IS_ERR_OR_NULL(const void *ptr)
{
    return unlikely(!ptr) || IS_ERR_VALUE((unsigned long)(ptr));
}

#endif /* __CYANEA_ERRNO_H__ */
