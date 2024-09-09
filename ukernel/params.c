/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/params.h>
#include <cyanea/errno.h>
#include <asm/setup.h>
#include <cyanea/stddef.h>

/**
 * __param_get_option() - Process command-line options.
 * @cmdline: command-line.
 * @cmdline_len: maximum length of command-line.
 * @option: option to search in the command-line.
 * @buf: result.
 * @buf_len: length of result.
 *
 * For @cmdline ''op1=v1 op2=v2 op1=v3 op3'', for @option = 'op2', @buf is 'v2',
 * for @option = 'op1', @buf is 'v3' (second 'op1' override the first 'op1'),
 * for @option = 'op3', it returns -2.
 *
 * Return: -2 if @option is an existing boolean, -1 if @option does not exist.
 * Otherwise, size of option value returned in @ref_buf.
 */
static int __param_get_option(const char *cmdline, int cmdline_len, const char *option, char *buf,
    int buf_len)
{
    char ch;
    int i = 0, n = -1;
    const char *op = NULL;
    char *buffer = buf;

    if (cmdline == NULL)
        return -1;

    enum {
        ST_WORDSTART,   /* Start of word after a whitespace. */
        ST_WORDCMP,     /* Search for the option -- compare word. */
        ST_WORDSKIP,    /* Skip the word. */
        ST_BUFCOPY,     /* Copying option's value to buffer */
    } state = ST_WORDSTART;

    do {
        ch = cmdline[i];

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
                state = ST_WORDSTART;
            else if (++n < buf_len)
                *buffer++ = ch;

            break;
        }
    } while (ch && (++i < cmdline_len));

    if (buf_len != 0)
        *buffer = '\0';

    return n;
}

int param_get_option(const char *cmdline, const char *option, char *buf, int buf_len)
{
    int err;

    err = __param_get_option(cmdline, COMMAND_LINE_SIZE, option, buf, buf_len);
    switch (err) {
    case -2:    /* ''BOOLEAN'' parameter. */
        return 0;
    case -1:    /* invalid parameter. */
        return -EINVAL;
    default:
        return err;
    }
}
