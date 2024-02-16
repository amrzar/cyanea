/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/minmax.h>

#include <string.h>

#include "../io_buffer.h"

static size_t wbuffer_no_flush(_IO_BUFFER io, const char *buffer, size_t count)
{
    size_t n, written = 0;

    if (io->in != 0) {
        if (io->flush(io))
            return 0;
    }

    while (count != 0) {
        if (io->out == io->buf_size) {
            if (io->flush(io))
                break;
        }

        if (io->out == 0 && io->buf_size <= count) {

            /* Buffer is empty. However, the requested size is greater then the buffer. */
            /* Bypass the buffer. */

            n = io->write(io, buffer, count);
            if (!n)
                break;

        } else {

            /* Buffer is not empty. We need to fill it before calling 'write' callback. */
            /* Buffer is empty. However, the requested size is smaller than the buffer. */

            n = min(count, io->buf_size - io->out);

            memcpy(&io->buffer[io->out], buffer, n);

            io->out += n;
        }

        buffer += n;
        written += n;
        count -= n;
    }

    return written;
}

size_t __iob_write(_IO_BUFFER io, const char *buffer, size_t count)
{
    size_t written = 0;
    size_t f_len;               /* Size of buffer should be flushed.     */
    size_t u_len;               /* Size of buffer shoule not be flushed. */

    switch (io->mode) {
    case _IOFBF:
        f_len = 0;
        u_len = count;
        break;

    case _IOLBF:
        f_len = count;
        u_len = 0;

        /* Flush up to new line. */
        while ((f_len != 0) && (buffer[f_len - 1] != '\n')) {
            f_len--;
            u_len++;
        }

        break;

    /*case _IONBF: */
    default:
        f_len = count;
        u_len = 0;
    }

    if (f_len != 0) {
        written = wbuffer_no_flush(io, buffer, f_len);

        if (written != f_len)
            return written;

        /* We have written 'f_len' byte, let's flush it. */
        if (io->flush(io))
            return written;

        buffer += written;
    }

    if (u_len != 0)
        written += wbuffer_no_flush(io, buffer, u_len);

    return written;
}

size_t __iob_read(_IO_BUFFER io, char *buffer, size_t count)
{
    char *buffer_ptr;
    size_t n, read = 0;

    if (io->out != 0) {
        if (io->flush(io))
            return 0;
    }

    while (count != 0) {
        while (io->in == 0) {

            /* Check if we should bypass buffering. */
            int buffered = (count < io->buf_size);
            if (buffered) {
                buffer_ptr = io->buffer + io->io_unget_slop;
                n = io->buf_size;
            } else {
                buffer_ptr = buffer;
                n = count;
            }

            if (!(n = io->read(io, buffer_ptr, n)))
                return read;

            if (buffered) {
                io->inptr = buffer_ptr;
                io->in = n;
            } else {
                buffer += n;
                read += n;
                count -= n;
            }

            if (count == 0)
                return read;
        }

        n = min(count, io->in);
        memcpy(buffer, io->inptr, n);

        buffer += n;
        read += n;
        count -= n;

        io->inptr += n;
        io->in -= n;
    }

    return read;
}

int __iob_ungetc(_IO_BUFFER io, char c)
{
    if ((io->out != 0) || _IO_UNGET_SLOP_FULL(io))
        return -1;

    io->inptr--;
    io->inptr[0] = c;
    io->in++;

    return c;
}
