/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_PARAMS_H__
#define __CYANEA_PARAMS_H__

int __param_get_option(const char *, int, const char *, char *, int,
    char (*)(const char *, int));

int param_get_option(const char *cmdline, const char *option, char *buf, int buf_size);

#endif /* __CYANEA_PARAMS_H__ */
