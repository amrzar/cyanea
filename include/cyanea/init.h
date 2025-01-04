/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_INIT_H__
#define __CYANEA_INIT_H__

#ifndef __ASSEMBLY__

#include <asm/setup.h>

#define __init __section(".init.text")
#define __head __section(".head.text")
#define __initdata __section(".init.data")
#define __initconst __section(".init.rodata")

#define __ro_after_init __section(".data..ro_after_init")

extern char __initdata boot_command_line[COMMAND_LINE_SIZE];

/* ''THIS IS SIMILAR TO LINUX OBSOLETE PARAMETER FORMAT''. */

struct setup_param {
	const char *param;
	int (*setup_func)(char *);
	int early;
};

extern const struct setup_param __setup_start[], __setup_end[];

#define __setup_param(str, uid, f, early)                       \
	static const char __sstr_ ## uid[] __initconst = str;   \
	static struct setup_param __sp_ ## uid __used           \
	__aligned(__alignof__(struct setup_param))              \
	__section(".init.setup") = { __sstr_ ## uid, f, early }

#define __setup(str, f)     __setup_param(str, f, f, 0)
#define early_param(str, f) __setup_param(str, f, f, 1)

/* Constructors. */

#define __PURE_CONSTRUCTOR__ __attribute__((constructor(101)))    /* PURE: */

#endif /*  __ASSEMBLY__ */
#endif /* __CYANEA_INIT_H__ */
