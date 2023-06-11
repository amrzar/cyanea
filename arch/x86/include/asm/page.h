/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PAGE_H__
#define __X86_ASM_PAGE_H__

#include <cyanea/types.h>
#include <cyanea/compiler.h>

#include <asm/page_types.h>

#define __phys_addr_kernel(x) (_AT(unsigned long, x) - __START_KERNEL_map)

#ifndef __ASSEMBLY__

extern phys_addr_t phys_base;

static always_inline phys_addr_t __phys_addr(unsigned long x)
{
    phys_addr_t y = __phys_addr_kernel(x);

    /* There are two mappings (1) '__START_KERNEL_map' to 'phys_base' and
     * (2) 'PAGE_OFFSET' to beginning of memory. For virtual address above
     * '__START_KERNEL_map' uses first mapping and for virtual addresses bellow
     * '__START_KERNEL_map' uses second mapping.
     *
     * Also, see comments in head_64.c.
     *
     * Use the carry flag to determine if 'x' is < '__START_KERNEL_map'.
     */

    x = y + ((x > y) ? (phys_base) : (__START_KERNEL_map - PAGE_OFFSET));

    return x;
}

#define __phys_addr_symbol(x) (__phys_addr_kernel(x) + phys_base)

#define __pa(x) __phys_addr((unsigned long) (x))
#define __pa_symbol(x) __phys_addr_symbol((unsigned long) (x))
#define __va(x) ((void *)((phys_addr_t) (x) + PAGE_OFFSET))

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_PAGE_H__ */
