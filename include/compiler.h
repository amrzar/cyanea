/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __COMPILER_H__
#define __COMPILER_H__

#define __packed __attribute__((__packed__))
#define __aligned(a) __attribute__((__aligned__(a)))
#define __section(section) __attribute__((__section__(section)))

#define __visible __attribute__((__externally_visible__))

#define __noreturn __attribute__((__noreturn__))

#define __always_unused __attribute__((__unused__))
#define __maybe_unused __attribute__((__unused__))

#define always_inline inline __attribute__((__always_inline__))
#define noinline __attribute__((__noinline__))
#define fallthrough __attribute__((__fallthrough__))

#define likely(x)  __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define __must_check __attribute__((__warn_unused_result__))

#define static_assert(expr, ...) __static_assert(expr, ##__VA_ARGS__, #expr)
#define __static_assert(expr, msg, ...) _Static_assert(expr, msg)

#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

#define __used __attribute__((__used__))

#define __weak __attribute__((__weak__))

#define __pure __attribute__((__pure__))

#endif /* __COMPILER_H__ */
