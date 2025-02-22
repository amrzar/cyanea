/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_DESC_DEFS_H__
#define __X86_ASM_DESC_DEFS_H__

#include <cyanea/types.h>

/* 3.4.5 Segment Descriptors. */

/**
 * @brief Represents a descriptor structure in x86 architecture.
 *
 * This union allows access to the descriptor as a raw 64-bit value or as a
 * structured set of fields.
 */
union desc_struct {
	u64 raw_desc;          /**< The raw 64-bit descriptor value. */
	struct {
		u16 limit0;    /**< The lower 16 bits of the segment limit. */
		u16 base0;     /**< The segment base address bits 0 - 15. */
		u16 base1: 8,  /**< The Segment base address bits 16 - 23. */
		    type: 4,   /**< The type field.
                                  For system descriptors in IA-32e mode:
                                    0x2 = LDT.
                                    0x9 = 64-bit TSS (Available).
                                    0xB = 64-bit TSS (Busy).
                                    0xC = 64-bit Call Gate.
                                    0xE = 64-bit Interrupt Gate.
                                    0xF = 64-bit Trap Gate.
                                  For code/data descriptors in IA-32e mode:
                                    0xA = Execute/Read Code Segment (Accessed).
                                    0xB = Execute/Read Code Segment.
                                    0x2 = Read/Write Data Segment.
                                    0x3 = Read/Write Data Segment (Accessed).
                                */
		    s: 1,      /**< The descriptor type (0 = system, 1 = code or data). */
		    dpl: 2,    /**< The descriptor privilege level. */
		    p: 1;      /**< The present bit. */
		u16 limit1: 4, /**< The upper 4 bits of the segment limit. */
		    avl: 1,    /**< The available bit. */
		    l: 1,      /**< The 64-bit code segment (IA-32e mode only). */
		    d: 1,      /**< The default operation size (0 = 16-bit segment, 1 = 32-bit segment). */
		    g: 1,      /**< The granularity of limit field (0 = byte units, 1 = 4-KByte units). */
		    base2: 8;  /**< The Segment base address bits 24 - 31. */
	};
} __packed;
typedef union desc_struct desc_struct_t;

#define GDT_ENTRY_INIT(a, b, limit) \
	{ .raw_desc = GDT_ENTRY(a, b, limit) }

/**
 * @brief LDT/TSS descriptor structure in 64-bit mode.
 *
 * This structure defines the layout of a Local Descriptor Table (LDT) or
 * Task State Segment (TSS) descriptor in the x86 architecture.
 *
 * @see 8.2.3 TSS Descriptor in 64-bit mode.
 */
struct ldttss_desc {
	u16 limit0;    /**< The lower 16 bits of the segment limit. */
	u16 base0;     /**< The segment base address bits 0 - 15. */
	u16 base1: 8,  /**< The segment base address bits 16 - 23. */
	    type: 5,   /**< The type field (LDT or TSS). */
	    dpl: 2,    /**< The descriptor privilege level. */
	    p: 1;      /**< The present bit. */
	u16 limit1: 4, /**< The upper 4 bits of the segment limit. */
	    zero0: 3,  /**< Reserved, set to zero. */
	    g: 1,      /**< The granularity of limit field (0 = byte units, 1 = 4-KByte units). */
	    base2 : 8; /**< The segment base address bits 24 - 31. */
	u32 base3;     /**< The segment base address bits 32 - 63. */
	u32 zero1;     /**< Reserved, set to zero. */
} __packed;
typedef struct ldttss_desc ldt_desc_t;
typedef struct ldttss_desc tss_desc_t;

/* 6.11 IDT DESCRIPTORS. */

/**
 * @brief Defines the types of gates used in 64-bit mode.
 *
 * This enumeration specifies the types of gates that can be used in the 64-bit mode.
 *
 * - GATE_INTERRUPT: Represents an interrupt gate. When an interrupt gate is used,
 *   the interrupt flag (IF) is cleared, which disables further interrupts until
 *   the interrupt service routine (ISR) has finished executing. This ensures that
 *   the ISR is not interrupted by another interrupt.
 *
 * - GATE_TRAP: Represents a trap gate. When a trap gate is used, the interrupt flag
 *   (IF) is not cleared, which allows other interrupts to occur while the current
 *   ISR is executing.
 *
 * @see 6.12.1.3 Flag Usage By Exception- or Interrupt-Handler Procedure.
 * @see 6.14.1 64-Bit Mode IDT.
 */
enum gate_type {
	GATE_INTERRUPT = 0xE,
	GATE_TRAP = 0xF,
};

/*
 * 6.14.4 Stack Switching in IA-32e Mode.
 *
 * The 64-bit extensions of Intel 64 architecture implement a modified version
 * of the legacy stack-switching mechanism and an alternative stack-switching
 * mechanism called the interrupt stack table (IST).
 *
 * In IA-32e mode, the legacy stack-switch mechanism is modified. ... IA-32e
 * mode loads only an inner-level RSP from the TSS. The new SS selector is forced
 * to NULL and the SS selector RPL field is set to the new CPL.
 *
 * 6.14.5 Interrupt Stack Table.
 *
 * This mechanism unconditionally switches stacks when it is enabled.
 *
 * The IST mechanism provides up to seven IST pointers in the TSS. The pointers
 * are referenced by an interrupt-gate descriptor in the IDT. The gate
 * descriptor contains a 3-bit IST index field that provides an offset into
 * the IST section of the TSS.
 *
 * If the IST index is zero, the modified legacy stack-switching is used.
 */

struct idt_bits {
	u16 ist: 3, zero: 5, type: 5, dpl: 2, p: 1;
} __packed;

/**
 * @brief Represents a gate descriptor in 64-bit mode.
 * @see 6.14.1 64-Bit Mode IDT.
 */
struct gate_desc {
	u16 offset_low;       /**< The lower 16 bits of the handler function address. */
	u16 segment;          /**< The segment selector. */
	struct idt_bits bits; /**< @ref idt_bits. */
	u16 offset_middle;    /**< The middle 16 bits of the handler function address. */
	u32 offset_high;      /**< The higher 32 bits of the handler function address. */
	u32 reserved;         /**< Reserved, set to zero. */
} __packed;
typedef struct gate_desc gate_desc_t;

/* 3.5.1 Segment Descriptor Tables. */

struct dt_ptr {
	u16 size;
	/* 16-byte aligned (or 8-byte). */
	u64 base_address;
} __packed;

#endif /* __X86_ASM_DESC_DEFS_H__ */
