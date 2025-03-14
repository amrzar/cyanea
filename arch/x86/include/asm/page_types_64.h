/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PAGE_TYPES_64_H__
#define __X86_ASM_PAGE_TYPES_64_H__

#ifndef __X86_ASM_PAGE_TYPES_H__
#  error "Use 'page_types.h'"
#endif

#define __PAGE_OFFSET UL(0xFFFFFF8000000000)
#define __START_KERNEL_map UL(0xFFFFFFFF80000000)

#define STACK_SIZE_ORDER 2
#define STACK_SIZE BIT_UL(PAGE_SHIFT + STACK_SIZE_ORDER)

/* 4.1.1 Four Paging Modes, Table Table 4-1. */
/* The real physical address is always bounded by 'MAXPHYADDR' (it is at most 52)
 * (CPUID.80000008H:EAX[7:0]). Any bit between 'MAXPHYADDR' to 52 is set to zero
 * in paging data structures.
 */

/* Maximum supported physical address width. */
#define __PHYSICAL_MASK_SHIFT 52

#endif /* __X86_ASM_PAGE_TYPES_64_H__ */
