/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/types.h>
#include <asm/fixmap.h>
#include <asm/io.h>

/* 82093AA I/O ADVANCED PROGRAMMABLE INTERRUPT CONTROLLER (IOAPIC). */

#define NR_IOAPIC CONFIG_NR_IOAPIC

static struct {
	phys_addr_t phys_addr;
	u32 gsi_base;
	u32 gsi_end;
} ioapics[NR_IOAPIC];

/* 3.0 REGISTER DESCRIPTION. */
/* The IOREGSEL and IOWIN Registers ... are aligned on 128 bit boundaries.
 * All APIC registers are accessed using 32-bit loads and stores.
 */
struct io_apic {
	u32 index;      /* I/O Register Select (IOREGSEL). */
	u32 __pad[3];
	u32 data;       /* I/O Window (IOWIN). */
};

static struct io_apic *io_apic_base(int apic)
{
	return (struct io_apic *)(fix_to_virt(FIX_IO_APIC_BASE_0 + apic) +
	                (ioapics[apic].phys_addr & ~PAGE_MASK));
}

static u32 io_apic_read(int apic, unsigned int reg)
{
	struct io_apic *io_apic = io_apic_base(apic);

	writel(reg, &io_apic->index);
	return readl(&io_apic->data);
}

static void io_apic_write(int apic, unsigned int reg, u32 value)
{
	struct io_apic *io_apic = io_apic_base(apic);

	writel(reg, &io_apic->index);
	writel(value, &io_apic->data);
}









u32 __initdata io_apic_address[NR_IOAPIC];
u32 __initdata io_apic_gsi_base[NR_IOAPIC];

static int __initdata assigned_io_apics;
void __init register_ioapic(int id, u32 address, u32 gsi_base)
{
	int io_apic;

	io_apic = assigned_io_apics++;
	if (io_apic < NR_IOAPIC) {
		io_apic_address[io_apic] = address;
		io_apic_gsi_base[io_apic] = gsi_base;

		ulog_info("IO-APIC (%u) address (0x%x) gsi_base (0x%x).\n",
		        id, address, gsi_base);

	} else /* 'CONFIG_NR_IOAPIC'. */
		ulog_err("IO-APIC (%u) dropped.", id);
}

void __init io_apic_init_mappings(void)
{
	// int i, idx = FIX_IO_APIC_BASE_0;
	// phys_addr_t ioapic_phys;

	// for (i = 0; i < nr_ioapics; i++) {

	// }
}
