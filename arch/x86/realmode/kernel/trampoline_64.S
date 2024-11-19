# SPDX-License-Identifier: GPL-2.0-or-later

#include <asm/cr_eflags_defs.h>
#include <asm/msr.h>
#include <asm/page.h>
#include <asm/segment.h>

    .text
    .code16

    .global startup
    .balign 16, 0x90
startup:

    # For 16-bit code; Symbol's values are offsets as virtual address starts from
    # zero in 'realmode.lds.S'. So the relocation is done by updating the segments.
    # Only 'real_mode_seg' (R_386_16) and 'pa_startup_32' (R_386_32) need relocation.

    cli
    wbinvd              # Write back and flush Internal caches.

    .byte   0xea        # 'LJMPW CS:1f'. We can update 'CS' as part of relocation.
    .word   1f, real_mode_seg
1:

    mov     %cs, %ax    # Use same segment as 'CS'.
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %ss

    movl    $rm_stack_end, %esp

.Lswitch_to_protected:

    lgdtl   gdt         # 'lgdtd DS:xx'

    movl    $(CR0_STATE & ~_CR0_PG), %eax
    movl    %eax, %cr0                      # Enable protected-mode.

    ljmpl   $__BOOT32_CS, $pa_startup_32    # Flush prefetch and jump to 32-bit code.

    .type   startup, @function
    .size   startup, . - startup

#

    .section ".text32", "ax"
    .code32

    .global startup_32
    .balign 16, 0x90
startup_32:

    # For 32-bit code; Segments are started from zero. So for relocation symbol's value
    # is added to the base address where the image has been loaded.

    movl    $__BOOT_DS, %edx

    movl    %edx, %ss
    movl    %edx, %ds
    movl    %edx, %es
    movl    %edx, %fs
    movl    %edx, %gs

    addl    $pa_real_mode_base, %esp    # 'ESP' already holds the value of 'rm_stack_end' from
                                        # 16-bit code. Add 'real_mode_base''s value. Updated on relocation.

#
# Switching from IA-32 to IA-32e mode
#  - 'CR4.PAE' enable 2nd generation address translation,
#  - 'EFER.LME' enable long mode, and
#  - 'CR0.PG' enable paging.
#

    movl    pa_cr4, %eax
    movl    %eax, %cr4

    movl    pa_trampoline_pgd, %eax
    movl    %eax, %cr3

    movl    $MSR_EFER, %ecx
    movl    pa_efer, %eax
    movl    pa_efer + 4, %edx
    wrmsr

    movl    $CR0_STATE, %eax
    movl    %eax, %cr0

    # Leave 32-bit compatibility mode (i.e. '__BOOT32_CS'); Jump to in 64-bit mode.

    ljmpl   $__BOOT_CS, $pa_startup_64

    .type   startup_32, @function
    .size   startup_32, . - startup_32

#

    .section ".text64", "ax"
    .code64

    .global startup_64
    .balign 16, 0x90
startup_64:

    jmpq    *initial_code(%rip)     # Start the 64-bit code.

    .type   startup_64, @function
    .size   startup_64, . - startup_64

#

    .section ".rodata", "a"

    .global gdt
    .balign 16                  # Recommended by Intel.
gdt:
    .word   gdt_end - gdt - 1   # GDT size.
    .long   pa_gdt              # GDT base.
    .word   0                   # Padding to next GDT entry.
    .quad   GDT_ENTRY(0xC09A, 0, 0xFFFFF)   # '__BOOT32_CS', R/X, 4GiB, Base 0.
    .quad   GDT_ENTRY(0xA09A, 0, 0xFFFFF)   # '__BOOT_CS', 'long mode' enabled. 
    .quad   GDT_ENTRY(0xC092, 0, 0xFFFFF)   # '__BOOT_DS', R/W, 4GiB, Base 0.
gdt_end:
    .type   gdt_end, @object

    .type   gdt, @object
    .size   gdt, . - gdt

#

    .bss
rm_stack:
    .space  PAGE_SIZE
    .balign 16
rm_stack_end:
    .type   rm_stack_end, @object

    .type   rm_stack, @object
    .size   rm_stack, . - rm_stack

    .global trampoline_pgd
    .balign PAGE_SIZE
trampoline_pgd:
    .space  PAGE_SIZE

    .type   trampoline_pgd, @object
    .size   trampoline_pgd, . - trampoline_pgd

#
# Real-mode kernel parameters 'struct ap_config {...}'.
#

    .global ap_config
    .global efer
    .global cr4

    .balign 8
ap_config:

initial_code:
    .space 8                # 64-bit start address.
                            # Only accessed in 'startup_64' as 'RIP'-relative.

    .type   initial_code, @object
    .size   initial_code, . - initial_code

efer:
    .space 8                # 32-bit 'EFER' register.

    .type   efer, @object
    .size   efer, . - efer
    
cr4:
    .space 4                # 32-bit 'CR4' register.

    .type   cr4, @object
    .size   cr4, . - cr4

    .type   ap_config, @object
    .size   ap_config, . - ap_config
