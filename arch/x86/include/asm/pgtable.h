/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PGTABLE_H__
#define __X86_ASM_PGTABLE_H__

#ifndef __CYANEA_PGTABLE_H__
#  error "Use 'cyanea/pgtable.h'"
#endif

#include <asm/pgtable_types.h>

#ifndef __ASSEMBLY__

extern pgprotval_t __supported_pte_mask;

extern pgd_t init_top_pgt[PTRS_PER_PGD] __section(".bss..page_aligned");
extern pud_t level3_kernel_pgt[PTRS_PER_PUD] __section(".bss..page_aligned");
extern pmd_t level2_kernel_pgt[PTRS_PER_PMD] __section(".bss..page_aligned");
extern pmd_t level2_fixmap_pgt[PTRS_PER_PMD] __section(".bss..page_aligned");
extern pte_t level1_fixmap_pgt[2][PTRS_PER_PT] __section(".bss..page_aligned");

static inline void pte_set(pte_t *ptep, pte_t pte)
{
	*ptep = pte;
}

static inline void pte_clear(pte_t *ptep)
{
	pte_set(ptep, __pte_t(0));
}

static inline void pmd_set(pmd_t *pmdp, pmd_t pmd)
{
	*pmdp = pmd;
}

static inline void pmd_clear(pmd_t *pmdp)
{
	pmd_set(pmdp, __pmd_t(0));
}

static inline void pud_set(pud_t *pudp, pud_t pud)
{
	*pudp = pud;
}

static inline void pud_clear(pud_t *pudp)
{
	pud_set(pudp, __pud_t(0));
}

static inline void pgd_set(pgd_t *pgdp, pgd_t pgd)
{
	*pgdp = pgd;
}

static inline void pgd_clear(pgd_t *pgdp)
{
	pgd_set(pgdp, __pgd_t(0));
}

static inline pgprotval_t check_pgprot(pgprot_t pgprot)
{
	pgprotval_t protval = pgprot_val(pgprot);

	if (protval & _PAGE_PRESENT)
		protval &= __supported_pte_mask;

	return protval;
}

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_PGTABLE_H__ */
