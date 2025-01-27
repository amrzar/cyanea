/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/smp.h>

unsigned int nr_cpu_ids = 1;

/* Per-cpu CPU ID mappings. */
u32 cpuid_to_apic_id __percpu;
u32 cpuid_to_acpi_id __percpu;

