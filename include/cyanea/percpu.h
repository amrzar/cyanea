/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_PERCPU_H__
#define __CYANEA_PERCPU_H__

#include <cyanea/init.h>
#include <cyanea/pgtable.h>

#define NR_CPUS CONFIG_NR_CPUS

#include <asm/percpu.h>

#define per_cpu_offset(x) (__per_cpu_offset[x])

#define SHIFT_PERCPU_PTR(ptr, offset) \
	((typeof(ptr))((unsigned long)(ptr) + (offset)))

#define per_cpu_ptr(ptr, cpu) \
	SHIFT_PERCPU_PTR(ptr, per_cpu_offset(cpu))

#define this_cpu_ptr(ptr) arch_raw_cpu_ptr(ptr)

#define per_cpu(var, cpu) (*per_cpu_ptr(&(var), cpu))

#endif /* __CYANEA_PERCPU_H__ */
