/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_CONSOLE_H__
#define __CYANEA_CONSOLE_H__

#include <cyanea/stddef.h>

struct console {
	char name[8];               /* Console port name, e.g. 'ttyS' */
	short flags;                /* UNUSED. */
	int index;                  /* Port index, '-1' matches any ports with same name. */
	unsigned int speed;         /* Port baud rate for the console. */

	void (*write)(struct console *, const char *, size_t);
	int (*setup)(struct console *, char *);
	int (*match)(struct console *, char *, char *);
	int (*exit)(struct console *);
};

int add_preferred_console(char *name, int index, char *options);
int register_console(struct console *con);
int console_write(const char *s, size_t count);

#endif /* __CYANEA_CONSOLE_H__ */
