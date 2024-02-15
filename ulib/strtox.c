/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/stddef.h>
#include <cyanea/container_of.h>
#include <cyanea/minmax.h>
#include <cyanea/errno.h>

#include <ulib/strtox.h>
#include <ulib/string.h>

#include "io_buffer.h"

struct strtox {
    struct io_buffer io;

    const char *str;
    size_t n;
};

static size_t iob_read(_IO_BUFFER io, char *buffer, size_t count)
{
    struct strtox *x = container_of(io, struct strtox, io);

    count = min(x->n, count);
    memcpy(buffer, x->str, count);
    x->str += count;
    x->n -= count;

    return count;
}

int strtoull(const char *str, int base, unsigned long long *result_ret)
{
    struct strtox x = { 0 };
    char scratch_buf[32];
    unsigned long long result;
    int ret;

    x.io.buffer = scratch_buf;
    x.io.buf_size = 32;
    x.io.read = iob_read;
    x.str = str;
    x.n = strlen(str);

    if ((ret = iob_strtoull(&x.io, base, &result)) < 0)
        return ret;

    *result_ret = result;
    return SUCCESS;
}

int strtoul(const char *str, int base, unsigned long *result_ret)
{
    unsigned long long result;
    int ret;

    if ((ret = strtoull(str, base, &result)) < 0)
        return ret;

    if (result != (unsigned long)(result))
        return -ERANGE;

    *result_ret = result;
    return SUCCESS;
}

int strtoll(const char *str, int base, long long *result_ret)
{
    struct strtox x = { 0 };
    char scratch_buf[32];
    long long result;
    int ret;

    x.io.buffer = scratch_buf;
    x.io.buf_size = 32;
    x.io.read = iob_read;
    x.str = str;
    x.n = strlen(str);

    if ((ret = iob_strtoll(&x.io, base, &result)) < 0)
        return ret;

    *result_ret = result;
    return SUCCESS;
}

int strtol(const char *str, int base, long *result_ret)
{
    long long result;
    int ret;

    if ((ret = strtoll(str, base, &result)) < 0)
        return ret;

    if (result != (long)(result))
        return -ERANGE;

    *result_ret = result;
    return SUCCESS;
}
