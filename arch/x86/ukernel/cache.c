/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/string.h>
#include <cyanea/types.h>
#include <asm/msr.h>
#include <asm/pgtable_types.h>
#include <asm/tlb.h>

/* 12.12 PAGE ATTRIBUTE TABLE (PAT). */

#define __idx_pat2pte(i) \
	((((i) & 4) << (_PAGE_BIT_PAT - 2)) | \
	        (((i) & 2) << (_PAGE_BIT_PCD - 1)) | \
	        (((i) & 1) << _PAGE_BIT_PWT))

#define __pte2pat(m) \
	((((m) >> (_PAGE_BIT_PAT - 2)) & 4) | \
	        (((m) >> (_PAGE_BIT_PCD - 1)) & 2) | \
	        (((m) >> _PAGE_BIT_PWT) & 1))

/* There is no entries for 'WC' and 'WP' in PAT's default setting. Use 'UC-'. */
static u16 __cachemode_to_pte[_PAGE_CACHE_MODE_NUM] = {
	[_PAGE_CACHE_MODE_WB] = __idx_pat2pte(0),
	[_PAGE_CACHE_MODE_WT] = __idx_pat2pte(1),
	[_PAGE_CACHE_MODE_UC_MINUS] = __idx_pat2pte(2),
	[_PAGE_CACHE_MODE_UC] = __idx_pat2pte(3),
	[_PAGE_CACHE_MODE_WC] = __idx_pat2pte(2),
	[_PAGE_CACHE_MODE_WP] = __idx_pat2pte(2),
};

/* Cache mode as programmed in the PAT. */
static enum page_cache_mode __pat_idx_to_cachemode[8] = {
	/* 12.12.4 Programming the PAT. */
	/* Setting for each PAT entry following a power up or reset: */
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

	return __pat_idx_to_cachemode[__pte2pat(masked)];
}

static void update_cache_mode_entry(unsigned idx, enum page_cache_mode cache)
{
	if (!idx) {
		assert((cache == _PAGE_CACHE_MODE_WB),
		        "entry 0 must be WB (hardwired to speed up translations).");
	}

	__cachemode_to_pte[cache] = __idx_pat2pte(idx);
	__pat_idx_to_cachemode[idx] = cache;
}

/* 12.12.2 IA32_PAT MSR. */
#define X86_MEMTYPE_UC 0ULL         /* Uncacheable. */
#define X86_MEMTYPE_WC 1ULL         /* Write Combining. */
/* RESERVED            2ULL */
/* RESERVED            3ULL */
#define X86_MEMTYPE_WT 4ULL         /* Write Through. */
#define X86_MEMTYPE_WP 5ULL         /* Write Protected. */
#define X86_MEMTYPE_WB 6ULL         /* Write Back. */
#define X86_MEMTYPE_UC_MINUS 7ULL   /* Weak Uncacheable. */

#define PAT_VALUE(p0, p1, p2, p3, p4, p5, p6, p7) \
	((X86_MEMTYPE_ ## p0) | (X86_MEMTYPE_ ## p1 << 8) | \
	        (X86_MEMTYPE_ ## p2 << 16) | (X86_MEMTYPE_ ## p3 << 24) | \
	        (X86_MEMTYPE_ ## p4 << 32) | (X86_MEMTYPE_ ## p5 << 40) | \
	        (X86_MEMTYPE_ ## p6 << 48) | (X86_MEMTYPE_ ## p7 << 56))

/* 'X86_MEMTYPE_' to '_PAGE_CACHE_MODE_'. */
static enum page_cache_mode __init pat_get_cache_mode(unsigned int pat_val,
        char *msg)
{
	enum page_cache_mode cache;
	char *cache_mode;

	switch (pat_val) {
	case X86_MEMTYPE_UC:
		cache = _PAGE_CACHE_MODE_UC;
		cache_mode = "UC  ";
		break;
	case X86_MEMTYPE_WC:
		cache = _PAGE_CACHE_MODE_WC;
		cache_mode = "WC  ";
		break;
	case X86_MEMTYPE_WT:
		cache = _PAGE_CACHE_MODE_WT;
		cache_mode = "WT  ";
		break;
	case X86_MEMTYPE_WP:
		cache = _PAGE_CACHE_MODE_WP;
		cache_mode = "WP  ";
		break;
	case X86_MEMTYPE_WB:
		cache = _PAGE_CACHE_MODE_WB;
		cache_mode = "WB  ";
		break;
	case X86_MEMTYPE_UC_MINUS:
		cache = _PAGE_CACHE_MODE_UC_MINUS;
		cache_mode = "UC- ";
		break;
	default:
		cache = _PAGE_CACHE_MODE_WB;
		cache_mode = "WB  ";
		break;
	}

	memcpy(msg, cache_mode, 4);

	return cache;
}

/* Update '__cachemode_to_pte' and '__pat_idx_to_cachemode' for @pat. */
static void __init init_cache_modes(u64 pat)
{
	int i;
	enum page_cache_mode cache;
	char pat_msg[64] = { '\0' };

	for (i = 0; i <= 7; i++) {
		cache = pat_get_cache_mode((pat >> (i * 8)) & 7, &pat_msg[4 * i]);
		update_cache_mode_entry(i, cache);
	}

	ulog_info("x86/PAT: Configuration [0-7]: %s\n", pat_msg);
}

static u64 __ro_after_init pat_msr_val;
static void __init pat_bp_init(void)
{
	/* Copied from LINUX. */
	/* PTE encoding:
	 *      PAT
	 *      |PCD
	 *      ||PWT  PAT
	 *      |||    slot
	 *      000    0    WB : _PAGE_CACHE_MODE_WB
	 *      001    1    WC : _PAGE_CACHE_MODE_WC
	 *      010    2    UC-: _PAGE_CACHE_MODE_UC_MINUS
	 *      011    3    UC : _PAGE_CACHE_MODE_UC
	 *      100    4    WB : Reserved
	 *      101    5    WP : _PAGE_CACHE_MODE_WP
	 *      110    6    UC-: Reserved
	 *      111    7    WT : _PAGE_CACHE_MODE_WT
	 *
	 *  - Put WT in slot 7 to improve robustness in the presence of errata
	 *    that might cause the high PAT bit to be ignored. A buggy slot 7
	 *    access hits slot 3, and slot 3 is UC, so at worst we lose performance.
	 *  - The reserved slots are unused, but mapped to their corresponding
	 *    types in the presence of PAT errata.
	 */
	pat_msr_val = PAT_VALUE(WB, WC, UC_MINUS, UC, WB, WP, UC_MINUS, WT);

	init_cache_modes(pat_msr_val);
}

/* Update PAT on current CPU. */
void __init pat_cpu_init(void)
{
	wrmsrl(MSR_IA32_CR_PAT, pat_msr_val);

	flush_tlb_all();
}

/* 12.11 MEMORY TYPE RANGE REGISTERS (MTRRS). */

void __init mtrr_cpu_init(void)
{
	/* 12.11.2.1 IA32_MTRR_DEF_TYPE MSR. */
	/* Let's didable MTRR. Use PAT only. */

	unsigned long mtrr_def;
# define MTRR_DEF_TYPE_ENABLE    (MTRR_DEF_TYPE_FE | MTRR_DEF_TYPE_E)
# define MTRR_DEF_TYPE_DISABLE   ~(MTRR_DEF_TYPE_TYPE | MTRR_DEF_TYPE_ENABLE)

	/* 12.11.7.2 MemTypeSet() Function. */

	mtrr_def = rdmsrl(MSR_MTRRdefType);

	/* Disable MTRRs and set the default type to X86_MEMTYPE_UC. */
	wrmsrl(MSR_MTRRdefType, mtrr_def & MTRR_DEF_TYPE_DISABLE);

	wbinvd();
	flush_tlb_all();
}

void __init cache_bp_init(void)
{
	pat_bp_init();

	mtrr_cpu_init();
	pat_cpu_init();
}
