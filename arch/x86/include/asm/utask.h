/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_UTASK_H__
#define __X86_ASM_UTASK_H__

#include <asm/cpu.h>

enum {
	/* These regs are callee-preserved. */
	R15, R14, R13, R12, RBP, RBX,
	/* These regs are callee-clobbered. */
	R11, R10, R9, R8, RAX, RCX, RDX, RSI, RDI,
	NR_REGISTERS
};

/**
 * @brief Stores the CPU register state for a user task.
 *
 * This structure is used to save and restore the state of registers
 * during task switching, system calls, exceptions, and interrupts.
 */
struct utask_regs {
	/**
	 * @brief General-purpose registers.
	 *
	 * Stores the values of all general-purpose registers.
	 */
	unsigned long registers[NR_REGISTERS];

	/**
	 * @brief Differentiates between syscall, exception, and interrupt.
	 *
	 * - On syscall entry, this stores the syscall number.
	 * - On a CPU exception, this stores the error code (or -1 if none).
	 * - On an interrupt, this stores the IRQ number.
	 *
	 * Before calling the C function, this is set to -1 for CPU exceptions
	 * and interrupts, as the original value is passed as a second argument.
	 * It is used to distinguish syscall entries (when != -1) from others.
	 */
	unsigned long orig_rax;

	unsigned long rip;
	unsigned long cs;
	unsigned long eflags;
	unsigned long rsp;
	unsigned long ss;
};

struct utask {

};

#endif /* __X86_ASM_UTASK_H__ */
