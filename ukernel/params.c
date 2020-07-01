/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/stddef.h>
#include <cyanea/compiler.h>

static char default_param_read_char(const char *cmdline, int n)
{
    return cmdline[n];
}

int __param_get_option(const char *cmdline, int size,
    const char *option, char *buf, int buf_size,
    char (*read_char)(const char *, int))
{
    char ch;
    int i = 0, n = -1;
    const char *opptr = NULL;
    char *buffer = buf;

    enum {
        ST_WORDSTART,           /* Start of word after a whitespace. */
        ST_WORDCMP,             /* Search for the option -- compare word. */
        ST_WORDSKIP,            /* Skip the word. */
        ST_BUFCOPY,             /* Copying option's value to buffer */
    } state = ST_WORDSTART;

    if (cmdline == NULL)
        return -1;

    if (read_char == NULL)
        read_char = default_param_read_char;

    do {
        ch = read_char(cmdline, i);

        switch (state) {
        case ST_WORDSTART:
            if (ch == ' ')
                break;

            state = ST_WORDCMP;
            opptr = option;

            fallthrough;
        case ST_WORDCMP:
            if (*opptr == '\0') {
                if (ch == '=') {
                    n = 0;
                    buffer = buf;
                    state = ST_BUFCOPY;

                    break;
                } else if ((ch == ' ') || (ch == '\0')) {
                    return -2;  /* ... found a boolean option. */
                }
            } else if (ch == *opptr++)
                break;

            state = ST_WORDSKIP;

            fallthrough;
        case ST_WORDSKIP:
            if (ch == ' ')
                state = ST_WORDSTART;

            break;

        case ST_BUFCOPY:
            if (ch == ' ')
                state = ST_WORDSTART;
            else {
                if (++n < buf_size)
                    *buffer++ = ch;
            }

            break;
        }
    } while ((ch) && (++i < size));

    if (buf_size != 0)
        *buffer = '\0';

    return n;
}
