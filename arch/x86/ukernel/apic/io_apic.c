/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/types.h>

#define NR_IOAPIC CONFIG_NR_IOAPIC

u32 __initdata io_apic_address[NR_IOAPIC];
u32 __initdata io_apic_gsi_base[NR_IOAPIC];

static int __initdata assigned_io_apics;
void __init register_ioapic(int id, u32 address, u32 gsi_base)
{
    int io_apic;

    io_apic = assigned_io_apics++;
    if (io_apic < NR_IOAPIC) {
        io_apic_address[io_apic] = address;
        io_apic_gsi_base[io_apic] = gsi_base;

        ulog_info("IO-APIC (%u) address (0x%x) gsi_base (0x%x).\n",
            id, address, gsi_base);

    } else /* 'CONFIG_NR_IOAPIC'. */
        ulog_err("IO-APIC (%u) dropped.", id);
}
