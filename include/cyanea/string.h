/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_STRING_H__
#define __CYANEA_STRING_H__

#include <cyanea/stddef.h>

int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);

size_t strlen(const char *str);
size_t strnlen(const char *str, size_t n);

char *strstr(const char *str1, const char *str2);
char *strnstr(const char *str1, const char *str2, size_t n);
char *strchr(const char *str, int c);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *str, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *str1, const void *str2, size_t n);

#endif /* __CYANEA_STRING_H__ */
