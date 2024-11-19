/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_CTYPE_H__
#define __CYANEA_CTYPE_H__

int isupper(int);
int islower(int);
int isdigit(int);
int isspace(int);
int isprint(int);
int ispunct(int);
int iscntrl(int);

int isalnum(int);
int isalpha(int);
int isgraph(int);

int isxdigit(int);

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
