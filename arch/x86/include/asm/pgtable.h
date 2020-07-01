/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PGTABLE_H__
#define __X86_ASM_PGTABLE_H__

#include <asm/pgtable_types.h>

#ifndef __ASSEMBLY__
#ifdef CONFIG_X86_32

/* Pages in the low-mem region, i.e. 'PAGE_OFFSET' to end of memory. **/

#define LOWMEM_PAGES ((((unsigned long)(-1) - PAGE_OFFSET) + 1) >> PAGE_SHIFT)

extern pgd_t initial_page_table[PTRS_PER_PGD] __section(".bss..page_aligned");
extern pgd_t swapper_pg_dir[PTRS_PER_PGD] __section(".bss..page_aligned");

#else

extern pgd_t init_top_pgt[PTRS_PER_PGD] __section(".bss..page_aligned");
extern pud_t level3_kernel_pgt[PTRS_PER_PUD] __section(".bss..page_aligned");
extern pmd_t level2_kernel_pgt[PTRS_PER_PMD] __section(".bss..page_aligned");

#endif /* CONFIG_X86_32 */
#endif /* __ASSEMBLY__ */

#define KERNEL_PGD_BOUNDARY pgd_index(PAGE_OFFSET)
#define KERNEL_PGD_PTRS (PTRS_PER_PGD - KERNEL_PGD_BOUNDARY)

#endif /* __X86_ASM_PGTABLE_H__ */
