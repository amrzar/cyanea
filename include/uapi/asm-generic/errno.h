/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UAPI_ASM_GENERIC_ERRNO_H__
#define __UAPI_ASM_GENERIC_ERRNO_H__

/* **/
/* Pointers in the first page ''0 .. 4095'' have extra information so that a
 * function can return a normal pointer or an error code with a same return
 * value. This is a per-architecture ''enum'', to allow different error and
 * pointer decisions.
 * 
 * */

enum {
    EINVAL = 1,                 /* Invalid argument. */
    ERANGE = 2,                 /* Results not representable. */
    ENOSPC = 3,                 /* No space available. */
    EEXIST = 4,                 /* Entry exists. */
    ENOENT = 5,                 /* Entry does not exist. */
    ENODEV = 6,                 /* No such device. */
    ENOMEM = 7,                 /* Out of memory */

    __errno_asm_generic_end
};

#endif /* __UAPI_ASM_GENERIC_ERRNO_H__ */
