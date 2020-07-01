/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <asm/setup.h>
#include <asm/e820.h>
#include <asm/sections.h>

struct boot_params boot_params;

unsigned long _brk_start = (unsigned long)__brk_base;
unsigned long _brk_end = (unsigned long)__brk_base;

extern void __init bsp_cpuinfo_init(void);

void __init setup_arch(char cmdline[])
{
    /* First step, we capture the BSP capabilities. */

    bsp_cpuinfo_init();

    e820__memory_setup();

}
