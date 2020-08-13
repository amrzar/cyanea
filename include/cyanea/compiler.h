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

#ifndef __CYANEA_COMPILER_H
#define __CYANEA_COMPILER_H

#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)

#ifdef __GNUC__
#include <cyanea/compiler-gcc.h>
#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#ifndef unreachable
#define unreachable() do { } while (1)
#endif

#ifndef __must_check
#define __must_check
#endif

#ifndef __used
#define __used
#endif

#ifndef __maybe_unused
#define __maybe_unused
#endif

#ifndef noinline
#define noinline
#endif

#ifndef __always_inline
#define __always_inline inline
#endif

#ifndef __attribute_const__
#define __attribute_const__
#endif

#ifndef __cold
#define __cold
#endif

#ifndef __section
#define __section(S) __attribute__((__section__(#S)))
#endif

#ifndef __visible
#define __visible
#endif

#ifndef __same_type
#define __same_type(a, b) \
    __builtin_types_compatible_p(typeof(a), typeof(b))
#endif

#endif /* __CYANEA_COMPILER_H */
