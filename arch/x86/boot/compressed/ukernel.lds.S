/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm/page.h>

#ifdef CONFIG_X86_32
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(startup_32)
#else
OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(startup_64)
#endif

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

#ifdef CONFIG_X86_64
    . = ALIGN(PAGE_SIZE);
    .pgtable : {
        _pgtable = . ;
        *(.pgtable)
        _epgtable = . ;
    }
#endif /* CONFIG_X86_64 */

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

#ifdef CONFIG_X86_64
    ASSERT((SIZEOF(.got.plt) == 0x0), "Unexpected '.got.plt' entries detected!")
#else

    /*
     * '.got.plt' contains the target addresses after they have been looked up by
     * 'ld.so' or address of the next instruction at the lookup site that invokes
     * the first instruction in '.plt' to trigger 'ld.so'.
     *
     * The code at the '.plt' uses the first three entries of '.got.plt' which
     * point to the '.dynamic' section '_DYNAMIC' and the fixup routine in 'ld.so'.
     * Hence, '0x0C'.
     *
     * */

    ASSERT((SIZEOF(.got.plt) == 0x0C), "Unexpected '.got.plt' entries detected!")
#endif /* CONFIG_X86_64 */

    .got : { *(.got) }
    .plt : { *(.plt) *(.plt.*) }
    .rel.dyn : { *(.rel.*) *(.rel_*) }
    .rela.dyn : { *(.rela.*) *(.rela_*) }

    ASSERT((SIZEOF(.got) == 0x0), "Unexpected '.got' entries detected!")
    ASSERT((SIZEOF(.plt) == 0x0), "Unexpected run-time procedure linkages detected!")  
    ASSERT((SIZEOF(.rel.dyn) == 0x0) || (SIZEOF(.rela.dyn) == 0x0),
        "Unexpected run-time relocations detected!")   
}
