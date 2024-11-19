/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>

#include <asm/apic.h>

/* 11.4.6 Local APIC ID. */
static u32 physical_flat_get_apic_id(void)
{
	return (apic_read(APIC_ID) >> 24) & 0xFF;
}

static struct apic apic_phys = {
	.name = "physical flat",
	.read = native_apic_mem_read,
	.write = native_apic_mem_write,
	.get_apic_id = physical_flat_get_apic_id,
};

apic_driver(apic_phys);

struct apic *apic __ro_after_init = &apic_phys;
