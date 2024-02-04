/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/init_utask.h>
#include <cyanea/pgtable.h>
#include <cyanea/string.h>
#include <cyanea/stddef.h>
#include <cyanea/irq.h>
#include <asm/page.h>
#include <asm/ukernel.lds.h>
#include <asm/setup.h>
#include <asm/cpu.h>
#include <asm/desc_types.h>
#include <asm/utask.h>

void __init x86_64_start_kernel(phys_addr_t);
void __init start_kernel(void);
void __init idt_setup_early_handler(void);

/* Initialisation variable used by ''head_64.S''. Used for SMP boot. */

unsigned long initial_code = (unsigned long)x86_64_start_kernel;
unsigned long smpboot_cpu = 0;

/* It is used trough out the boot till 'init_top_pgt' get initialised and enabled
 * in 'init_mem_mapping' (see mm.c).
 */

pgd_t __initdata early_top_pgt[PTRS_PER_PGD] __aligned(PAGE_SIZE) = { 0 };

/* Use '.bss' so the assembly code reset the '_PAGE_PRESENT' for us. */

pgd_t init_top_pgt[PTRS_PER_PGD] __section(".bss..page_aligned");
pud_t level3_kernel_pgt[PTRS_PER_PUD] __section(".bss..page_aligned");
pmd_t level2_kernel_pgt[PTRS_PER_PMD] __section(".bss..page_aligned");
pmd_t level2_fixmap_pgt[PTRS_PER_PMD] __section(".bss..page_aligned");

/* Two pages for the fixmap addresses, see 'asm/fixmap.h'. */
pte_t level1_fixmap_pgt[2][PTRS_PER_PT] __section(".bss..page_aligned");

phys_addr_t phys_base;

#define EARLY_FREE_PAGES 64

static unsigned int __initdata next_page;
static char __initdata early_pages[EARLY_FREE_PAGES][PAGE_SIZE]
__aligned(PAGE_SIZE);
static void __init *get__free_page(unsigned int *np)
{
    return (*np < EARLY_FREE_PAGES) ? early_pages[(*np)++] : NULL;
}

static void __init purge_ident_mapping(void)
{
    /* Purge the 'early_top_pgt', except the ''kernel symbol map''. */

    memset(early_top_pgt, 0, sizeof(pgd_t) * (PTRS_PER_PGD - 1));

    /* Page 0 is being used for boot PUD; see '__startup_64'. */

    next_page = 1;

    /* Here, 'CR4.PCIDE' is zero. */

    /* This invalidates all TLB entries associated with PCID 000H except those
     * for global pages. It also invalidates all entries in all paging-structure
     * caches associated with PCID 000H.
     */

    write_cr3(__pa(early_top_pgt));
}

static void __init copy_bootdata(struct boot_params *bp)
{
    phys_addr_t cmd_line_ptr;

    memcpy(&boot_params, bp, sizeof(boot_params));

    cmd_line_ptr = boot_params.hdr.cmd_line_ptr;
    cmd_line_ptr |= ((phys_addr_t) boot_params.ext_cmd_line_ptr << 32);

    if (cmd_line_ptr)
        memcpy(boot_command_line, __va(cmd_line_ptr), COMMAND_LINE_SIZE);
}

void __init x86_64_start_kernel(phys_addr_t bp)
{
    cr4_init_shadow();

    purge_ident_mapping();

    idt_setup_early_handler();

    copy_bootdata(__va(bp));

    start_kernel();
}

/* Boot PAGEFAULT handler! */

static void __init *get_free_zero_page(void)
{
    void *page = get__free_page(&next_page);

    if (page)
        memset(page, 0, PAGE_SIZE);

    return page;
}

/* All page-tables are present in kernel mapping; use it to get virtual address. */
#define __va_symbol(x) ((x) + __START_KERNEL_map - phys_base)

static bool __init do_early_map(unsigned long address)
{
    unsigned int i;

    phys_addr_t phys_addr = address - PAGE_OFFSET;

    pgd_t *pgd = early_top_pgt;
    pud_t *pud;
    pmd_t *pmd;

    i = pgd_index(address);

    /* Everything should fit in a PUD allocated in '__startup_64'. */

    assert(pgd_val(pgd[i]), "out-of-bound memory access during boot.");

    pud = (pud_t *) __va_symbol((pgd_val(pgd[i]) & PG_PFN_MASK));

    i = pud_index(address);

    if (pud_val(pud[i]))
        pmd = (pmd_t *) __va_symbol((pud_val(pud[i]) & PG_PFN_MASK));
    else {

        /* 'PMD' is not present for 'address', allocate and map. */

        pmd = get_free_zero_page();
        if (!pmd)
            return false;

        pud[i] = __pud_t((pudval_t) __pa_symbol(pmd) | _KERNPG_TABLE);
    }

#define __PAGE_KERNEL_LARGE_NO_GLOBAL (__PAGE_KERNEL_LARGE & ~_PAGE_GLOBAL)

    /* Page is not present for 'address', map it . */

    pmd[pmd_index(address)] =
        __pmd_t((pmdval_t)(phys_addr & PMD_MASK) |
            __PAGE_KERNEL_LARGE_NO_GLOBAL);

    return true;
}

void __init do_boot_exception(struct utask_regs *regs, int exp)
{
    switch (exp) {
    case EXCEPTION_PF:
        if (!do_early_map(read_cr2()))
            goto hlt_loop;

        break;

    /* HLT. Exception at boot. */
    default:
        ulog_err("exp %d (HLT.).", exp);
hlt_loop:

        HALT;
    }
}

/* Function used while running on direct mapping. */

static void __head *fixup_pointer(void *ptr, phys_addr_t load_phys_addr)
{
    return (ptr - (void *)(_text) + (void *)(load_phys_addr));
}

static void __head *early_get_free_page(phys_addr_t load_phys_addr)
{
    void *page = get__free_page(fixup_pointer(&next_page, load_phys_addr));

    return fixup_pointer(page, load_phys_addr);
}

static void __head ident_map(pgd_t *pgd, phys_addr_t address,
    phys_addr_t load_phys_addr)
{
    pud_t *pud;
    pmd_t *pmd;

    if (pgd_val(pgd[pgd_index(address)]))
        pud = (pud_t *)(pgd_val(pgd[pgd_index(address)]) & PG_PFN_MASK);
    else {

        /* 'PUD' is not present for 'address', allocate and map. */

        pud = early_get_free_page(load_phys_addr);
        pgd[pgd_index(address)] = __pgd_t((pgdval_t)(pud) | _KERNPG_TABLE);
    }

    if (pud_val(pud[pud_index(address)]))
        pmd = (pmd_t *)(pud_val(pud[pud_index(address)]) & PG_PFN_MASK);
    else {

        /* 'PMD' is not present for 'address', allocate and map. */

        pmd = early_get_free_page(load_phys_addr);
        pud[pud_index(address)] = __pud_t((pudval_t)(pmd) | _KERNPG_TABLE);
    }

#define __PAGE_KERNEL_LARGE_EXEC_NO_GLOBAL (__PAGE_KERNEL_LARGE_EXEC & ~_PAGE_GLOBAL)

    /* Page may be not present for 'address', map it anyway. */

    pmd[pmd_index(address)] =
        __pmd_t((pmdval_t)(address) | __PAGE_KERNEL_LARGE_EXEC_NO_GLOBAL);
}

void __head __startup_64(phys_addr_t load_phys_addr)
{
    int i;

    pgd_t *pgd = fixup_pointer(early_top_pgt, load_phys_addr);

    /* We get a page for boot PUD to keep 'init_mem_mapping' simple. */

    pud_t *pud = early_get_free_page(load_phys_addr);

    pmd_t *pmd = fixup_pointer(level2_kernel_pgt, load_phys_addr);
    pmd_t *pmd_fixmap = fixup_pointer(level2_fixmap_pgt, load_phys_addr);
    pte_t *pt_fixmap_1 = fixup_pointer(level1_fixmap_pgt[0], load_phys_addr);
    pte_t *pt_fixmap_2 = fixup_pointer(level1_fixmap_pgt[1], load_phys_addr);

    unsigned long load_delta = load_phys_addr - __phys_addr_kernel(_text);

    /* 'min_alignment' is 'PMD_SHIFT' in header.S.
     * So if we are loaded with a delta it should be aligned to 'PMD_SHIFT'.
     */

    if (!IS_ALIGNED(load_delta, PMD_SHIFT))
        HALT;

    /* PGD [511]. */
    pgd[pgd_index(__START_KERNEL_map)] =
        __pgd_t((pgdval_t)(pud) | _KERNPG_TABLE);

    /* PUD [510]. */
    pud[pud_index(__START_KERNEL_map)] =
        __pud_t((pudval_t)(pmd) | _KERNPG_TABLE);

    /* PUD [511]. */
    pud[pud_index(__START_KERNEL_map) + 1] =
        __pud_t((pudval_t)(pmd_fixmap) | _KERNPG_TABLE);

    /* 'pmd' maps '_text' to the actual physical address 'load_phys_addr' where the
     * kernel is loaded, i.e. in theory instead of mapping '__START_KERNEL_map' to 0,
     * we map '__START_KERNEL_map' to the 'load_delta'.
     */

    /* However, the range 'load_delta' up to '_text' is not mapped. */

    /* Use '__PAGE_KERNEL_LARGE_EXEC'. '_PAGE_XD' is not set. '_PAGE_GLOBAL' is set. */
    /* '_PAGE_GLOBAL' will be ignored if not supported by CPU. */

    for (i = 0; i < __KERNEL_DIV_ROUND_UP(_end - _text, PMD_SIZE); i++) {
        pmd[i + pmd_index(_text)] =
            __pmd_t((pmdval_t)((load_phys_addr +
                        (i * PMD_SIZE)) | __PAGE_KERNEL_LARGE_EXEC));
    }

    /* Map fixmap tables. */
    pmd_fixmap[511] = __pmd_t((pmdval_t)(pt_fixmap_2) | _KERNPG_TABLE);
    pmd_fixmap[510] = __pmd_t((pmdval_t)(pt_fixmap_1) | _KERNPG_TABLE);

    /* Mapping identity mappings for switchover (Temp). */
    for (i = 0; i < __KERNEL_DIV_ROUND_UP(_end - _text, PMD_SIZE); i++) {
        ident_map(pgd, load_phys_addr + (i * PMD_SIZE), load_phys_addr);
    }

    *(unsigned long *)fixup_pointer(&phys_base, load_phys_addr) = load_delta;
}
