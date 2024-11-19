/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/ctype.h>

enum {
#define UPPER (print | upper)
	upper = 0x01,
#define LOWER (print | lower)
	lower = 0x02,
#define X xdigit
#define DIGIT (print | digit)
	digit = 0x04,
	xdigit = 0x08,
#define SPACE (print | space)
	space = 0x10,
	print = 0x20,
#define PUNCT (print | punct)
	punct = 0x40,
#define CTRL cntrl
#define CTRLSP (cntrl | space)
	cntrl = 0x80,
};

#define isx(c, type) (ctypes[(int)(unsigned char)(c)] & (type))
const unsigned char ctypes[] = {
	CTRL,                       /* [0x00] NULL. */
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL,                       /* [0x07] BELL. */
	CTRLSP,                     /* [0x08] Backspace. */
	CTRLSP,                     /* [0x09] Horizontal TAB. */
	CTRLSP,                     /* [0x0A] Line Feed. */
	CTRLSP,                     /* [0x0B] Vertical TAB. */
	CTRLSP,                     /* [0x0C] Form Feed. */
	CTRLSP,                     /* [0x0D] Carriage return. */
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	SPACE,                      /* [0x20] Space. */
	PUNCT,                      /* [0x21] ''!''. */
	PUNCT,                      /* [0x22] ''"''. */
	PUNCT,                      /* [0x23] ''#''. */
	PUNCT,                      /* [0x24] ''$''. */
	PUNCT,                      /* [0x25] ''%''. */
	PUNCT,                      /* [0x26] ''&''. */
	PUNCT,                      /* [0x27] '''''. */
	PUNCT,                      /* [0x28] ''(''. */
	PUNCT,                      /* [0x29] '')''. */
	PUNCT,                      /* [0x2A] ''*''. */
	PUNCT,                      /* [0x2B] ''+''. */
	PUNCT,                      /* [0x2C] '',''. */
	PUNCT,                      /* [0x2D] ''-''. */
	PUNCT,                      /* [0x2E] ''.''. */
	PUNCT,                      /* [0x2F] ''/''. */
	DIGIT | X, DIGIT | X, DIGIT | X, DIGIT | X,
	DIGIT | X, DIGIT | X, DIGIT | X, DIGIT | X,
	DIGIT | X, DIGIT | X,
	PUNCT,                      /* [0x3A] '':''. */
	PUNCT,                      /* [0x3B] '';''. */
	PUNCT,                      /* [0x3C] ''<''. */
	PUNCT,                      /* [0x3D] ''=''. */
	PUNCT,                      /* [0x3E] ''>''. */
	PUNCT,                      /* [0x3F] ''?''. */
	PUNCT,                      /* [0x40] ''@''. */

	/* [0x41] 'A' .. [0x5A] 'Z'. */
	UPPER | X, UPPER | X, UPPER | X,
	UPPER | X, UPPER | X, UPPER | X,
	UPPER, UPPER, UPPER, UPPER, UPPER,
	UPPER, UPPER, UPPER, UPPER, UPPER,
	UPPER, UPPER, UPPER, UPPER, UPPER,
	UPPER, UPPER, UPPER, UPPER, UPPER,

	PUNCT,                      /* [0x5B] ''[''. */
	PUNCT,                      /* [0x5C] ''\''. */
	PUNCT,                      /* [0x5D] '']''. */
	PUNCT,                      /* [0x5E] ''^''. */
	PUNCT,                      /* [0x5F] ''_''. */
	PUNCT,                      /* [0x60] ''`''. */

	/* [0x61] 'a' .. [0x7A] 'z'. */
	LOWER | X, LOWER | X, LOWER | X,
	LOWER | X, LOWER | X, LOWER | X,
	LOWER, LOWER, LOWER, LOWER, LOWER,
	LOWER, LOWER, LOWER, LOWER, LOWER,
	LOWER, LOWER, LOWER, LOWER, LOWER,
	LOWER, LOWER, LOWER, LOWER, LOWER,

	PUNCT,                      /* [0x7B] ''{''. */
	PUNCT,                      /* [0x7C] ''|''. */
	PUNCT,                      /* [0x7D] ''}''. */
	PUNCT,                      /* [0x7E] ''~''. */

	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL,
	CTRL, CTRL, CTRL,

	SPACE,                      /* [0xA0] Non-breaking Space. */

	PUNCT, PUNCT, PUNCT, PUNCT, PUNCT,
	PUNCT, PUNCT, PUNCT, PUNCT, PUNCT,
	PUNCT, PUNCT, PUNCT, PUNCT, PUNCT,
	PUNCT, PUNCT, PUNCT, PUNCT, PUNCT,
	PUNCT, PUNCT, PUNCT, PUNCT, PUNCT,
	PUNCT, PUNCT, PUNCT, PUNCT, PUNCT,
	PUNCT, UPPER, UPPER, UPPER, UPPER,
	UPPER, UPPER, UPPER, UPPER, UPPER,
	UPPER, UPPER, UPPER, UPPER, UPPER,
	UPPER, UPPER, UPPER, UPPER, UPPER,
	UPPER, UPPER, UPPER, UPPER, PUNCT,
	UPPER, UPPER, UPPER, UPPER, UPPER,
	UPPER, UPPER, LOWER, LOWER, LOWER,
	LOWER, LOWER, LOWER, LOWER, LOWER,
	LOWER, LOWER, LOWER, LOWER, LOWER,
	LOWER, LOWER, LOWER, LOWER, LOWER,
	LOWER, LOWER, LOWER, LOWER, LOWER,
	LOWER, PUNCT, LOWER, LOWER, LOWER,
	LOWER, LOWER, LOWER, LOWER, LOWER,
};

int isupper(int c)
{
	return isx(c, upper);
}

int islower(int c)
{
	return isx(c, lower);
}

int isdigit(int c)
{
	return isx(c, digit);
}

int isspace(int c)
{
	return isx(c, space);
}

int isprint(int c)
{
	return isx(c, print);
}

int ispunct(int c)
{
	return isx(c, punct);
}

int iscntrl(int c)
{
	return isx(c, cntrl);
}

int isalnum(int c)
{
	return isx(c, upper | lower | digit);
}

int isalpha(int c)
{
	return isx(c, upper | lower);
}

int isgraph(int c)
{
	return isx(c, upper | lower | digit | punct);
}

int isxdigit(int c)
{
	return isx(c, xdigit);
}
