# SPDX-License-Identifier: GPL-2.0-or-later

#
# head_64.S is position-independent code.
#

#include <asm-offsets.h>
#include <asm/desc_defs.h>
#include <asm/segment.h>
#include <asm/cpu_registers.h>
#include <asm/page.h>

#include "../boot.h"

    .section ".head.text", "ax"

#
# In 64-bit code with '-mcmodel=small' symbols are linked in the lower 2GiB of
# address space. When building relocatable file '.builtin.o', compiler uses relocation
# types 'R_X86_64_32' as 32-bit is enough to store virtual address in a 2GiB range.
# However, when building 'ukernel.elf' as position independant executable (PIE),
# this assumption is not valid anymore, hence the error:
# 
#   ld: .builtin.o: relocation R_X86_64_32 against `.head.text' can not
#          be used when making a PIE object; ...
#
# Standard PIC addressing technique used for 32-bit, i.e. using '_GLOBAL_OFFSET_TABLE_'
# as a convenient but ''arbitrary'' anchor and using 'GOTOFF' attribute to access
# symbols is not practical for '.code32' in 'head_64.S' since 'GOTOFF' references
# are 64-bit only.
#
# As for '.code64', most of the code is position independant and uses 'RIP'-relative
# addressing. However, the 64-bit code can not reference symbol addresses as
# 'they will be' after moving the compressed ukernel 'before the actual move'.
#

#
# We use 'startup_32' as an ''arbitrary'' anchor and use 'relative-va' macro to
# calculate the relative displacement to 'startup_32', i.e. 'X - startup_32'.
# Using '(X - startup_32)(%reg)', where '%reg' is base 'startup_32', to access
# symbol 'X' forces linker to store displacement without any relocation entry.
# This is valid because 'X' and 'startup_32' are in a same section.
#
# To access the symbol address after moving the code, it only needs to update '%reg'
# to 'new_base + startup_32', see description of '_end@GOTOFF' in 'head_32.S'.
# As 'startup_32' is zero, '%reg' need to be updated with 'new_base'.
#

#define relative_va(X) ((X) - startup_32)

    .code32

    .global startup_32
    .balign 16, 0x90
startup_32:
    cli

    #
    # Calculate the address of 'startup_32' and store in 'EBP'.
    # To get current 'IP' use a short call where the 4-byte return addreess is 
    # pushed into stack. For a stack it uses 'scratch' field of 'boot_params'.
    #

    leal    (BP_scratch + 4)(%esi), %esp
    call    1f
1:  popl    %ebp
    subl    $relative_va(1b), %ebp

    #
    # Getting here; we inherited the GDT from the boot-loader. Reload our 'GDT'.
    # Do it is just to be confident and avoid making any extra assumption about
    # the boot-loader.
    #

    leal    relative_va(gdt)(%ebp), %eax    # Store address of 'gdt' in 'EAX'.
    movl    %eax, 2(%eax)                   # Patch the 'GDT' base address, i.e. 'gdt + 2'
    lgdt    (%eax)

    movl    $__BOOT_DS, %eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss

    leal    relative_va(boot_stack_end)(%ebp), %esp     # Setup the stack.

    pushl   $__BOOT32_CS
    leal    relative_va(.Lon_boot32_cs)(%ebp), %eax
    pushl   %eax

    lretl

.Lon_boot32_cs:

#ifdef CONFIG_RELOCATABLE

    #
    # In relocatable kernel, the kernel runtime start address is determined by
    # ''align_up(load_address, kernel_alignment)''. 'init_size' indicates the
    # amount of linear contiguous memory starting at the runtime start address.
    #

    movl    %ebp, %ebx                      # Store address of 'startup_32' in 'EBX'.
    
    movl    BP_kernel_alignment(%esi), %eax
    decl    %eax
    addl    %eax, %ebx
    notl    %eax
    andl    %eax, %ebx
#else

    movl    $CONFIG_PHYSICAL_START, %ebx
#endif /* CONFIG_RELOCATABLE */

    addl    BP_init_size(%esi), %ebx        # 'EBX' is end of buffer.

    #
    # 'EBX' should be 'EBX - _end + startup_32' or the address where kernel image
    # should be moved to continue running plus 'startup_32'. Therefore, to access
    # symbole after the move we can use 'relative_va(symbole)(%ebx)'.
    #

    subl    $relative_va(_end), %ebx

#
# Switching from IA-32 to IA-32e mode
#  - 'CR4.PAE' enable 2nd generation address translation,
#  - 'EFER.LME' enable long mode, and
#  - 'CR0.PG' enable paging.
#

    movl    %cr4, %eax
    orl     $_CR4_PAE, %eax
    movl    %eax, %cr4

    xorl    %eax, %eax
    leal    relative_va(pgtable)(%ebx), %edi
    movl    $(BOOT_PGT_SIZE >> 2), %ecx
    rep; stosl

    #
    # Do 'ident-mapping' for the first 4GiB of memory.
    #

    leal    relative_va(pgtable + 0)(%ebx), %edi
    leal    0x1007(%edi), %eax      # 'EAX' is physical address of second page in
    movl    %eax, 0(%edi)           # 'pgtable', i.e. 'EDI' + 0x1000. '0x7' is 
                                    # _PAGE_BIT_PRESENT |
                                    # _PAGE_BIT_RW |
                                    # _PAGE_BIT_USER.

    leal    relative_va(pgtable + 0x1000)(%ebx), %edi
    leal    0x1007(%edi), %eax
    movl    $4, %ecx                # Initialise 4 entries for the 3rd level of
1:  movl    %eax, 0x0(%edi)         # page-table with the address of 3rd, 4th,
    addl    $0x1000, %eax           # 5th, and 6th page in the 'pgtable'.
    addl    $8, %edi
    decl    %ecx
    jnz     1b

    leal    relative_va(pgtable + 0x2000)(%ebx), %edi
    movl    $0x183, %eax            # Initialise 4 pages '512 * 4 = 2048' entries 
    movl    $2048, %ecx             # to map 2Mib Pages. '0x183' is
1:  movl    %eax, 0(%edi)           # _PAGE_BIT_PRESENT |
    addl    $0x200000, %eax         # _PAGE_BIT_RW |
    addl    $8, %edi                # _PAGE_BIT_PSE |
    decl    %ecx                    # _PAGE_BIT_GLOBAL
    jnz     1b                      # '_PAGE_BIT_NX' must remain zero.

    leal    relative_va(pgtable)(%ebx), %eax
    movl    %eax, %cr3

    movl    $MSR_EFER, %ecx
    rdmsr
    btsl    $_EFER_LME, %eax
    wrmsr

    pushl   $__BOOT_CS
    leal    relative_va(startup_64)(%ebp), %eax
    pushl   %eax

    movl	$CR0_STATE, %eax        # Enable paging.
    movl	%eax, %cr0

    lretl   # Moving from 32-bit compatibility mode into 64-bit mode by loading
            # '__BOOT_CS' selector. Return to 'startup_64'.

    .type   startup_32, @function
    .size   startup_32, . - startup_32

#

    .code64

    #
    # 64-bit entry should always starts at offest 0x200.
    # It is necessary so that the boot-loader can find the 64-bit entry address.
    #

    .org    0x200

    .global startup_64
    .balign 16, 0x90
startup_64:
    cli

    #
    # Getting here either from the code in 'startup_32' or directly from the
    # boot-loader. We may inherit the GDT from the boot-loader. Reload our GDT.
    # Do it is just to be confident and avoid making any extra assumption about
    # the boot-loader.
    #
    # Getting here from boot-loader, we could be loaded anywhere above 4GiB.
    # We must rely on the mapping provided by the boot-loader.
    #

    leaq    gdt64(%rip), %rax       # Store address of 'gdt64' in 'RAX'.
    addq    %rax, 2(%rax)           # Patch the 'GDT' base address, i.e. 'gdt + 2'
    lgdt    (%rax)

    xorl    %eax, %eax              # 'null-seceltor'
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss               # No GP(0) here!! Loading null-selector to
                                    # 'SS' generates GP(0) only if 'CPL = 3' or
                                    # 'CPL < 3 and CPL != RPL'.

    leaq    boot_stack_end(%rip), %rsp      # Setup the stack.

    pushq   $__BOOT_CS
    leaq    .Lon_boot_cs(%rip), %rax
    pushq   %rax

    lretq
 
.Lon_boot_cs:

#ifdef CONFIG_RELOCATABLE
    leaq    startup_32(%rip), %rbp  # Store address of 'startup_32' in 'RBP'.

    movl    BP_kernel_alignment(%rsi), %eax
    decl    %eax
    addq    %rax, %rbp
    notq    %rax
    andq    %rax, %rbp
#else

    movq    $CONFIG_PHYSICAL_START, %rbp
#endif /* CONFIG_RELOCATABLE */

    movl    BP_init_size(%rsi), %ebx
    subl    $relative_va(_end), %ebx

    #
    # 'RBX' is the address where kernel image should be moved to continue
    # running plus 'startup_32'. Therefore, to access symbole after the move we
    # can use 'relative_va(symbole)(%rbx)'.
    #

    addq    %rbp, %rbx

    leaq    relative_va(boot_stack_end)(%rbx), %rsp     # Update stack for after the move.
    pushq   $0                                          # Reset the flags.
    popfq

    pushq   %rsi                                # Save 'RSI'.

    leaq    (_bss - 8)(%rip), %rsi              # Start to copy from end of the
    leaq    relative_va(_bss - 8)(%rbx), %rdi   # code, i.e. '_bss' up to begining
    movl    $(_bss - startup_32), %ecx          # of the code 'startup_32'.
    shrl    $3, %ecx                            # The end of the destination obtained
    std                                         # using 'relative_va(_bss-8)(%rbx)'.
    rep; movsq
    cld

    popq    %rsi                                # Restore 'RSI'.

    leaq    relative_va(gdt64)(%rbx), %rax      # Reset the 'GDTR' after the move.
    leaq    relative_va(gdt)(%rbx), %rdx
    movq    %rdx, 2(%rax)
    lgdt    (%rax)

    leaq    relative_va(.Lrelocated)(%rbx), %rax
    jmp     *%rax                               # Finally, jump to relocated code.

    .type   startup_64, @function
    .size   startup_64, . - startup_64

#

    .text
.Lrelocated:
    
    #
    # Reset BSS: stack is there.
    #

    xorl    %eax, %eax          # Zero 'RAX', writing 'EAX' zeros the upper 32-bit.
    leaq    _bss(%rip), %rdi
    leaq    _ebss(%rip), %rcx
    subq    %rdi, %rcx
    shrq    $3, %rcx
    rep; stosq

    #
    # 'output_len', 'input_len', and 'input_data' are defined in 'piggy.S',
    # See 'mkpiggy.c'.
    #

    pushq   %rsi                        # Save 'ESI'.

    movq    %rsi, %rdi                  # bp - boot_params address.
    leaq    boot_heap(%rip), %rsi       # heap - address of available heap.
    leaq    input_data(%rip), %rdx      # input_data - compressed ukernel.
    movl    input_len(%rip), %ecx       # input_len - size of compressed ukernel.
    movq    %rbp, %r8                   # output - uncompressed ukernel.
    movl    output_len(%rip), %r9d      # output_len - size of uncompressed ukernel.
    
    call    decompress_ukernel          # Returns ukernel location in 'EAX'.

    popq    %rsi                        # Restore 'ESI'.

    jmp     *%rax

    .type   .Lrelocated, @function
    .size   .Lrelocated, . - .Lrelocated

#

    .data
gdt64:
    .word   gdt_end - gdt - 1   # GDT size.
    .quad   gdt - gdt64         # GDT base, filled above.

    .type   gdt64, @object
    .size   gdt64, . - gdt64

    .balign 8
gdt:
    .word   gdt_end - gdt - 1   # GDT size.
    .long   0                   # GDT base, filled above.
    .word   0                   # Padding to next GDT entry.
    .quad   GDT_ENTRY(0xC09A, 0, 0xFFFFF)   # '__BOOT32_CS', R/X, 4GiB, Base 0.
    .quad   GDT_ENTRY(0xA09A, 0, 0xFFFFF)   # '__BOOT_CS', 'long mode' enabled. 
    .quad   GDT_ENTRY(0xC092, 0, 0xFFFFF)   # '__BOOT_DS', R/W, 4GiB, Base 0.
gdt_end:
    .type   gdt_end, @object

    .type   gdt, @object
    .size   gdt, . - gdt

    .bss
    .balign 4
boot_heap:
    .fill   BOOT_HEAP_SIZE, 1, 0

    .type   boot_heap, @object
    .size   boot_heap, . - boot_heap

boot_stack:
    .fill   BOOT_STACK_SIZE, 1, 0
    .balign 16
boot_stack_end:
    .type   boot_stack_end, @object

    .type   boot_stack, @object
    .size   boot_stack, . - boot_stack
       
    #
    # This section must be after '.bss' as page-table for IA-32e is initialised
    # before moving the code for decompression. Only codes between 'startup_32'
    # up to ''_bss'' is moved to the end of the buffer. Therefore, '.pgtable'
    # remains untouched across the move.
    #

    .section ".pgtable", "aw", @nobits
    .balign PAGE_SIZE
pgtable:
    .fill   BOOT_PGT_SIZE, 1, 0

    .type   pgtable, @object
    .size   pgtable, . - pgtable
