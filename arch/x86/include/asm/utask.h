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

struct utask_regs {
	unsigned long registers[NR_REGISTERS];

	/* On syscall entry, this is syscall number,
	 * On CPU exception, this is error code (default to -1 if no err.), and
	 * On interrupt, this is IRQ number.
	 */

	/* It is set to -1 for CPU exception and interrupt before calling the
	 * C function as the original value is passed as second argument.
	 * It is used to differentiate between syscall entry (!= -1) from others.
	 */

	unsigned long orig_rax;

	/* Return frame for IRETQ. */

	unsigned long rip;
	unsigned long cs;
	unsigned long eflags;
	unsigned long rsp;
	unsigned long ss;
};

struct utask {

};

#endif /* __X86_ASM_UTASK_H__ */
