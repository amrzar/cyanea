/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_ENTRY_64_H__
#define __X86_ASM_ENTRY_64_H__

#ifndef __ASSEMBLY__
#include <asm/utask.h>

/* DECLARE_IDTENTRY - Declare IDT entry point without error code. */
#define DECLARE_IDTENTRY(func) \
	void asm_##func(void)

/* DEFINE_IDTENTRY - C simple IDT entry points. */
#define DEFINE_IDTENTRY(func) \
	void func(struct utask_regs *regs)

#define DECLARE_IDTENTRY_WITH_ERROR_CODE(func) \
	void asm_##func(void)

#define DEFINE_IDTENTRY_WITH_ERROR_CODE(func) \
	void func(struct utask_regs *regs, unsigned long error_code)

/* DECLARE_IDTENTRY_IRQ - Declare IDT entry point for device interrupt. */
#define DECLARE_IDTENTRY_IRQ(func) \
	DECLARE_IDTENTRY_WITH_ERROR_CODE(func)

/* DEFINE_IDTENTRY_IRQ - C device interrupt IDT entry points. */
#define DEFINE_IDTENTRY_IRQ(func)                                       \
	static void __##func(struct utask_regs *regs, u8 vector);       \
	\
	void func(struct utask_regs *regs, unsigned long error_code)    \
	{                                                               \
		__##func(regs, (u8)(error_code));                       \
	}                                                               \
	\
	static void __##func(struct utask_regs *regs, u8 vector)

#else /* !__ASSEMBLY__ */

#endif /* __ASSEMBLY__ */
#endif /* __X86_ASM_ENTRY_64_H__ */
