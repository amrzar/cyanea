/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PGTABLE_TYPES_H__
#define __X86_ASM_PGTABLE_TYPES_H__
#define __PGTABLE_TYPES__

#include <asm/page_types.h>

#define _PAGE_BIT_PRESENT   0   /* Present. */
#define _PAGE_BIT_RW        1   /* Writeable. */
#define _PAGE_BIT_USER      2   /* User addressable. */
#define _PAGE_BIT_PWT       3   /* Page write through. */
#define _PAGE_BIT_PCD       4   /* Page cache disabled. */
#define _PAGE_BIT_ACCESSED  5   /* Accessed. */
#define _PAGE_BIT_DIRTY     6   /* Dirty. */
#define _PAGE_BIT_PSE       7   /* Large page. */
#define _PAGE_BIT_PAT       7   /* PAT on 4KB page. */
#define _PAGE_BIT_GLOBAL    8   /* Global. */
#define _PAGE_BIT_PAT_LARGE 12  /* PAT on large page. */
#define _PAGE_BIT_XD        63  /* Execute disable. */

#define _PAGE_PRESENT   (_AT(pteval_t, 1) << _PAGE_BIT_PRESENT)
#define _PAGE_RW        (_AT(pteval_t, 1) << _PAGE_BIT_RW)
#define _PAGE_USER      (_AT(pteval_t, 1) << _PAGE_BIT_USER)
#define _PAGE_PWT       (_AT(pteval_t, 1) << _PAGE_BIT_PWT)
#define _PAGE_PCD       (_AT(pteval_t, 1) << _PAGE_BIT_PCD)
#define _PAGE_ACCESSED  (_AT(pteval_t, 1) << _PAGE_BIT_ACCESSED)
#define _PAGE_DIRTY     (_AT(pteval_t, 1) << _PAGE_BIT_DIRTY)
#define _PAGE_PSE       (_AT(pteval_t, 1) << _PAGE_BIT_PSE)
#define _PAGE_PAT       (_AT(pteval_t, 1) << _PAGE_BIT_PAT)
#define _PAGE_GLOBAL    (_AT(pteval_t, 1) << _PAGE_BIT_GLOBAL)
#define _PAGE_PAT_LARGE (_AT(pteval_t, 1) << _PAGE_BIT_PAT_LARGE)
#define _PAGE_XD        (_AT(pteval_t, 1) << _PAGE_BIT_XD)

#define __PP _PAGE_PRESENT
#define __RW _PAGE_RW
#define _USR _PAGE_USER
#define ___A _PAGE_ACCESSED
#define ___D _PAGE_DIRTY
#define ___G _PAGE_GLOBAL
#define __XD _PAGE_XD
#define _PSE _PAGE_PSE

#define _KERNPG_TABLE            (__PP|__RW|   0|___A|   0|___D|   0|   0)

#define __PAGE_KERNEL            (__PP|__RW|   0|___A|__XD|___D|   0|___G)
#define __PAGE_KERNEL_EXEC       (__PP|__RW|   0|___A|   0|___D|   0|___G)
#define __PAGE_KERNEL_LARGE      (__PP|__RW|   0|___A|__XD|___D|_PSE|___G)
#define __PAGE_KERNEL_LARGE_EXEC (__PP|__RW|   0|___A|   0|___D|_PSE|___G)

#ifdef CONFIG_X86_32
#  include "pgtable_types_32.h"
#else
#  include "pgtable_types_64.h"
#endif

#ifndef __ASSEMBLY__

#define __PG_PFN_MASK (PHYSICAL_PAGE_MASK)      /* PFN of page-table structure page. */
#define __PG_FLAGS_MASK (~__PG_PFN_MASK)        /* Flags of page-table structure entry. */

typedef unsigned long pgdval_t;
typedef struct {
    pgdval_t pgd;
} pgd_t;

#define pgd_val(x) ((x).pgd)
#define __pgd_t(x) ((pgd_t) { (x) })
#define pgd_flags(x) (pgd_val(x) & __PG_FLAGS_MASK)

typedef unsigned long pteval_t;
typedef struct {
    pteval_t pte;
} pte_t;

#define pte_val(x) ((x).pte)
#define __pte_t(x) ((pte_t) { (x) })
#define pte_flags(x) (pte_val(x) & __PG_FLAGS_MASK)

typedef unsigned long pgprotval_t;
typedef struct pgprot {
    pgprotval_t pgprot;
} pgprot_t;

#define pgprot_val(x) ((x).pgprot)
#define __pgprot_t(x) ((pgprot_t) { (x) })

#include <asm-generic/nolevel.h>

static inline pudval_t pud_pfn_mask(pud_t pud)
{
    return (pud_val(pud) & _PAGE_PSE) ? PHYSICAL_PUD_PAGE_MASK : __PG_PFN_MASK;
}

static inline pudval_t pud_flags(pud_t pud)
{
    return pud_val(pud) & ~pud_pfn_mask(pud);
}

static inline pmdval_t pmd_pfn_mask(pmd_t pmd)
{
    return (pmd_val(pmd) & _PAGE_PSE) ? PHYSICAL_PMD_PAGE_MASK : __PG_PFN_MASK;
}

static inline pmdval_t pmd_flags(pmd_t pmd)
{
    return pmd_val(pmd) & ~pmd_pfn_mask(pmd);
}

#define PAGE_KERNEL             __pgprot_t(__PAGE_KERNEL)
#define PAGE_KERNEL_EXEC        __pgprot_t(__PAGE_KERNEL_EXEC)
#define PAGE_KERNEL_LARGE       __pgprot_t(__PAGE_KERNEL_LARGE)
#define PAGE_KERNEL_LARGE_EXEC  __pgprot_t(__PAGE_KERNEL_LARGE_EXEC)

#endif /* __ASSEMBLY__ */

#  undef  __PGTABLE_TYPES__
#endif /* __X86_ASM_PGTABLE_TYPES_H__ */
