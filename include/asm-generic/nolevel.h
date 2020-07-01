/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASM_GENERIC_NOLEVEL_H__
#define __ASM_GENERIC_NOLEVEL_H__
#ifndef __PGTABLE_TYPES__
#  error use 'pgtable_types.h' instead.
#endif

#ifndef __HAS_PUD_LEVEL__

#define PUD_SHIFT PGD_SHIFT
#define PUD_SIZE BIT_UL(PUD_SHIFT)
#define PUD_MASK (~((PUD_SIZE) - 1))
#define PTRS_PER_PUD 1

typedef unsigned long pudval_t;
typedef struct {
    pgd_t pgd;
} pud_t;

#define pud_val(x) pgd_val((x).pgd)
#define __pud_t(x) ((pud_t) { __pgd_t((pgdval_t)(x)) })

#endif /* __HAS_PUD_LEVEL__ */

#ifndef __HAS_PMD_LEVEL__

#define PMD_SHIFT PUD_SHIFT
#define PMD_SIZE BIT_UL(PMD_SHIFT)
#define PMD_MASK (~((PMD_SIZE) - 1))
#define PTRS_PER_PMD 1

typedef unsigned long pmdval_t;
typedef struct {
    pud_t pud;
} pmd_t;

#define pmd_val(x) pud_val((x).pud)
#define __pmd_t(x) ((pmd_t) { __pud_t((pudval_t)(x)) })

#endif /* __HAS_PMD_LEVEL__ */

#endif /* __ASM_GENERIC_NOLEVEL_H__ */
