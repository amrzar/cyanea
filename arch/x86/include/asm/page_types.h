/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PAGE_TYPES_H__
#define __X86_ASM_PAGE_TYPES_H__
#define __PAGE_TYPES__

#include <cyanea/const.h>

#define PAGE_SHIFT 12
#define PAGE_SIZE BIT_UL(PAGE_SHIFT)
#define PAGE_MASK (~((PAGE_SIZE) - 1))

#ifdef CONFIG_X86_32
#  include "page_types_32.h"
#else
#  include "page_types_64.h"
#endif

#define __PHYSICAL_START CONFIG_PHYSICAL_START
#define __START_KERNEL (__START_KERNEL_map + __PHYSICAL_START)

#ifndef __ASSEMBLY__
#define __PHYSICAL_MASK ((unsigned long)(BIT_ULL(__PHYSICAL_MASK_SHIFT) - 1))

#define PAGE_OFFSET ((unsigned long)(__PAGE_OFFSET))

#define PHYSICAL_PAGE_MASK (__PHYSICAL_MASK & PAGE_MASK)
#define PHYSICAL_PMD_PAGE_MASK (__PHYSICAL_MASK & PMD_MASK)
#define PHYSICAL_PUD_PAGE_MASK (__PHYSICAL_MASK & PUD_MASK)
#endif /* __ASSEMBLY__ */

#  undef  __PAGE_TYPES__
#endif /* __X86_ASM_PAGE_TYPES_H__ */
