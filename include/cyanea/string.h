/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_STRING_H__
#define __CYANEA_STRING_H__

#include <asm/string.h>
#include <cyanea/types.h>

#ifndef __HAVE_ARCH_STRCMP
extern int strcmp(const char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNCMP
extern int strncmp(const char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRLEN
extern size_t strlen(const char *);
#endif

#ifndef __HAVE_ARCH_STRNLEN
extern size_t strnlen(const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRSTR
extern char *strstr(const char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNSTR
extern char *strnstr(const char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRCHR
extern char *strchr(const char *, int);
#endif

#ifndef __HAVE_ARCH_STRCPY
extern char *strcpy(char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNCPY
extern char *strncpy(char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_MEMCPY
extern void *memcpy(void *, const void *, size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET
extern void *memset(void *, int, size_t);
#endif

#ifndef __HAVE_ARCH_MEMMOVE
extern void *memmove(void *, const void *, size_t);
#endif

#endif /* __CYANEA_STRING_H__ */
