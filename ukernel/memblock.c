/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/memblock.h>
#include <cyanea/errno.h>
#include <cyanea/init.h>
#include <cyanea/string.h>
#include <cyanea/minmax.h>

#include <asm/page_types.h>

static int memblock_debug = 0;

#define memblock_dbg(...) \
	do { \
		if (memblock_debug) {  \
			ulog_info(__VA_ARGS__); \
			ulog_info(" (%s)\n", __func__); \
		} \
	} while(0)

#define NR_REGIONS CONFIG_MEMBLOCK_MEMORY_REGIONS

static struct memblock_region regions[NR_REGIONS];
static struct memblock memory = {
	.regions = regions,
	.max = NR_REGIONS,
	.name = "memory"
};

#define for_each_memblock_region(i, reg) \
	for (i = 0, reg = memory.regions; i < memory.n; i++, reg = &memory.regions[i])

static void insert_region(int i, phys_addr_t base, size_t size, int nid,
        enum memblock_flags flags)
{
	struct memblock_region *region = &memory.regions[i];

	assert(memory.n + 1 <= memory.max, "out-of-space.");

	/* Free up an entry in the region array; move everything one up. */
	memmove(region + 1, region, (memory.n - i) * sizeof(*region));

	/* INSERT. */
	region->base = base;
	region->size = size;
	region->flags = flags;
	region->nid = nid;

	memory.n++;
	memory.size += size;
}

static void remove_region(int i)
{
	struct memblock_region *region = &memory.regions[i];

	/* Remove 'region' from the array. */
	memmove(region, region + 1, (memory.n - (i + 1)) * sizeof(*region));

	memory.n--;
	memory.size -= region->size;
}

static void merge_regions(int start_region, int end_region)
{
	int i = start_region;

	/* Here, [start_region .. end_region] at least has two entries. */

	if (i) {

		/* Merging from 'start_region - 1' to 'end_region'. */

		i--;
	}

	while (i < end_region) {
		struct memblock_region *curr = &memory.regions[i];
		struct memblock_region *next = &memory.regions[i + 1];

		if ((curr->base + curr->size != next->base) ||
		        (curr->nid != next->nid) || (curr->flags != next->flags)) {
			i++;

			continue;
		}

		curr->size += next->size;

		/* Get rid of 'next' and move every region after 'next' to replace it. */

		memmove(next, next + 1, (memory.n - (i + 2)) * sizeof(*next));
		memory.n--;

		/* Move back 'end_region' for one entry as 'next' removed already. */

		end_region--;
	}
}

static int add_range(phys_addr_t base, size_t size, int nid,
        enum memblock_flags flags)
{
	int i, start_region = -1, end_region;
	struct memblock_region *region;

	if (!size)
		return SUCCESS;

	phys_addr_t end = base + size;

	/* First region!? */
	if (!memory.n) {
		memory.regions[0].base = base;
		memory.regions[0].size = size;
		memory.regions[0].flags = flags;
		memory.regions[0].nid = nid;

		memory.n = 1;
		memory.size = size;

		return SUCCESS;
	}

	for_each_memblock_region(i, region) {
		phys_addr_t r_base = region->base;
		phys_addr_t r_end = r_base + region->size;

		/* Possible cases:
		 *  (1) 'region' is already after the new region.
		 *  (2) 'region' does not have overlap with the new region.
		 *  (3) 'region' has overlap with the new region.
		 *
		 * */

		if (r_base >= end)
			break;                  /* (1) */

		if (r_end <= base)
			continue;               /* (2) */

		/* (3) */
		/* Add the area before 'region' [base .. r_base] if exists. */
		if (r_base > base) {
			insert_region(i, base, r_base - base, nid, flags);
			if (start_region == -1)
				start_region = i;
			end_region = i + 1;
		}

		/* Add the area after 'region' [r_end .. end] if exist. */
		base = min(r_end, end);

		/* Here, 'base' == 'end' if this area is empty. */
	}

	/* Check if any area left to add, then insert it. */
	if (base < end) {
		insert_region(i, base, end - base, nid, flags);
		if (start_region == -1)
			start_region = i;
		end_region = i + 1;
	}

	/* Try to merge [start_region .. end_region]. */
	if (start_region != -1)
		merge_regions(start_region, end_region);

	return SUCCESS;
}

static int isolate_range(phys_addr_t base, size_t size, int *start_range,
        int *end_range)
{
	int i;
	struct memblock_region *region;

	/* Index of first and last (noninclusive) regions inside requested range. */
	*start_range = *end_range = 0;

	if (!size)
		return SUCCESS;

	phys_addr_t end = base + size;

	/* We require two extra regions. */
	if (memory.n + 2 > memory.max) {
		ulog_err("out-of-space.");

		return -ENOSPC;
	}

	for_each_memblock_region(i, region) {
		phys_addr_t r_base = region->base;
		phys_addr_t r_end = r_base + region->size;

		/* Possible cases:
		 *  (1) 'region' is already after the new region.
		 *  (2) 'region' does not have overlap with the new region.
		 *  (3) 'region' has overlap with the beginning of the new region.
		 *  (4) 'region' has overlap with the end of the new region.
		 *  (5) 'region' is fully contained in the new region.
		 *
		 * */

		if (r_base >= end)
			break;                  /* (1) */

		if (r_end <= base)
			continue;               /* (2) */

		if (r_base < base) {
			/* (3) */
			region->base = base;
			region->size -= base - r_base;
			memory.size -= base - r_base;
			insert_region(i, r_base, base - r_base, region->nid, region->flags);

		} else if (r_end > end) {
			/* (4) */
			region->base = end;
			region->size -= end - r_base;
			memory.size -= end - r_base;
			/* --i so next iteration, the new region will be fully contained. */
			insert_region(i--, r_base, end - r_base, region->nid, region->flags);
		} else {
			/* (5) */
			if (!*end_range)
				*start_range = i;
			*end_range = i + 1;
		}
	}

	return SUCCESS;
}

static phys_addr_t find_in_range_node(size_t size, unsigned long align,
        phys_addr_t start, phys_addr_t end, int nid, enum memblock_flags flags)
{
	int i;
	struct memblock_region *region;
	phys_addr_t base, r_base, r_end;

# define START_PHYSICAL_ADDRESS ((phys_addr_t)(PAGE_SIZE))
	start = max(start, START_PHYSICAL_ADDRESS);
	end = max(start, end);

	for_each_memblock_region(i, region) {
		if ((region->flags & flags) == flags) {
			if (nid != -1 && nid != region->nid)
				continue;

			r_base = clamp(region->base, start, end);
			r_end = clamp(region->base + region->size, start, end);

			base = ALIGN_KERNEL(r_base, align);
			if (base < r_end && r_end - base >= size)
				return base;
		}
	}

	return 0;
}

/* ''EXTERNAL API''. */

void __next_mem_pfn_range(int *i, int nid, unsigned long *start_pfn,
        unsigned long *end_pfn, int *out_nid)
{
	struct memblock_region *reg;

# define PFN_UP(x)   (ROUND_UP(x, PAGE_SIZE) >> PAGE_SHIFT)
# define PFN_DOWN(x) (ROUND_DOWN(x, PAGE_SIZE) >> PAGE_SHIFT)

	while (++*i < memory.n) {
		reg = &memory.regions[*i];

		/* Make sure after alignment, there is at least a page. */
		if (PFN_UP(reg->base) >= PFN_DOWN(reg->base + reg->size))
			continue;

		if (nid == -1 || nid == reg->nid)
			break;
	}

	if (*i < memory.n) {
		if (start_pfn)
			*start_pfn = PFN_UP(reg->base);

		if (end_pfn)
			*end_pfn = PFN_DOWN(reg->base + reg->size);

		if (out_nid)
			*out_nid = reg->nid;

	} else {
		/* End-of-Region. */
		*i = -1;
	}
}

int memblock_add(phys_addr_t base, size_t size, int nid,
        enum memblock_flags flags)
{
	memblock_dbg("[mem %#018llx .. %#018llx]", base, base + size - 1);

	/* User can not directly pass 'MEMBLOCK_RESERVED' as flag. */
	return add_range(base, size, nid, flags & ~MEMBLOCK_RESERVED);
}

int memblock_remove(phys_addr_t base, size_t size)
{
	int i, err, start, end;

	err = isolate_range(base, size, &start, &end);
	if (err)
		return err;

	/* Reserve interation to avoid 'memmove' of entries that will be removed. */
	for (i = end - 1; i >= start; i--)
		remove_region(i);

	return SUCCESS;
}

int memblock_setclr_flag(phys_addr_t base, size_t size, int set,
        enum memblock_flags flag)
{
	int i, err, start, end;

	err = isolate_range(base, size, &start, &end);
	if (err)
		return err;

	for (i = start; i < end; i++) {
		if (set)
			memory.regions[i].flags |= flag;
		else
			memory.regions[i].flags &= ~flag;
	}

	merge_regions(start, end);

	return SUCCESS;
}

int memblock_set_node(phys_addr_t base, size_t size, int nid)
{
	int i, err, start, end;

	err = isolate_range(base, size, &start, &end);
	if (err)
		return err;

	for (i = start; i < end; i++)
		memory.regions[i].nid = nid;

	merge_regions(start, end);

	return SUCCESS;
}

phys_addr_t memblock_alloc(size_t size, unsigned long align,
        phys_addr_t start, phys_addr_t end, int nid, bool exact_nid)
{
	phys_addr_t found;

again:

	found = find_in_range_node(size, align, start, end, nid, MEMBLOCK_NONE);
	if (found && !memblock_setclr_flag(found, size, 1, MEMBLOCK_RESERVED)) {
		memblock_dbg("[mem %#018llx .. %#018llx]", found, found + size - 1);

		return found;
	}

	/* Retry without 'nid'. */
	if (nid != -1 && !exact_nid) {
		nid = -1;

		goto again;
	}

	return 0;
}

void memblock_free(phys_addr_t base, size_t size)
{
	memblock_setclr_flag(base, size, 0, MEMBLOCK_RESERVED);
}

int memblock_reserve(phys_addr_t base, size_t size)
{
	return memblock_setclr_flag(base, size, 1, MEMBLOCK_RESERVED);
}

size_t memblock_phys_mem_size(void)
{
	return memory.size;
}

void memblock_dump(void)
{
	int i;
	struct memblock_region *region;

	for_each_memblock_region(i, region) {
		ulog("[%#2d][%#018llx .. %#018llx] %#18llx %s", i,
		        region->base, region->base + region->size - 1,
		        region->size, region->flags ? "[R]" : "");

		if (region->nid != -1)
			ulog(" on node %d\n", region->nid);
		else
			ulog("\n");
	}
}

static int __init early_memblock(char *arg)
{
	if (strstr(arg, "debug")) {
		memblock_debug = 1;

		return SUCCESS;
	}

	return -EINVAL;
}

early_param("memblock", early_memblock);
