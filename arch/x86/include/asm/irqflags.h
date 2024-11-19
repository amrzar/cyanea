/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_IRQFLAGS_H__
#define __X86_ASM_IRQFLAGS_H__

#include <asm/cr_eflags_defs.h>

#ifndef __ASSEMBLY__

static __always_inline unsigned long arch_local_save_flags(void)
{
	unsigned long flags;

	asm volatile(
	        "pushf      ;"
	        "pop    %0  ;"
	        : "=rm" (flags)
	        : : "memory"
	);

	return flags;
}

static __always_inline void arch_local_irq_disable(void)
{
	asm volatile("cli" : : : "memory");
}

static __always_inline void arch_local_irq_enable(void)
{
	asm volatile("sti" : : : "memory");
}

static __always_inline int arch_irqs_disabled_flags(unsigned long flags)
{
	return !(flags & _EFLAGS_IF);
}

static __always_inline int arch_irqs_disabled(void)
{
	unsigned long flags = arch_local_save_flags();

	return arch_irqs_disabled_flags(flags);
}

static __always_inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags = arch_local_save_flags();
	arch_local_irq_disable();
	return flags;
}

static __always_inline void arch_local_irq_restore(unsigned long flags)
{
	if (!arch_irqs_disabled_flags(flags))
		arch_local_irq_enable();
}

static __always_inline void halt(void)
{
	asm volatile("hlt" : : : "memory");
}

#endif /* __ASSEMBLY__ */
#endif /* __X86_ASM_IRQFLAGS_H__ */
