/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/init_utask.h>
#include <cyanea/pgtable.h>
#include <cyanea/string.h>
#include <cyanea/minmax.h>
#include <asm/page.h>
#include <asm/sections.h>
#include <asm/setup.h>
#include <asm/tlbflush.h>
#include <asm/cpuid.h>
#include <asm/desc.h>

extern void __init i386_start_kernel(unsigned long);
extern void __init start_kernel(void);

/* Initialisation variable used by ''head_32.S''. Used for SMP boot. **/

unsigned long initial_code = (unsigned long)(i386_start_kernel);
unsigned long initial_stack = (unsigned long)(__end_utask_init);

/* Keep 'initial_page_table' around for 'ident-mapping' and switch to 'swapper_pg_dir'. **/

pgd_t initial_page_table[PTRS_PER_PGD] __section(".bss..page_aligned");
pgd_t swapper_pg_dir[PTRS_PER_PGD] __section(".bss..page_aligned");

static void __init copy_bootdata(unsigned long rm)
{
    /* Copy boot data to a safe place. * */

    memcpy(&boot_params, (void *)(rm), sizeof(boot_params));

    if (boot_params.hdr.cmd_line_ptr) {
        memcpy(boot_command_line,
            (void *)(boot_params.hdr.cmd_line_ptr), COMMAND_LINE_SIZE);
    }
}

void __init i386_start_kernel(unsigned long rm)
{
    copy_bootdata(rm);

    /* Move to 'swapper_pg_dir'. */

    memcpy(swapper_pg_dir + KERNEL_PGD_BOUNDARY,
        initial_page_table + KERNEL_PGD_BOUNDARY, KERNEL_PGD_PTRS);

    write_cr3(__pa(swapper_pg_dir));

    /* 'initial_page_table' has global pages. Flush all TLB. */

    /* check for '_CPUID_BIT_PGE' ?! */

    if (get_cpuid_edx(0x00000001) & (1U << 13))
        __native_tlb_flush_global(read_cr4());

    start_kernel();
}

#define __brk_ptr() __pa(*(unsigned long *)__pa(&_brk_end))

void __head mk_early_pgtable_32(void)
{
    int i;
    pgd_t pgd, *pgdp = (pgd_t *) __pa(initial_page_table);
    pte_t pte, *ptep = (pte_t *) __brk_ptr();

    pte = __pte_t(__PAGE_KERNEL_EXEC);

    /* We map at least the real-mode memory. * */
#define LIMIT max(__pa(_end), 0x100000U)

    while ((pte_val(pte) & __PG_PFN_MASK) < LIMIT) {
        pgd = __pgd_t((pgdval_t) (ptep) | _KERNPG_TABLE);

        pgdp[0] = pgd;          /* ''ident-mapping'' */
        pgdp[pgd_index(PAGE_OFFSET)] = pgd;

        for (i = 0; i < PTRS_PER_PT; i++) {
            *ptep = pte;
            pte = __pte_t(pte_val(pte) + PAGE_SIZE);
            ptep++;
        }

        pgdp++;
    }

    *(unsigned long *)__pa(&_brk_end) = (unsigned long)__va(ptep);
}
