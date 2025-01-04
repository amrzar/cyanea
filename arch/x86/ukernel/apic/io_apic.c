/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/errno.h>
#include <cyanea/init.h>
#include <cyanea/types.h>
#include <asm/fixmap.h>
#include <asm/io.h>
#include <asm/io_apic.h>

/* 82093AA I/O ADVANCED PROGRAMMABLE INTERRUPT CONTROLLER (IOAPIC). */

#define NR_IOAPIC CONFIG_NR_IOAPIC

static int nr_ioapics;
static struct {
	int nr_registers;               /* Number of IRQ routing registers. */
	u8 id;
	phys_addr_t phys_addr;
	u32 gsi_base;
	u32 gsi_end;
} ioapics[NR_IOAPIC];

/* 3.0 REGISTER DESCRIPTION. */
struct io_apic {
	u8 index;       /* I/O Register Select (IOREGSEL). */
	u8 __pad[15];
	u32 data;       /* I/O Window (IOWIN). */
} __packed;

static struct io_apic *io_apic_base(int ioapic)
{
	/* Base should be 0xFEC0xy00. */
	unsigned long y = ioapics[ioapic].phys_addr & ~PAGE_MASK;

	return (struct io_apic *)(fix_to_virt(FIX_IO_APIC_BASE_0 + ioapic) + y);
}

/* IOAPIC Registers. */
/* 00h       'IOAPIC ID' (R/W). */
/* 01h       'IOAPIC Version' (RO). */
/* 02h       'IOAPIC Arbitration ID' (RO). */
/* 10h - 3Fh 'Redirection Table (Entries 0-23) (64 bits each)' (R/W). */

static u32 io_apic_read(int ioapic, u8 reg)
{
	struct io_apic *base = io_apic_base(ioapic);

	writeb(reg, &base->index);

	return readl(&base->data);
}

// static void io_apic_write(int ioapic, u8 reg, u32 value)
// {
//      struct io_apic *base = io_apic_base(ioapic);

//      writeb(reg, &base->index);

//      writel(value, &base->data);
// }

// static union io_apic_route_entry ioapic_read_entry(int ioapic, int pin)
// {
//      union io_apic_route_entry entry;

//      entry.w1 = io_apic_read(ioapic, 0x10 + 2 * pin);
//      entry.w2 = io_apic_read(ioapic, 0x11 + 2 * pin);

//      return entry;
// }

// static void ioapic_write_entry(int ioapic, int pin, union io_apic_route_entry entry)
// {
//      /*
//       * Write the high word first!
//       * If the mask bit in the low word is clear, we will enable the interrupt,
//       * and we need to make sure the entry is fully populated
//       * before that happens.
//       */

//      io_apic_write(ioapic, 0x11 + 2 * pin, entry.w2);
//      io_apic_write(ioapic, 0x10 + 2 * pin, entry.w1);
// }

static int io_apic_get_redir_entries(int ioapic)
{
	union io_apic_ver reg;

	reg.raw = io_apic_read(ioapic, IOAPICVER_INDEX);

	return reg.bits.entries + 1;
}

/* Does 'io_apic_init_mappings'. */
int __init mp_register_ioapic(u8 id, u32 address, u32 gsi_base)
{
	int entries;

	if (nr_ioapics == NR_IOAPIC) {
		ulog_err("IO-APIC (%u) dropped.", id);

		return -ENOSPC;
	}

	set_fixmap_nocache(FIX_IO_APIC_BASE_0 + nr_ioapics, address);

	entries = io_apic_get_redir_entries(nr_ioapics);

	/* TODO. Is IOAPIC ID is same ad id?! */
	ioapics[nr_ioapics].id = id;

	ioapics[nr_ioapics].phys_addr = address;
	ioapics[nr_ioapics].gsi_base = gsi_base;
	ioapics[nr_ioapics].gsi_end = gsi_base + entries - 1;
	ioapics[nr_ioapics].nr_registers = entries;

	ulog_info("IO-APIC (%u) address (0x%x) GSI (%d - %d).\n", id, address,
	        ioapics[nr_ioapics].gsi_base, ioapics[nr_ioapics].gsi_end);

	nr_ioapics++;

	return SUCCESS;
}
