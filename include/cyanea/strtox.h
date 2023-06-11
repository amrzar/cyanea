/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_STRTOX_H__
#define __CYANEA_STRTOX_H__

int strtoull(const char *, int, unsigned long long *);
int strtoul(const char *, int, unsigned long *);
int strtoll(const char *, int, long long *);
int strtol(const char *, int, long *);

#endif /* __CYANEA_STRTOX_H__ */
