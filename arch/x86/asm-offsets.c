/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASM_OFFSET_BUILD
# define __ASM_OFFSET_BUILD
#endif /* __ASM_OFFSET_BUILD */

#include <asm-offsets-defs.h>

#include <cyanea/stddef.h>
#include <cyanea/compiler.h>

#include <uapi/asm/bootparam.h>
#include <asm/current.h>

static void __always_unused unused_common_function(void)
{
    /* struct boot_params. */
    SIZE_OF(BP_size, boot_params);
    OFFSET(BP_scratch, boot_params, scratch);
    OFFSET(BP_kernel_alignment, boot_params, hdr.kernel_alignment);
    OFFSET(BP_init_size, boot_params, hdr.init_size);
    OFFSET(BP_cmd_line_ptr, boot_params, hdr.cmd_line_ptr);

    /* struct percpu_hot. */
    OFFSET(X86_top_of_stack, percpu_hot, top_of_stack);
    OFFSET(X86_current_task, percpu_hot, current_task);
}

#ifdef CONFIG_X86_64
#include <asm/utask.h>

static void __always_unused unused_x86_64_function(void)
{
    OFFSET(TSS_rsp0, tss, tss.rsp0);
    OFFSET(TSS_rsp1, tss, tss.rsp1);
    OFFSET(TSS_rsp2, tss, tss.rsp2);

# define ENTRY(entry) OFFSET(utask_regs_ ## entry, utask_regs, entry)
    ENTRY(cs);
    ENTRY(orig_rax);
# undef ENTRY
}
#endif /* CONFIG_X86_64 */

#undef __ASM_OFFSET_BUILD
