/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <asm/desc.h>

desc_struct_t gdt_page[GDT_ENTRIES] __percpu_page_aligned = {
#ifdef CONFIG_X86_32
    [GDT_ENTRY_KERNEL_CS] = GDT_ENTRY_INIT(0xC09A, 0, 0xFFFFF),
    [GDT_ENTRY_KERNEL_DS] = GDT_ENTRY_INIT(0xC092, 0, 0xFFFFF),
    [GDT_ENTRY_DEFAULT_USER_CS] = GDT_ENTRY_INIT(0xC0FA, 0, 0xFFFFF),
    [GDT_ENTRY_DEFAULT_USER_DS] = GDT_ENTRY_INIT(0xC0F2, 0, 0xFFFFF),
    [GDT_ENTRY_PERCPU] = GDT_ENTRY_INIT(0xC092, 0, 0xFFFFF),
#else
    [GDT_ENTRY_KERNEL32_CS] = GDT_ENTRY_INIT(0xC09B, 0, 0xFFFFF),
    [GDT_ENTRY_KERNEL_CS] = GDT_ENTRY_INIT(0xA09B, 0, 0xFFFFF),
    [GDT_ENTRY_KERNEL_DS] = GDT_ENTRY_INIT(0xC093, 0, 0xFFFFF),
    [GDT_ENTRY_DEFAULT_USER32_CS] = GDT_ENTRY_INIT(0xC0FB, 0, 0xFFFFF),
    [GDT_ENTRY_DEFAULT_USER_DS] = GDT_ENTRY_INIT(0xC0F3, 0, 0xFFFFF),
    [GDT_ENTRY_DEFAULT_USER_CS] = GDT_ENTRY_INIT(0xA0FB, 0, 0xFFFFF),
#endif /* CONFIG_X86_32 */
};

/* Use 'gdt_page' to initialise 'early_gdt_descr'. It is used for BSP on boot.
 * 'gdt_page' is in ''init'' area and will be released. After initialising the
 * per-cpu, we update BSP's GDT with the per-cpu version of 'pgt_page'.
 * 'early_gdt_descr' is used by APs (after updating 'base_address') during smp boot. **/

struct gdt_ptr early_gdt_descr = {
    GDT_ENTRIES * 8 - 1, (unsigned long)(gdt_page)
};
