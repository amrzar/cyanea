/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm/current.h>
#include <asm/utask.h>

/* We keep two stacks: (1) Per-cpu stack, and (2) utask's stack. */
/* Per-cpu stack is used for kernel entry and exit. */

struct utask_regs *switch_utask_stack(struct utask_regs *percpu_regs)
{
	struct utask_regs *regs =
	        ((struct utask_regs *)(this_cpu_read(percpu_hot.top_of_stack))) - 1;

	*regs = *percpu_regs;

	return regs;
}
