/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/pgtable.h>
#include <asm/cache.h>

OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(phys_startup_64)

#define LOAD_OFFSET __START_KERNEL_map

PHDRS {
    text PT_LOAD;
    rodata PT_LOAD;
    data PT_LOAD;
    init PT_LOAD;
    note PT_NOTE;
}

SECTIONS
{
    . = __START_KERNEL;

    phys_startup_64 = ABSOLUTE(startup_64 - LOAD_OFFSET);

    .text : AT(ADDR(.text) - LOAD_OFFSET) {
        _text = .;
        _stext = .;
        *(.head.text)
        *(.text .text.*)
    } : text

    . = ALIGN(PAGE_SIZE);
    __end_text = .;

    .rodata : AT(ADDR(.rodata) - LOAD_OFFSET) {
        __start_rodata = .;
        *(.rodata .rodata.*)
        
        *(.data..ro_after_init)
    } : rodata

    . = ALIGN(PAGE_SIZE);
    __end_rodata = .;

    .data : AT(ADDR(.data) - LOAD_OFFSET) {
        _sdata = .;

        __init_utask_stack = .;
        . = __init_utask_stack + STACK_SIZE;
        __end_utask_init = .;

        *(.data)

        _edata = .;
    } : data

    /* Init: starts here! */
    . = ALIGN(PAGE_SIZE);
    __init_begin = .;

    .init.text : AT(ADDR(.init.text) - LOAD_OFFSET) {
        _sinittext = .;
        *(.init.text)
        *(.init.text.*)
        _einittext = .;
    } : init

    .init.data : AT(ADDR(.init.data) - LOAD_OFFSET) {
        *(.init.data)
        *(.init.data.*)

        . = ALIGN(8);
        __ctors_start = .;
        *(SORT(.ctors.*))
        *(.ctors)
        *(SORT(.init_array.*))
        *(.init_array)
        __ctors_end = .;

        . = ALIGN(8);
        __setup_start = .;
        *(.init.setup)
        __setup_end = .;
    }

    . = ALIGN(PAGE_SIZE);
    .data..percpu : AT(ADDR(.data..percpu) - LOAD_OFFSET) {
        __per_cpu_start = .;
        *(.data..percpu..page_aligned)
        . = ALIGN(L1_CACHE_BYTES);
        *(.data..percpu)
        __per_cpu_end = .;
    }

    /* Init: ends here!   */
    . = ALIGN(PAGE_SIZE);
    __init_end = .;

    /* BSS */
    . = ALIGN(PAGE_SIZE);
    .bss : AT(ADDR(.bss) - LOAD_OFFSET) {
        __bss_start = .;
        *(.bss..page_aligned)
        . = ALIGN(PAGE_SIZE);

        *(.bss)

        . = ALIGN(PAGE_SIZE);
        __bss_stop = .;
    } : data
   
    __end_of_kernel_reserve = .;

    . = ALIGN(PAGE_SIZE);
    .brk : AT(ADDR(.brk) - LOAD_OFFSET) {
        __brk_base = .;
        . += 64 * 1024;
        __brk_limit = .;
    }

    . = ALIGN(PAGE_SIZE);
    _end = .;

    /DISCARD/ :
    {
        *(.note.*)
        *(.comment)
    }

    /* Section that should be empty, make sure it is the case. */

    .got      : { *(.got) *(.igot.*) }
    .plt      : { *(.plt) *(.plt.*) *(.iplt) }
    .got.plt  : { *(.got.plt) }
    .rel.dyn  : { *(.rel.*) *(.rel_*) }
    .rela.dyn : { *(.rela.*) *(.rela_*) }

    ASSERT((SIZEOF(.got) == 0x0), "Unexpected '.got' section.")
    ASSERT((SIZEOF(.plt) == 0x0), "Unexpected '.plt' section.")
    ASSERT((SIZEOF(.got.plt) == 0x0), "Unexpected '.got.plt' section.")
    ASSERT((SIZEOF(.rel.dyn) == 0x0), "Unexpected '.rel.dyn' section.")
    ASSERT((SIZEOF(.rela.dyn) == 0x0),  "Unexpected '.rela.dyn' section.")

    /* Sections that we need them if they exist, e.g. debug sections. */

    /* DWARF 1.1 and DWARF 2. **/
    .debug_aranges  0 : { *(.debug_aranges) }

    /* DWARF 2. **/
    .debug_info     0 : { *(.debug_info) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }

    /* DWARF 3. **/
    .debug_ranges   0 : { *(.debug_ranges) }
}
