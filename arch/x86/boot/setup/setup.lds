/* SPDX-License-Identifier: GPL-2.0-or-later */

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
    . = 0;
    .bstext : { *(.bstext) }

    .header : { *(.header) }

    .entrytext : { *(.entrytext) }

    .text : {
        *(.text)
        *(.text.*)
    }

    .text32 : { *(.text32) }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
    }

    /* See 'build.c'. */
    setup_sects = ABSOLUTE(ALIGN(ABSOLUTE(.), 4096) / 512);

    . = ALIGN(8);
    .bss : {
        __bss_start = .;
        *(.bss)
        . = ALIGN(8);
        __bss_end = .;
    }

    . = ALIGN(16);
    _end = .;

    /DISCARD/ : {
        *(.note.*) *(.comment)
    }

    ASSERT((_end <= 0x8000), "Setup is too big, See comments in 'build.c'.")
    ASSERT((hdr == 0x1F1), "Setup header has the wrong offset.")
}
