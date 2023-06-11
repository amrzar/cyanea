/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UART_H__
#define __UART_H__

#define UART_NR CONFIG_NR_UARTS

struct uart_port {
    char up_name[8];            /* Port name, e.g. 'ttyS', set by driver. */

#define PORT_UNKNOWN 0
#define PORT_8250    1
    unsigned int type;          /* Port type. */

#define UART_IO_PORT 0          /* 8-bit I/O port access. */
#define UART_IO_MEM  1          /* Driver specific access. */
    unsigned char io_type;      /* I/O ''access style''. */
    union {
        unsigned short port;
        unsigned char *mem;
    } io;

    int index;                  /* Port index, e.g. '0' for ttyS0 */
    unsigned int uart_clock;    /* Base UART clock. */

    void (*poll_putchar)(struct uart_port *, int);
};

extern struct uart_port uart_ports[UART_NR];

struct uart_port *__find_match_port(const struct uart_port *);
int register_uart_port(struct uart_port *);
void uart_poll_put_char(unsigned int, int);
int uart_io_parse_option(char *, unsigned char *, unsigned long *, char **);

#endif /* __UART_H__ */
