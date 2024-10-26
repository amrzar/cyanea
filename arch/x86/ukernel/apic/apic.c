/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/bitops.h>
#include <cyanea/irq.h>

#include <asm/apic.h>

void __init register_boot_apic_id(u32);

u32 boot_cpu_apic_id __ro_after_init;
