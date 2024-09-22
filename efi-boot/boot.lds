/* SPDX-License-Identifier: GPL-2.0 */

ENTRY(__efi_header)

SECTIONS
{
    . = 0;
    .head : { *(.head) }
    .text : { *(.text .text.*) }

    . = ALIGN(8);
    .rodata : {
        *(.gz..data)
        *(.rodata .rodata.*)

        . = ALIGN(4096);
        _etext = .;
    }

    .data : {
        *(.data)

        . = ALIGN(512);
        _edata = .;
    }

    .bss : {
        *(.bss)

        . = ALIGN(512);
        _end = .;
    }
 
    /DISCARD/ :
    {
        *(.note.*)
        *(.comment)
    }
}
 
    __data_rawsize = ABSOLUTE(_edata - _etext);
    __data_size = ABSOLUTE(_end - _etext);
