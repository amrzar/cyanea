/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_CONSOLE_H__
#define __CYANEA_CONSOLE_H__

#include <cyanea/types.h>

struct console {
    char name[8];               /* Console port name, e.g. 'ttyS' */

    short flags;                /* UNUSED. */

    int index;                  /* Port index, '-1' matches any ports with same name. */
    unsigned int speed;         /* Port baud rate for the console. */

    int (*setup)(struct console *, char *);
    int (*match)(struct console *, char *, char *);
    int (*exit)(struct console *);
};

int add_preferred_console(char *, int, char *);
int register_console(struct console *);
void console_write(const char *, size_t);

#endif /* __CYANEA_CONSOLE_H__ */
