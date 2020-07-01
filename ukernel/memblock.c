/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/memblock.h>
#include <cyanea/minmax.h>
#include <cyanea/string.h>
#include <cyanea/errno.h>
#include <cyanea/init.h>

#include <asm/pgtable_types.h>

#define NR_REGIONS CONFIG_MEMBLOCK_MEMORY_REGIONS

/* [base .. base + size] can not exceed the maximum physical memory. */

#define cap_size(base, size) min(size, BIT_ULL(__PHYSICAL_MASK_SHIFT) - base)

static struct memblock_region region_lst[NR_REGIONS];
static struct memblock memory = {
    .regions = region_lst,
    .max = NR_REGIONS
};

#define for_each_region(i, reg) \
    for (i = 0, reg = &memory.regions[0]; i < memory.n; reg = &memory.regions[++i])

static int insert_region(int i, unsigned long base, size_t size, int nid,
    enum memblock_flags flags)
{
    struct memblock_region *region = &memory.regions[i];

    if (memory.n + 1 > memory.max) {

        /* TODO. Support to extend 'region_lst'. */

        return -ENOSPC;
    }

    /* Free up an entry in the region array, move everything one up. */

    memmove(region + 1, region, (memory.n - i) * sizeof(*region));

    region->base = base;
    region->size = size;
    region->flags = flags;
    region->nid = nid;
    memory.n++;
    memory.size += size;

    return SUCCESS;
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
        struct memblock_region *this = &memory.regions[i];
        struct memblock_region *next = &memory.regions[i + 1];

        if ((this->base + this->size != next->base) ||
            (this->nid != next->nid) || (this->flags != next->flags)) {
            i++;

            continue;
        }

        this->size += next->size;

        /* Get rid of 'next' and move every region after 'next' to replace it. */

        memmove(next, next + 1, (memory.n - (i + 2)) * sizeof(*next));
        memory.n--;

        /* Move back 'end_region' for one entry as 'next' removed already. */

        end_region--;
    }
}

static int __add_range(unsigned long base, size_t size, int nid,
    enum memblock_flags flags)
{
    int i, start_region = -1, end_region;
    struct memblock_region *region;

    size = cap_size(base, size);
    unsigned long end = base + size;

    if (!size)
        return SUCCESS;

    /* First region! */

    if (!memory.n) {
        memory.regions[0].base = base;
        memory.regions[0].size = size;
        memory.regions[0].flags = flags;
        memory.regions[0].nid = nid;

        memory.n = 1;
        memory.size = size;

        return SUCCESS;
    }

    for_each_region(i, region) {
        unsigned long r_base = region->base;
        unsigned long r_end = r_base + region->size;

        if (r_base >= end)
            break;              /* 'region' is already after new region. */

        if (r_end <= base)
            continue;           /* 'region' does not have overlap with new region. */

        /* 'region' has overlap with the new region. */
        /* Add the area before 'region' [base .. r_base] if exists. */

        if (r_base > base) {
            if (insert_region(i, base, r_base - base, nid, flags)) {

                /* Unable to insert the new range. */

                goto out;
            }

            /* Record the range if indexes that should be process for merging. */

            if (start_region == -1)
                start_region = i;
            end_region = i + 1;
        }

        /* else, add the area after 'region' [r_end .. end] if exist.
         * Here, 'base' == 'end' if this area is empty. */

        base = min(r_end, end);
    }

    /* Check if any area left to add, then insert it. */

    if (base < end) {

        if (!insert_region(i, base, end - base, nid, flags)) {
            if (start_region == -1)
                start_region = i;
            end_region = i + 1;
        }
    }

 out:

    /* Try to merge [start_region .. end_region]. */

    if (start_region != -1)
        merge_regions(start_region, end_region);

    return SUCCESS;
}

static int __isolate_range(unsigned long base, size_t size, int *start_range,
    int *end_range)
{
    int i;
    struct memblock_region *region;

    size = cap_size(base, size);
    unsigned long end = base + size;

    /* Index of first and last (noninclusive) regions inside requested range. */

    *start_range = *end_range = 0;

    if (!size)
        return SUCCESS;

    /* We require two extra regions. */

    if (memory.n + 2 > memory.max) {

        /* TODO. Support to extend 'region_lst'. */

        return -ENOSPC;
    }

    for_each_region(i, region) {
        unsigned long r_base = region->base;
        unsigned long r_end = r_base + region->size;

        if (r_base >= end)
            break;              /* 'region' is already after new region. */

        if (r_end <= base)
            continue;           /* 'region' does not have overlap with new region. */

        if (r_base < base) {

            /* 'region' has overlap with the beginning of [base .. base + size]. */

            region->base = base;
            region->size -= base - r_base;
            memory.size -= base - r_base;
            insert_region(i, r_base, base - r_base, region->nid, region->flags);

        } else if (r_end > end) {

            /* 'region' has overlap with the end of [base .. base + size].
             * '--i' so next iteration, the new region will be fully contained,
             * See 'else' bellow. */

            region->base = end;
            region->size -= end - r_base;
            memory.size -= end - r_base;
            insert_region(i--, r_base, end - r_base, region->nid,
                region->flags);

        } else {

            /* 'region' is fully contained in [base .. base + size]. */

            if (!*end_range)
                *start_range = i;
            *end_range = i + 1;
        }
    }

    return SUCCESS;
}

static unsigned long __find_in_range_node(size_t size, unsigned long align,
    unsigned long start, unsigned long end, int nid, enum memblock_flags flags)
{
    int i;
    struct memblock_region *region;
    unsigned long base, r_base, r_end;

    /* Avoid allocating the first page. */

    start = max(start, PAGE_SIZE);
    end = max(start, end);

    for_each_region(i, region) {
        if ((region->flags & flags) == flags) {

            if (nid != -1 && nid != region->nid)
                continue;

            /* 'region' with 'nid' and EXACT 'flags' set. */

            r_base = clamp(r_base, start, end);
            r_end = clamp(r_end, start, end);

            base = __KERNEL_DIV_ROUND_UP(r_base, align) * align;
            if (base < r_base && r_end - base >= size)
                return base;

        }
    }

    return 0;
}

/* ''EXTERNAL API''. */

int memblock_add(unsigned long base, size_t size, int nid,
    enum memblock_flags flags)
{
    /* User can not directly pass '__MEMBLOCK_RESERVED' as flag.
     * 'memblock_alloc' and 'memblock_free' do that! */

    return __add_range(base, size, nid, flags & ~__MEMBLOCK_RESERVED);
}

int memblock_remove(unsigned long base, size_t size)
{
    int i, err, start, end;

    err = __isolate_range(base, size, &start, &end);
    if (err)
        return err;

    /* Reserve interation to avoid 'memmove' of entries that will be removed. */

    for (i = end - 1; i >= start; i--)
        remove_region(i);

    return SUCCESS;
}

int memblock_setclr_flag(unsigned long base, size_t size, int set,
    enum memblock_flags flag)
{
    int i, err, start, end;

    err = __isolate_range(base, size, &start, &end);
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

int memblock_set_node(unsigned long base, size_t size, int nid)
{
    int i, err, start, end;

    err = __isolate_range(base, size, &start, &end);
    if (err)
        return err;

    for (i = start; i < end; i++)
        memory.regions[i].nid = nid;

    merge_regions(start, end);

    return SUCCESS;
}

unsigned long memblock_alloc(size_t size, unsigned long align,
    unsigned long start, unsigned long end, int nid, bool exact_nid)
{
    unsigned long found;

 again:

    found = __find_in_range_node(size, align, start, end, nid, MEMBLOCK_NONE);

    if (found && !memblock_setclr_flag(found, size, 1, __MEMBLOCK_RESERVED))
        return found;

    if (nid != -1 && !exact_nid) {

        /* Retry without 'nid'. */

        nid = -1;

        goto again;
    }

    return 0;
}

void memblock_free(unsigned long base, size_t size)
{
    /* Ignore the return value; On failure the area remains reserved. */

    memblock_setclr_flag(base, size, 0, __MEMBLOCK_RESERVED);
}
