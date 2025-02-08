/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_SEGMENT_H__
#define __X86_ASM_SEGMENT_H__

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
 * calculations, the segment base is treated as zero. Some code-segment (CS)
 * descriptor content (the base address and limit fields) is ignored; the remaining
 * fields function normally (except for the readable bit in the type field).
 *
 * Code segment descriptors and selectors are needed in IA-32e mode to establish
 * the processor’s operating mode and execution privilege-level.
 */

#define GDT_ENTRY(attribute, base, limit)             \
	((((base) & ULL(0xFF000000)) << (56 - 24)) |  \
	        (((attribute) & ULL(0x0000F0FF)) << 40) |    \
	        (((limit) & ULL(0x000F0000)) << (48 - 16)) | \
	        (((base) & ULL(0x00FFFFFF)) << 16) | (((limit) & ULL(0x0000FFFF))))

#define GDT_ENTRY_BOOT32_CS 1
#define GDT_ENTRY_BOOT_CS 2
#define GDT_ENTRY_BOOT_DS 3
#define __BOOT32_CS     (GDT_ENTRY_BOOT32_CS * 8)
#define __BOOT_CS       (GDT_ENTRY_BOOT_CS * 8)
#define __BOOT_DS       (GDT_ENTRY_BOOT_DS * 8)

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

#define GDT_ENTRY_KERNEL32_CS 1
#define GDT_ENTRY_KERNEL_CS 2
#define GDT_ENTRY_KERNEL_DS 3

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

#define GDT_ENTRY_DEFAULT_USER32_CS 4
#define GDT_ENTRY_DEFAULT_USER_DS 5
#define GDT_ENTRY_DEFAULT_USER_CS 6

/* System descriptors are occupying the space of two entries. */

#define GDT_ENTRY_TSS 8
#define GDT_ENTRY_LDT 10

#define GDT_ENTRIES 12

#define __KERNEL32_CS   (GDT_ENTRY_KERNEL32_CS * 8)
#define __KERNEL_CS     (GDT_ENTRY_KERNEL_CS * 8)
#define __KERNEL_DS     (GDT_ENTRY_KERNEL_DS * 8)
#define __USER32_CS     (GDT_ENTRY_DEFAULT_USER32_CS * 8 + 3)
#define __USER_DS       (GDT_ENTRY_DEFAULT_USER_DS * 8 + 3)
#define __USER_CS       (GDT_ENTRY_DEFAULT_USER_CS * 8 + 3)

#define GDT_SIZE (GDT_ENTRIES * 8)

#endif /* __X86_ASM_SEGMENT_H__ */
