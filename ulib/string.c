/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/string.h>
#include <cyanea/stddef.h>

#ifndef __HAVE_ARCH_STRCMP
int strcmp(const char *str1, const char *str2)
{
    const unsigned char *s1 = (const unsigned char *)(str1);
    const unsigned char *s2 = (const unsigned char *)(str2);

    while ((*s1 == *s2) && (*s1)) {
        s1++;
        s2++;
    }

    return (*s1 - *s2);
}
#endif

#ifndef __HAVE_ARCH_STRNCMP
int strncmp(const char *str1, const char *str2, size_t n)
{
    const unsigned char *s1 = (const unsigned char *)(str1);
    const unsigned char *s2 = (const unsigned char *)(str2);
    unsigned char ch;
    int d = 0;

    while (n--) {
        d = (int)(ch = *s1++) - (int)(*s2++);
        if (d || !ch)
            break;
    }

    return d;
}
#endif

#ifndef __HAVE_ARCH_STRLEN
size_t strlen(const char *str)
{
    const char *s = str;

    while (*s)
        s++;

    return (s - str);
}
#endif

#ifndef __HAVE_ARCH_STRNLEN
size_t strnlen(const char *str, size_t n)
{
    const char *s = str;

    while ((n--) && (*s))
        s++;

    return (s - str);
}
#endif

#ifndef __HAVE_ARCH_STRSTR
char *strstr(const char *str1, const char *str2)
{
    return strnstr(str1, str2, strlen(str1));
}
#endif

#ifndef __HAVE_ARCH_STRNSTR
char *strnstr(const char *str1, const char *str2, size_t n)
{
    size_t n2;

    n2 = strlen(str2);
    if (!n2)
        return (char *)(str1);

    while (n >= n2) {
        if (!strncmp(str1, str2, n2))
            return (char *)(str1);

        n--;
        str1++;
    }

    return NULL;
}
#endif

#ifndef __HAVE_ARCH_STRCHR
char *strchr(const char *str, int c)
{
    while (*str != (char)(c)) {
        if (*str == '\0')
            return NULL;
        str++;
    }

    return (char *)(str);
}
#endif

#ifndef __HAVE_ARCH_STRCPY
char *strcpy(char *dest, const char *src)
{
    char *p = dest;
    const char *q = src;
    char ch;

    do {
        *p++ = ch = *q++;
    } while (ch);

    return dest;
}
#endif

#ifndef __HAVE_ARCH_STRNCPY
char *strncpy(char *dest, const char *src, size_t n)
{
    char *q = dest;
    const char *p = src;
    char ch;

    while (n--) {
        *q++ = ch = *p++;
        if (!ch)
            break;
    }

    memset(q, 0, n);            /* spec!? */
    return dest;
}
#endif

#ifndef __HAVE_ARCH_MEMCPY
void *memcpy(void *dest, const void *src, size_t n)
{
    char *q = dest;
    const char *p = src;

    while (n--)
        *q++ = *p++;

    return dest;
}
#endif

#ifndef __HAVE_ARCH_MEMSET
void *memset(void *str, int c, size_t n)
{
    char *s = str;

    while (n--)
        *s++ = c;

    return str;
}
#endif

#ifndef __HAVE_ARCH_MEMMOVE
void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;

    if (d <= s || d - s >= n)
        return memcpy(dest, src, n);

    while (n-- > 0)
        d[n] = s[n];

    return dest;
}
#endif
