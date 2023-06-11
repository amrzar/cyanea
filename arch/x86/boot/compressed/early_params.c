/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "compressed.h"
#include "../../../../ukernel/params.c"

static unsigned long get_cmd_line(void)
{
    unsigned long cmd_line_ptr = boot_params->hdr.cmd_line_ptr;
    cmd_line_ptr |= ((unsigned long) boot_params->ext_cmd_line_ptr) << 32;

    return cmd_line_ptr;
}

int early_param_parse(const char *option, char *buf, int buf_size)
{
    return __param_get_option((const char *)(get_cmd_line()), 0x10000,
            option, buf, buf_size, NULL);
}
