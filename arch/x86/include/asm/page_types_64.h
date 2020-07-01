/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PAGE_TYPES_64_H__
#define __X86_ASM_PAGE_TYPES_64_H__
#ifndef __PAGE_TYPES__
#  error use 'page_types.h' instead.
#endif

#define __PAGE_OFFSET UL(0xFFFFFF8000000000)
#define __START_KERNEL_map UL(0xFFFFFFFF80000000)

#define STACK_SIZE_ORDER 2
#define STACK_SIZE BIT_UL(PAGE_SHIFT + STACK_SIZE_ORDER)

#define __PHYSICAL_MASK_SHIFT 52

#ifndef __ASSEMBLY__

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_PAGE_TYPES_64_H__ */
