/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/percpu.h>
#include <cyanea/smp.h>
#include <asm/ukernel.lds.h>

#define BOOT_PERCPU_OFFSET ((unsigned long)__per_cpu_load)

/**
 * @var __per_cpu_offset
 * @brief Array that holds the offset values for each CPU's per-CPU area.
 *
 * `__per_cpu_offset` is the offset that needs to be added to a per-CPU variable
 * to access the instance for a specific CPU.
 *
 * For x86_64, `__per_cpu_offset` is the address of the per-CPU area.
 * `__per_cpu_offset` is initialized with `__per_cpu_load` so that the BSP can
 * use the standard API during boot to directly access the per-CPU section
 * `.data..percpu`.
 */
unsigned long __per_cpu_offset[NR_CPUS] __ro_after_init = {
	[0 ... NR_CPUS - 1] = BOOT_PERCPU_OFFSET
};

/* Store offset in per-CPU area, so it can be accessed for current cpu easily. */
unsigned long this_cpu_off __percpu = BOOT_PERCPU_OFFSET;

void __init setup_per_cpu_areas(void)
{
	ulog_info("NR_CPUS: %d nr_cpu_ids:%u\n", NR_CPUS, nr_cpu_ids);
}
