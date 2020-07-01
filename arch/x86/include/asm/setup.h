/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_SETUP_H__
#define __X86_ASM_SETUP_H__

#define COMMAND_LINE_SIZE 2048

#ifndef __ASSEMBLY__
#include <uapi/asm/bootparam.h>

extern struct boot_params boot_params;

extern unsigned long _brk_start, _brk_end;

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_SETUP_H__ */
