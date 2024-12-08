/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PERCPU_H__
#define __X86_ASM_PERCPU_H__

#ifndef __CYANEA_PERCPU_H__
#  error "Use 'cyanea/percpu.h'"
#endif

#include <cyanea/cache.h>

/* PER-CPU SECTIONS. */

#define __percpu __section(".data..percpu")
#define __percpu_cache_aligned __section(".data..percpu..cache_aligned") \
	____cacheline_aligned
#define __percpu_page_aligned __section(".data..percpu..page_aligned") \
	__aligned(PAGE_SIZE)

extern unsigned long this_cpu_off __percpu;

/* GS address space accessors. */

#ifdef __SEG_GS
# define __seg_gs_as_type(var) typeof(var) __seg_gs
#else
# error "GCC does not support __SEG_GS."
#endif

/* Use '__seg_gs_as_ptr' to get a pointer in GS address space from a per-cpu pointer.
 * Otherwise, GCC spits ''error: cast to __seg_gs address space pointer from
 * disjoint generic address space pointer''.
 */

#define __seg_gs_as_ptr(ptr) ((__seg_gs_as_type(*(ptr)) *)(unsigned long)(ptr))

#define __raw_cpu_read(qual, pcp) ({ \
		*(qual __seg_gs_as_type(pcp) *)__seg_gs_as_ptr(&(pcp)); \
	})

#define __raw_cpu_write(qual, pcp, val) do { \
		*(qual __seg_gs_as_type(pcp) *)__seg_gs_as_ptr(&(pcp)) = (val); \
	} while(0)

/* https://github.com/torvalds/linux/commit/0b9ccc0a9b146b49e83bf1e32f70d2396a694bfb. */

#define raw_cpu_read(pcp) __raw_cpu_read(, pcp)
#define raw_cpu_write(pcp, val) __raw_cpu_write(, pcp, val)

#define this_cpu_read(pcp) __raw_cpu_read(volatile, pcp)
#define this_cpu_write(pcp, val) __raw_cpu_write(volatile, pcp, val)

#define arch_raw_cpu_ptr(ptr) ({ \
		unsigned long __ptr = raw_cpu_read(this_cpu_off); \
		__ptr += (unsigned long)(ptr); \
		(typeof(ptr))__ptr; \
	})

#endif /* __X86_ASM_PERCPU_H__ */
