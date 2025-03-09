/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/memblock.h>
#include <cyanea/percpu.h>
#include <cyanea/smp.h>
#include <cyanea/string.h>
#include <asm/current.h>
#include <asm/page.h>
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

unsigned long this_cpu_off __percpu = BOOT_PERCPU_OFFSET;

extern u32 __initdata early_cpuid_to_apic_id[NR_CPUS];
extern u32 __initdata early_cpuid_to_acpi_id[NR_CPUS];

/* ukernel/cpu/cpu.c. */
void __init switch_gdt_and_percpu_base(int cpu);

void __init setup_per_cpu_areas(void)
{
	const size_t static_size = __per_cpu_end - __per_cpu_start;
	phys_addr_t paddr;
	int cpu;

	ulog_info("NR_CPUS: %d nr_cpu_ids:%u\n", NR_CPUS, nr_cpu_ids);

	for (cpu = 0; cpu < nr_cpu_ids; cpu++) {
		paddr = memblock_alloc(static_size, PAGE_SIZE, 0, -1, -1, false);
		assert(paddr, "cannot initialize per-CPU area.");

		memcpy(__va(paddr), __per_cpu_load, static_size);

		per_cpu_offset(cpu) = (unsigned long)__va(paddr);
		per_cpu(this_cpu_off, cpu) = per_cpu_offset(cpu);
		per_cpu(percpu_hot.cpu_number, cpu) = cpu;
		per_cpu(cpuid_to_apic_id, cpu) = early_cpuid_to_apic_id[cpu];
		per_cpu(cpuid_to_acpi_id, cpu) = early_cpuid_to_acpi_id[cpu];

		if (!cpu) {
			/* Update BSP to use real per-CPU area. */
			switch_gdt_and_percpu_base(cpu);
		}
	}
}
