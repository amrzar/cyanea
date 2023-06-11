/* SPDX-License-Identifier: GPL-2.0-or-later */

#define DEFAULT_BAUD 9600

unsigned short early_serial_port = 0;

static void early_serial_init(unsigned short port, unsigned int baud)
{
    unsigned int divisor;
    unsigned char c;

    outb(UART_LCR_WLEN8, port +
        UART_LCR);      /* '8n1' 8-bits word, no parity, 1 stop bit. */
    outb(0, port + UART_IER);   /* Disable all interrupts. */
    outb(0, port + UART_FCR);   /* Disable FIFOs. */
    outb(UART_MCR_DTR | UART_MCR_RTS, port + UART_MCR); /* DTR + RTS */

    divisor = 115200 / baud;
    c = inb(port + UART_LCR);
    outb(c | UART_LCR_DLAB, port + UART_LCR);
    outb((divisor & 0xFF), port + UART_DLL);
    outb((divisor >> 8) & 0xFF, port + UART_DLH);
    outb(c, port + UART_LCR);

    early_serial_port = port;
}

static void parse_console_uart8250(void)
{
    char buf[64], *options;
    unsigned short port = 0;
    unsigned int baud = DEFAULT_BAUD;

    if (early_param_parse("console", buf, sizeof(buf)) <= 0)
        return;

    options = buf;

    /* ''uart8250,io,<port>,<option>''. */
    /* ''uart,io,<port>,<option>''.     */

    if (!strncmp(options, "uart8250,io,", 12))
        port = simple_strtoull(options + 12, &options, 0);
    else if (!strncmp(options, "uart,io,", 8))
        port = simple_strtoull(options + 8, &options, 0);
    else
        return;

    if (options && (options[0] == ','))
        baud = simple_strtoull(options + 1, &options, 0);

    if (port)
        early_serial_init(port, baud);
}

void early_serial_putchar(int c)
{
    unsigned int status, t = 10000;

    if (!early_serial_port)
        return;

    while (1) {
        status = inb(early_serial_port + UART_LSR);
        if ((status & UART_LSR_THRE) || (--t == 0))
            break;

        __builtin_ia32_pause();
    }

    outb(c, early_serial_port + UART_THR);
}

void early_console_init(void)
{
    parse_console_uart8250();

    early_serial_putchar('\n');
    early_serial_putchar('\n');
}
