/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/percpu.h>
#include <asm/ukernel.lds.h>

#define BOOT_PERCPU_OFFSET ((unsigned long)__per_cpu_load)

/* '__per_cpu_offset' is the offset that has to be added to a percpu variable
 * to get to the instance for a certain processor.
 */

/* For x86_64, '__per_cpu_offset' is the address of percpu area. */

/* We initialise '__per_cpu_offset' with '__per_cpu_load' so that the BSP can
 * use standard API during boot to directly access per-cpu section '.data..percpu'
 * in linker script.
 */

unsigned long __per_cpu_offset[NR_CPUS] __ro_after_init = {
	[0 ... NR_CPUS - 1] = BOOT_PERCPU_OFFSET
};

/* Store offset in percpu area, so it can be accessed for current cpu easily. */

unsigned long this_cpu_off __percpu = BOOT_PERCPU_OFFSET;
