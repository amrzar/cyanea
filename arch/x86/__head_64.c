/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/init_utask.h>
#include <cyanea/pgtable.h>
#include <cyanea/string.h>
#include <asm/page.h>
#include <asm/sections.h>
#include <asm/setup.h>
#include <asm/system.h>
#include <asm/desc.h>

extern void __init x86_64_start_kernel(unsigned long);
extern void __init start_kernel(void);

/* Initialisation variable used by ''head_64.S''. Used for SMP boot. **/

unsigned long initial_code = (unsigned long)(x86_64_start_kernel);
unsigned long initial_stack = (unsigned long)(__end_utask_init);

/* We put them in '.bss' so the assembly code reset the '_PAGE_PRESENT' for us. **/

pgd_t init_top_pgt[PTRS_PER_PGD] __section(".bss..page_aligned");
pud_t level3_kernel_pgt[PTRS_PER_PUD] __section(".bss..page_aligned");
pmd_t level2_kernel_pgt[PTRS_PER_PMD] __section(".bss..page_aligned");

unsigned long phys_base;

static void __head *fixup_pointer(void *ptr, unsigned long phys_addr)
{
    return (ptr - (void *)(_text) + (void *)(phys_addr));
}

static char __initdata __early_pages[8][PAGE_SIZE] __aligned(PAGE_SIZE);
static unsigned int __initdata next_page;
static void __head *alloc_page(unsigned long phys_addr)
{
    int *np = fixup_pointer(&next_page, phys_addr);

    void *page = &__early_pages[(*np)++];

    return fixup_pointer(page, phys_addr);
}

static unsigned long __init get_cmd_line(void)
{
    unsigned long cmd_line_ptr = boot_params.hdr.cmd_line_ptr;

    cmd_line_ptr |= ((unsigned long)(boot_params.ext_cmd_line_ptr)) << 32;

    return cmd_line_ptr;
}

static void __init copy_bootdata(unsigned long rm)
{
    unsigned long cmd_line_ptr;

    /* Copy boot data to a safe place. * */

    memcpy(&boot_params, (void *)(rm), sizeof(boot_params));

    cmd_line_ptr = get_cmd_line();
    if (cmd_line_ptr)
        memcpy(boot_command_line, (void *)(cmd_line_ptr), COMMAND_LINE_SIZE);
}

static void __init purge_ident_mapping(void)
{
    /* Purge the 'init_top_pgt', except the ''kernel symbol map''. * */

    memset(init_top_pgt, 0, sizeof(pgd_t) * (PTRS_PER_PGD - 1));
    next_page = 0;

    /* We are sure 'CR4.PCIDE' is zero and no global page.
     * No need to flush all TLB. **/

    write_cr3(__pa(init_top_pgt));
}

void __init x86_64_start_kernel(unsigned long rm)
{
    copy_bootdata(rm);

    /* We do not need the 'ident-mapping' anymore. * */

    purge_ident_mapping();

    start_kernel();
}

/* 'ident_map' maps single large page, i.e. 2Mib, at address 'address'. **/

static void __head ident_map(unsigned long address, unsigned long phys_addr)
{
    pgd_t *pgd = fixup_pointer(init_top_pgt, phys_addr);
    pud_t *pud;
    pmd_t *pmd;

    if (pgd_val(pgd[pgd_index(address)]))
        pud = (pud_t *) (pgd_val(pgd[pgd_index(address)]) & __PG_PFN_MASK);
    else {

        /* 'PUD' is not present for 'address', allocate and map. * */

        pud = alloc_page(phys_addr);
        pgd[pgd_index(address)] = __pgd_t((pgdval_t) (pud) | _KERNPG_TABLE);
    }

    if (pud_val(pud[pud_index(address)]))
        pmd = (pmd_t *) (pud_val(pud[pud_index(address)]) & __PG_PFN_MASK);
    else {

        /* 'PMD' is not present for 'address', allocate and map. * */

        pmd = alloc_page(phys_addr);
        pud[pud_index(address)] = __pud_t((pudval_t) (pmd) | _KERNPG_TABLE);
    }

#define __PAGE_KERNEL_LARGE_EXEC_NO_GLOBAL (__PAGE_KERNEL_LARGE_EXEC & ~_PAGE_GLOBAL)

    /* Page may be not present for 'address', map it anyway. * */

    pmd[pmd_index(address)] =
        __pmd_t((pmdval_t) (address) | __PAGE_KERNEL_LARGE_EXEC_NO_GLOBAL);
}

void __head __startup_64(unsigned long phys_addr)
{
    int i;

    pgd_t *pgd = fixup_pointer(init_top_pgt, phys_addr);
    pud_t *pud = fixup_pointer(level3_kernel_pgt, phys_addr);
    pmd_t *pmd = fixup_pointer(level2_kernel_pgt, phys_addr);

    unsigned long load_delta = phys_addr - __phys_addr_kernel(_text);

    /* 'min_alignment' is 'PMD_SHIFT', see ''arch/x86/boot/boot.h''. * */

    if (!IS_ALIGNED(load_delta, PMD_SHIFT))
        while (1) ;

    pgd[pgd_index(__START_KERNEL_map)] =
        __pgd_t((pgdval_t) (pud) | _KERNPG_TABLE);

    pud[pud_index(__START_KERNEL_map)] =
        __pud_t((pudval_t) (pmd) | _KERNPG_TABLE);

    /* * */
    /* 'pmd' maps '_text' to the actual physical address 'phys_addr' where the
     * kernel is loaded, i.e. instead of mapping '__START_KERNEL_map' to 0, we
     * map '__START_KERNEL_map' to the 'load_delta'.
     * 
     * */

    for (i = 0; i < __KERNEL_DIV_ROUND_UP(_end - _text, PMD_SIZE); i++) {
        pmd[i + pmd_index(_text)] =
            __pmd_t((pmdval_t) ((phys_addr +
                    (i * PMD_SIZE)) | __PAGE_KERNEL_LARGE_EXEC));
    }

    /* We need two 'ident-mapping' ranges (1) a mapping for real-mode region,
     * and (2) mapping for switchover. The real-mode region is used for saving
     * real-mode information such as 'boot-param' and 'command-line'.
     *
     * */

    ident_map(0, phys_addr);    /* (1). * */
    for (i = 0; i < __KERNEL_DIV_ROUND_UP(_end - _text, PMD_SIZE); i++) {
        ident_map(phys_addr + (i * PMD_SIZE), phys_addr);       /* (2). * */
    }

    *(unsigned long *)fixup_pointer(&phys_base, phys_addr) = load_delta;
}
