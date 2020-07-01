/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_INIT_H__
#define __CYANEA_INIT_H__

#ifndef __ASSEMBLY__
#include <cyanea/compiler.h>
#include <asm/setup.h>
#include <asm/cache.h>

#define __init __section(".init.text")
#define __head __section(".head.text")
#define __initdata __section(".init.data")

#define __ro_after_init __section(".data..ro_after_init")

#define __percpu __section(".data..percpu")
#define __percou_smp __section(".data..percpu") __aligned(L1_CACHE_BYTES)
#define __percpu_page_aligned __section(".data..percpu..page_aligned")

extern char __initdata boot_command_line[COMMAND_LINE_SIZE];

typedef void (*ctor_fn_t)(void);
struct kernel_param {
    const char *param;
    int (*setup_func)(char *);
    int early;
};

#define __setup_param(str, uid, fn, early) \
    static const char __setup_n_##uid[] __initdata = str; \
    static struct kernel_param __setup_##uid __used __section(".init.setup") \
        __aligned(__alignof__(struct kernel_param)) \
            = { __setup_n_##uid, fn, early };

#define __setup(str, fn) __setup_param(str, fn, fn, 0)
#define __setup_early(str, fn) __setup_param(str, fn, fn, 1)

#endif /*  __ASSEMBLY__ */

#endif /* __CYANEA_INIT_H__ */
