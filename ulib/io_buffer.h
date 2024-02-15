/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __IO_BUFFER_H__
#define __IO_BUFFER_H__

#include <cyanea/types.h>

#include <ulib/stdarg.h>

enum io_bufmode { _IONBF, _IOLBF, _IOFBF };
#define _IONBF _IONBF           /* Unbuffered. */
#define _IOLBF _IOLBF           /* Line Buffering. */
#define _IOFBF _IOFBF           /* Full Buffering. */

typedef struct io_buffer {
    enum io_bufmode mode;

    /* The 'io_buffer' can be used as input buffer or output buffer.
     * It is determined based on whether 'in' or 'out' has a valid value.
     *
     * 'buffer' is pointer to beginning of buffer 'slop + buffer'
     * 'buf_size' is the buffer size -- 'slop' not included.
     * 'io_unget_slop' is size of extra buffer 'slop' for 'ungetc'.
     * 'inptr' is pointer to the beginning of input data in 'buffer'.
     */

    size_t in, out;
    size_t buf_size;
    char *inptr, *buffer;

#define _IO_UNGET_SLOP_FULL(io) ((io)->inptr == (io)->buffer)
    size_t io_unget_slop;

    size_t (*read)(struct io_buffer *, char *, size_t);
    size_t (*write)(struct io_buffer *, const char *, size_t);

    /* Flush callback return non-zero if failed to reset (i.e. 'in' and 'out'
     * set to zero) the buffer.
     */

    int (*flush)(struct io_buffer *);
} *_IO_BUFFER;

size_t __iob_write(_IO_BUFFER, const char *, size_t);
size_t __iob_read(_IO_BUFFER, char *, size_t);
int __iob_ungetc(_IO_BUFFER, char);

int iob_strtoull(_IO_BUFFER, int, unsigned long long *);
int iob_strtoll(_IO_BUFFER, int, long long *);

int iob_vsnprintf(_IO_BUFFER, const char *, va_list);
int iob_vsscanf(_IO_BUFFER, const char *, va_list);

#endif /* __IO_BUFFER_H__ */
