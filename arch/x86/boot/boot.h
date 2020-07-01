/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_BOOT_BOOT_H__
#define __X86_BOOT_BOOT_H__

#include <cyanea/pgtable.h>

#define BOOT_HEAP_SIZE 0x10000

#ifdef CONFIG_X86_32
#define BOOT_STACK_SIZE 0x1000

#define BOOT_ALIGN_SHIFT PAGE_SHIFT     /* 'min_alignment'. */

#else
#define BOOT_STACK_SIZE 0x4000
#define BOOT_PGT_SIZE   0x6000

#define BOOT_ALIGN_SHIFT PMD_SHIFT      /* 'min_alignment'. */
#endif /* CONFIG_X86_32 */

#endif /* __X86_BOOT_BOOT_H__ */
