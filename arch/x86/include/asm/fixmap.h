/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_FIXMAP_H__
#define __X86_ASM_FIXMAP_H__

#include <cyanea/pgtable.h>
#include <cyanea/types.h>

#define FIX_TOP_ADDR ((unsigned long)(-1) - PAGE_SIZE + 1)

/*
 * There are total of 1024 fixed-size 4KiB mappings (in 'level1_fixmap_pgt'):
 *   (1) 512 entries in 'level1_fixmap_pgt[0]' for temporary mappings and
 *   (2) 512 entries in 'level1_fixmap_pgt[1]' for permanent entries.
 */

enum fixed_addresses {
# define TOTAL_FIX_BITMAPS PTRS_PER_PT
	FIX_TOP,
	FIX_CONSOLE_MEM_BASE,
	FIX_APIC_BASE,
	FIX_IO_APIC_BASE_0,
	FIX_IO_APIC_BASE_END = FIX_IO_APIC_BASE_0 + CONFIG_NR_IOAPIC - 1,

	__end_of_permanent_fixed_addresses = TOTAL_FIX_BITMAPS,

	FIX_BITMAP_END = __end_of_permanent_fixed_addresses,
	FIX_BITMAP_BEGIN = FIX_BITMAP_END + TOTAL_FIX_BITMAPS - 1,

	__end_of_fixed_addresses
};

#define fix_to_virt(x) (FIX_TOP_ADDR - ((x) << PAGE_SHIFT))
#define virt_to_fix(x) ((FIX_TOP_ADDR - ((x) & PAGE_MASK)) >> PAGE_SHIFT)

void set_fixmap(enum fixed_addresses idx, phys_addr_t phys_addr, pgprot_t prot);

/* set_fixmap_x. */
#define set_fixmap_nocache(idx, phys) \
	set_fixmap((idx), (phys), PAGE_KERNEL_IO_NOCACHE)

#endif /* __X86_ASM_FIXMAP_H__ */
