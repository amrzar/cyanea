/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_MEMBLOCK_H__
#define __CYANEA_MEMBLOCK_H__

#include <cyanea/types.h>
#include <cyanea/stddef.h>

enum memblock_flags {
    MEMBLOCK_NONE = 0x0,
    MEMBLOCK_RESERVED = 0xFFFF,
};

struct memblock_region {
    enum memblock_flags flags;

    phys_addr_t base;
    size_t size;

    int nid;
};

struct memblock {
    unsigned long n, max;
    size_t size;

    struct memblock_region *regions;
    const char *name;
};

int memblock_add(phys_addr_t, size_t, int, enum memblock_flags);
int memblock_remove(phys_addr_t, size_t);
int memblock_set_node(phys_addr_t, size_t, int);
int memblock_setclr_flag(phys_addr_t, size_t, int, enum memblock_flags);

/* ''ALLOCATION API''. */

void __next_mem_pfn_range(int *, int, unsigned long *, unsigned long *, int *);
void memblock_free(phys_addr_t, size_t);
phys_addr_t memblock_alloc(size_t, unsigned long, phys_addr_t, phys_addr_t, int,
    bool);
int memblock_reserve(phys_addr_t, size_t);
size_t memblock_phys_mem_size(void);
void memblock_dump(void);

#define for_each_mem_pfn_range(i, n, start_pfn, end_pfn, nid) \
    for (i = -1, __next_mem_pfn_range(&i, n, start_pfn, end_pfn, nid); \
        i >= 0; __next_mem_pfn_range(&i, n, start_pfn, end_pfn, nid))

#endif /* __CYANEA_MEMBLOCK_H__ */
