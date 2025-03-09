/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_IDTENTRY_H__
#define __X86_ASM_IDTENTRY_H__

#include <asm/utask.h>

/**
 * @def DECLARE_IDTENTRY(func)
 * @brief Declare an IDT entry point function without an error code.
 *
 * This macro declares an IDT entry point function that does not receive an
 * error code. The corresponding assembly stub is defined in `entry_64.S`,
 * which typically performs the necessary low-level setup before calling the
 * specified C handler function.
 *
 * The declared function follows the convention of taking a pointer to
 * `struct utask_regs`, which holds the register state at the time of the
 * interrupt.
 *
 * @param func The function name of the entry point.
 */
#define DECLARE_IDTENTRY(func) \
	void asm_##func(void); \
	void func(struct utask_regs *regs)

/**
 * @def DEFINE_IDTENTRY(func)
 * @brief Emit code for the IDT entry point function without an error code.
 *
 * This macro defines an IDT entry point function that was previously declared
 * using @ref DECLARE_IDTENTRY.
 *
 * The function is called from the assembly entry code with interrupts disabled.
 * It defines both the public handler (`func`) and an inline implementation
 * function (`__##func`), which allows for common code that must be executed
 * before and after the handler.
 *
 * @param func The function name of the entry point.
 *
 * Example usage:
 * @code
 * DEFINE_IDTENTRY(my_interrupt_handler) {
 *     // Handle the interrupt
 * }
 * @endcode
 */
#define DEFINE_IDTENTRY(func) \
	static __always_inline void __##func(struct utask_regs *regs); \
	void func(struct utask_regs *regs) \
	{ \
		__##func(regs); \
	} \
	\
	static __always_inline void __##func(struct utask_regs *regs)

/**
 * @def DECLARE_IDTENTRY_WITH_ERROR_CODE(func)
 * @brief Declare an IDT entry points function with an error code.
 *
 * This macro functions similarly to @ref DECLARE_IDTENTRY, but the C handler
 * receives an additional `error_code` argument.
 *
 * @param func The function name of the entry point.
 */
#define DECLARE_IDTENTRY_WITH_ERROR_CODE(func) \
	void asm_##func(void); \
	void func(struct utask_regs *regs, unsigned long error_code)

/**
 * @def DEFINE_IDTENTRY_WITH_ERROR_CODE(func)
 * @brief Emit code for the IDT entry point function with an error code.
 *
 * This macro defines an IDT entry point function that was previously declared
 * using @ref DECLARE_IDTENTRY_WITH_ERROR_CODE.
 *
 * @param func The function name of the entry point.
 *
 * Example usage:
 * @code
 * DEFINE_IDTENTRY_WITH_ERROR_CODE(my_fault_handler) {
 *     // Handle the interrupt with the error code
 * }
 * @endcode
 */
#define DEFINE_IDTENTRY_WITH_ERROR_CODE(func) \
	static __always_inline void __##func(struct utask_regs *regs, \
	        unsigned long error_code); \
	void func(struct utask_regs *regs, unsigned long error_code) \
	{ \
		__##func(regs, error_code); \
	} \
	\
	static __always_inline void __##func(struct utask_regs *regs, \
	        unsigned long error_code)

/**
 * @def DECLARE_IDTENTRY_IRQ(func)
 * @brief Declare an IDT entry point function for a device interrupt.
 *
 * It is equivalent to @ref DECLARE_IDTENTRY_WITH_ERROR_CODE.
 *
 * @param func The function name of the entry point.
 */
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



#endif /* __X86_ASM_IDTENTRY_H__ */
