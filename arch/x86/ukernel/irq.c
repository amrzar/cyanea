/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>

#include <asm/desc_types.h>
#include <asm/entry_64.h>

#define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof((arr)[0]))

/* ASSEMBLY interrupt handlers. */

extern char irq_entries_start[];

DECLARE_IDTENTRY_WITH_ERROR_CODE(exc_page_fault);

/* 6.12.1.2 Protection of Exception- and Interrupt-Handler Procedures. */

/* The processor checks the DPL of the interrupt or trap gate only if an exception
 * or interrupt is generated with an INT n, INT3, or INTO instruction.
 */

#define DPL0 0x0
#define DPL3 0x3

/* Use modified version of the legacy stack-switching mechanism.
 * See comments in desc_types.h.
 */

#define DEFAULT_STACK 0

struct idt_data {
    unsigned int vector;
    unsigned short segment;
    unsigned long offset;
    struct idt_bits bits;
};

#define G(v, segment, offset, stack, type, dpl) \
    { v, segment, (unsigned long)(offset), { stack, 0, type, dpl, 1 } }

/* Interrupt gate. */
#define INTG(v, off) \
    G(v, __KERNEL_CS, off, DEFAULT_STACK, GATE_INTERRUPT, DPL0)

/* System interrupt gate. */
#define SYSG(v, off) \
    G(v, __KERNEL_CS, off, DEFAULT_STACK, GATE_INTERRUPT, DPL3)

/* Interrupt gate with interrupt stack. */
#define ISTG(v, off, stack) \
    G(v, __KERNEL_CS, off, stack + 1, GATE_INTERRUPT, DPL0)

static __init void init_gate_desc(gate_desc_t *gate, const struct idt_data *d)
{
    gate->offset_low = (u16)(d->offset);
    gate->offset_middle = (u16)(d->offset >> 16);
    gate->offset_high = (u32)(d->offset >> 32);
    gate->segment = d->segment;
    gate->bits = d->bits;
    gate->reserved = 0;
}

static __init void idt_setup_from_table(gate_desc_t idt[],
    const struct idt_data t[], int n)
{
    int i;

    for (i = 0; i < n; i++)
        init_gate_desc(&idt[t[i].vector], &t[i]);
}

/* 6.10 INTERRUPT DESCRIPTOR TABLE (IDT)*/

/* Because IDT entries are always eight bytes long, the limit should always be
 * one less than an integral multiple of eight (that is, 8N – 1).
 */

static gate_desc_t idt_table[IDT_ENTRIES] __section(".bss..page_aligned");

#define IDT_TABLE_SIZE (IDT_ENTRIES * sizeof(gate_desc_t))

static struct dt_ptr idt __ro_after_init = {
    IDT_TABLE_SIZE - 1, (unsigned long) idt_table,
};

/* See comments in head_64.S for the reason to use '9'. */

extern char boot_idt_handler_array[IDT_EXCEPTION_ENTRIES][9];

void __init idt_setup_early_handler(void)
{
    int i;

    for (i = 0; i < IDT_EXCEPTION_ENTRIES; i++) {
        struct idt_data early_idt[] = {
            INTG(i, &boot_idt_handler_array[i])
        };

        idt_setup_from_table(idt_table, early_idt, ARRAY_SIZE(early_idt));
    }

    load_idt(&idt);
}

static struct idt_data  __initdata pf_idt[] = {
    INTG(EXCEPTION_PF, asm_exc_page_fault)
};

/* After moving to ukernel's page-table, we should move away from early
 * page fault handler 'do_early_map' that handles faults in 'early_top_pgt'.
 */

void __init idt_setup_pf(void)
{
    idt_setup_from_table(idt_table, pf_idt, ARRAY_SIZE(pf_idt));
}
