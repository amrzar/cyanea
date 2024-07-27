/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/pgtable.h>
#include <cyanea/cache.h>

#include <asm/ukernel.lds.h>

OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(phys_startup_64)

#define LOAD_OFFSET __START_KERNEL_map

PHDRS {
    text   PT_LOAD;
    rodata PT_LOAD;
    data   PT_LOAD;
    pcpu   PT_LOAD;
    init   PT_LOAD;
    note   PT_NOTE;
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

        . = ALIGN(PAGE_SIZE);
        *(.data..page_aligned)

        /* Keep start and end of stack aligned. */
        . = ALIGN(STACK_SIZE);
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
        *(.init.rodata)
        *(.init.rodata.*)

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

    .apic.drivers : AT(ADDR(.apic.drivers) - LOAD_OFFSET) {
        . = ALIGN(8);
        __apic_drivers_start = .;
        *(.apic.drivers);
        __apic_drivers_end = .;
    }

    __per_cpu_load = .;

    /* Use address 0; We want to calculate offset for variable in this esction. */
    .data..percpu 0 : AT(__per_cpu_load - LOAD_OFFSET) {
        __per_cpu_start = .;
        *(.data..percpu..head)

        . = ALIGN(PAGE_SIZE);
        *(.data..percpu..page_aligned)

        . = ALIGN(SMP_CACHE_BYTES);
        *(.data..percpu)
        *(.data..percpu..cache_aligned)
        __per_cpu_end = .;
    } : pcpu

    . = __per_cpu_load + SIZEOF(.data..percpu);

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

    .brk : AT(ADDR(.brk) - LOAD_OFFSET) {
        __brk_base = .;
        . = . + (CONFIG_BRK_ALLOC_PAGES * PAGE_SIZE);
        __brk_limit = .;
    }
    
    . = ALIGN(PAGE_SIZE);
    _end = .;

    __ukernel_code_size = ABSOLUTE(_end - _text);

    /DISCARD/ :
    {
        *(.note.*)
        *(.comment)
    }

    /* Sections that should be empty, make sure it is the case. */

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

    /* Sections that we should keep if they exist, e.g. debug sections. */
    /* DWARF 1.1 and DWARF 2. */
    .debug_aranges  0 : { *(.debug_aranges) }
    /* DWARF 2. */
    .debug_info     0 : { *(.debug_info) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    /* DWARF 3. */
    .debug_ranges   0 : { *(.debug_ranges) }
}
