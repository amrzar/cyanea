/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_ENTRY_64_H__
#define __X86_ASM_ENTRY_64_H__

/* ADD new interrupt handler.
 *
 * (1) Use 'DECLARE_IDTENTRY_X(func)' in entry_64.S for assembly handler 'asm_func'.
 * (2) Add 'DECLARE_IDTENTRY_X(func)' in asm/irq.c and add the IDT entry for 'asm_func'.
 * (3) Add 'DEFINE_IDTENTRY_X(func) { ... }' in respective code for C handler.
 *
 * Handler is called with interrupts disable.
 */

#ifndef __ASSEMBLY__

#include <cyanea/irq.h>
#include <asm/utask.h>

/* DECLARE_IDTENTRY - Declare IDT entrypoint without error code. */
# define DECLARE_IDTENTRY(func) \
    void asm_##func(void)

/* DEFINE_IDTENTRY - C simple IDT entry points. */
# define DEFINE_IDTENTRY(func) \
    void func(struct utask_regs *regs)

/* DECLARE_IDTENTRY_WITH_ERROR_CODE and DEFINE_IDTENTRY_WITH_ERROR_CODE.
 * Same as DECLARE_IDTENTRY and DEFINE_IDTENTRY but with error code.
 */

# define DECLARE_IDTENTRY_WITH_ERROR_CODE(func) \
    void asm_##func(void)

# define DEFINE_IDTENTRY_WITH_ERROR_CODE(func) \
    void func(struct utask_regs *regs, unsigned long error_code)

/* DECLARE_IDTENTRY_IRQ - Declare functions for device interrupt IDT entrypoint. */
# define DECLARE_IDTENTRY_IRQ(func) \
    DECLARE_IDTENTRY_WITH_ERROR_CODE(func)

/* DEFINE_IDTENTRY_IRQ - C device interrupt IDT entry points. */
# define DEFINE_IDTENTRY_IRQ(func) \
    static void __##func(struct utask_regs *regs, u8 vector); \
    \
    void func(struct utask_regs *regs, unsigned long error_code) \
    { \
        __##func(regs, (u8) error_code); \
    } \
    \
    static void __##func(struct utask_regs *regs, u8 vector)

#else /* !__ASSEMBLY__ */

/* *INDENT-OFF* */

/* X86 64-bit calling convention. */

/* caller-saved [callee-clobbered] RDI, RSI, RDX, RCX, R8 - R9: Used for arguments.
 * caller-saved [callee-clobbered] R10 - R11.
 * callee-saved RBX, RBP, R12 - R15.
 *
 * RAX and RDX are used for return value. Return values up to 64-bits in size are
 * stored in RAX. Return values up to 128-bit are stored in RAX and RDX.
 *
 * For structures larger than 128-bit, the caller puts a pointer to an on-stack
 * return struct [allocated in the caller's stack frame] into the first argument,
 * i.e. into RDI. All other arguments shift up by one in this case.
 *
 *  RDI, RSI, RDX, RCX, R8 - R9 are used for arguments. Leftover arguments are
 * passed over the stack frame.
 */

/* 'PUSH_REG' pushes registers except RDI and RSI. */
.macro PUSH_REGS
    pushq   %rdx
    pushq   %rcx
    pushq   %rax
    pushq   %r8
    pushq   %r9
    pushq   %r10
    pushq   %r11
    pushq   %rbx
    pushq   %rbp
    pushq   %r12
    pushq   %r13
    pushq   %r14
    pushq   %r15
.endm # PUSH_REGS

/* 'PUSH_REGS_ALL' pushes all registers. */
.macro PUSH_REGS_ALL
    pushq   %rdi
    pushq   %rsi
    PUSH_REGS
.endm # PUSH_REGS_ALL

/* 'PUSH_REGS_RETURN_ADDRESS' keeps the return address on stack. */
.macro PUSH_REGS_RETURN_ADDRESS
    pushq   %rsi    # Preserve the return address in RDI position; start from RSI.
    movq    8(%rsp), %rsi   # Store the return address in RSI.
    movq    %rdi, 8(%rsp)   # Store RDI in its position.
    PUSH_REGS
    pushq   %rsi    # Put the return address on the stack again.
.endm # PUSH_REGS_RETURN_ADDRESS

/* 'POP_REGS' pops registers except RSI and RDI. */
.macro POP_REGS
    popq    %r15
    popq    %r14
    popq    %r13
    popq    %r12
    popq    %rbp
    popq    %rbx
    popq    %r11
    popq    %r10
    popq    %r9
    popq    %r8
    popq    %rax
    popq    %rcx
    popq    %rdx
.endm # POP_REGS

/* 'POP_REGS_ALL' pops all registers. */
.macro POP_REGS_ALL
    POP_REGS
    popq    %rsi
    popq    %rdi    
.endm # POP_REGS_ALL

/* 'DECLARE_IDTENTRY'
 * 'DECLARE_IDTENTRY_WITH_ERROR_CODE'
 * 'DECLARE_IDTENTRY_IRQ' generates assembly handlers in entry_64.S.
 */

# define DECLARE_IDTENTRY(func) \
    IDTENTRY asm_##func func has_error_code=0

# define DECLARE_IDTENTRY_WITH_ERROR_CODE(func) \
    IDTENTRY asm_##func func has_error_code=1

# define DECLARE_IDTENTRY_IRQ(func) \
    IDTENTRY_IRQ func

/* *INDENT-ON* */

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_ENTRY_64_H__ */
