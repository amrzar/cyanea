/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_UKERNEL_LDS_H__
#define __X86_ASM_UKERNEL_LDS_H__

/* Header that matches 'ukernel.lds.S'. */
/* Make sure to reflect any changes in 'ukernel.lds.S'. */

#ifndef __ASSEMBLY__

extern char _text[], _stext[], __end_text[];
extern char __start_rodata[], __end_rodata[];
extern char _sdata[], _edata[];
extern char __init_begin[], __init_end[];
extern char _sinittext[], _einittext[];
extern char __per_cpu_load[], __per_cpu_start[], __per_cpu_end[];
extern char __bss_start[], __bss_stop[];
extern char __end_of_kernel_reserve[];
extern char __brk_base[], __brk_limit[];
extern char _end[];

extern char __init_utask_stack[], __end_utask_init[];

extern char __ctors_start[], __ctors_end[];

#else /* !__ASSEMBLY__ */

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_UKERNEL_LDS_H__ */
