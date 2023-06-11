# SPDX-License-Identifier: GPL-2.0-or-later

#
# ''64-bit entry to ukernel''.
#
# We come here either directly from a 64-bit bootloader, or from from compressed
# image. Paging is enabled. Someone has loaded an identity mapped page table for
# us. These identity mapped page tables map all of the kernel pages and possibly
# all of memory.
#
# 'RSI' holds the real-mode address of 'boot_params'.
#

#include <cyanea/pgtable.h>
#include <cyanea/irq.h>

#include <asm-offsets.h>
#include <asm/setup.h>
#include <asm/cpu.h>
#include <asm/page.h>

    .section ".head.text", "ax"

#define pa(x) __phys_addr_kernel(x)

    .global startup_64
    .balign 16, 0x90
startup_64:
    cli

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

    movq    $pa(early_top_pgt), %rax    # Use 'early_top_pgt' for 'BSP'.
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
    # For the 'BSP' this switches to 'early_top_pgt' which still has the
    # indentity mappings present. For the 'AP's, it switches to 'init_top_pgt'.
    #

    movq    %rax, %cr3

    #
    # Do a global TLB flush after the 'CR3' switch to make sure the TLB entries
    # from the original indentity mappings are flushed. 
    #

    movq    %cr4, %rcx
    movq    %rcx, %rax
    xorq    $_CR4_PGE, %rcx
    movq    %rcx, %cr4
    movq    %rax, %cr4

    movq    $1f, %rax                       # Move to virtual address.
    jmp     *%rax
1:

    movl    smpboot_cpu(%rip), %ecx

    movq    __per_cpu_offset(,%rcx,8), %rdx

    # Update 'GDT'. Use percpu 'gdt_page'.

    subq    $16, %rsp
    movw    $(GDT_ENTRIES * 8 - 1), (%rsp)
    leaq    gdt_page(%rdx), %rax
    movq    %rax, 2(%rsp)
    lgdt    (%rsp)                          # See 'desc_defs.h'.
    addq    $16, %rsp

    xorl    %eax, %eax                      # 'null-seceltor'
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss                       # No GP(0) here!! See 'head_64.S'. 

    movq    percpu_hot+X86_top_of_stack(%rdx), %rsp     # Setup the stack.

    # 'RDX' already has the offset value that should be stored in 'GS'.

    movl    $MSR_GS_BASE, %ecx
    movl    %edx, %eax
    shrq    $32, %rdx
    wrmsr

    #
    # TODO. '_EFER_NXE' is supported!?
    #

    movl    $MSR_EFER, %ecx
    rdmsr
    btsl    $_EFER_NXE, %eax
    btsl    $_EFER_SCE, %eax
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

    .section ".init.data", "aw"

boot_gdt_descr:
    .word   boot_gdt_end - boot_gdt - 1     # GDT size.
    .quad   boot_gdt - boot_gdt_descr       # GDT base, filled above.

    .type   boot_gdt_descr, @object
    .size   boot_gdt_descr, . - boot_gdt_descr

    .balign 16                              # Recommended by Intel.
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
# Early IDT Array.
#

#include <asm/entry_64.h>

    .section ".init.text", "ax"

    .global boot_idt_handler_array
    .balign 16, 0x90
boot_idt_handler_array:

        i = 0
    .rept IDT_EXCEPTION_ENTRIES
1:
    .if ((IDT_EXCEPTION_WITH_ERRCODE_MASK >> i) & 1) == 0
        pushq   $-1                         # 00 00 (2 bytes) if no error code.
    .endif
        pushq   $i                          # 00 00 (2 bytes) Push vector number.
        jmp     boot_idt_handler_common     # 00 00 or 00 00 00 00 00 (2 or 5 bytes)

    # Each stub is packed into 9 bytes.

    # Size of each entry in the array should be at least 9 (2 + 2 + 5) to fit all
    # instructions (pushq + pushq + jmp). Fill the unused bytes so all entries
    # in array are same size.

    # TODO. Should we align to a value greater than 9, e.g. 16!?

    .fill 1b + 9 - ., 1, 0x90
        i = i + 1
    .endr

    .type   boot_idt_handler_array, @notype # not a function! 
    .size   boot_idt_handler_array, . - boot_idt_handler_array

    .balign 16, 0x90

    # So far we have (1) hardware frame, (2) errorcode or -1, and (3) vector number.

boot_idt_handler_common:
    cld

    pushq   %rsi                # Preserve the vector number in RDI position; start from RSI.
    movq    8(%rsp), %rsi       # Store the vector number in RSI.
    movq    %rdi, 8(%rsp)       # Store RDI in its position.
    PUSH_REGS

    movq    %rsp, %rdi          # Put pt_regs pointer in 'RDI'.
    call    do_boot_exception   # 'do_boot_exception(struct utask_regs *, int)'.
    POP_REGS_ALL
    addq    $8, %rsp            # Skip 'orig_rax'.

    iretq

    .type   boot_idt_handler_common, @function
    .size   boot_idt_handler_common, . - boot_idt_handler_common
