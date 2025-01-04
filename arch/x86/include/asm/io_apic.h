/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_IO_APIC_H__
#define __X86_ASM_IO_APIC_H__

#include <cyanea/types.h>

/* 82093AA I/O ADVANCED PROGRAMMABLE INTERRUPT CONTROLLER (IOAPIC). */

/* 3.2.1 IOAPICID - IOAPIC IDENTIFICATION REGISTER. */
union io_apic_id {
# define IOAPICID_INDEX  0
	u32 raw;
	struct {
		u32 reserved_1 : 24,
		    ID : 4,
		    reserved_2 : 4;
	} bits;
};

/* 3.2.2 IOAPICVER - IOAPIC VERSION REGISTER. */
union io_apic_ver {
# define IOAPICVER_INDEX 1
	u32 raw;
	struct {
		u32 version : 8,
		    reserved_1 : 8,
		    entries : 8,
		    reserved_2 : 8;
	} bits;
};

/* 3.2.3 IOAPICARB - IOAPIC ARBITRATION REGISTER. */
union ioa_apic_abr {
# define IOAPICARB_INDEX 2
	u32 raw;
	struct {
		u32 reserved_1 : 24,
		    arbitration : 4,
		    reserved_2 : 4;
	} bits;
};

/* 3.2.4 IOREDTBL - I/O REDIRECTION TABLE REGISTERS. */
union io_apic_route_entry {
	struct {
		u64 w1 : 32,
		    w2 : 32;
	};
	struct {
		u64 vector : 8,
		    delivery_mode : 3,
		    dest_mode_logical : 1,
		    delivery_status : 1,
		    active_low : 1,
		    irr : 1,
		    is_level : 1,
		    masked : 1,
		    reserved_1 : 15,
		    reserved_2 : 24,
		    destid : 8;
	};
};

#endif /* __X86_ASM_IO_APIC_H__ */
