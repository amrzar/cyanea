/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "setup.h"

#define SMAP 0x534d4150         /* ASCII ''SMAP'' */

static void detect_memory_e820(void)
{
    u8 error;
    u32 eax, ecx, ebx = 0;
    int count = 0;
    struct bios_e820_entry buf = { 0 };

    do {
        ecx = sizeof(buf);

        asm volatile(
            "int    $0x15   ;"
            "setc   %0      ;"
            : "=d" (error), "+b" (ebx), "=a" (eax), "+c" (ecx), "=m" (buf)
            : "D" (&buf), "d" (SMAP), "a" (0xe820)
        );

        /* BIOSes which terminate the chain with 'CF' set instead of '!EBX'
         * do not always report the ''SMAP'' on the final iteration.
         */

        if (error != 0)
            break;

        if (eax != SMAP) {
            count = 0;

            break;
        }

        boot_params.e820_table[count++] = buf;
    } while (ebx && count < BIOS_E820_MAX_ENTRIES);

    boot_params.e820_entries = count;
}

void detect_memory(void)
{
    /* TODO. Support for different memory detection, e.g. 'e801' or '88'. */

    detect_memory_e820();
}
