/*
 * Copyright 2020 Amirreza Zarrabi <amrzar@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __CYANEA_TYPES_H
#define __CYANEA_TYPES_H

#include <asm/types.h>

#ifndef __ASSEMBLY__
#include <cyanea/ukernel_types.h>

typedef _Bool bool;

#ifndef _SIZE_T
#define _SIZE_T
typedef __ukernel_size_t size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef __ukernel_ssize_t ssize_t;
#endif

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef __ukernel_ptrdiff_t ptrdiff_t;
#endif

typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
typedef u64 uint64_t;

typedef unsigned long uintptr_t;

#endif /* __ASSEMBLY__ */
#endif /* __CYANEA_TYPES_H */
