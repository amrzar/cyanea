/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_DESC_TYPES_H__
#define __X86_ASM_DESC_TYPES_H__

#include <cyanea/const.h>
#include <cyanea/types.h>

/* 3.2.4 Segmentation in IA-32e Mode. */

/* In 64-bit mode, segmentation is generally (but not completely) disabled,
 * creating a flat 64-bit linear-address space. The processor treats the segment
 * base of CS, DS, ES, SS as zero, creating a linear address that is equal to
 * the effective address. The FS and GS segments are exceptions. These segment
 * registers (which hold the segment base) can be used as additional base registers
 * in linear address calculations.
 */

/* 3.4.4 Segment Loading Instructions in IA-32e Mode. */

/* Because ES, DS, and SS segment registers are not used in 64-bit mode, their
 * fields (base, limit, and attribute) in segment descriptor registers are ignored.
 */

/* 5.2.1 Code-Segment Descriptor in 64-bit Mode. */

/* Code segments continue to exist in 64-bit mode even though, for address
 * calculations, the segment base is treated as zero.
 *
 * Code segment descriptors and selectors are needed in IA-32e mode to establish
 * the processor’s operating mode and execution privilege-level.
 */

#define GDT_ENTRY(attribute, base, limit) \
    ((((base) & ULL(0xFF000000)) << (56 - 24)) | \
        (((attribute) & ULL(0x0000F0FF)) << 40) | \
        (((limit) & ULL(0x000F0000)) << (48 - 16)) | \
        (((base) & ULL(0x00FFFFFF)) << 16) | \
        (((limit) & ULL(0x0000FFFF))))

#define GDT_ENTRY_BOOT32_CS 1
#define GDT_ENTRY_BOOT_CS   2
#define GDT_ENTRY_BOOT_DS   3
#define __BOOT32_CS      (GDT_ENTRY_BOOT32_CS * 8)
#define __BOOT_CS        (GDT_ENTRY_BOOT_CS * 8)
#define __BOOT_DS        (GDT_ENTRY_BOOT_DS * 8)

/* 5.8.8 Fast System Calls in 64-Bit Mode. */

/* For SYSCALL, the processor saves RFLAGS into R11 and the RIP of the next
 * instruction into RCX; it then gets the privilege-level 0 target CS,
 * RIP, SS, and RFLAGS:
 *   - Target CS - Reads a non-NULL selector from IA32_STAR[47:32].
 *   - Target RIP - Reads a 64-bit address from IA32_LSTAR.
 *   - SS - Computed by adding 8 to the value in IA32_STAR[47:32].
 *   - RFLAGS - Sets to the logical-AND of its current value with the complement
 *              of the value in the IA32_FMASK MSR.
 */

# define GDT_ENTRY_KERNEL32_CS  1
# define GDT_ENTRY_KERNEL_CS    2
# define GDT_ENTRY_KERNEL_DS    3

/* When SYSRET transfers control to 64-bit mode user code using REX.W, the
 * processor gets the privilege level 3 target CS, RIP, SS, and RFLAGS:
 *   - Target CS - Reads a non-NULL selector from IA32_STAR[63:48] + 16.
 *   - Target RIP - Copies the value in RCX into RIP.
 *   - SS - IA32_STAR[63:48] + 8.
 *   - RFLAGS - Loaded from R11.
 *
 * When SYSRET transfers control to 32-bit mode user code using a 32-bit operand
 * size, the processor gets the privilege level 3 target CS, EIP, SS, and EFLAGS:
 *   - Target CS - Reads a non-NULL selector from IA32_STAR[63:48].
 *   - Target EIP - Copies the value in ECX into EIP.
 *   - SS - IA32_STAR[63:48] + 8.
 *   - EFLAGS — Loaded from R11.
 */

# define GDT_ENTRY_DEFAULT_USER32_CS 4
# define GDT_ENTRY_DEFAULT_USER_DS   5
# define GDT_ENTRY_DEFAULT_USER_CS   6

/* System descriptors are occupying the space of two entries. */

# define GDT_ENTRY_TSS 8
# define GDT_ENTRY_LDT 10

# define GDT_ENTRIES   16

# define __KERNEL32_CS   (GDT_ENTRY_KERNEL32_CS * 8)
# define __KERNEL_CS     (GDT_ENTRY_KERNEL_CS * 8)
# define __KERNEL_DS     (GDT_ENTRY_KERNEL_DS * 8)
# define __USER32_CS     (GDT_ENTRY_DEFAULT_USER32_CS * 8 + 3)
# define __USER_DS       (GDT_ENTRY_DEFAULT_USER_DS * 8 + 3)
# define __USER_CS       (GDT_ENTRY_DEFAULT_USER_CS * 8 + 3)

#ifndef __ASSEMBLY__

#include <string.h>

union desc_struct {
    struct {
        u16 limit0;
        u16 base0;
        u16 base1: 8, type: 4, s: 1, dpl: 2, p: 1;
        u16 limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
    };

    u64 __raw_desc;
} __packed;

typedef union desc_struct desc_struct_t;

#define GDT_ENTRY_INIT(a, b, limit) \
    { .__raw_desc = GDT_ENTRY(a, b, limit) }

/* 3.5 SYSTEM DESCRIPTOR TYPES. */

/*  - System-segment descriptors (LDT and TSS segments).
 *  - Gate descriptors.
 */

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

enum desc_type {
    DESC_LDT = 0x2,             /* Local descriptor-table (LDT) segment. */
    DESC_TSS = 0x9,             /* Task-state segment (TSS) (Available). */
    DESCTYPE_S = 0x10,          /* Other descriptors (not system) */
};

/* 6.14.4 Stack Switching in IA-32e Mode. */

/* The 64-bit extensions of Intel 64 architecture implement (1) a modified version
 * of the legacy stack-switching mechanism and (2) an alternative stack-switching
 * mechanism called the interrupt stack table (IST).
 */

/* The modified version of the legacy stack-switching mechanism happens as part
 * of a 64-bit mode privilege-level change. IA-32e mode loads only an inner-level
 * RSP from the TSS. The new SS selector is forced to NULL and the SS selector’s
 * RPL field is set to the new CPL.
 */

/* 6.14.5 Interrupt Stack Table (IST). */

/* This mechanism unconditionally switches stacks when it is enabled. */

/* The IST mechanism provides up to seven IST pointers in the TSS. The pointers
 * are referenced by an interrupt-gate descriptor in the IDT. The gate descriptor
 * contains a 3-bit IST index field that provides an offset into the IST section
 * of the TSS.
 */

/* If the IST index is zero, the modified legacy stack-switching mechanism is used. */

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
    /* GATE_TASK = 0x5,            Task Gate -- only 32-bit. */
    /* GATE_CALL = 0xC,            Call Gate -- UNUSED. */
    GATE_INTERRUPT = 0xE,       /* Interrupt Gate. */
    GATE_TRAP = 0xF,            /* Trap Gate. */
};

struct dt_ptr {
    unsigned short size;

    /* 16-byte aligned (or 8-byte). */
    /* 'u32' for 32-bit runtime and 'u64' for 64-bit runtime. */
    unsigned long base_address;
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
