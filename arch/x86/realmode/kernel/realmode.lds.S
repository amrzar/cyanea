/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/pgtable.h>

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(startup)
 
SECTIONS
{
    . = 0;

    real_mode_seg = 0;

    .header : {
        pa_real_mode_base = .;
        *(.header)
    }

    . = ALIGN(8);
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    . = ALIGN(PAGE_SIZE);
    __end_rodata = .;
    
    .text : {
        _text = .;
        *(.text)
        *(.text.*)
    }
    
    .text32 : {
        *(.text32)
        *(.text32.*)
    }

    .text64 : {
        *(.text64)
        *(.text64.*)
    }

    . = ALIGN(PAGE_SIZE);
    __end_text = .;
    
    .data : {
        *(.data)
        *(.data.*)
    }

    . = ALIGN(8);
    .bss : {
        __bss_start = .;
        *(.bss*)
        . = ALIGN(8);
        __bss_stop = .;
    }

    /DISCARD/ : {
        *(.note*)
        *(.debug*)
    }

#include "pasyms.h"

}
