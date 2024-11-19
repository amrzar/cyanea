/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UKLIB_IO_BUFFER_H__
#define __UKLIB_IO_BUFFER_H__

#include <cyanea/stddef.h>
#include <cyanea/stdarg.h>

enum io_bufmode { _IONBF, _IOLBF, _IOFBF };
# define _IONBF _IONBF           /* Unbuffered. */
# define _IOLBF _IOLBF           /* Line Buffering. */
# define _IOFBF _IOFBF           /* Full Buffering. */

struct io_buffer;
struct io_buffer_operations {
	size_t (*read)(struct io_buffer *, char *, size_t);
	size_t (*write)(struct io_buffer *, const char *, size_t);
	int (*flush)(struct io_buffer *);
};

/**
 * struct io_buffer - Simple buffer for IO.
 * @mode: mode of buffering.
 * @in: size of data in input buffer.
 * @out: size of data in output buffer.
 * @buffer: pointer to the buffer 'slop + buffer'.
 * @buf_size: buffer size, not including 'slop'.
 * @inptr: pointer to the beginning of input data in buffer.
 * @io_unget_slop: size of extra buffer 'slop' for 'ungetc'.
 *
 * @in and @out determine if io_buffer is input or output buffer as @in and @out both
 * can not be non-zero.
 */
struct io_buffer {
	enum io_bufmode mode;
	size_t in, out;
	char *buffer;
	size_t buf_size;
	char *inptr;
	size_t io_unget_slop;

	struct io_buffer_operations *ops;
};

typedef struct io_buffer *_IO_BUFFER;

#define _IO_UNGET_SLOP_FULL(io) ((io)->inptr == (io)->buffer)

size_t __iob_write(_IO_BUFFER io, const char *buffer, size_t count);
size_t __iob_read(_IO_BUFFER io, char *buffer, size_t count);
int __iob_ungetc(_IO_BUFFER io, char c);

int iob_strtoull(_IO_BUFFER io, int base, unsigned long long *result_ret);
int iob_strtoll(_IO_BUFFER io, int base, long long *result_ret);

int iob_vsnprintf(_IO_BUFFER io, const char *format, va_list _ap);

#endif /* __UKLIB_IO_BUFFER_H__ */
