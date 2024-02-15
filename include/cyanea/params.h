/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_PARAMS_H__
#define __CYANEA_PARAMS_H__

#include <cyanea/errno.h>
#include <asm/setup.h>

#include <ulib/stddef.h>

int __param_get_option(const char *, int, const char *, char *, int,
    char (*)(const char *, int));

static int param_get_option(const char *cmdline, const char *option,
    char *buf, int buf_size)
{
    int err = __param_get_option(cmdline, COMMAND_LINE_SIZE,
            option, buf, buf_size, NULL);

    switch (err) {
    case -2:                   /* ''BOOLEAN'' parameter. * */
        return 0;

    case -1:                   /* invalid parameter.* */
        return -EINVAL;

    default:
        return err;
    }
}

#endif /* __CYANEA_PARAMS_H__ */
