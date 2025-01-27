/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_DESC_TYPES_H__
#define __X86_ASM_DESC_TYPES_H__

#ifndef __ASSEMBLY__

#include <cyanea/types.h>

/* 3.4.5 Segment Descriptors. */

union desc_struct {
	struct {
		u16 limit0;
		u16 base0;
		u16 base1: 8, type: 4, s: 1, dpl: 2, p: 1;
		u16 limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
	};

	u64 raw_desc;
} __packed;

typedef union desc_struct desc_struct_t;

#define GDT_ENTRY_INIT(a, b, limit) \
	{ .raw_desc = GDT_ENTRY(a, b, limit) }

/* 8.2.3 TSS Descriptor in 64-bit mode. */

struct ldttss_desc {
	u16 limit0;
	u16 base0;
	u16 base1: 8, type: 5, dpl: 2, p: 1;
	u16 limit1: 4, zero0: 3, g: 1, base2: 8;
	u32 base3;
	u32 zero1;
} __packed;

typedef struct ldttss_desc ldt_desc_t;
typedef struct ldttss_desc tss_desc_t;

/* 6.14.4 Stack Switching in IA-32e Mode. */

/*
 * The 64-bit extensions of Intel 64 architecture implement (1) a modified
 * version of the legacy stack-switching mechanism and (2) an alternative
 * stack-switching mechanism called the interrupt stack table (IST).
 */

/*
 * The modified version of the legacy stack-switching mechanism happens as part
 * of a 64-bit mode privilege-level change. IA-32e mode loads only an inner-level
 * RSP from the TSS. The new SS selector is forced to NULL and the SS selector
 * RPL field is set to the new CPL.
 */

/* 6.14.5 Interrupt Stack Table (IST). */

/* This mechanism unconditionally switches stacks when it is enabled. */

/*
 * The IST mechanism provides up to seven IST pointers in the TSS. The pointers
 * are referenced by an interrupt-gate descriptor in the IDT. The gate
 * descriptor contains a 3-bit IST index field that provides an offset into
 * the IST section of the TSS.
 */

/* If the IST index is zero, the modified legacy stack-switching is used. */

/* 6.11 IDT DESCRIPTORS. */

struct idt_bits {
	u16 ist: 3, zero: 5, type: 5, dpl: 2, p: 1;
} __packed;

struct gate_desc {
	u16 offset_low;
	u16 segment;
	/* u16. */ struct idt_bits bits;
	u16 offset_middle;
	u32 offset_high;
	u32 reserved;
} __packed;

typedef struct gate_desc gate_desc_t;

enum gate_type {
	/* GATE_TASK = 0x5,            Task Gate. */
	GATE_INTERRUPT = 0xE,       /* Interrupt Gate. */
	GATE_TRAP = 0xF,            /* Trap Gate. */
};

/* 3.5.1 Segment Descriptor Tables. */

struct dt_ptr {
	u16 size;
	/* 16-byte aligned (or 8-byte). */
	u64 base_address;
} __packed;

static inline void load_gdt(const struct dt_ptr *dtr)
{
	asm volatile("lgdt %0" : : "m" (*dtr));
}

static inline void load_idt(const struct dt_ptr *dtr)
{
	asm volatile("lidt %0" : : "m" (*dtr));
}

#endif /* __ASSEMBLY__ */
#endif /* __X86_ASM_DESC_TYPES_H__ */
