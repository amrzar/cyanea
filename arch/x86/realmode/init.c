/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/const.h>
#include <cyanea/memblock.h>
#include <cyanea/errno.h>
#include <cyanea/sizes.h>
#include <asm/realmode.h>
#include <asm/page.h>

struct real_mode_header *real_mode_header;

static inline size_t real_mode_size_needed(void)
{
    return ROUND_UP(real_mode_blob_end - real_mode_blob, PAGE_SIZE);
}

int __init reserve_real_mode(void)
{
    phys_addr_t mem;
    size_t size;

    size = real_mode_size_needed();
    if (!size)
        return SUCCESS;

    /* */

    mem = memblock_alloc(size, PAGE_SIZE, (0), (0 + SZ_1M), -1, false);
    if (!mem)
        return -ENOMEM;

    real_mode_header = (struct real_mode_header *)__va(mem);

    return SUCCESS;
}
