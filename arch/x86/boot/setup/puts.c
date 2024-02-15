/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm/io.h>

#include <ulib/stddef.h>

#include "string.h"
#include "setup.h"

#include "../../../../drivers/uart/uart_regs.h"
#include "../early_serial_console.c"

static void __bios_putchar(int c)
{
    asm volatile(
        "pushal         ;"
        "pushw  %%ds    ;"
        "int    $0x10   ;"
        "popw   %%ds    ;"
        "popal          ;"
        : : "b" (0x0007), "c" (0x0001), "a" (0x0e00 | c)
    );
}

void putchar(int c)
{
    if (c == '\n')
        putchar('\r');

    if (early_serial_port != 0)
        early_serial_putchar(c);
    else {
        /* Use BIOS if serial port is not ready. */
        __bios_putchar(c);
    }
}

void puts(const char *str)
{
    while (*str) {
        putchar(*str++);
    }
}
