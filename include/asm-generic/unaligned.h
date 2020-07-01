/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASM_GENERIC_UNALIGNED_H__
#define __ASM_GENERIC_UNALIGNED_H__

#include <cyanea/compiler.h>
#include <cyanea/byteorder.h>

/* **/
/* '__get_unaligned_t' and '__put_unaligned_t' by using '__packed' convince the
 * compiler that alignment of pointer 'ptr' is one. So that, the compiler issues
 * the correct load and store instructions.
 *
 * */

#define __get_unaligned_t(type, ptr) ({ \
        const struct { type x; } __packed *__pptr = (typeof(__pptr))(ptr); \
        __pptr->x; \
    })

#define __put_unaligned_t(type, value, ptr) do { \
        struct { type x; } __packed *__pptr = (typeof(__pptr))(ptr); \
        __pptr->x = (value); \
    } while (0)

#define get_unaligned(p)        __get_unaligned_t(typeof(*(p)), (p))
#define put_unaligned(value, p) __put_unaligned_t(typeof(*(p)), (value), (p))

#define __gen_put_unaligned(size, type) \
static inline void put_unaligned_ ## type ## size (u ## size value, void *p) \
{ \
    __put_unaligned_t(__ ## type ## size, cpu_to_ ## type ## size (value), p); \
}

__gen_put_unaligned(16, le);    /* ''put_unaligned_le16'' */
__gen_put_unaligned(32, le);    /* ''put_unaligned_le32'' */
__gen_put_unaligned(64, le);    /* ''put_unaligned_le64'' */
__gen_put_unaligned(16, be);    /* ''put_unaligned_be16'' */
__gen_put_unaligned(32, be);    /* ''put_unaligned_be32'' */
__gen_put_unaligned(64, be);    /* ''put_unaligned_be64'' */
#  undef __gen_put_unaligned

#define __gen_get_unaligned(size, type) \
static inline u ## size get_unaligned_ ## type ## size (const void *p) \
{ \
    return type ## size ## _to_cpu(__get_unaligned_t(__ ## type ## size, p)); \
}

__gen_get_unaligned(16, le);    /* ''get_unaligned_le16'' */
__gen_get_unaligned(32, le);    /* ''get_unaligned_le32'' */
__gen_get_unaligned(64, le);    /* ''get_unaligned_le64'' */
__gen_get_unaligned(16, be);    /* ''get_unaligned_be16'' */
__gen_get_unaligned(32, be);    /* ''get_unaligned_be32'' */
__gen_get_unaligned(64, be);    /* ''get_unaligned_be64'' */
#  undef __gen_get_unaligned

#endif /* __ASM_GENERIC_UNALIGNED_H__ */
