/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PGTABLE_TYPES_32_H__
#define __X86_ASM_PGTABLE_TYPES_32_H__
#ifndef __PGTABLE_TYPES__
#  error use 'pgtable_types.h' instead.
#endif

#define PGD_SHIFT 22
#define PGD_SIZE BIT_UL(PGD_SHIFT)
#define PGD_MASK (~((PGD_SIZE) - 1))

#define PTRS_PER_PGD 1024
#define PTRS_PER_PT  1024

#ifndef __ASSEMBLY__

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_PGTABLE_TYPES_32_H__ */
