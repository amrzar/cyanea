/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/percpu.h>
#include <cyanea/init_utask.h>
#include <cyanea/irq.h>

#include <asm/ukernel.lds.h>
#include <asm/desc_types.h>
#include <asm/current.h>
#include <asm/cpu.h>

desc_struct_t gdt_page[GDT_ENTRIES] __percpu_page_aligned = {
    [GDT_ENTRY_KERNEL32_CS] = GDT_ENTRY_INIT(0xC09B, 0, 0xFFFFF),
    [GDT_ENTRY_KERNEL_CS] = GDT_ENTRY_INIT(0xA09B, 0, 0xFFFFF),
    [GDT_ENTRY_KERNEL_DS] = GDT_ENTRY_INIT(0xC093, 0, 0xFFFFF),
    [GDT_ENTRY_DEFAULT_USER32_CS] = GDT_ENTRY_INIT(0xC0FB, 0, 0xFFFFF),
    [GDT_ENTRY_DEFAULT_USER_DS] = GDT_ENTRY_INIT(0xC0F3, 0, 0xFFFFF),
    [GDT_ENTRY_DEFAULT_USER_CS] = GDT_ENTRY_INIT(0xA0FB, 0, 0xFFFFF),
};

/* Shadow Registers. */
/* ''CR4'' */

static unsigned long cr4 __percpu;
static void cr4_update(unsigned long set, unsigned long clear)
{
    unsigned long new_cr, old_cr = read_cr4();

    new_cr = (old_cr & ~clear) | set;
    if (new_cr != old_cr) {
        this_cpu_write(cr4, new_cr);

        __write_cr4(new_cr);
    }
}

void cr4_set_bits(unsigned long mask)
{
    unsigned long flags;

    local_irq_save(flags);
    cr4_update(mask, 0);
    local_irq_restore(flags);
}

void cr4_clear_bits(unsigned long mask)
{
    unsigned long flags;

    local_irq_save(flags);
    cr4_update(0, mask);
    local_irq_restore(flags);
}

unsigned long read_cr4(void)
{
    return this_cpu_read(cr4);
}

void cr4_init_shadow(void)
{
    this_cpu_write(cr4, __read_cr4());
}

/* Per-cpu TSS segments. */

struct tss cpu_tss __percpu_page_aligned = {
    /* Initialize at runtime; at boot time we are always at ring 0. */
};

/* Per-cpu Hot Stuff! */

struct percpu_hot percpu_hot __percpu_cache_aligned = {
    .current_task = &init_utask,
    .top_of_stack = TOP_OF_INIT_STACK
};
