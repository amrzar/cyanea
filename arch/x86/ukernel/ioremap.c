/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/pgtable.h>
#include <asm/fixmap.h>
#include <asm/io.h>
#include <asm/tlb.h>

#define offset_in_page(p) ((unsigned long)(p) & ~PAGE_MASK)
#define PAGE_ALIGN(a) ALIGN_KERNEL((a), PAGE_SIZE)

static int __initdata mapping[TOTAL_FIX_BITMAPS] = { [0 ... TOTAL_FIX_BITMAPS - 1] = -1 };

static void ____set_fixmap(pte_t *ptep, phys_addr_t phys_addr, pgprot_t prot)
{
	pgprotval_t pgprot;

	phys_addr &= PAGE_MASK;

	pgprot = pgprot_val(prot) & __supported_pte_mask;
	if (pgprot)
		pte_set(ptep, __pte_t(phys_addr | pgprot));
	else
		pte_clear(ptep);
}

/* Permanent FIXMAPs.*/
void set_fixmap(enum fixed_addresses idx, phys_addr_t phys_addr, pgprot_t prot)
{
	assert(((idx > FIX_TOP) && (idx < FIX_BITMAP_END)),
	        "%d out of %d to %d.\n", idx, FIX_TOP, FIX_BITMAP_END);

	unsigned long address = fix_to_virt(idx);

	pte_t *ptep = &level1_fixmap_pgt[1][pte_index(address)];

	____set_fixmap(ptep, phys_addr, prot);
	flush_tlb_one(address);
}

/* Set a temporary mapping. */
static void __init __set_fixmap(enum fixed_addresses idx,
        phys_addr_t phys_addr, pgprot_t prot)
{
	assert(((idx >= FIX_BITMAP_END) && (idx <= FIX_BITMAP_BEGIN)),
	        "%d out of %d to %d.\n", idx, FIX_BITMAP_BEGIN, FIX_BITMAP_END);

	unsigned long address = fix_to_virt(idx);

	pte_t *ptep = &level1_fixmap_pgt[0][pte_index(address)];

	____set_fixmap(ptep, phys_addr, prot);
	flush_tlb_one(address);
}

static int __init find_free_range(int nrpages)
{
	int n = 0, i = 0, start = 0;

	while (n < nrpages) {
		if (i > TOTAL_FIX_BITMAPS)
			return -1;

		if (mapping[i] != -1) {
			n = 0;
			start = (i += mapping[i]);
		} else {
			n++;
			i++;
		}
	}

	/* RESERVE range. */
	mapping[start] = nrpages;

	return (FIX_BITMAP_BEGIN - start);
}

static void __init *__ioremap(phys_addr_t phys_addr, size_t size, pgprot_t prot)
{
	phys_addr_t end = phys_addr + size;
	unsigned int nrpages;
	unsigned long offset;
	int i, slot;

	if (!size)
		return NULL;

	offset = offset_in_page(phys_addr);
	phys_addr &= PAGE_MASK;
	size = PAGE_ALIGN(end) - phys_addr;

	nrpages = size >> PAGE_SHIFT;
	if (nrpages > TOTAL_FIX_BITMAPS)
		return NULL;

	slot = find_free_range(nrpages);
	if (slot == -1)
		return NULL;

	for (i = slot; nrpages > 0; nrpages--, i--) {
		__set_fixmap(i, phys_addr, prot);

		phys_addr += PAGE_SIZE;
	}

	return (void *)(fix_to_virt(slot) + offset);
}

/**
 * ioremap() - map address into CPU space as uncacheable.
 * @phys_addr: physical address to map.
 * @size: size to map.
 *
 * It's useful if some control registers are in such an area and write combining
 * or read caching is not desirable (fully uncacheable, strongly ordered).
 * It must be unmapped with iounmap().
 *
 * Return mapped address or NULL on failure.
 */
void __init *ioremap(phys_addr_t phys_addr, size_t size)
{
	return __ioremap(phys_addr, size, PAGE_KERNEL_NOCACHE);
}

void __init *ioremap_prot(phys_addr_t phys_addr, size_t size, pgprot_t prot)
{
	return __ioremap(phys_addr, size, prot);
}

void __init iounmap(void *virt_addr, size_t size)
{
	unsigned int nrpages;
	unsigned long offset;
	unsigned long addr;
	int i, slot;

	if (!virt_addr)
		return;

	addr = (unsigned long)virt_addr;
	if ((addr < fix_to_virt(FIX_BITMAP_BEGIN)) ||
	        (addr > fix_to_virt(FIX_BITMAP_END)))
		return;

	slot = virt_to_fix(addr);
	offset = offset_in_page(addr);
	nrpages = PAGE_ALIGN(offset + size) >> PAGE_SHIFT;

	if (mapping[FIX_BITMAP_BEGIN - slot] != nrpages)
		return;

	for (i = slot; nrpages > 0; nrpages--, i--)
		__set_fixmap(i, 0, __pgprot_t(0));

	mapping[FIX_BITMAP_BEGIN - slot] = -1;
}

/**
 * memremap() - map address into CPU space as cacheable.
 * @phys_addr: physical address of memory.
 * @size: size of memory to map.
 *
 * It matches the default mapping for System RAM.
 * It must be unmapped with iounmap().
 *
 * Return mapped address or NULL on failure.
 */
void __init *memremap(phys_addr_t phys_addr, size_t size)
{
	return __ioremap(phys_addr, size, PAGE_KERNEL);
}

void __init memunmap(void *virt_addr, size_t size)
{
	iounmap(virt_addr, size);
}
