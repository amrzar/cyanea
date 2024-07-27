/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/params.h>
#include <cyanea/errno.h>
#include <asm/setup.h>
#include <cyanea/stddef.h>

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
    const char *op = NULL;
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
            op = option;

            fallthrough;
        case ST_WORDCMP:
            if (*op == '\0') {
                if (ch == '=') {
                    n = 0;
                    buffer = buf;
                    state = ST_BUFCOPY;

                    break;
                } else if ((ch == ' ') || (ch == '\0')) {
                    return -2;
                }
            } else if (ch == *op++)
                break;

            state = ST_WORDSKIP;

            fallthrough;
        case ST_WORDSKIP:
            if (ch == ' ')
                state = ST_WORDSTART;

            break;

        case ST_BUFCOPY:
            if (ch == ' ')
                /* Found, but continue till the end of 'cmdline' in case it is overridden. */
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

int param_get_option(const char *cmdline, const char *option,
    char *buf, int buf_size)
{
    int err = __param_get_option(cmdline, COMMAND_LINE_SIZE,
            option, buf, buf_size, NULL);

    switch (err) {
    case -2:    /* ''BOOLEAN'' parameter. */
        return 0;
    case -1:    /* invalid parameter. */
        return -EINVAL;
    default:
        return err;
    }
}
