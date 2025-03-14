/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/acpi.h>
#include <cyanea/errno.h>
#include <cyanea/string.h>

#include <asm/apic.h>
#include <asm/cpuinfo.h>

/* ukernel/apic/apic.c. */
void __init register_apic_id(u32 apic_id, u32 acpi_id);
/* ukernel/apic/apic.c. */
void __init register_lapic_address(phys_addr_t address);
/* ukernel/apic/io_apic.c. */
int __init mp_register_ioapic(u8 id, u32 address, u32 gsi_base);

int acpi_lapic;         /* LAPIC found. */
int acpi_ioapic;        /* IOAPIC found. */

static phys_addr_t acpi_lapic_addr __initdata = APIC_DEFAULT_PHYS_BASE;

/* Local APIC Parsers. */
/* acpi_parse_lapic
 * acpi_parse_x2apic
 * acpi_parse_lapic_nmi
 * acpi_parse_x2apic_nmi
 */

static int __init acpi_parse_lapic(struct acpi_subtable_header *subtable_header)
{
	struct acpi_madt_local_apic *apic;

	apic = (struct acpi_madt_local_apic *)subtable_header;

	if (apic->id == 0xFF)
		return SUCCESS;

	if (apic->lapic_flags & ACPI_MADT_ENABLED)
		register_apic_id(apic->id, apic->uid);

	return SUCCESS;
}

static int __init acpi_parse_x2apic(struct acpi_subtable_header
        *subtable_header)
{
	struct acpi_madt_local_x2apic *x2apic;

	x2apic = (struct acpi_madt_local_x2apic *)subtable_header;

	if (x2apic->id == 0xFFFFFFFF)
		return SUCCESS;

	if (x2apic->lapic_flags & ACPI_MADT_ENABLED)
		register_apic_id(x2apic->id, x2apic->uid);

	return SUCCESS;
}

static int __init acpi_parse_lapic_nmi(struct acpi_subtable_header
        *subtable_header)
{
	return SUCCESS;
}

static int __init acpi_parse_x2apic_nmi(struct acpi_subtable_header
        *subtable_header)
{
	return SUCCESS;
}

static int __init acpi_parse_madt_lapic_entries(struct acpi_table_header
        *table_header)
{
	int count, x2count;

	/* Local APIC. */

	count = acpi_subtable_parse(table_header, sizeof(struct acpi_table_madt),
	                ACPI_MADT_TYPE_LOCAL_APIC, acpi_parse_lapic);

	x2count = acpi_subtable_parse(table_header, sizeof(struct acpi_table_madt),
	                ACPI_MADT_TYPE_LOCAL_X2APIC, acpi_parse_x2apic);

	if (!count && !x2count) {
		ulog_err("No LAPIC entries present.");

		return -ENODEV;
	} else if (count < 0 || x2count < 0) {
		ulog_err("Unable to parse LAPIC entry.");

		return -ENODEV;
	}

	count = acpi_subtable_parse(table_header, sizeof(struct acpi_table_madt),
	                ACPI_MADT_TYPE_LOCAL_APIC_NMI, acpi_parse_lapic_nmi);

	x2count = acpi_subtable_parse(table_header, sizeof(struct acpi_table_madt),
	                ACPI_MADT_TYPE_LOCAL_X2APIC_NMI, acpi_parse_x2apic_nmi);

	if (count < 0 || x2count < 0) {
		ulog_err("Unable to parse LAPIC NMI entry.");

		return -ENODEV;
	}

	return SUCCESS;
}

/* IO-APIC Parsers. */
/* acpi_parse_ioapic
 * acpi_parse_interrupt_override
 * acpi_parse_nmi_source
 */

static int __init acpi_parse_ioapic(struct acpi_subtable_header
        *subtable_header)
{
	struct acpi_madt_io_apic *ioapic;

	ioapic = (struct acpi_madt_io_apic *)subtable_header;

	mp_register_ioapic(ioapic->id, ioapic->address, ioapic->global_irq_base);

	return SUCCESS;
}

static int __init acpi_parse_interrupt_override(struct acpi_subtable_header
        *subtable_header)
{
	/* TODO ISA interrupts. */

	return SUCCESS;
}

static int __init acpi_parse_nmi_source(struct acpi_subtable_header
        *subtable_header)
{
	return SUCCESS;
}

static int __init acpi_parse_madt_ioapic_entries(struct acpi_table_header
        *table_header)
{
	int count;

	/* IO APIC. */

	count = acpi_subtable_parse(table_header, sizeof(struct acpi_table_madt),
	                ACPI_MADT_TYPE_IO_APIC, acpi_parse_ioapic);

	if (!count) {
		ulog_err("No IOAPIC entries present.");

		return -ENODEV;
	} else if (count < 0) {
		ulog_err("Unable to parse IO APIC entry.");

		return -ENODEV;
	}

	count = acpi_subtable_parse(table_header, sizeof(struct acpi_table_madt),
	                ACPI_MADT_TYPE_INTERRUPT_OVERRIDE, acpi_parse_interrupt_override);

	if (count < 0) {
		ulog_err("Unable to parse interrupt source overrides entry.");

		return -ENODEV;
	}

	count = acpi_subtable_parse(table_header, sizeof(struct acpi_table_madt),
	                ACPI_MADT_TYPE_NMI_SOURCE, acpi_parse_nmi_source);

	if (count < 0) {
		ulog_err("Unable to parse NMI source entry.");

		return -ENODEV;
	}

	return SUCCESS;
}

/* Parse MADT. */

static int __init acpi_parse_lapic_address_override(struct acpi_subtable_header
        *subtable_header)
{
	struct acpi_madt_local_apic_override *acpi_lapic_addr_ovr;

	acpi_lapic_addr_ovr = (struct acpi_madt_local_apic_override *)subtable_header;
	acpi_lapic_addr = acpi_lapic_addr_ovr->address;

	ulog_info("ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE (address[0x%llx]).\n",
	        acpi_lapic_addr);

	return SUCCESS;
}

static int __init acpi_parse_madt(struct acpi_table_header *table_header)
{
	int err;
	struct acpi_table_madt *madt = (struct acpi_table_madt *)table_header;

	if (madt->address)
		acpi_lapic_addr = madt->address;

	acpi_subtable_parse(table_header, sizeof(struct acpi_table_madt),
	        ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE, acpi_parse_lapic_address_override);

	register_lapic_address(acpi_lapic_addr);

	/* Parse MADT LAPIC entries. */
	err = acpi_parse_madt_lapic_entries(table_header);
	if (!err) {
		acpi_lapic = 1;

		/* Parse MADT IO-APIC entries. */
		err = acpi_parse_madt_ioapic_entries(table_header);
		if (!err) {
			acpi_ioapic = 1;
		}
	}

	return SUCCESS;
}

int __init acpi_boot_init(void)
{
	int err;

	err = acpi_table_parse(ACPI_SIG_MADT, acpi_parse_madt);
	if (err) {
		/* ACPI found no MADT. */
	}

	return SUCCESS;
}

void __init acpi_boot_table_init(void)
{
	assert(!acpi_table_init(), "ACPI: failed.");
}
