/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_INIT_H__
#define __CYANEA_INIT_H__

#ifndef __ASSEMBLY__
#include <cyanea/compiler.h>
#include <asm/setup.h>
#include <asm/cache.h>
#include <asm/page.h>

#define __init __section(".init.text")
#define __head __section(".head.text")
#define __initdata __section(".init.data")

#define __ro_after_init __section(".data..ro_after_init")

extern char __initdata boot_command_line[COMMAND_LINE_SIZE];

/* It is Linux obsolete parameter format; It's enough for us. */

struct ukernel_param {
    const char *param;
    int (*setup_func)(char *);
    int early;
};

/* Force the alignment so the compiler doesn't space elements of the 'ukernel_param'
 * array too far apart in '.init.setup'.
 */

#define __setup_param(str, unique_id, fn, early) \
    static const char __setup_str_##unique_id[] __initdata = str; \
    static struct ukernel_param __setup_##unique_id \
    __used __section(".init.setup") \
    __aligned(__alignof__(struct ukernel_param)) = \
        { __setup_str_##unique_id, fn, early }

/* 'fn' returns 'SUCCESS' if the option argument is handled. */

#define __setup(str, fn) __setup_param(str, fn, fn, 0)

#define early_param(str, fn) __setup_param(str, fn, fn, 1)

/* Constructors. */

#define __pure_constructor __attribute__((constructor(101)))    /* PURE: */
#define __arch_constructor __attribute__((constructor(110)))    /* ARCH: */

#endif /*  __ASSEMBLY__ */

#endif /* __CYANEA_INIT_H__ */
