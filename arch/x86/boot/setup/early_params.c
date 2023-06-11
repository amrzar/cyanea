/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "setup.h"
#include "../../../../ukernel/params.c"

static char read_char(const char *cmdline, int n)
{
    return rdfs8(((unsigned long)(cmdline) & 0xF) + n);
}

int early_param_parse(const char *option, char *buf, int buf_size)
{
    unsigned long cmdline_ptr = boot_params.hdr.cmd_line_ptr;

    if (cmdline_ptr >= 0x100000)
        return -1;

    /* The kernel command-line can be located anywhere between the end
     * of the setup heap and '0xA0000'. It does not have to be located
     * in the same 64KiB segment as the real-mode code itself. Use 'FS'
     * to setup a segment.
     */

    set_fs(cmdline_ptr >> 4);
    return __param_get_option((const char *)(cmdline_ptr), 0x10000,
            option, buf, buf_size, read_char);
}
