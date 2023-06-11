/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/params.h>

#include <asm/ukernel.lds.h>

char __initdata boot_command_line[COMMAND_LINE_SIZE];

static void __init do_ctors(void)
{
    typedef void (*ctor_fn_t)(void);

    ctor_fn_t *fn;

    for (fn = (ctor_fn_t *) __ctors_start; fn < (ctor_fn_t *) __ctors_end; fn++)
        (*fn)();
}

extern const struct ukernel_param __setup_start[], __setup_end[];
static void __init do_param(int early)
{
    char param[64];
    const struct ukernel_param *p;

    for (p = __setup_start; p < __setup_end; p++) {
        int err;

        if (p->early != early)
            continue;

        err = param_get_option(boot_command_line, p->param, param, sizeof(param));
        if (err < 0)
            continue;

        p->setup_func(param);
    }
}

extern const char ukernel_banner[];

void __init start_kernel(void)
{
    do_param(1);

    do_ctors();

    ulog("\n%s", ukernel_banner);

    while (1) ;
}
