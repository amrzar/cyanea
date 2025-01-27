/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_MINMAX_H__
#define __CYANEA_MINMAX_H__

/* Sure comparing arguments of same types. It is too strict but simple. */
#define __typecheck(a, b) (!!(sizeof((typeof(a) *)(1) == (typeof(b) *)(1))))

#define __no_side_effects(a, b) (__builtin_constant_p(a) && __builtin_constant_p(b))

#define __cmp_const(a, b, op) ((a) op (b) ? (a) : (b))
#define __cmp_once(a, b, op) \
	({ \
		typeof(a) a_value = (a); \
		typeof(b) b_value = (b); \
		__cmp_const(a_value, b_value, op); \
	})

#define __careful_cmp(a, b, op) \
	__builtin_choose_expr((__typecheck(a, b) && __no_side_effects(a, b)), \
	        __cmp_const(a, b, op), __cmp_once(a, b, op))

#define min(a, b) __careful_cmp(a, b, <)
#define max(a, b) __careful_cmp(a, b, >)

#define clamp(a, low, high) min(max(a, low), high)

#endif /* __CYANEA_MINMAX_H__ */
