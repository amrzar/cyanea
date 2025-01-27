/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PERCPU_H__
#define __X86_ASM_PERCPU_H__

#ifndef __CYANEA_PERCPU_H__
#  error "Use 'cyanea/percpu.h'"
#endif

/**
 * @file percpu.h
 * @brief Per-CPU variable access macros for x86 architecture.
 *
 * This header file defines macros for reading and writing per-CPU variables
 * in the x86 architecture. These macros provide a way to access variables
 * that are specific to each CPU core.
 *
 * @details
 * The following macros are defined:
 * - `raw_cpu_read(pcp)`: Reads the value of the per-CPU variable `pcp` without
 *   any memory ordering constraints.
 * - `raw_cpu_write(pcp, val)`: Writes the value `val` to the per-CPU variable
 *   `pcp` without any memory ordering constraints.
 * - `this_cpu_read(pcp)`: Reads the value of the per-CPU variable `pcp` with
 *   volatile memory ordering constraints.
 * - `this_cpu_write(pcp, val)`: Writes the value `val` to the per-CPU variable
 *   `pcp` with volatile memory ordering constraints.
 *
 * These macros internally use the `__raw_cpu_read` and `__raw_cpu_write`
 * functions to perform the actual read and write operations.
 *
 * @note
 * The `volatile` keyword in `this_cpu_read` and `this_cpu_write` ensures that
 * the compiler does not optimize away the read or write operations, providing
 * stronger memory ordering guarantees.
 *
 * @see https://github.com/torvalds/linux/commit/0b9ccc0a9b146b49e83bf1e32f70d2396a694bfb
 */

#include <cyanea/cache.h>

#define __percpu __section(".data..percpu")
#define __percpu_cache_aligned __section(".data..percpu..cache_aligned") \
	____cacheline_aligned
#define __percpu_page_aligned __section(".data..percpu..page_aligned") \
	__aligned(PAGE_SIZE)

/* ''GS address space accessors''. */

#ifdef __SEG_GS
# define __seg_gs_as_type(var) typeof(var) __seg_gs
#else
# error "GCC does not support __SEG_GS."
#endif

/*
 * Get a pointer in GS address space from a per-CPU pointer.
 * This prevents GCC from generating the error:
 *   '' cast to __seg_gs address space pointer from disjoint
 *      generic address space pointer ''.
 */
#define __seg_gs_as_ptr(ptr) ((__seg_gs_as_type(*(ptr)) *)(unsigned long)(ptr))

#define __raw_cpu_read(qual, pcp) \
	({ *(qual __seg_gs_as_type(pcp) *)__seg_gs_as_ptr(&(pcp)); })

#define __raw_cpu_write(qual, pcp, val) \
	do { \
		*(qual __seg_gs_as_type(pcp) *)__seg_gs_as_ptr(&(pcp)) = (val); \
	} while(0)

#define raw_cpu_read(pcp)               __raw_cpu_read(, pcp)
#define raw_cpu_write(pcp, val)         __raw_cpu_write(, pcp, val)
#define this_cpu_read(pcp)              __raw_cpu_read(volatile, pcp)
#define this_cpu_write(pcp, val)        __raw_cpu_write(volatile, pcp, val)

extern unsigned long this_cpu_off __percpu;

extern unsigned long __per_cpu_offset[NR_CPUS] __ro_after_init;

#define arch_raw_cpu_ptr(ptr) \
	({ \
		unsigned long tcp_ptr__ = raw_cpu_read(this_cpu_off); \
		tcp_ptr__ += (unsigned long)(ptr); \
		(typeof(ptr))tcp_ptr__; \
	})

#endif /* __X86_ASM_PERCPU_H__ */
