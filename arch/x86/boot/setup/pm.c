/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/compiler.h>

#include <asm/desc_types.h>

#include "setup.h"

static void realmode_switch_hook(void)
{

    /* 'realmode_swtch' is a real-mode far subroutine set by boot-loader.
     * It must be invoked immediately before entering the protected mode.
     */

    if (boot_params.hdr.realmode_swtch) {
        asm volatile("lcallw *%0"
            : : "m" (boot_params.hdr.realmode_swtch)
            : "eax", "ebx", "ecx", "edx");
    } else {

        /* As the default subroutine disables NMI, we do the the same here! */

        asm volatile("cli");

        /* See: https://wiki.osdev.org/CMOS. */
        outb(0x80, 0x70);       /* Disable NMI. */
        slow_down_io();
    }
}

#define PIC1_DATA_PORT 0x21
#define PIC2_DATA_PORT 0xA1
static void mask_pic_irqs(void)
{
    outb(0xFF, PIC2_DATA_PORT);
    slow_down_io();

    /* '0XFB' all except the cascade irq. */
    outb(0xFB, PIC1_DATA_PORT);
    slow_down_io();
}

static void setup_gdt(void)
{
    static const desc_struct_t boot_gdt[] __aligned(16) = {
        /* code, R/X, 4GiB, Base 0. */
        [GDT_ENTRY_BOOT_CS] = GDT_ENTRY_INIT(0xC09A, 0, 0xFFFFF),
        /* data, R/W, 4GiB, Base 0. */
        [GDT_ENTRY_BOOT_DS] = GDT_ENTRY_INIT(0xC092, 0, 0xFFFFF),
    };

    struct dt_ptr gdt = {
        .size = sizeof(boot_gdt) - 1,
        .base_address = ADDR(ds, boot_gdt)
    };

    asm volatile("lgdtl %0" : : "m" (gdt));
}

static void setup_idt(void)
{
    struct dt_ptr idt = { 0 };  /* NULL IDR. */

    asm volatile("lidtl %0" : : "m" (idt));
}

void go_to_protected_mode(void)
{
    realmode_switch_hook();

    if (enable_a20())
        error("\nA20 gate not responding, die.\n");

    mask_pic_irqs();
    setup_idt();
    setup_gdt();

    protected_mode_jump(boot_params.hdr.code32_start, ADDR(ds, &boot_params));
}
