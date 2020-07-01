/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _ASM_CPU_REGISTERS_H__
#define _ASM_CPU_REGISTERS_H__

#include <cyanea/const.h>

/** Basic CPU control in 'CR0'. **/

#define _CR0_BIT_PE         0   /* Protection Enable. */
#define _CR0_PE         BIT_UL(_CR0_BIT_PE)
#define _CR0_BIT_MP         1   /* Monitor Coprocessor. */
#define _CR0_MP         BIT_UL(_CR0_BIT_MP)
#define _CR0_BIT_EM         2   /* Emulation. */
#define _CR0_EM         BIT_UL(_CR0_BIT_EM)
#define _CR0_BIT_TS         3   /* Task Switched. */
#define _CR0_TS         BIT_UL(_CR0_BIT_TS)
#define _CR0_BIT_ET         4   /* Extension Type. */
#define _CR0_ET         BIT_UL(_CR0_BIT_ET)
#define _CR0_BIT_NE         5   /* Numeric Error. */
#define _CR0_NE         BIT_UL(_CR0_BIT_NE)
#define _CR0_BIT_WP         16  /* Write Protect. */
#define _CR0_WP         BIT_UL(_CR0_BIT_WP)
#define _CR0_BIT_AM         18  /* Alignment Mask. */
#define _CR0_AM         BIT_UL(_CR0_BIT_AM)
#define _CR0_BIT_NW         29  /* Not Write-through. */
#define _CR0_NW         BIT_UL(_CR0_BIT_NW)
#define _CR0_BIT_CD         30  /* Cache Disable. */
#define _CR0_CD         BIT_UL(_CR0_BIT_CD)
#define _CR0_BIT_PG         31  /* Paging. */
#define _CR0_PG         BIT_UL(_CR0_BIT_PG)

#define CR0_STATE (_CR0_PE | \
    _CR0_MP | _CR0_ET | \
    _CR0_NE | _CR0_WP | \
    _CR0_AM | _CR0_PG)

/** Paging options in 'CR3'. **/

#define _CR3_BIT_PWT        3   /* Page-level Write-Through. */
#define _CR3_PWT        BIT_UL(CR3_BIT_PWT)
#define _CR3_BIT_PCD        4   /* Page-level Cache Disable. */
#define _CR3_PCD        BIT_UL(CR3_BIT_PCD)

/** Intel CPU features in 'CR4'. **/

#define _CR4_BIT_PSE        4   /* Page Size Extensions. */
#define _CR4_PSE        BIT_UL(_CR4_BIT_PSE)
#define _CR4_BIT_PAE        5   /* Physical Address Extension. */
#define _CR4_PAE        BIT_UL(_CR4_BIT_PAE)
#define _CR4_BIT_PGE        7   /* Page Global Enable. */
#define _CR4_PGE        BIT_UL(_CR4_BIT_PGE)

/** Extended Feature Enable Register. **/

#define MSR_EFER            0xC0000080
#define _EFER_SCE           0   /* SYSCALL/SYSRET instructions in 64-bit mode. */
#define _EFER_LME           8   /* IA-32e Mode Enable. */
#define _EFER_LMA           10  /* IA-32e Mode Active. */
#define _EFER_NXE           11  /* Execute Disable Bit Enable. */

#endif /* _ASM_CPU_REGISTERS_H__ */
