/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_TLBFLUSH_H__
#define __X86_ASM_TLBFLUSH_H__

#include <asm/system.h>
#include <asm/cpu_registers.h>

/* ''Intel 64 and IA-32 Architectures Software Developer's Manual''
 * See: 4.10.4 Invalidation of TLBs and Paging-Structure Caches. **/

static inline void __native_tlb_flush_global(unsigned long cr4)
{
    write_cr4(cr4 ^ _CR4_PGE);
    write_cr4(cr4);
}

#endif /* __X86_ASM_TLBFLUSH_H__ */
