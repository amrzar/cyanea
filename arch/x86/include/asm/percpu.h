/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PERCPU_H__
#define __X86_ASM_PERCPU_H__

#ifndef __CYANEA_PERCPU_H__
#  error "Use 'cyanea/percpu.h'"
#endif

#include <cyanea/cache.h>

#define __percpu __section(".data..percpu")

#define __percpu_cache_aligned __section(".data..percpu..cache_aligned") \
	____cacheline_aligned

#define __percpu_page_aligned __section(".data..percpu..page_aligned") \
	__aligned(PAGE_SIZE)

/* GS address space accessors. */

#ifdef __SEG_GS
# define __my_cpu_type(var) typeof(var) __seg_gs
#else
# error "GCC does not support __SEG_GS."
#endif

/**
 * @def __my_cpu_ptr(ptr)
 * @brief Get a pointer in GS address space from a per-CPU pointer.
 *
 * This macro prevents GCC from generating the error:
 *     "cast to `__seg_gs` address space pointer from disjoint
 *      generic address space pointer".
 *
 * @return Pointer in GS address space.
 */
#define __my_cpu_ptr(ptr) ((__my_cpu_type(*(ptr)) *)(unsigned long)(ptr))

/**
 * @def __raw_cpu_read(qual, pcp)
 * @brief Reads the value of the per-CPU variable with the specified qualifier.
 * @return The value of the per-CPU variable.
 */
#define __raw_cpu_read(qual, pcp) \
	({ *(qual __my_cpu_type(pcp) *)__my_cpu_ptr(&(pcp)); })

/**
 * @def __raw_cpu_write(qual, pcp, val)
 * @brief Writes the value to the per-CPU variable with the specified qualifier.
 */
#define __raw_cpu_write(qual, pcp, val) \
	do { \
		*(qual __my_cpu_type(pcp) *)__my_cpu_ptr(&(pcp)) = (val); \
	} while(0)

#define raw_cpu_read(pcp) __raw_cpu_read(, pcp)
#define raw_cpu_write(pcp, val) __raw_cpu_write(, pcp, val)

/**
 * @def this_cpu_read(pcp)
 * @brief Read the value of a per-CPU variable.
 *
 * @note The `volatile` keyword in @ref this_cpu_read ensures that the compiler
 * does not optimize away the read or write operations, providing stronger
 * memory ordering guarantees.
 *
 * @see https://github.com/torvalds/linux/commit/0b9ccc0a9b146b49e83bf1e32f70d2396a694bfb
 *
 * @return The value of the per-CPU variable.
 */
#define this_cpu_read(pcp) __raw_cpu_read(volatile, pcp)

/* See 'this_cpu_read' comments. */
#define this_cpu_write(pcp, val) __raw_cpu_write(volatile, pcp, val)

extern unsigned long this_cpu_off __percpu;
extern unsigned long __per_cpu_offset[NR_CPUS] __ro_after_init;

#define arch_raw_cpu_ptr(ptr) \
	({ \
		unsigned long tcp_ptr__ = raw_cpu_read(this_cpu_off); \
		tcp_ptr__ += (unsigned long)(ptr); \
		(typeof(ptr))tcp_ptr__; \
	})

#endif /* __X86_ASM_PERCPU_H__ */
