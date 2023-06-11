/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/compiler.h>

#include "string.h"
#include "setup.h"

#include "../boot.h"

#if (CONFIG_PHYSICAL_ALIGN & (CONFIG_PHYSICAL_ALIGN - 1))
#  error 'CONFIG_PHYSICAL_ALIGN' should be power-of-two.
#endif

#if (CONFIG_PHYSICAL_ALIGN < (1UL << BOOT_ALIGN_SHIFT))
#  error 'CONFIG_PHYSICAL_ALIGN' is less than minimum kernel alignment.
#endif

#if (CONFIG_PHYSICAL_START & (CONFIG_PHYSICAL_ALIGN - 1))
#  error 'CONFIG_PHYSICAL_START' should meet alignment restriction.
#endif

struct boot_params boot_params __aligned(16);

void __noreturn cpu_hlt(void)
{
    while (1) {
        asm volatile("hlt");
    }
}

void __c_setup(void)
{
    memcpy(&boot_params.hdr, &hdr, sizeof(hdr));

    if (boot_params.hdr.cmd_line_ptr == 0) {
        error("boot-loader does not support the 2.02+ protocol.\n");
    }

    early_console_init();

    if (!(boot_params.hdr.loadflags & CAN_USE_HEAP)) {
        puts("'CAN_USE_HEAP' not set, no heap for setup code.\n");
    }

    /* 'KASLR_FLAG' is solely for in-kernel use, clear it. */
    boot_params.hdr.loadflags &= ~KASLR_FLAG;

    detect_memory();
    go_to_protected_mode();

    /* ''SHOULD NEVER GET HERE!'' */
    cpu_hlt();
}
