# SPDX-License-Identifier: GPL-2.0-or-later

#
# head_32.S is position-independent code.
#

#include <asm-offsets.h>
#include <asm/desc_defs.h>
#include <asm/segment.h>

#include "../boot.h"

    .section ".head.text", "ax"

    .global startup_32
    .balign 4, 0x90
startup_32:
    cli

    #
    # Calculate the address of '_GLOBAL_OFFSET_TABLE_' and store in 'EDX'.
    # To get current 'IP' use a short call where the 4-byte return addreess is 
    # pushed into stack. For a stack it uses 'scratch' field of 'boot_params'.
    #

    leal    (BP_scratch + 4)(%esi), %esp
    call    1f
1:  popl    %edx

    #
    # In '.builtin.o', a relocation entry of type 'R_386_GOTPC', i.e. 'GOT + A - PC'
    # is generated for '_GLOBAL_OFFSET_TABLE_'. This is the offset of GOT from
    # current 'IP'. Therefore, 'EDX' is address of '_GLOBAL_OFFSET_TABLE_'.
    #

    addl    $_GLOBAL_OFFSET_TABLE_ + (. - 1b), %edx 
  
    #
    # 'gdt@GOTOFF' is offset of 'gdt' from '_GLOBAL_OFFSET_TABLE_'.
    # 'gdt' is the Global Descriptor Table (GDT) used at boot, see bellow.
    # The first and second entrioes are 'Reserved'. However, the first entry is
    # used to store the value of 'GDTR', i.e. 'size' and 'base address' of the GDT.
    #

    #
    # Getting here; we inherited the GDT from the boot-loader. Reload our GDT.
    # Do it is just to be confident and avoid making any extra assumption about
    # the boot-loader.
    #

    leal    gdt@GOTOFF(%edx), %eax      # Store address of 'gdt' in 'EAX'.
    movl    %eax, 2(%eax)               # Patch the 'GDT' base address, i.e. 'gdt + 2'
    lgdt    (%eax)

    movl    $__BOOT_DS, %eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss

    leal    boot_stack_end@GOTOFF(%edx), %esp   # Setup the stack.

    pushl   $__BOOT_CS
    leal    .Lon_boot_cs@GOTOFF(%edx), %eax
    pushl   %eax

    lretl

.Lon_boot_cs:

#ifdef CONFIG_RELOCATABLE

    #
    # ''align_up(load_address, kernel_alignment)'' is runtime start address for
    # relocatable kernel. 'init_size' indicates the amount of linear contiguous
    # memory starting at the runtime start address.
    #

    leal    startup_32@GOTOFF(%edx), %ebx   # 'EBX' is address of 'startup_32'.
    
    movl    BP_kernel_alignment(%esi), %eax
    decl    %eax
    addl    %eax, %ebx
    notl    %eax
    andl    %eax, %ebx
#else

    movl    $CONFIG_PHYSICAL_START, %ebx
#endif /* CONFIG_RELOCATABLE */

    movl    %ebx, %ebp                      # Save 'EBX'.

    addl    BP_init_size(%esi), %ebx        # 'EBX' is end of buffer.

    #
    # '_end@GOTOFF' produces relocation of type 'R_386_GOTOFF', i.e. 'S + A - GOT'.
    # After subtraction 'EBX' is 'EBX - _end + GOT' or the address where kernel 
    # image should be moved temporarily for safe in-place decompression plus
    # '_GLOBAL_OFFSET_TABLE_'. Therefore, to access symbole after the move we can
    # use 'symbole@GOTOFF(%ebx)'
    #

    subl    $_end@GOTOFF, %ebx

    leal    boot_stack_end@GOTOFF(%ebx), %esp   # Update stack for after the move.
    pushl   $0                                  # Reset the flags.
    popfl

    pushl   %esi                            # Save 'ESI'.

    leal    (_bss@GOTOFF - 4)(%edx), %esi   # Start to copy from end of the code,
    leal    (_bss@GOTOFF - 4)(%ebx), %edi   # i.e. '_bss' up to begining of the code
    movl    $(_bss - startup_32), %ecx      # 'startup_32'. The end of the destination
    shrl    $2, %ecx                        # obtained using '(_bss@GOTOFF-4)(%ebx)'.
    std
    rep; movsl
    cld

    popl    %esi                            # Restore 'ESI'.

    leal    gdt@GOTOFF(%ebx), %eax          # Reset the 'GDTR' after the move.
    movl    %eax, 2(%eax)
    lgdt    (%eax)

    leal    .Lrelocated@GOTOFF(%ebx), %eax
    jmp     *%eax                           # Finally, jump to relocated code.

    .type   startup_32, @function
    .size   startup_32, . - startup_32

#

    .text
.Lrelocated:

    #
    # Reset BSS: stack is there.
    #

    xorl    %eax, %eax
    leal    _bss@GOTOFF(%ebx), %edi
    leal    _ebss@GOTOFF(%ebx), %ecx
    subl    %edi, %ecx
    shrl    $2, %ecx
    rep; stosl

    #
    # 'output_len', 'input_len', and 'input_data' are defined in 'piggy.S'.
    # See 'mkpiggy.c'.
    #

    pushl   output_len@GOTOFF(%ebx)         # output_len - size of uncompressed ukernel.
    pushl   %ebp                            # output - uncompressed ukernel.
    pushl   input_len@GOTOFF(%ebx)          # input_len - size of compressed ukernel.
    leal    input_data@GOTOFF(%ebx), %eax   #
    pushl   %eax                            # input_data - compressed ukernel.
    leal    boot_heap@GOTOFF(%ebx), %eax    # heap - address of available heap.
    pushl   %eax                            #
    pushl   %esi                            # bp - 'boot_params' address.
    
    call    decompress_ukernel              # Returns ukernel location in 'EAX'.  
    addl    $24, %esp

    jmp     *%eax

    .type   .Lrelocated, @function
    .size   .Lrelocated, . - .Lrelocated

#

    .data
    .balign 8
gdt:
    .word   gdt_end - gdt - 1       # GDT size.
    .long   0                       # GDT base, filled above.
    .word   0                       # Padding to next GDT entry.
    .quad   0x0000000000000000      # Reserved.
    .quad   GDT_ENTRY(0xC09A, 0, 0xFFFFF)   # '__BOOT_CS', R/X, 4GiB, Base 0.
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
