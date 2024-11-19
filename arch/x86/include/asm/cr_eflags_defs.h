/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CR_EFLAGS_DEFS_H__
#define __X86_ASM_CR_EFLAGS_DEFS_H__

#include <cyanea/const.h>

/* 2.3 SYSTEM FLAGS AND FIELDS IN THE EFLAGS REGISTER. */

#define _EFLAGS_BIT_CF      0   /* Carry Flag. */
#define _EFLAGS_CF      BIT_UL(_EFLAGS_BIT_CF)
#define _EFLAGS_BIT_FIXED   1   /* Bit 1 - always on. */
#define _EFLAGS_FIXED   BIT_UL(_EFLAGS_BIT_FIXED)
#define _EFLAGS_BIT_PF      2   /* Parity Flag . */
#define _EFLAGS_PF      BIT_UL(_EFLAGS_BIT_PF)
#define _EFLAGS_BIT_AF      4   /* Auxiliary carry Flag. */
#define _EFLAGS_AF      BIT_UL(_EFLAGS_BIT_AF)
#define _EFLAGS_BIT_ZF      6   /* Zero Flag. */
#define _EFLAGS_ZF      BIT_UL(_EFLAGS_BIT_ZF)
#define _EFLAGS_BIT_SF      7   /* Sign Flag. */
#define _EFLAGS_SF      BIT_UL(_EFLAGS_BIT_SF)
#define _EFLAGS_BIT_TF      8   /* Trap Flag. */
#define _EFLAGS_TF      BIT_UL(_EFLAGS_BIT_TF)
#define _EFLAGS_BIT_IF      9   /* Interrupt Flag. */
#define _EFLAGS_IF      BIT_UL(_EFLAGS_BIT_IF)
#define _EFLAGS_BIT_DF      10  /* Direction Flag. */
#define _EFLAGS_DF      BIT_UL(_EFLAGS_BIT_DF)
#define _EFLAGS_BIT_OF      11  /* Overflow Flag. */
#define _EFLAGS_OF      BIT_UL(_EFLAGS_BIT_OF)
#define _EFLAGS_BIT_IOPL    12  /* I/O Privilege Level (2 bits). */
#define _EFLAGS_IOPL  (_UL(3) << _EFLAGS_BIT_IOPL)
#define _EFLAGS_BIT_NT      14  /* Nested Task. */
#define _EFLAGS_NT      BIT_UL(_EFLAGS_BIT_NT)
#define _EFLAGS_BIT_RF      16  /* Resume Flag. */
#define _EFLAGS_RF      BIT_UL(_EFLAGS_BIT_RF)
#define _EFLAGS_BIT_VM      17  /* Virtual Mode. */
#define _EFLAGS_VM      BIT_UL(_EFLAGS_BIT_VM)
#define _EFLAGS_BIT_AC      18  /* Alignment Check/Access Control. */
#define _EFLAGS_AC      BIT_UL(_EFLAGS_BIT_AC)
#define _EFLAGS_BIT_VIF     19  /* Virtual Interrupt Flag. */
#define _EFLAGS_VIF     BIT_UL(_EFLAGS_BIT_VIF)
#define _EFLAGS_BIT_VIP     20  /* Virtual Interrupt Pending. */
#define _EFLAGS_VIP     BIT_UL(_EFLAGS_BIT_VIP)
#define _EFLAGS_BIT_ID      21  /* CPUID detection. */
#define _EFLAGS_ID      BIT_UL(_EFLAGS_BIT_ID)

/* 2.5 CONTROL REGISTERS. */

/* Basic CPU control in 'CR0'. */
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

#define CR0_STATE \
	(_CR0_PE | \
	        _CR0_MP | \
	        _CR0_ET | \
	        _CR0_NE | \
	        _CR0_WP | \
	        _CR0_AM | \
	        _CR0_PG)

/* Paging options in 'CR3'. */
#define _CR3_BIT_PWT        3   /* Page-level Write-Through. */
#define _CR3_PWT        BIT_UL(CR3_BIT_PWT)
#define _CR3_BIT_PCD        4   /* Page-level Cache Disable. */
#define _CR3_PCD        BIT_UL(CR3_BIT_PCD)

/* Intel CPU features in 'CR4'. */
#define _CR4_DE         BIT_UL(_CR4_BIT_DE)
#define _CR4_BIT_PSE        4   /* Page Size Extensions. */
#define _CR4_PSE        BIT_UL(_CR4_BIT_PSE)
#define _CR4_BIT_PAE        5   /* Physical Address Extension. */
#define _CR4_PAE        BIT_UL(_CR4_BIT_PAE)
#define _CR4_BIT_PGE        7   /* Page Global Enable. */
#define _CR4_PGE        BIT_UL(_CR4_BIT_PGE)
#define _CR4_BIT_PCIDE      17  /* PCID support. */
#define _CR4_PCIDE      BIT_UL(_CR4_BIT_PCIDE)
#define _CR4_BIT_SMEP       20  /* Supervisor Mode Execution Prevention (SMEP). */
#define _CR4_SMEP       BIT_UL(_CR4_BIT_SMEP)
#define _CR4_BIT_SMAP       21  /* Supervisor Mode Access Prevention (SMAP). */
#define _CR4_SMAP       BIT_UL(_CR4_BIT_SMAP)

#endif /* __X86_ASM_CR_EFLAGS_DEFS_H__ */
