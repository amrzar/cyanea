/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_TLB_H__
#define __X86_ASM_TLB_H__

#include <cyanea/irqflags.h>
#include <asm/cpu.h>
#include <asm/cpuinfo.h>

/* 4.10.4 Invalidation of TLBs and Paging-Structure Caches. */

/* We use single address space for each task, i.e. single ASID per-task and
 * kernel mapping uses global pages.
 */

/* For implementation with Page Table Isolation (PTI), there are two ASID per task,
 * (1) userspace mapping with minimum kernel mapping, and (2) kernel mapping
 * with all kernel mapping and all userspace (with NX) bit.
 */

enum invpcid_type {
	INVPCID_TYPE_ADDRESS,
	INVPCID_TYPE_ONE_CONTEXT,
	INVPCID_TYPE_ALL_CONTEXT,
	INVPCID_TYPE_ALL_CONTEXT_NO_GLOBAL,
};

static inline void invpcid(unsigned long pcid, unsigned long addr,
        enum invpcid_type type)
{
	struct {
		u64 d[2];
	} desc = { {pcid, addr} };

	asm volatile("invpcid %[desc], %[type]"
	        : : [desc] "m" (desc), [type] "r" ((unsigned long)(type))
	        : "memory");
}

/* Invalidates mappings - except global translations - associated with 'pcid' and 'addr'. */
static inline void invpcid_flush_one(unsigned long pcid, unsigned long addr)
{
	invpcid(pcid, addr, INVPCID_TYPE_ADDRESS);
}

/* Invalidates mappings - except global translations - associated with 'pcid'. */
static inline void invpcid_flush_single_context(unsigned long pcid)
{
	invpcid(pcid, 0, INVPCID_TYPE_ONE_CONTEXT);
}

/* Invalidates mappings - including global translations - associated with all PCIDs. */
static inline void invpcid_flush_all(void)
{
	invpcid(0, 0, INVPCID_TYPE_ALL_CONTEXT);
}

/* Invalidates mappings - except global translations - associated with all PCIDs. */
static inline void invpcid_flush_all_nonglobals(void)
{
	invpcid(0, 0, INVPCID_TYPE_ALL_CONTEXT_NO_GLOBAL);
}

/* Flush one page. */
static inline void __flush_tlb_one(unsigned long addr)
{
	/* It invalidates any TLB entries that are for a page number corresponding
	 * to the linear address and that are associated with the current PCID.
	 * It also invalidates any global TLB entries with that page number,
	 * regardless of PCID. It invalidates ''all entries'' in all paging-structure
	 * caches associated with the current PCID.
	 */

	/* Without PTI, it always flush the kernel mapping. */

	asm volatile("invlpg (%0)" : : "r"(addr) : "memory");
}

/* Flush everything. */
static inline void __flush_tlb_global(void)
{
	unsigned long flags, cr4;

	if (this_cpu_has(_CPUID_BIT_INVPCID)) {
		invpcid_flush_all();
		return;
	}

	/* 4.10.4.1 Operations that Invalidate TLBs and Paging-Structure Caches. */
	/* MOV to CR4. */
	/* The instruction invalidates all TLB entries (including global entries) and all
	 * entries in all paging-structure caches (for all PCIDs) if
	 *   (1) it changes the value of CR4.PGE; or
	 *   (2) it changes the value of the CR4.PCIDE.
	 */

	local_irq_save(flags);
	cr4 = read_cr4();
	__write_cr4(cr4 ^ _CR4_PGE);
	__write_cr4(cr4);
	local_irq_restore(flags);
}

/* Flush the entire current mapping. */
static inline void __flush_tlb_local(void)
{
	unsigned long flags;

	/* 4.10.4.1 Operations that Invalidate TLBs and Paging-Structure Caches. */
	/* MOV to CR3. */
	/* If CR4.PCIDE = 0, the instruction invalidates all TLB entries associated
	 * with PCID 000H except those for global pages. It also invalidates all entries
	 * in all paging-structure caches associated with PCID 000H.
	 *
	 * If CR4.PCIDE = 1 and bit 63 of the instruction’s source operand is 0, the
	 * instruction invalidates all TLB entries associated with the PCID specified in
	 * bits 11:0 of the instruction’s source operand except those for global pages.
	 * It also invalidates all entries in all paging-structure caches associated with
	 * that PCID. It is not required to invalidate entries in the TLBs and
	 * paging-structure caches that are associated with other PCIDs.
	 *
	 * If CR4.PCIDE = 1 and bit 63 of the instruction’s source operand is 1, the
	 * instruction is not required to invalidate any TLB entries or entries in
	 * paging-structure caches.
	 */

	/* Without PTI, it always flush the user mapping. */

	local_irq_save(flags);
	write_cr3(read_cr3());
	local_irq_restore(flags);
}

/* TLB FLUSHING API. */

static inline void flush_tlb_one(unsigned long addr)
{
	__flush_tlb_one(addr);
}

static inline void flush_tlb_all(void)
{
	__flush_tlb_global();
}

static inline void flush_tlb_local(void)
{
	__flush_tlb_local();
}

#endif /* __X86_ASM_TLB_H__ */
