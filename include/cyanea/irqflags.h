/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_IRQFLAGS_H__
#define __CYANEA_IRQFLAGS_H__

#include <asm/irqflags.h>

#ifndef __ASSEMBLY__

#define local_save_flags(flags) \
	do { \
		flags = arch_local_save_flags(); \
	} while (0)

#define local_irq_disable() arch_local_irq_disable()
#define local_irq_enable() arch_local_irq_enable()

#define irqs_disabled_flags(flags) arch_irqs_disabled_flags(flags)
#define irqs_disabled() arch_irqs_disabled()

#define local_irq_save(flags) \
	do { \
		flags = arch_local_irq_save(); \
	} while (0)

#define local_irq_restore(flags) arch_local_irq_restore(flags)

#endif /* __ASSEMBLY__ */
#endif /* __CYANEA_IRQFLAGS_H__ */
