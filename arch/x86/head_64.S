# SPDX-License-Identifier: GPL-2.0-or-later

#
# ''64-bit entry to ukernel''. We get here from compressed image with 'RSI'
# points to the 'boot_params'. Paging is enabled. The 'ident-mapping' has
# been initialised for us.
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

    .global startup_64
    .balign 16, 0x90
startup_64:

    leaq    boot_gdt_descr(%rip), %rax
    addq    %rax, 2(%rax)                   # Patch 'boot_gdt_descr'.
    lgdt    (%rax)

    xorl    %eax, %eax                      # 'null-seceltor'
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss                       # No GP(0) here!! See 'head_64.S'. 

    leaq    __end_utask_init(%rip), %rsp    # Setup the stack.

    pushq   $__BOOT_CS
    leaq    .Lon_boot_cs(%rip), %rax
    pushq   %rax

    lretq
 
.Lon_boot_cs:

    #
    # Reset BSS.
    #

    cld
    xorl    %eax, %eax
    leaq    __bss_start(%rip), %rdi
    leaq    __bss_stop(%rip), %rcx
    subq    %rdi, %rcx
    shrq    $3, %rcx
    rep; stosq

    leaq    _text(%rip), %rdi
    pushq   %rsi                        # Keep 'ESI' safe.
    call    __startup_64                # Perform page-table initialisation.
    popq    %rsi

    movq    $pa(init_top_pgt), %rax    # Use 'init_top_pgt' for 'BSP'.
                                        # Add 'phys_base' bellow.

    jmp     .Ldefault_entry             # '.Ldefault_entry' shared by 'BSP' and 'AP's.

    .type   startup_64, @function
    .size   startup_64, . - startup_64

#
# Application CPU entry point; entered from trampoline.S.
#

    .global startup_64_smp
    .balign 16, 0x90
startup_64_smp:



.Ldefault_entry:
    addq    phys_base(%rip), %rax

    movq    $(_CR4_PAE | _CR4_PGE), %rcx 
    movq    %rcx, %cr4

    #
    # For the 'BSP' this switches to 'init_top_pgt' which still has the
    # 'ident-mapping' present. For the 'AP's, it does not.
    #

    movq    %rax, %cr3

    #
    # Do a global TLB flush after the 'CR3' switch to make sure the TLB
    # entries from the original 'ident-mapping' are flushed. 'ident-mapping' in
    # 'init_top_pgt' do not set '_PAGE_GLOBAL', see ''__head_64.c''.
    #

    movq    %cr4, %rcx
    movq    %rcx, %rax
    xorq    $_CR4_PGE, %rcx
    movq    %rcx, %cr4
    movq    %rax, %cr4

    movq    $1f, %rax                       # Move to virtual address.
    jmp     *%rax
1:

    lgdt    early_gdt_descr(%rip)

    xorl    %eax, %eax                      # 'null-seceltor'
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss                       # No GP(0) here!! See 'head_64.S'. 

    movq    initial_stack(%rip), %rsp       # Setup the stack.

    #
    # TODO. '_EFER_NXE' is supported!?
    #

    movl    $MSR_EFER, %ecx
    rdmsr
    btsl    $_EFER_NXE, %eax
    wrmsr
    
    movl	$CR0_STATE, %eax
    movq	%rax, %cr0

    pushq   $0
    popfq                                   # Reset the flags.

    movq    %rsi, %rdi                      # Pass 'boot_params' address.

    pushq   $1f
    pushq   $__KERNEL_CS
    movq    initial_code(%rip), %rax
    pushq   %rax

    lretq

1:  jmp     1b                          # SHOULD NOT GET HERE!

    .type   startup_64_smp, @function
    .size   startup_64_smp, . - startup_64_smp

#

    .data
boot_gdt_descr:
    .word   boot_gdt_end - boot_gdt - 1     # GDT size.
    .quad   boot_gdt - boot_gdt_descr       # GDT base, filled above.

    .type   boot_gdt_descr, @object
    .size   boot_gdt_descr, . - boot_gdt_descr

    .balign 8
boot_gdt:
    .quad   0x0000000000000000              # Reserved.
    .quad   GDT_ENTRY(0xC09A, 0, 0xFFFFF)   # '__BOOT32_CS', R/X, 4GiB, Base 0.
    .quad   GDT_ENTRY(0xA09A, 0, 0xFFFFF)   # '__BOOT_CS', 'long mode' enabled. 
    .quad   GDT_ENTRY(0xC092, 0, 0xFFFFF)   # '__BOOT_DS', R/W, 4GiB, Base 0.
boot_gdt_end:
    .type   boot_gdt_end, @object

    .type   boot_gdt, @object
    .size   boot_gdt, . - boot_gdt

#
