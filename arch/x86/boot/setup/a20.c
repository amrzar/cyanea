/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "setup.h"

#define KBC_DATA_PORT 0x60
#define KBC_STATUS_PORT 0x64
#define KBC_COMMAND_PORT 0x64

static int empty_8042(void)
{
    u8 status;

#define MAX_8042_LOOPS  100000
    int loops = MAX_8042_LOOPS;

    while (loops--) {
        slow_down_io();

        status = inb(KBC_STATUS_PORT);

        /*
         * 'STATUS_OUTPUT_BUFFER' must be set before attempting to read data from
         * 'KBC_DATA_PORT'. Similarly, 'STATUS_INPUT_BUFFER' must be clear before
         * attempting to write data to 'KBC_DATA_PORT or 'KBC_COMMAND_PORT'.
         *
         * */

#define STATUS_OUTPUT_BUFFER (1 << 0)
#define STATUS_INPUT_BUFFER (1 << 1)

        if (status & STATUS_OUTPUT_BUFFER) {
            slow_down_io();

            inb(KBC_DATA_PORT);
        } else if (!(status & STATUS_INPUT_BUFFER)) {
            return 0;           /* ... both buffers are empty. */
        }
    }

    return -1;
}

static void enable_a20_kbc(void)
{
    empty_8042();

    outb(0xD1, KBC_COMMAND_PORT);
    empty_8042();

    outb(0xDF, KBC_DATA_PORT);
    empty_8042();
}

/*
 * To test if A20 is enabled, 'a20_test' uses a safe address in the first segment,
 * i.e. '0x0000:nnnn' and writes an arbitary value and tries to re-read the value
 * using the last segment -- starting from '1MiB - 0x10'. So, if value read from
 * '0xFFFF:(nnnn + 0x10)' is the same, then A20 is still disabled.
 *
 * Here, the 'nnnn' used as a ''safe'' address for the test is 0x80-th vector IVT.
 *
 * */

#define A20_TEST_ADDR (4 * 0x80)

static int a20_test(int loops)
{
    int ret = 0;
    u32 old_value, n;

    set_fs(0x0000);
    set_gs(0xFFFF);

    n = old_value = rdfs32(A20_TEST_ADDR);

    while (!ret && loops--) {
        wrfs32(++n, A20_TEST_ADDR);
        slow_down_io();

        ret = (rdgs32(A20_TEST_ADDR + 0x10) != n);
    }

    wrfs32(old_value, A20_TEST_ADDR);
    return ret;
}

#define A20_TEST_SHORT 0x20
#define A20_TEST_LONG 0x200000

static int a20_test_short(void)
{
    return a20_test(A20_TEST_SHORT);
}

static int a20_test_long(void)
{
    return a20_test(A20_TEST_LONG);
}

static void enable_a20_bios(void)
{
    asm volatile(
        "pushfl         ;"
        "int    $0x15   ;"
        "popfl          ;"
        : : "a" (0x2401)
    );
}

#define A20_FAST_PORT 0x92

static void enable_a20_fast(void)
{
    u8 port;

    port = inb(A20_FAST_PORT);
    port |= 0x02;               /* Enable A20.           */
    port &= ~0x01;              /* Do not reset machine. */
    outb(port, A20_FAST_PORT);
}

#define A20_ENABLE_LOOPS 255

int enable_a20(void)
{
    int loops = A20_ENABLE_LOOPS;

    while (loops--) {
        if (a20_test_short())
            return 0;

        enable_a20_bios();

        if (a20_test_short())
            return 0;

        empty_8042();

        /* Check if BIOS worked, again. */
        if (a20_test_short())
            return 0;

        enable_a20_kbc();

        if (a20_test_long())
            return 0;

        enable_a20_fast();

        if (a20_test_long())
            return 0;
    }

    return -1;
}
