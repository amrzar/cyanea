/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_SECTIONS_H__
#define __X86_ASM_SECTIONS_H__

/** ''ukernel.lds.S'' **/

extern char _text[], _stext[], __end_text[];
extern char __start_rodata[], __end_rodata[];
extern char _sdata[], _edata[];
extern char __init_begin[], __init_end[];
extern char _sinittext[], _einittext[];
extern char __bss_start[], __bss_stop[];
extern char __end_of_kernel_reserve[];
extern char _end[];

extern char __brk_base[], __brk_limit[];

extern char __init_utask_stack[], __end_utask_init[];

extern char __ctors_start[], __ctors_end[];

#endif /* __X86_ASM_SECTIONS_H__ */
