# SPDX-License-Identifier: GPL-2.0-or-later

#
# ''32-bit entry to ukernel''. We get here from compressed image with 'ESI'
# points to the 'boot_params'. Paging is disable. 'DS', 'ES', 'FS', 'GS',
# and 'SS' are flat 4GiB segment - '__BOOT_DS', 'CS' is flat 4GiB segment
# - '__BOOT_CS'.
#

#include <asm-offsets.h>
#include <asm/desc_defs.h>
#include <asm/segment.h>
#include <asm/setup.h>
#include <asm/cpu_registers.h>
#include <cyanea/pgtable.h>
#include <asm/page.h>

    .section ".head.text", "ax"

#define pa(x) __phys_addr_kernel(x)

    .global startup_32
    .balign 4, 0x90
startup_32:

    lgdt    pa(boot_gdt_descr)

    movl    $(__BOOT_DS),%eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss

    movl    pa(initial_stack), %eax
    leal    -__START_KERNEL_map(%eax), %esp

    pushl   $__BOOT_CS
    movl    $pa(.Lon_boot_cs), %eax
    pushl   %eax

    lretl

.Lon_boot_cs:

    #
    # Reset BSS.
    #

    cld
    xorl    %eax, %eax
    movl    $pa(__bss_start), %edi
    movl    $pa(__bss_stop), %ecx
    subl    %edi, %ecx
    shrl    $2, %ecx
    rep; stosl

    pushl   %esi
    call    mk_early_pgtable_32         # Init 'initial_page_table'.
    popl    %esi

    jmp     .Ldefault_entry             # '.Ldefault_entry' shared by 'BSP' and 'AP's.

    .type   startup_32, @function
    .size   startup_32, . - startup_32

#
# Application CPU entry point; entered from trampoline.S.
#

    .global startup_32_smp
    .balign 4, 0x90
startup_32_smp:



.Ldefault_entry:
    pushl   $0      # Reset 'EFLAGS'. Disable interrupts etc.
    popfl

    movl	$(CR0_STATE & ~_CR0_PG), %eax   # Make sure paging is disable.
    movl	%eax, %cr0

    #
    # TODO. '_CR4_PSE' and '_CR4_PGE' supported!?
    #

    movl    $_CR4_PSE, %eax     # Set PSE. 'initial_page_table' may include 4MiB
    movl    %eax, %cr4          # pages after syncing back with 'swapper_pg_dir'
                                # for AP's boot.

    # Enable paging.

    movl    $pa(initial_page_table), %eax
    movl    %eax, %cr3
    movl    $CR0_STATE, %eax
    movl    %eax, %cr0

    ljmp    $__BOOT_CS, $1f     # Reset prefetch and normalize 'EIP'.
1:

    addl    $__START_KERNEL_map, %esp       # Update 'ESP' above '__PAGE_OFFSET'.

    movl    $(_CR4_PSE | _CR4_PGE), %eax    # Set PGE ''after'' enabling paging.
    movl    %eax, %cr4

    lgdt    early_gdt_descr
    ljmp    $__KERNEL_CS, $1f
1:

    movl    $__KERNEL_DS, %eax
    movl    %eax, %ss

    movl    $__USER_DS, %eax    # Use '__USER_DS' as data segment 'DS' and 'ES'.
    movl    %eax, %ds           # Use instead of '__KERNEL_DS' to save some cycle.
    movl    %eax, %es

    xorl    %eax, %eax          # Loading null-selector fo unused segments. 
    movl    %eax, %fs
    movl    %eax, %gs

    lldt    %ax

    movl    %esi, %eax          # Pass 'boot_params' address.
    call    *(initial_code)

1:  jmp     1b                  # SHOULD NOT GET HERE!

    .type   startup_32_smp, @function
    .size   startup_32_smp, . - startup_32_smp

#

    .data
boot_gdt_descr:
    .word   boot_gdt_end - boot_gdt - 1
    .long   boot_gdt - __START_KERNEL_map

    .type   boot_gdt_descr, @object
    .size   boot_gdt_descr, . - boot_gdt_descr

    .balign 8
boot_gdt:
    .quad   0x0000000000000000              # Reserved.
    .quad   0x0000000000000000              # Reserved.
    .quad   GDT_ENTRY(0xC09A, 0, 0xFFFFF)   # '__BOOT_CS', R/X, 4GiB, Base 0.
    .quad   GDT_ENTRY(0xC092, 0, 0xFFFFF)   # '__BOOT_DS', R/W, 4GiB, Base 0.
boot_gdt_end:
    .type   boot_gdt_end, @object

    .type   boot_gdt, @object
    .size   boot_gdt, . - boot_gdt

#
