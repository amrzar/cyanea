/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_MEMBLOCK_H__
#define __CYANEA_MEMBLOCK_H__

#include <cyanea/types.h>

enum memblock_flags {
    MEMBLOCK_NONE = 0x0,

    /* Internal Flags. */

    __MEMBLOCK_RESERVED = 0x1,
};

struct memblock_region {
    enum memblock_flags flags;

    unsigned long base;
    size_t size;

    int nid;
};

struct memblock {
    unsigned long n, max;
    size_t size;

    struct memblock_region *regions;
};

extern int memblock_add(unsigned long, size_t, int, enum memblock_flags);
extern int memblock_remove(unsigned long, size_t);
extern int memblock_set_node(unsigned long, size_t, int);
extern int memblock_setclr_flag(unsigned long, size_t, int,
    enum memblock_flags);

/* ''ALLOCATION API''. */

extern void memblock_free(unsigned long, size_t);
extern unsigned long memblock_alloc(size_t, unsigned long, unsigned long,
    unsigned long, int, bool);

#endif /* __CYANEA_MEMBLOCK_H__ */
