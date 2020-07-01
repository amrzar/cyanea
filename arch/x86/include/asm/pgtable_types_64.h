/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PGTABLE_TYPES_64_H__
#define __X86_ASM_PGTABLE_TYPES_64_H__
#ifndef __PGTABLE_TYPES__
#  error use 'pgtable_types.h' instead.
#endif

#define PGD_SHIFT 39
#define PGD_SIZE BIT_UL(PGD_SHIFT)
#define PGD_MASK (~((PGD_SIZE) - 1))

#define PUD_SHIFT 30
#define PUD_SIZE BIT_UL(PUD_SHIFT)
#define PUD_MASK (~((PUD_SIZE) - 1))

#define __HAS_PUD_LEVEL__

#define PMD_SHIFT 21
#define PMD_SIZE BIT_UL(PMD_SHIFT)
#define PMD_MASK (~((PMD_SIZE) - 1))

#define __HAS_PMD_LEVEL__

#define PTRS_PER_PGD 512
#define PTRS_PER_PUD 512
#define PTRS_PER_PMD 512
#define PTRS_PER_PT  512

#ifndef __ASSEMBLY__

typedef unsigned long pudval_t;
typedef struct {
    pudval_t pud;
} pud_t;

#define pud_val(x) ((x).pud)
#define __pud_t(x) ((pud_t) { (x) })

typedef unsigned long pmdval_t;
typedef struct {
    pmdval_t pmd;
} pmd_t;

#define pmd_val(x) ((x).pmd)
#define __pmd_t(x) ((pmd_t) { (x) })

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_PGTABLE_TYPES_64_H__ */
