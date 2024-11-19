/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>

#include <asm/apic.h>

/* 11.4.6 Local APIC ID.*/
static u32 physical_x2apic_get_apic_id(void)
{
	return apic_read(APIC_ID);
}

static struct apic apic_phys = {
	.name = "physical x2apic",
	.read = native_apic_msr_read,
	.write = native_apic_msr_write,
	.get_apic_id = physical_x2apic_get_apic_id,
};

apic_driver(apic_phys);
