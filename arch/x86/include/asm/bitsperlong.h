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

#ifndef __ASM_X86_BITSPERLONG_H
#define __ASM_X86_BITSPERLONG_H

#if (defined(__x86_64__) && defined(__ILP32__)) || defined(__i386__)
# error 'x32' and 'i386' are not supported.
#endif

#define __BITS_PER_LONG 64

#include <asm-generic/bitsperlong.h>

#endif /* __ASM_X86_BITSPERLONG_H */
