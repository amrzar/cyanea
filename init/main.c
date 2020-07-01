/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/params.h>
#include <asm/sections.h>

char __initdata boot_command_line[COMMAND_LINE_SIZE];

static void __init do_ctors(void)
{
    ctor_fn_t *fn = (ctor_fn_t *) __ctors_start;

    for (; fn < (ctor_fn_t *) __ctors_end; fn++)
        (*fn) ();
}

extern const struct kernel_param __setup_start[], __setup_end[];
static void __init do_param(int early)
{
    char buf[64];
    const struct kernel_param *p;

    for (p = __setup_start; p < __setup_end; p++) {
        if (p->early == early) {
            if (param_get_option(boot_command_line, p->param, buf,
                    sizeof(buf)) < 0)
                continue;       /* 'p->param' does not exist. */

            p->setup_func(buf);
        }
    }
}

extern void __init setup_arch(char[]);
void __init start_kernel(void)
{

    setup_arch(boot_command_line);
    //do_param(1);

    //do_ctors();

    while (1) ;
}
