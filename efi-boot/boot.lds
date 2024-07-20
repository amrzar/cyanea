/* SPDX-License-Identifier: GPL-2.0 */

ENTRY(__efistub_efi_header)

SECTIONS
{
    . = ALIGN(4096);
    .head : {
        *(.head)
    }

    .text : {
        *(.text .text.*)
    }

    .rodata : ALIGN(8) {
        __efistub__gz_data_start = .;
        *(.gz..data)
        __efistub__gz_data_end = .;

        *(.rodata .rodata.*)

        . = ALIGN(4096);
        _etext = .;
    }

    . = ALIGN(4096);
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
 
    __efistub__gz_data_size = ABSOLUTE(__efistub__gz_data_end - __efistub__gz_data_start);

    __data_rawsize = ABSOLUTE(_edata - _etext);
    __data_size = ABSOLUTE(_end - _etext);
