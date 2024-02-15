/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/console.h>
#include <cyanea/stddef.h>
#include <cyanea/errno.h>
#include <cyanea/init.h>
#include <cyanea/ctype.h>
#include <cyanea/strtox.h>

#include <ulib/string.h>

#include "uart.h"

#define MAX_CONSOLE 8           /* .. */

static struct preferred_console {
    char name[8];
    int index;
    int user_specified;         /* Requested on command-line. */
    char options[64];
} pcs[MAX_CONSOLE] = { 0 };

static struct console *active_console = NULL;

static int __add_preferred_console(char *name, int index, char *options,
    int user_specified)
{
    int i;
    struct preferred_console *pc = pcs;

    for (i = 0; i < MAX_CONSOLE && pc->name[0]; i++, pc++) {
        if (!strcmp(pc->name, name) && (pc->index == index)) {
            if (user_specified)
                pc->user_specified = 1;

            return SUCCESS;
        }
    }

    if (i == MAX_CONSOLE)
        return -ENOSPC;

    strncpy(pc->name, name, sizeof(pc->name));
    pc->index = index;
    pc->user_specified = user_specified;
    strncpy(pc->options, options, sizeof(pc->options));

    return SUCCESS;
}

static int __init console_setup(char *arg)
{
    unsigned long index;
    char buf[16] = { '\0' };

    /* ''console=<name><index>,<options>''. */
    /* ''console=<name>,<options>''. */

    char *s, *options = strchr(arg, ',');
    if (options)
        *(options++) = '\0';

    strncpy(buf, arg, sizeof(buf) - 1);   /* Keep '\0' for ending. */
    for (s = buf; *s; s++)
        if (isdigit(*s))
            break;

    strtoul(s, 0, &index);
    *s = '\0';

    /* Passing <name>, <index>, and <option>. */

    return __add_preferred_console(buf, index, options, 1);
}

early_param("console", console_setup);

int add_preferred_console(char *name, int index, char *options)
{
    return __add_preferred_console(name, index, options, 0);
}

static int try_enable_preferred_console(struct console *con, int user_specified)
{
    int i, err;
    struct preferred_console *pc = pcs;

    for (i = 0; i < MAX_CONSOLE && pc->name[0]; i++, pc++) {
        if (pc->user_specified != user_specified)
            continue;

        if (!con->match || con->match(con, pc->name, pc->options)) {
            if (!strcmp(pc->name, con->name))
                continue;

            if (con->index >= 0) {

                /* Check if exact port was requested!? */
                if (con->index != pc->index)
                    continue;
            } else {
                con->index = pc->index;
            }

            if (con->setup) {
                err = con->setup(con, pc->options);
                if (err)
                    return err;
            }
        }

        return SUCCESS;
    }

    return -ENOENT;
}

int register_console(struct console *con)
{
    if (try_enable_preferred_console(con, 1) &&
        try_enable_preferred_console(con, 0))
        return -ENOENT;

    if (active_console) {
        if (active_console->exit)
            active_console->exit(active_console);
    }

    active_console = con;

    return SUCCESS;
}

int console_write(const char *s, size_t count)
{
    int i;

    if (!active_console)
        return -EINVAL;

    for (i = 0; i < count; i++, s++)
        uart_poll_put_char(active_console->index, *s);

    return SUCCESS;
}
