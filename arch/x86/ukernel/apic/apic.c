/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/bitops.h>
#include <cyanea/init.h>
#include <cyanea/irqflags.h>
#include <cyanea/smp.h>
#include <asm/apic.h>
#include <asm/fixmap.h>

int x2apic_mode;

void __init check_x2apic(void)
{
	if (x2apic_enabled()) {
		x2apic_mode = 1;

		ulog_info("x2APIC has been enabled by BIOS.\n");
	}
}

/* Boot CPU apic_id. */
u32 boot_cpu_apic_id __ro_after_init;

static phys_addr_t lapic_phys_addr __ro_after_init;
unsigned long lapic_addr __ro_after_init;

/* CPU ID mappings. */
u32 __initdata early_cpuid_to_apic_id[NR_CPUS];
u32 __initdata early_cpuid_to_acpi_id[NR_CPUS];

/* BSP gets CPU# 0. */
static unsigned int __initdata nr_assigned_cpus = 1;

void __init register_apic_id(u32 apic_id, u32 acpi_id)
{
	int cpu;

	cpu = (apic_id == boot_cpu_apic_id) ? 0 : nr_assigned_cpus++;
	if (cpu < NR_CPUS) {
		early_cpuid_to_apic_id[cpu] = apic_id;
		early_cpuid_to_acpi_id[cpu] = acpi_id;

		ulog_info("CPUID (%d) apic_id (%u), acpi_id (%u).\n",
		        cpu, apic_id, acpi_id);
	} else {
		ulog_err("CPU apic_id (%u), acpi_id (%u) dropped.", apic_id, acpi_id);
	}
}

void __init register_lapic_address(phys_addr_t address)
{
	lapic_phys_addr = address;

	if (!x2apic_mode) {
		set_fixmap_nocache(FIX_APIC_BASE, lapic_phys_addr);
		lapic_addr = APIC_BASE;

		ulog_info("APIC mapped to %#llx (%#llx).\n", lapic_addr, lapic_phys_addr);
	}

	boot_cpu_apic_id = read_apic_id();
}

void __init apic_install_driver(struct apic *driver)
{
	if (apic != driver) {
		apic = driver;

		ulog_info("APIC driver: %s.\n", driver->name);
	}
}

void __init cpu_nrs_init(void)
{
	unsigned int assigned = nr_assigned_cpus;

	set_nr_cpu_ids(assigned);
}
