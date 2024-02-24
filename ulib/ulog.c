/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/console.h>
#include <cyanea/errno.h>

#include "io_buffer.h"

static size_t iob_write(_IO_BUFFER io, const char *buffer, size_t count)
{
    console_write(buffer, count);

    return count;
}

static int iob_flush(_IO_BUFFER io)
{
    console_write(io->buffer, io->out);

    /* Flush  whole output buffer. */
    io->out = 0;

    return SUCCESS;
}

void __va_ulog(const char *fmt, va_list ap)
{
    static char buffer[128];
    static struct io_buffer io = {
        .mode = _IOLBF,
        .buffer = buffer,
        .buf_size = 128,
        .write = iob_write,
        .flush = iob_flush
    };

    iob_vsnprintf(&io, fmt, ap);
}

void __attribute__((format(printf, 1, 2))) ulog(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    __va_ulog(fmt, ap);
    va_end(ap);
}
