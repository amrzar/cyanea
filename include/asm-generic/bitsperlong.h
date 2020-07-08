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

#ifndef __ASM_GENERIC_BITSPERLONG_H
#define __ASM_GENERIC_BITSPERLONG_H

#define BITS_PER_LONG 64 /* ... 64-bit kernel. */

#if defined(__BITS_PER_LONG)
#if BITS_PER_LONG != __BITS_PER_LONG
# error inconsistent word size.
#endif
#else
# error '__BITS_PER_LONG' is undefined.
#endif

#ifndef BITS_PER_LONG_LONG
#define BITS_PER_LONG_LONG 64
#endif

#endif /* __ASM_GENERIC_BITSPERLONG_H */
