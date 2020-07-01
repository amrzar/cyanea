/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_DESC_H__
#define __X86_ASM_DESC_H__

#include <asm/segment.h>
#include <asm/desc_defs.h>
#include <asm/page_types.h>

extern desc_struct_t gdt[GDT_ENTRIES] __aligned(PAGE_SIZE);

#endif /* __X86_ASM_DESC_H__ */
