/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/types.h>
#include <asm/pgtable_types.h>

/* See: 12.11 MEMORY TYPE RANGE REGISTERS (MTRRS). */
/* See: 12.12 PAGE ATTRIBUTE TABLE (PAT). */

/* Page 12-20. Following a hardware reset, the P6 and more recent processor
 * families disable all the fixed and variable MTRRs.
 */

/* TODO. Configure MTRR. */

#define __pat2pte(i) \
    ((((i) & 4) << (_PAGE_BIT_PAT - 2)) | \
        (((i) & 2) << (_PAGE_BIT_PCD - 1)) | \
        (((i) & 1) << _PAGE_BIT_PWT))

#define __pte2pat(m) \
    ((((m) >> (_PAGE_BIT_PAT - 2)) & 4) | \
        (((m) >> (_PAGE_BIT_PCD - 1)) & 2) | \
        (((m) >> _PAGE_BIT_PWT) & 1))

/* There is no entries for 'WC' and 'WP' in PAT's default setting. Use 'UC-'. */

static u16 __cachemode_to_pte[_PAGE_CACHE_MODE_NUM] = {
    [_PAGE_CACHE_MODE_WB] = __pat2pte(0),
    [_PAGE_CACHE_MODE_WT] = __pat2pte(1),
    [_PAGE_CACHE_MODE_UC_MINUS] = __pat2pte(2),
    [_PAGE_CACHE_MODE_UC] = __pat2pte(3),
    [_PAGE_CACHE_MODE_WC] = __pat2pte(2),
    [_PAGE_CACHE_MODE_WP] = __pat2pte(2),
};

/* Cache mode as programmed in the PAT. */
/* Initialised with the default setting for each PAT entry following a power up
 * or reset of the processor.
 */

static enum page_cache_mode __pat_to_cachemode[8] = {
    /* PAT0 */ _PAGE_CACHE_MODE_WB,
    /* PAT1 */ _PAGE_CACHE_MODE_WT,
    /* PAT2 */ _PAGE_CACHE_MODE_UC_MINUS,
    /* PAT3 */ _PAGE_CACHE_MODE_UC,
    /* PAT4 */ _PAGE_CACHE_MODE_WB,
    /* PAT5 */ _PAGE_CACHE_MODE_WT,
    /* PAT6 */ _PAGE_CACHE_MODE_UC_MINUS,
    /* PAT7 */ _PAGE_CACHE_MODE_UC,
};

unsigned long cachemode2protval(enum page_cache_mode pcm)
{
    if (pcm == _PAGE_CACHE_MODE_WB) {
        /* 'WB' is default cache mode, keep it at PAT0. */
        return 0;
    }

    return __cachemode_to_pte[pcm];
}

enum page_cache_mode pgprot2cachemode(pgprot_t pgprot)
{
    pgprotval_t masked = pgprot_val(pgprot) & _PAGE_CACHE_MASK;

    if (masked == 0) {
        /* PAT, PCD, and PWT are zero. PAT0 is always 'WB'. */
        return _PAGE_CACHE_MODE_WB;
    }

    return __pat_to_cachemode[__pte2pat(masked)];
}

/* TODO. Configure PAT. */
