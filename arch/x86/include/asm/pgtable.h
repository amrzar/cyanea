/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PGTABLE_H__
#define __X86_ASM_PGTABLE_H__

#include <asm/pgtable_types.h>

#ifndef __ASSEMBLY__

extern pgd_t init_top_pgt[PTRS_PER_PGD] __section(".bss..page_aligned");
extern pud_t level3_kernel_pgt[PTRS_PER_PUD] __section(".bss..page_aligned");
extern pmd_t level2_kernel_pgt[PTRS_PER_PMD] __section(".bss..page_aligned");

#endif /* __ASSEMBLY__ */

#define KERNEL_PGD_BOUNDARY pgd_index(PAGE_OFFSET)
#define KERNEL_PGD_PTRS (PTRS_PER_PGD - KERNEL_PGD_BOUNDARY)

#endif /* __X86_ASM_PGTABLE_H__ */
