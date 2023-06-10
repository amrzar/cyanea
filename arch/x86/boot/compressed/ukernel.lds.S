/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm/page.h>

OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(startup_64)

SECTIONS
{
    . = 0;
    .head.text : {
        _head = . ;
        *(.head.text)
        _ehead = . ;
    }

    .rodata..compressed : {
        *(.rodata..compressed)
    }

    .text : {
        _text = .;
        *(.text) *(.text.*)
        _etext = . ;
    }

    .rodata : {
        _rodata = . ;
        *(.rodata) *(.rodata.*)
        _erodata = . ;
    }

    .data : {
        _data = . ;
        *(.data) *(.data.*)
        _edata = . ;
    }

    . = ALIGN(8);
    .bss : {
        _bss = . ;
        *(.bss) *(.bss.*)
        *(COMMON)
        . = ALIGN(8);
        _ebss = .;
    }

    . = ALIGN(PAGE_SIZE);
    .pgtable : {
        _pgtable = . ;
        *(.pgtable)
        _epgtable = . ;
    }

    . = ALIGN(PAGE_SIZE);
    _end = .;

    /DISCARD/ : {
        *(.dynamic) *(.dynsym) *(.dynstr) *(.dynbss)
        *(.hash) *(.gnu.hash)
        *(.gnu.version*)
        *(.note.*)
        *(.comment)
    }

    .got.plt : { *(.got.plt) }

    ASSERT((SIZEOF(.got.plt) == 0x0), "Unexpected '.got.plt' entries detected!")

    .got : { *(.got) }
    .plt : { *(.plt) *(.plt.*) }
    .rel.dyn : { *(.rel.*) *(.rel_*) }
    .rela.dyn : { *(.rela.*) *(.rela_*) }

    ASSERT((SIZEOF(.got) == 0x0), "Unexpected '.got' entries detected!")
    ASSERT((SIZEOF(.plt) == 0x0), "Unexpected run-time procedure linkages detected!")  
    ASSERT((SIZEOF(.rel.dyn) == 0x0) || (SIZEOF(.rela.dyn) == 0x0),
        "Unexpected run-time relocations detected!")   
}
