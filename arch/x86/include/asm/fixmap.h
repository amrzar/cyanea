/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_FIXMAP_H__
#define __X86_ASM_FIXMAP_H__

#include <cyanea/pgtable.h>

/* There are total of 1024 fixed-size 4KiB mappings (in 'level1_fixmap_pgt'):
 * 512 entries (i.e. 'PTRS_PER_PT) in 'level1_fixmap_pgt[0]' for temporary
 * mappings and 512 entries 'level1_fixmap_pgt[1]' for permanent entries.
 */

#define TOTAL_FIX_BITMAPS PTRS_PER_PT

/* TLB entries will not be flushed across task switches for permanent mappings. */

enum fixed_addresses {
    __end_of_permanent_fixed_addresses = TOTAL_FIX_BITMAPS,
    FIX_BITMAP_END = __end_of_permanent_fixed_addresses,

    FIX_BITMAP_BEGIN = FIX_BITMAP_END + TOTAL_FIX_BITMAPS - 1,
    __end_of_fixed_addresses
};

#endif /* __X86_ASM_FIXMAP_H__ */
