# SPDX-License-Identifier: GPL-2.0-or-later

#include <cyanea/cache.h>

#include <asm-offsets.h>
#include <asm/entry_64.h>
#include <asm/irq.h>

    .section ".text.entry", "ax"

#
# 'error_entry' Switch GS (if necessary), save registers, and return the
# current utask's stack in RAX.
#

    .balign 16, 0x90
error_entry:
    PUSH_REGS_RETURN_ADDRESS            # Push all registers and keep return address on stack.

    testb   $3, utask_regs_cs+8(%rsp)   # ukernel or user entry!? (8 for return address)
    jz      .Lerror_entry_ukernel

.Lerror_entry_user:
    swapgs          # Swap GS for kernel.

    leaq    8(%rsp), %rdi               # Pass utask_regs as argument.
    jmp     switch_utask_stack          # Get utask's stack; directy returns to the caller.

.Lerror_entry_ukernel:

    # TODO. Fixups for entry from kernel, e.g. IRET.

    leaq    8(%rsp), %rax               # We are already on utask's stack.
    ret

    .type   error_entry, @notype            # not a function! 
    .size   error_entry, . - error_entry

#

    .balign 16, 0x90
common_interrupt_return:
return_to_user:
    POP_REGS
    popq    %rsi

    # Keep RDI on stack and use it for scratch.

    # Switch back to percpu stack.
    # The stack is RDI, orig_ax, RIP, CS, EFLAGS, RSP, SS.

    movq    %rsp, %rdi
    movq    %gs:cpu_tss+TSS_rsp0, %rsp

    pushq   6*8(%rdi)   # SS
    pushq   5*8(%rdi)   # RSP
    pushq   4*8(%rdi)   # EFLAGS
    pushq   3*8(%rdi)   # CS
    pushq   2*8(%rdi)   # RIP

    # All registers are valid except RDI. RDI holds the address of the utasks's
    # stack where the value of RDI is stored.

    movq    (%rdi), %rdi

    swapgs          # Swap GS for user.

    jmp     .Lnative_iret

return_to_ukernel:
    POP_REGS_ALL

    addq    $8, %rsp    # Skip 'orig_ax'.

    # Return at same address, always!

.Lnative_iret:
    iretq

    .type   common_interrupt_return, @notype    # not a function! 
    .size   common_interrupt_return, . - common_interrupt_return

#

    .balign 16, 0x90
error_return:

    testb   $3, utask_regs_cs(%rsp)     # ukernel or user entry!?
    jz      return_to_ukernel
    jmp     return_to_user

    .type   error_return, @notype            # not a function! 
    .size   error_return, . - error_return

# 'IDTENTRY_BODY'
# 'IDTENTRY'
# 'IDTENTRY_IRQ'

.macro IDTENTRY_BODY cfunc has_error_code:req
    call    error_entry     # Returns utask's stack in RAX. 
    movq    %rax, %rsp

    # call 'cfunc(struct utask_regs *, int)'.

    movq    %rsp, %rdi
    .if \has_error_code == 1
        movq    utask_regs_orig_rax(%rsp), %rsi

        # Set 'orig_rax' to -1 so we can differentiate this from standard syscall.
        # See utask.h.

        movq    $-1, utask_regs_orig_rax(%rsp)
    .endif

    call    \cfunc

    jmp     error_return
.endm # IDTENTRY_BODY

.macro IDTENTRY asmsym cfunc has_error_code:req
    .global \asmsym
    .balign 16, 0x90
\asmsym:

    cld
    .if \has_error_code == 0
        pushq   $-1
    .endif

    IDTENTRY_BODY \cfunc \has_error_code

    .type   \asmsym, @notype                # not a function! 
    .size   \asmsym, . - \asmsym
.endm # IDTENTRY

.macro IDTENTRY_IRQ cfunc
    .balign SMP_CACHE_BYTES
    IDTENTRY asm_\cfunc \cfunc has_error_code=1
.endm # IDTENTRY_IRQ

# IDT Array; all call 'asm_common_interrupt'.

    .global irq_entries_start
    .balign 16, 0x90
irq_entries_start:

        vector = FIRST_EXTERNAL_VECTOR
    .rept NR_EXTERNAL_VECTORS
1:      pushq   $vector
        jmp     asm_common_interrupt

    .fill 1b + 16 - ., 1, 0x90
        vector = vector + 1
    .endr

    .type   irq_entries_start, @notype      # not a function! 
    .size   irq_entries_start, . - irq_entries_start

# Interrupt Handlers (assembly entries).

DECLARE_IDTENTRY_IRQ(common_interrupt);

DECLARE_IDTENTRY_WITH_ERROR_CODE(exc_page_fault);
