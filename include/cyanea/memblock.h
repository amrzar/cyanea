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

int memblock_add(phys_addr_t base, size_t size, int nid,
        enum memblock_flags flags);
int memblock_remove(phys_addr_t base, size_t size);
int memblock_set_node(phys_addr_t base, size_t size, int nid);
int memblock_setclr_flag(phys_addr_t base, size_t size, int set,
        enum memblock_flags flags);

/* ''ALLOCATION API''. */

/**
 * @brief Iterate over memory ranges.
 *
 * This function iterates over memory ranges and updates @p start_pfn,
 * @p end_pfn, and @p out_nid. If @p nid is not -1, only the memory ranges that
 * match the specified node ID are considered. If @p nid is -1, all memory
 * ranges are included in the iteration.
 *
 * @p i is updated to point to the next range. If the end of the iteration is
 * reached, @p i is set to -1.
 *
 * @param i [in, out] Pointer to the current index of the memory range.
 * @param nid [in] Node ID to filter the memory ranges.
 * @param start_pfn [out] Pointer to store the start pfn of the range.
 * @param end_pfn [out] Pointer to store the end pfn of the range.
 * @param out_nid [out] Pointer to store the node ID of the current range.
 */
void __next_mem_pfn_range(int *i, int nid, unsigned long *start_pfn,
        unsigned long *end_pfn, int *out_nid);

void memblock_free(phys_addr_t base, size_t size);

/**
 * @brief Allocates a memory block within the specified range.
 *
 * @param size The size of the memory block to allocate.
 * @param align The alignment requirement for the memory block.
 * @param start The start address of the memory range.
 * @param end The end address of the memory range.
 * @param nid The node ID where the memory should be allocated.
 * @param exact_nid If true, the memory must be allocated on the specified node ID.
 * @return The physical address of the allocated memory block, or 0 on failure.
 */
phys_addr_t memblock_alloc(size_t size, unsigned long align, phys_addr_t start,
        phys_addr_t end, int nid, bool exact_nid);
int memblock_reserve(phys_addr_t base, size_t size);
size_t memblock_phys_mem_size(void);
void memblock_dump(void);

#define for_each_mem_pfn_range(i, nid, start_pfn, end_pfn, out_nid) \
	for (i = -1, __next_mem_pfn_range(&i, nid, start_pfn, end_pfn, out_nid); \
	        i >= 0; __next_mem_pfn_range(&i, nid, start_pfn, end_pfn, out_nid))

#endif /* __CYANEA_MEMBLOCK_H__ */
