/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/stddef.h>
#include <cyanea/string.h>
#include <cyanea/strtox.h>
#include <cyanea/errno.h>

#include "uart.h"

/* ''uart_core.c'' is only used for TX and debug console. */

struct uart_port uart_ports[UART_NR] = { 0 };

int uart_io_parse_option(char *p, unsigned char *io_type, unsigned long *addr,
    char **options)
{
    if (!strncmp(p, "io,", 3)) {        /* ''io,<addr>,<options>''. */
        *io_type = UART_IO_PORT;

        p += 3;
    } else if (!strncmp(p, "0x", 2)) {  /* ''0x<addr>,<options>''.  */
        *io_type = UART_IO_MEM;

    } else
        return -EINVAL;

    if (strtoul(p, 0, addr) < 0)
        return -EINVAL;

    if ((p = strchr(p, ',')))
        p++;

    *options = p;

    return SUCCESS;
}

static int uart_match_port(const struct uart_port *up1,
    const struct uart_port *up2)
{
    if (up1->io_type != up2->io_type)
        return 0;

    switch (up1->io_type) {
    case UART_IO_PORT:
        return up1->io.port == up2->io.port;

    case UART_IO_MEM:
        return up1->io.mem == up2->io.mem;

    default:

        break;
    }

    return 0;
}

struct uart_port *__find_match_port(const struct uart_port *up)
{
    int i;

    for (i = 0; i < UART_NR; i++) {
        if (uart_match_port(&uart_ports[i], up))
            return &uart_ports[i];
    }

    return NULL;
}

void uart_poll_put_char(unsigned int index, int ch)
{
    struct uart_port *up = &uart_ports[index];

    if (ch == '\n')
        up->__poll_putchar(up, '\r');
    up->__poll_putchar(up, ch);
}

int register_uart_port(struct uart_port *up)
{
    int i;

    if (up->uart_clock == 0)
        return -EINVAL;

    if (__find_match_port(up))
        return -EEXIST;

    for (i = 0; i < UART_NR; i++) {
        if (uart_ports[i].type == PORT_UNKNOWN)
            break;
    }

    if (i == UART_NR)
        return -ENOSPC;

    uart_ports[i].index = i;
    uart_ports[i].type = up->type;
    uart_ports[i].io_type = up->io_type;
    uart_ports[i].io = up->io;
    uart_ports[i].uart_clock = up->uart_clock;
    uart_ports[i].__poll_putchar = up->__poll_putchar;
    strcpy(uart_ports[i].up_name, up->up_name);

    return SUCCESS;
}
