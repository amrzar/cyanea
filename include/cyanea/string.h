/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_STRING_H__
#define __CYANEA_STRING_H__

#include <cyanea/types.h>

#ifndef __HAVE_ARCH_STRCMP
int strcmp(const char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNCMP
int strncmp(const char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRLEN
size_t strlen(const char *);
#endif

#ifndef __HAVE_ARCH_STRNLEN
size_t strnlen(const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRSTR
char *strstr(const char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNSTR
char *strnstr(const char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRCHR
char *strchr(const char *, int);
#endif

#ifndef __HAVE_ARCH_STRCPY
char *strcpy(char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNCPY
char *strncpy(char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_MEMCPY
void *memcpy(void *, const void *, size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET
void *memset(void *, int, size_t);
#endif

#ifndef __HAVE_ARCH_MEMMOVE
void *memmove(void *, const void *, size_t);
#endif

#endif /* __CYANEA_STRING_H__ */
