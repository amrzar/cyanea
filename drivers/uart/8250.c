/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/console.h>
#include <cyanea/errno.h>
#include <cyanea/const.h>
#include <cyanea/init.h>

#include <asm/io.h>
#include <asm/uart.h>

#include <cyanea/string.h>

#include "../uart.h"
#include "uart_regs.h"

# define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof((arr)[0]))

# define __DRIVER_ID__ "ttyS"

static const struct {
	unsigned int baud_base;
	unsigned short port;
} serial_ports[] = {
	UART_PORTS
};

# define get_divisor(up, s) ((up)->uart_clock / ((s) * 16))

static unsigned int uart_in(struct uart_port *up, int offset)
{
	switch (up->io_type) {
	case UART_IO_PORT:
		return inb_p(up->io.port + offset);
		break;

	case UART_IO_MEM:
	default:
		return 0;
	}
}

static void uart_out(struct uart_port *up, int offset, int value)
{
	switch (up->io_type) {
	case UART_IO_PORT:
		outb_p(value, up->io.port + offset);
		break;

	case UART_IO_MEM:
	default:

		break;
	}
}

static void wait_for_lsr(struct uart_port *up, char bits)
{
	unsigned int status, t = 10000;

	while (1) {
		status = uart_in(up, UART_LSR);
		if (((status & bits) == bits) || (--t == 0))
			break;

		__builtin_ia32_pause();
	}
}

static void set_speed(struct uart_port *up, unsigned int speed)
{
	unsigned int divisor = get_divisor(up, speed);
	unsigned char c = uart_in(up, UART_LCR);

	uart_out(up, UART_LCR, c | UART_LCR_DLAB);
	uart_out(up, UART_DLL, (divisor & 0xFF));
	uart_out(up, UART_DLH, (divisor >> 8) & 0xFF);
	uart_out(up, UART_LCR, c);
}

static void putchar(struct uart_port *up, int c)
{
	wait_for_lsr(up, UART_LSR_THRE);

	uart_out(up, UART_THR, c);
}

static int setup_port(struct uart_port *up, unsigned int speed)
{
	uart_out(up, UART_IER, 0x0);        /* Disable all interrupts. */
	wait_for_lsr(up, UART_LSR_TEMT | UART_LSR_THRE);

	uart_out(up, UART_MCR, 0x0);        /* Reset 'DTR' and 'RTS'. */
	uart_out(up, UART_FCR, 0x0);        /* Disable FIFOs. */
	uart_out(up, UART_LCR,
	        UART_LCR_WLEN8);     /* '8n1' 8-bits word, no parity, 1 stop bit. */

	set_speed(up, speed);
	uart_out(up, UART_MCR, UART_MCR_DTR | UART_MCR_RTS);

	return SUCCESS;
}

static int exit_port(struct uart_port *up)
{
	wait_for_lsr(up, UART_LSR_TEMT | UART_LSR_THRE);

	uart_out(up, UART_MCR, 0x0);

	return SUCCESS;
}

static void __init init_ports(void)
{
	int i;
	struct uart_port up = {
		.up_name = __DRIVER_ID__,
		.type = PORT_8250,
		.io_type = UART_IO_PORT,
		.poll_putchar = putchar
	};

	for (i = 0; i < ARRAY_SIZE(serial_ports); i++) {
		up.io.port = serial_ports[i].port;
		up.uart_clock = serial_ports[i].baud_base * 16;

		register_uart_port(&up);
	}
}

/* Console Callbacks. */

static void con_write(struct console *con, const char *s, size_t count)
{
	int i;

	for (i = 0; i < count; i++, s++)
		uart_poll_put_char(con->index, *s);
}

static int con_setup(struct console *con, char *options)
{
	int err;
	struct uart_port *up = &uart_ports[con->index];

	/* TODO Read baud rate from 'options'. */

	err = setup_port(up, 9600);
	if (!err)
		con->speed = 9600;

	return err;
}

/* 'console_match' is called for matching preferred console of
 * ''console=uart,io,<addr>,<options>'' or ''console=uart,0x<addr>,<options>''
 * with registered UART ports.
 */

static int con_match(struct console *con, char *name, char *options)
{
	unsigned char io_type;
	unsigned long addr;

	struct uart_port up, *p;

	if (strncmp(name, "uart", 4) != 0)
		return -ENODEV;

	if (uart_io_parse_option(options, &io_type, &addr, &options))
		return -ENODEV;

	up.io_type = io_type;

	switch (io_type) {
	case UART_IO_PORT:
		up.io.port = (unsigned short)(addr);

		break;
	case UART_IO_MEM:
		up.io.mem = (unsigned char *)(addr);

		break;
	}

	if ((p = __find_match_port(&up)))
		con->index = p->index;

	return con_setup(con, options);
}

static int con_exit(struct console *con)
{
	struct uart_port *up = &uart_ports[con->index];

	return exit_port(up);
}

/* Default UART 8250 console. */

static struct console console_8250 = {
	.name = __DRIVER_ID__,
	.write = con_write,
	.setup = con_setup,
	.match = con_match,
	.exit = con_exit,
	.index = -1
};

__PURE_CONSTRUCTOR__
static void console_init(void)
{
	init_ports();
	register_console(&console_8250);
}
