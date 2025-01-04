# SPDX-License-Identifier: GPL-2.0-or-later

#include <asm-offsets.h>
#include <cyanea/cache.h>
#include <asm/entry_64.h>
#include <asm/irq_vector.h>
#include <asm/segment.h>

#
# X86 64-bit calling convention.
#
# caller-saved [callee-clobbered] RDI, RSI, RDX, RCX, R8 - R9: Used for arguments.
# caller-saved [callee-clobbered] R10 - R11.
# callee-saved RBX, RBP, R12 - R15.
#
# RAX and RDX are used for return value. Return values up to 64-bits in size are
# stored in RAX. Return values up to 128-bit are stored in RAX and RDX.
#
# For structures larger than 128-bit, the caller puts a pointer to an on-stack
# return struct [allocated in the caller's stack frame] into the first argument,
# i.e. into RDI. All other arguments shift up by one in this case.
#
#  RDI, RSI, RDX, RCX, R8 - R9 are used for arguments. Leftover arguments are
# passed over the stack frame.
#

# Pushes registers (except RDI and RSI).
.macro PUSH_REGS
	pushq	%rdx
	pushq	%rcx
	pushq	%rax
	pushq	%r8
	pushq	%r9
	pushq	%r10
	pushq	%r11
	pushq	%rbx
	pushq	%rbp
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
.endm

# Pops registers (except RSI and RDI).
.macro POP_REGS
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	popq	%rbx
	popq	%r11
	popq	%r10
	popq	%r9
	popq	%r8
	popq	%rax
	popq	%rcx
	popq	%rdx
.endm

# Pushes all registers.
.macro PUSH_REGS_ALL
	pushq	%rdi
	pushq	%rsi
	PUSH_REGS
.endm

# Pops all registers.
.macro POP_REGS_ALL
	POP_REGS
	popq	%rsi
	popq	%rdi
.endm

# Push all registers and keeps the return address on stack.
.macro PUSH_REGS_RETURN_ADDRESS
	pushq	%rsi		# Preserve the return address in RDI position; start from RSI.
	movq	8(%rsp), %rsi   # Store the return address in RSI.
	movq	%rdi, 8(%rsp)   # Store RDI in its position.
	PUSH_REGS
	pushq	%rsi		# Put the return address on the stack again.
.endm

#
# 'DECLARE_IDTENTRY'
# 'DECLARE_IDTENTRY_WITH_ERROR_CODE'
# 'DECLARE_IDTENTRY_IRQ' generates assembly handlers.
#

#define DECLARE_IDTENTRY(func) \
	IDTENTRY asm_##func func has_error_code=0

#define DECLARE_IDTENTRY_WITH_ERROR_CODE(func) \
	IDTENTRY asm_##func func has_error_code=1

#define DECLARE_IDTENTRY_IRQ(func) \
	IDTENTRY_IRQ func

	.section ".text.entry", "ax"

	.balign	16, 0x90
error_entry:
	PUSH_REGS_RETURN_ADDRESS	# Push all registers and keep return address on stack.

	testb	$3, utask_regs_cs+8(%rsp)	# ukernel or user entry!? (8 for return address)
	jz	.Lerror_entry_ukernel

.Lerror_entry_user:
	swapgs				# Swap GS for kernel.
	leaq	8(%rsp), %rdi		# Pass utask_regs as argument.
	jmp	switch_utask_stack	# Get utask's stack; directly returns to the caller.

.Lerror_entry_ukernel:

	# TODO. Fixups for entry from kernel, e.g. IRET.

	leaq	8(%rsp), %rax		# We are already on utask's stack.
	ret

	.type	error_entry, @notype			# not a function! 
	.size	error_entry, . - error_entry

	.balign	16, 0x90
common_interrupt_return:
return_to_user:
	POP_REGS

	popq	%rsi

	# Keep RDI on stack and use it for scratch.

	# Switch back to per-cpu stack.
	# The stack is RDI, orig_ax, RIP, CS, EFLAGS, RSP, SS.

	movq	%rsp, %rdi
	movq	%gs:cpu_tss+TSS_rsp0, %rsp

	pushq	6*8(%rdi)	# SS
	pushq	5*8(%rdi)	# RSP
	pushq	4*8(%rdi)	# EFLAGS
	pushq	3*8(%rdi)	# CS
	pushq	2*8(%rdi)	# RIP

	# All registers are valid except RDI; RDI holds the address of the utask's
	# stack where the value of RDI is stored.

	movq	(%rdi), %rdi

	swapgs			# Swap GS for user.
	jmp	.Lnative_iret

return_to_ukernel:
	POP_REGS_ALL

	addq	$8, %rsp	# Skip 'orig_ax'.

	# Return at same address, always!
.Lnative_iret:
	iretq

	.type	common_interrupt_return, @notype	# not a function! 
	.size	common_interrupt_return, . - common_interrupt_return

	.balign	16, 0x90
error_return:

	testb	$3, utask_regs_cs(%rsp)		# ukernel or user entry!?
	jz	return_to_ukernel
	jmp	return_to_user

	.type	error_return, @notype		# not a function! 
	.size	error_return, . - error_return

# 'IDTENTRY_BODY'
# 'IDTENTRY'
# 'IDTENTRY_IRQ'

.macro IDTENTRY_BODY cfunc has_error_code:req
	call	error_entry	# Returns utask's stack in RAX. 
	movq	%rax, %rsp

	# call 'cfunc(struct utask_regs *, int)'.

	movq	%rsp, %rdi
.if \has_error_code == 1
	movq	utask_regs_orig_rax(%rsp), %rsi
	movq	$-1, utask_regs_orig_rax(%rsp)	# See utask.h.
.endif

	call	\cfunc

	jmp	error_return
.endm

.macro IDTENTRY asmsym cfunc has_error_code:req
	.global	\asmsym
	.balign	16, 0x90
\asmsym:

	cld
.if \has_error_code == 0
	pushq	$-1
.endif

	IDTENTRY_BODY \cfunc \has_error_code

	.type	\asmsym, @notype		# not a function! 
	.size	\asmsym, . - \asmsym
.endm

.macro IDTENTRY_IRQ cfunc
	.balign	SMP_CACHE_BYTES

	IDTENTRY asm_\cfunc \cfunc has_error_code=1
.endm

###
# IDT Array; all call 'asm_common_interrupt'.

	.global	irq_entries_start
	.balign	16, 0x90
irq_entries_start:

	i = FIRST_EXTERNAL_VECTOR
.rept NR_EXTERNAL_VECTORS
1:	pushq	$i				# 6A 00 (2 bytes) 'i' range is [32 .. 127].
						# 68 00 00 00 00 (5 bytes) 'i' range is [-128 .. -1].
	jmp	asm_common_interrupt		# EB 00 (2 bytes for short jump) or
						# E9 00 00 00 00 (5 bytes for Neat jump).

.fill 1b + 16 - ., 1, 0x90
	i = i + 1
.endr

	.type	irq_entries_start, @notype	# not a function! 
	.size	irq_entries_start, . - irq_entries_start

# Interrupt Handlers (assembly entries).

DECLARE_IDTENTRY_IRQ(common_interrupt);
DECLARE_IDTENTRY_WITH_ERROR_CODE(exc_page_fault);

###
# Early IDT Array, only exception.

	.section ".init.text", "ax"

	.global	boot_idt_handler_array
	.balign	16, 0x90
boot_idt_handler_array:

	i = 0
.rept IDT_EXCEPTION_ENTRIES
1:
.if ((IDT_EXCEPTION_WITH_ERRCODE_MASK >> i) & 1) == 0
	pushq	$-1				# 6A FF (2 bytes) if no error code.
.endif
	pushq	$i				# 6A 00 (2 bytes) 'i' range is [0 .. 31].
	jmp	boot_idt_handler_common		# EB 00 (2 bytes for short jump) or
						# E9 00 00 00 00 (5 bytes for Neat jump).
	# Size of each entry in the array should be at least 9 (2 + 2 + 5) bytes to
	# fit all instructions (pushq + pushq + jmp).

.fill 1b + 9 - ., 1, 0x90
	i = i + 1
.endr

	.type	boot_idt_handler_array, @notype		# not a function! 
	.size	boot_idt_handler_array, . - boot_idt_handler_array

	.balign	16, 0x90
boot_idt_handler_common:

	# So far we have (1) hardware frame, (2) errorcode or -1, and (3) vector number.

	cld

	pushq	%rsi			# Preserve the vector number in RDI position; start from RSI.
	movq	8(%rsp), %rsi		# Store the vector number in RSI.
	movq	%rdi, 8(%rsp)		# Store RDI in its position.
	PUSH_REGS

	movq	%rsp, %rdi		# Put pt_regs pointer in 'RDI'.
	call	do_boot_exception	# 'do_boot_exception(struct utask_regs *, int)'.
	POP_REGS_ALL
	addq	$8, %rsp		# Skip 'orig_rax'.

	iretq

	.type	boot_idt_handler_common, @function
	.size	boot_idt_handler_common, . - boot_idt_handler_common
