/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CURRENT_H__
#define __X86_ASM_CURRENT_H__

#include <cyanea/percpu.h>
#include <cyanea/compiler.h>

#include <asm/utask.h>
#include <asm/cache.h>

struct percpu_hot {
    union {
        struct {
            struct utask *current_task;

            /* Current stack used on kernel entry: set on context switch. */
            unsigned long top_of_stack;
        };

        /* Let's keep everything in a single cache line. */
        char pad[L1_CACHE_BYTES];
    };
};

static_assert(sizeof(struct percpu_hot) == L1_CACHE_BYTES,
    "percpu_hot is larger than a cache line!");

extern struct percpu_hot percpu_hot __percpu_cache_aligned;

# define current get_current
static always_inline struct utask *get_current(void)
{
    return this_cpu_read(percpu_hot.current_task);
}

#endif /* __X86_ASM_CURRENT_H__ */
