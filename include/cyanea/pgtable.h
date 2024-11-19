/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_PGTABLE_H__
#define __CYANEA_PGTABLE_H__

#include <asm/pgtable.h>

#define pgd_index(a) (((unsigned long)(a) >> PGD_SHIFT) & (PTRS_PER_PGD - 1))
#define pud_index(a) (((unsigned long)(a) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define pmd_index(a) (((unsigned long)(a) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define pte_index(a) (((unsigned long)(a) >> PAGE_SHIFT) & (PTRS_PER_PT - 1))

#endif /* __CYANEA_PGTABLE_H__ */
