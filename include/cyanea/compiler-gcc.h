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
# error include 'compiler.h', instead.
#endif

#if __GNUC__ < 5
# error sorry, we assume reasonably recent compiler.
#endif

/* ... optimization barrier. */
#define barrier() __asm__ __volatile__("": : :"memory")

#define __always_inline inline __attribute__((always_inline))
#define  noinline __attribute__((noinline))

#define __packed __attribute__((packed))
#define __weak __attribute__((weak))
#define __alias(symbol) __attribute__((alias(#symbol)))

#define __noreturn __attribute__((noreturn))

#define __pure __attribute__((pure))
#define __aligned(x) __attribute__((aligned(x)))
#define __printf(a, b) __attribute__((format(printf, a, b)))
#define __scanf(a, b) __attribute__((format(scanf, a, b)))
#define __attribute_const__ __attribute__((const))

#define __maybe_unused __attribute__((unused))
#define __used __attribute__((used))

#define __must_check __attribute__((warn_unused_result))

#define __compiletime_assert(a, b) \
    _Static_assert(a, b)

#define __compiler_offsetof(a, b)  \
    __builtin_offsetof(a, b)

/*
 * From the GCC manual:
 *
 * ... attribute on functions is used to inform the compiler that
 * the function is unlikely to be executed ...
 *
 * ... paths leading to calls of cold functions within code are marked
 * as unlikely by the branch prediction mechanism.
 */
#define __cold __attribute__((cold))

#define unreachable() __builtin_unreachable()
#define __visible __attribute__((externally_visible))

/* ... suppress uninitialized variable warning. */
#define uninitialized_var(x) x = x
