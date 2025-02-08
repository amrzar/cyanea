/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_DESC_H__
#define __X86_ASM_DESC_H__

#include <asm/page.h>
#include <asm/segment.h>

#include "desc_defs.h"

struct gdt_page {
	desc_struct_t gdt[GDT_ENTRIES];
} __aligned(PAGE_SIZE);

static inline void native_load_gdt(const struct dt_ptr *dtr)
{
	asm volatile("lgdt %0" : : "m" (*dtr));
}

static inline void native_load_idt(const struct dt_ptr *dtr)
{
	asm volatile("lidt %0" : : "m" (*dtr));
}

#define load_gdt(dtr) native_load_gdt(dtr)
#define load_idt(dtr) native_load_idt(dtr)

#endif /* __X86_ASM_DESC_H__ */
