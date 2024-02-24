/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ULIB_STRING_H__
#define __ULIB_STRING_H__

#ifndef __SIZE_TYPE__
#  error '__SIZE_TYPE__' is not defined.
#endif

#ifndef __HAVE_ARCH_STRCMP
int strcmp(const char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNCMP
int strncmp(const char *, const char *, __SIZE_TYPE__);
#endif

#ifndef __HAVE_ARCH_STRLEN
__SIZE_TYPE__ strlen(const char *);
#endif

#ifndef __HAVE_ARCH_STRNLEN
__SIZE_TYPE__ strnlen(const char *, __SIZE_TYPE__);
#endif

#ifndef __HAVE_ARCH_STRSTR
char *strstr(const char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNSTR
char *strnstr(const char *, const char *, __SIZE_TYPE__);
#endif

#ifndef __HAVE_ARCH_STRCHR
char *strchr(const char *, int);
#endif

#ifndef __HAVE_ARCH_STRCPY
char *strcpy(char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRCAT
char *strcat(char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNCPY
char *strncpy(char *, const char *, __SIZE_TYPE__);
#endif

#ifndef __HAVE_ARCH_MEMCPY
void *memcpy(void *, const void *, __SIZE_TYPE__);
#endif

#ifndef __HAVE_ARCH_MEMSET
void *memset(void *, int, __SIZE_TYPE__);
#endif

#ifndef __HAVE_ARCH_MEMMOVE
void *memmove(void *, const void *, __SIZE_TYPE__);
#endif

#ifndef __HAVE_ARCH_MEMCMP
int memcmp(const void *, const void *, __SIZE_TYPE__);
#endif

#endif /* __ULIB_STRING_H__ */
