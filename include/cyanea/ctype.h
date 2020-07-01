/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_CTYPE_H__
#define __CYANEA_CTYPE_H__

extern int isupper(int);
extern int islower(int);
extern int isdigit(int);
extern int isspace(int);
extern int isprint(int);
extern int ispunct(int);
extern int iscntrl(int);

extern int isalnum(int);
extern int isalpha(int);
extern int isgraph(int);

extern int isxdigit(int);

static inline int tolower(int c)
{
    if (isupper(c))
        c -= 'A' - 'a';

    return c;
}

static inline int toupper(int c)
{
    if (islower(c))
        c -= 'a' - 'A';

    return c;
}

#endif /* __CYANEA_CTYPE_H__ */
