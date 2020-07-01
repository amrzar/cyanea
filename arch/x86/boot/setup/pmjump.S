/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm/desc_defs.h>
#include <asm/segment.h>
#include <asm/cpu_registers.h>

    .code16

#
# 'protected_mode_jump', 'EAX' stores the address of 32-bit entry.
# 'EDX' stores runtime physical address of 'boot_params'.
#

    .global	protected_mode_jump
protected_mode_jump:

    movl    %edx, %esi      # Save 'EDX'.

    xorl    %ebx, %ebx      # Calculate runtime physical address of '.Lin_pm32'
    movw    %cs, %bx        # function by adding offset of '.Lin_pm32' to 'CS << 4'
    shll    $4, %ebx        # and store the result at offset '2', bellow, i.e.
    addl    %ebx, 2f        # patch the 'offset'.

    jmp     1f              # Short jump to serialize on older processors.
1:

    movw    $__BOOT_DS, %cx

    movl    %cr0, %edx
    orb     $_CR0_PE, %dl
    movl    %edx, %cr0

    .byte   0x66, 0xea      # 'LJMPL' opcode
2:  .long   .Lin_pm32       # Offset of '.Lin_pm32' in real-mode 'CS'.
    .word   __BOOT_CS

    .type   protected_mode_jump, @function
    .size   protected_mode_jump, . - protected_mode_jump

#

    .code32
    .section ".text32", "ax"

.Lin_pm32:

    # Set up all segments 'DS', 'ES', 'FS', 'GS', 'SS' to '__BOOT_DS'.

    movl    %ecx, %ds
    movl    %ecx, %es
    movl    %ecx, %fs
    movl    %ecx, %gs
    movl    %ecx, %ss

    addl    %ebx, %esp      # We just moved to protected-mode with 'SS' set to
                            # 4GiB segment and base 0. 'EBX' which is 'CS << 4'
                            # set above is added to 'ESP' to get physical address
                            # in this segment.  
    
    xorl    %ecx, %ecx
    xorl    %edx, %edx
    xorl    %ebx, %ebx
    xorl    %ebp, %ebp
    xorl    %edi, %edi
    
    jmpl    *%eax           # Jump to the 32-bit entrypoint.

    .type   .Lin_pm32, @function
    .size   .Lin_pm32, . - .Lin_pm32
