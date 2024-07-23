/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_STRING_H__
#define __CYANEA_STRING_H__

#include <cyanea/stddef.h>

int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);

size_t strlen(const char *);
size_t strnlen(const char *, size_t);

char *strstr(const char *, const char *);
char *strnstr(const char *, const char *, size_t);
char *strchr(const char *, int);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
char *strncpy(char *, const char *, size_t);

void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
void *memmove(void *, const void *, size_t);
int memcmp(const void *, const void *, size_t);

#endif /* __CYANEA_STRING_H__ */
