/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_STRTOX_H__
#define __CYANEA_STRTOX_H__

int strtoull(const char *str, int base, unsigned long long *result_ret);
int strtoul(const char *str, int base, unsigned long *result_ret);
int strtoll(const char *str, int base, long long *result_ret);
int strtol(const char *str, int base, long *result_ret);

#endif /* __CYANEA_STRTOX_H__ */
