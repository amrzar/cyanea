/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_SMP_H__
#define __CYANEA_SMP_H__

#include <cyanea/types.h>
#include <cyanea/percpu.h>

extern unsigned int nr_cpu_ids;
static __always_inline void set_nr_cpu_ids(unsigned int nr)
{
	nr_cpu_ids = nr;
}

/* Per-cpu CPU ID mappings. */
extern u32 cpuid_to_apic_id __percpu;
extern u32 cpuid_to_acpi_id __percpu;

#endif /* __CYANEA_SMP_H__ */
