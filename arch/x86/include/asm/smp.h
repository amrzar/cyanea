/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_SMP_H__
#define __X86_ASM_SMP_H__

#ifndef __CYANEA_SMP_H__
#  error "Use 'cyanea/smp.h'"
#endif

#include <asm/current.h>

#define raw_smp_processor_id()  this_cpu_read(percpu_hot.cpu_number)
#define __smp_processor_id() __this_cpu_read(percpu_hot.cpu_number)

#endif /* __X86_ASM_SMP_H__ */
