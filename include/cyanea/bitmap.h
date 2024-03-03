/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_BITMAP_H__
#define __CYANEA_BITMAP_H__

#include <cyanea/bitops.h>
#include <cyanea/const.h>

#include <string.h>

#define BITS_TO_ULONGS(x) \
    __KERNEL_DIV_ROUND_UP(x, BITS_PER_TYPE(unsigned long))

#define DECLARE_BITMAP(name,bits) unsigned long name[BITS_TO_ULONGS(bits)]

static always_inline void bitmap_zero(unsigned long *dest, unsigned int nbits)
{
    memset(dest, 0, BITS_TO_ULONGS(nbits) * sizeof(unsigned long));
}

static always_inline void bitmap_fill(unsigned long *dest, unsigned int nbits)
{
    memset(dest, 0xFF, BITS_TO_ULONGS(nbits) * sizeof(unsigned long));
}

static always_inline void bitmap_set(unsigned long *map, unsigned int bit)
{
    __set_bit(bit, map);
}

static always_inline void bitmap_clear(unsigned long *map, unsigned int bit)
{
    __clear_bit(bit, map);
}

#endif /* __CYANEA_BITMAP_H__ */
