/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/types.h>
#include <cyanea/memblock.h>
#include <cyanea/pgtable.h>
#include <cyanea/irq.h>
#include <cyanea/errno.h>

#include <asm/cpuinfo.h>
#include <asm/ukernel.lds.h>
#include <asm/cpu.h>
#include <asm/tlb.h>
#include <asm/entry_64.h>
#include <asm/page.h>

#include <cyanea/string.h>

#include <cyanea/stddef.h>
#include <cyanea/minmax.h>

void __init idt_setup_pf(void);

/* Highest directly mapped pfn < 4 GiB. */
unsigned long max_low_pfn_mapped;

/* Highest directly mapped pfn > 4 GiB. */
unsigned long max_pfn_mapped;

/* Bits supported by hardware. */
pgprotval_t __supported_pte_mask = -1;

/* Setup BRK. */

/* The pages for initialising the page-tables are allocated form '__brk_alloc'
 * (i.e. at the end of the image), instead of 'memblock_alloc' as at this point NUMA
 * is not enabled and we do that to keep them close to ukernel.
 */

typedef char page_array_t[CONFIG_BRK_ALLOC_PAGES][PAGE_SIZE];

static page_array_t *__brk_alloc = (page_array_t *) __brk_base;
# define __brk_end ((*__brk_alloc)[next_page])

static unsigned int __initdata next_page;
static void __init *alloc_pages(unsigned int num)
{
    void *page;

    assert((next_page + num < CONFIG_BRK_ALLOC_PAGES),
        "not enough memory to init page table.");

    page = __brk_end;
    next_page += num;

    memset(page, 0, PAGE_SIZE * num);

    return page;
}

/* INITIALISE the direct mapping.
 *
 * 'physical_pt_init'
 * 'physical_pmd_init'
 * 'physical_pud_init'
 * 'physical_mapping_init'
 *
 * Halt if we fail on any of these functions.
 */

static void __init physical_pt_init(pte_t pt[], phys_addr_t start,
    phys_addr_t end, enum page_size ps)
{
    unsigned long vaddr = (unsigned long)__va(start);
    unsigned long vaddr_end = (unsigned long)__va(end);

    unsigned int i = pte_index(vaddr);

    for (; ((i < PTRS_PER_PT) && (vaddr < vaddr_end)); i++) {

        assert(!(pte_flags(pt[i]) & _PAGE_PRESENT), "memory already mapped.\n");

        pte_set(&pt[i], __pte_t(__pa(vaddr) | check_pgprot(PAGE_KERNEL)));

        vaddr = (vaddr & PAGE_MASK) + PAGE_SIZE;
    }
}

static void __init physical_pmd_init(pmd_t pmd[], phys_addr_t start,
    phys_addr_t end, enum page_size ps)
{
    unsigned long vaddr = (unsigned long)__va(start);
    unsigned long vaddr_end = (unsigned long)__va(end);

    unsigned int i = pmd_index(vaddr);

    for (; ((i < PTRS_PER_PMD) && (vaddr < vaddr_end)); i++) {
        pte_t *pt;

        if (pmd_flags(pmd[i]) & _PAGE_PRESENT) {

            assert((ps != PAGE_SIZE_2M), "unable to map page.\n");

            assert(!(pmd_flags(pmd[i]) & _PAGE_PSE),
                "memory already mapped.\n");

            /* PMD entry is already mapped to PT. */

            pt = __va(pmd_val(pmd[i]) & pmd_pfn_mask(pmd[i]));
            physical_pt_init(pt, __pa(vaddr), __pa(vaddr_end), ps);

        } else {

            if (ps == PAGE_SIZE_2M) {

                /* Map a 2MiB page. */

                pmd_set(&pmd[i],
                    __pmd_t(__pa(vaddr) | check_pgprot(PAGE_KERNEL_LARGE)));
            } else {
                pt = alloc_pages(1);
                physical_pt_init(pt, __pa(vaddr), __pa(vaddr_end), ps);

                pmd_set(&pmd[i], __pmd_t(__pa(pt) | _KERNPG_TABLE));
            }
        }

        vaddr = (vaddr & PMD_MASK) + PMD_SIZE;
    }
}

static void __init physical_pud_init(pud_t pud[], phys_addr_t start,
    phys_addr_t end, enum page_size ps)
{
    unsigned long vaddr = (unsigned long)__va(start);
    unsigned long vaddr_end = (unsigned long)__va(end);

    unsigned int i = pud_index(vaddr);

    for (; ((i < PTRS_PER_PUD) && (vaddr < vaddr_end)); i++) {
        pmd_t *pmd;

        if (pud_flags(pud[i]) & _PAGE_PRESENT) {

            assert((ps != PAGE_SIZE_1G), "unable to map page.\n");

            assert(!(pud_flags(pud[i]) & _PAGE_PSE),
                "memory already mapped.\n");

            /* PUD entry is already mapped to a PMD. */

            pmd = __va(pud_val(pud[i]) & pud_pfn_mask(pud[i]));
            physical_pmd_init(pmd, __pa(vaddr), __pa(vaddr_end), ps);

        } else {
            if (ps == PAGE_SIZE_1G) {

                /* Map a 1GiB page. */

                pud_set(&pud[i],
                    __pud_t(__pa(vaddr) | check_pgprot(PAGE_KERNEL_LARGE)));
            } else {
                pmd = alloc_pages(1);
                physical_pmd_init(pmd, __pa(vaddr), __pa(vaddr_end), ps);

                pud_set(&pud[i], __pud_t(__pa(pmd) | _KERNPG_TABLE));
            }
        }

        vaddr = (vaddr & PUD_MASK) + PUD_SIZE;
    }
}

static void __init physical_mapping_init(phys_addr_t start, phys_addr_t end,
    enum page_size ps)
{
    unsigned long vaddr = (unsigned long)__va(start);
    unsigned long vaddr_end = (unsigned long)__va(end);

    pgd_t *pgd = init_top_pgt;

    assert((vaddr_end < __START_KERNEL_map),
        "direct mapping overlap with '__START_KERNEL_map'.\n");

    unsigned int i = pgd_index(vaddr);

    for (; ((i < PTRS_PER_PGD) && (vaddr < vaddr_end)); i++) {
        pud_t *pud;

        if (pgd_flags(pgd[i]) & _PAGE_PRESENT) {

            /* PGD entry is already mapped. */

            pud = __va(pgd_val(pgd[i]) & PG_PFN_MASK);
            physical_pud_init(pud, __pa(vaddr), __pa(vaddr_end), ps);

        } else {
            pud = alloc_pages(1);
            physical_pud_init(pud, __pa(vaddr), __pa(vaddr_end), ps);

            pgd_set(&pgd[i], __pgd_t(__pa(pud) | _KERNPG_TABLE));
        }

        vaddr = (vaddr & PGD_MASK) + PGD_SIZE;
    }
}

static void add_range_mapped(unsigned long start_pfn, unsigned long end_pfn)
{
    max_pfn_mapped = max(max_pfn_mapped, end_pfn);

    if (start_pfn < (1UL << (32 - PAGE_SHIFT)))
        max_low_pfn_mapped = max(max_low_pfn_mapped, min(end_pfn,
                    1UL << (32 - PAGE_SHIFT)));
}

# define PFN_PHYS(x) ((phys_addr_t)(x) << PAGE_SHIFT)
# define PFN_UP(x) (ROUND_UP(x, PAGE_SIZE) >> PAGE_SHIFT)
# define PFN_DOWN(x) (ROUND_DOWN(x, PAGE_SIZE) >> PAGE_SHIFT)

/* Setup the direct mapping of the physical memory at PAGE_OFFSET. */

static void __init init_range_memory_mapping(phys_addr_t range_start,
    phys_addr_t range_end)
{
    int i;
    unsigned long start_pfn, end_pfn, limit_pfn, pfn;

    for_each_mem_pfn_range(i, -1, &start_pfn, &end_pfn, NULL) {
        phys_addr_t start = clamp(PFN_PHYS(start_pfn), range_start, range_end);
        phys_addr_t end = clamp(PFN_PHYS(end_pfn), range_start, range_end);

        if (start >= end)
            continue;

        limit_pfn = PFN_DOWN(end);

        /* The range that should be mapped [start .. end). */
        ulog_debug("[%s]: [mem %#018llx .. %#018llx]\n",
            __FUNCTION__, start, end - 1);

        /* We prefer 1GiB mapping. However, on the head (or tail) use 4Kib or 2Mib
         * pages to get to the required alignments.
         */

        /*  - 4KiB mapping from 'start' to the next multiple of 2MiB. */

        pfn = start_pfn = PFN_DOWN(start);
        end_pfn = ROUND_UP(pfn, PFN_DOWN(PMD_SIZE));

        if (end_pfn > limit_pfn)
            end_pfn = limit_pfn;

        if (start_pfn < end_pfn) {
            physical_mapping_init(PFN_PHYS(start_pfn), PFN_PHYS(end_pfn),
                PAGE_SIZE_4K);

            pfn = end_pfn;
        }

        /* - 2MiB mapping from 'pfn' to the next multiple of 1GiB. */

        start_pfn = ROUND_UP(pfn, PFN_DOWN(PMD_SIZE));
        end_pfn = ROUND_UP(pfn, PFN_DOWN(PUD_SIZE));

        if (end_pfn > ROUND_DOWN(limit_pfn, PFN_DOWN(PMD_SIZE)))
            end_pfn = ROUND_DOWN(limit_pfn, PFN_DOWN(PMD_SIZE));

        if (start_pfn < end_pfn) {
            physical_mapping_init(PFN_PHYS(start_pfn), PFN_PHYS(end_pfn),
                PAGE_SIZE_2M);

            pfn = end_pfn;
        }

        /* - 1GiB mapping from 'pfn' as much as possible. */

        start_pfn = ROUND_UP(pfn, PFN_DOWN(PUD_SIZE));
        end_pfn = ROUND_DOWN(limit_pfn, PFN_DOWN(PUD_SIZE));
        if (start_pfn < end_pfn) {
            physical_mapping_init(PFN_PHYS(start_pfn), PFN_PHYS(end_pfn),
                PAGE_SIZE_1G);

            pfn = end_pfn;
        }

        /* - 2MiB mapping from 'pfn' as much as possible. */

        start_pfn = ROUND_UP(pfn, PFN_DOWN(PMD_SIZE));
        end_pfn = ROUND_DOWN(limit_pfn, PFN_DOWN(PMD_SIZE));
        if (start_pfn < end_pfn) {
            physical_mapping_init(PFN_PHYS(start_pfn), PFN_PHYS(end_pfn),
                PAGE_SIZE_2M);

            pfn = end_pfn;
        }

        /* - 4KiB mapping from 'pfn' until end of region. */

        start_pfn = pfn;
        end_pfn = limit_pfn;
        if (start_pfn < end_pfn) {
            physical_mapping_init(PFN_PHYS(start_pfn), PFN_PHYS(end_pfn),
                PAGE_SIZE_4K);
        }

        add_range_mapped(start >> PAGE_SHIFT, end >> PAGE_SHIFT);
    }
}

static void __init init__init_top_pgt(void)
{
    pgd_t *pgd = init_top_pgt;
    pud_t *pud = level3_kernel_pgt;

    /* PGD [511]. */
    pgd_set(&pgd[pgd_index(__START_KERNEL_map)],
        __pgd_t(__pa(pud) | _KERNPG_TABLE));

    /* PMDs are already initialised in '__startup_64'; reuse them. */

    /* PUD [510]. */
    pud_set(&pud[pud_index(__START_KERNEL_map)],
        __pud_t(__pa(level2_kernel_pgt) | _KERNPG_TABLE));

    /* PUD [511]. */
    pud_set(&pud[pud_index(__START_KERNEL_map) + 1],
        __pud_t(__pa(level2_fixmap_pgt) | _KERNPG_TABLE));
}

void __init init_mem_mapping(void)
{
    init__init_top_pgt();

    /* Let's enable PCID on boot CPU. */
    cr4_set_bits(_CR4_PCIDE);

    init_range_memory_mapping(0, -1);

    /* Reserve the used portion of BRK used in 'init_range_memory_mapping'. */
    memblock_reserve(__pa_symbol(__brk_base), __brk_end - __brk_base);

    write_cr3(__pa(init_top_pgt));
    flush_tlb_all();

    /* Change the page fault handler. */
    idt_setup_pf();
}

DEFINE_IDTENTRY_WITH_ERROR_CODE(exc_page_fault)
{
    ulog_err("NOT IMPLEMENTED!");

    halt();
}
