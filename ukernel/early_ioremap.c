/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/pgtable.h>
#include <cyanea/irq.h>

#include <asm/tlb.h>
#include <asm-generic/early_ioremap.h>
#include <asm/fixmap.h>

# define offset_in_page(p) ((unsigned long)(p) & ~PAGE_MASK)
# define PAGE_ALIGN(a) ALIGN_KERNEL((a), PAGE_SIZE)

static int __initdata mapping[TOTAL_FIX_BITMAPS] = { [0 ... TOTAL_FIX_BITMAPS - 1] = -1 };

static int __init find_free_range(int nrpages)
{
    int n = 0, i = 0, start = 0;

    while (n < nrpages) {
        if (i > TOTAL_FIX_BITMAPS)
            return -1;

        if (mapping[i++] != -1) {
            n = 0;
            start = i;
        } else
            n++;
    }

    /* RESERVE range. */
    mapping[start] = nrpages;

    return (FIX_BITMAP_BEGIN - start);
}

static void __init early_set_fixmap(enum fixed_addresses idx,
    phys_addr_t phys_addr, pgprot_t prot)
{
    pte_t *ptep;
    pgprotval_t pgprot;

    assert(((idx >= FIX_BITMAP_END) && (idx <= FIX_BITMAP_BEGIN)),
        "%d out of %d to %d.\n", idx, FIX_BITMAP_BEGIN, FIX_BITMAP_END);

    unsigned long addr = fix_to_virt(idx);

    ptep = &level1_fixmap_pgt[0][pte_index(addr)];
    pgprot = pgprot_val(prot) & __supported_pte_mask;

    if (pgprot)
        pte_set(ptep, __pte_t(phys_addr | pgprot));
    else
        pte_clear(ptep);

    flush_tlb_one(addr);
}

void __init *early_ioremap(phys_addr_t phys_addr, size_t size, pgprot_t prot)
{
    int i, slot;
    unsigned int nrpages;
    unsigned long offset;
    phys_addr_t end;

    end = phys_addr + size - 1;

    /* Wraparound?! */
    if (warning_on(!size || end < phys_addr))
        return NULL;

    offset = offset_in_page(phys_addr);
    phys_addr &= PAGE_MASK;
    size = PAGE_ALIGN(end + 1) - phys_addr;

    nrpages = size >> PAGE_SHIFT;
    if (warning_on(nrpages > TOTAL_FIX_BITMAPS))
        return NULL;

    slot = find_free_range(nrpages);
    if (warning_on(slot == -1))
        return NULL;

    for (i = slot; nrpages > 0; nrpages--, i--) {
        early_set_fixmap(i, phys_addr, prot);

        phys_addr += PAGE_SIZE;
    }

    return (void *)(fix_to_virt(slot) + offset);
}

void __init early_iounmap(void *__addr, size_t size)
{
    int i, slot;
    unsigned int nrpages;
    unsigned long offset;
    unsigned long addr = (unsigned long)__addr;

    if (warning_on((addr < fix_to_virt(FIX_BITMAP_BEGIN)) ||
            (addr > fix_to_virt(FIX_BITMAP_END))))
        return;

    slot = virt_to_fix(addr);
    offset = offset_in_page(addr);
    nrpages = PAGE_ALIGN(offset + size) >> PAGE_SHIFT;

    if (warning_on(mapping[FIX_BITMAP_BEGIN - slot] != nrpages))
        return;

    for (i = slot; nrpages > 0; nrpages--, i--)
        early_set_fixmap(i, 0, __pgprot_t(0));

    mapping[FIX_BITMAP_BEGIN - slot] = -1;
}
