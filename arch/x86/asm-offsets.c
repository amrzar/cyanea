/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/stddef.h>
#include <build.h>

#include <uapi/asm/bootparam.h>
#include <asm/current.h>

static void __always_unused unused_common_function(void)
{
    /* struct percpu_hot. */
    DEFINE(X86_top_of_stack, offsetof(struct percpu_hot, top_of_stack));
    DEFINE(X86_current_task, offsetof(struct percpu_hot, current_task));
}

#ifdef CONFIG_X86_64
#include <asm/utask.h>

static void __always_unused unused_x86_64_function(void)
{
    DEFINE(TSS_rsp0, offsetof(struct tss, tss.rsp0));
    DEFINE(TSS_rsp1, offsetof(struct tss, tss.rsp1));
    DEFINE(TSS_rsp2, offsetof(struct tss, tss.rsp2));

    DEFINE(utask_regs_cs, offsetof(struct utask_regs, cs));
    DEFINE(utask_regs_orig_rax, offsetof(struct utask_regs, orig_rax));
}
#endif /* CONFIG_X86_64 */
