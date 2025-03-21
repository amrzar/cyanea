/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_MSR_H__
#define __X86_ASM_MSR_H__

/* 2.2.1 Extended Feature Enable Register. */
#define MSR_EFER            0xC0000080  /* extended feature register */
#  define _EFER_SCE         0   /* SYSCALL/SYSRET instructions in 64-bit mode. */
#  define _EFER_LME         8   /* IA-32e Mode Enable. */
#  define _EFER_LMA         10  /* IA-32e Mode Active. */
#  define _EFER_NXE         11  /* Execute Disable Bit Enable. */

#define MSR_FS_BASE         0xC0000100  /* 64bit FS base */
#define MSR_GS_BASE         0xC0000101  /* 64bit GS base */
#define MSR_KERNEL_GS_BASE  0xC0000102  /* SwapGS GS shadow */

/* 13.11.2.1 IA32_MTRR_DEF_TYPE MSR. */
#define MSR_MTRRdefType     0x000002FF
#  define MTRR_DEF_TYPE_TYPE 0x0FFUL
#  define MTRR_DEF_TYPE_FE   0x400UL    /* Fixed MTRRs enabled. */
#  define MTRR_DEF_TYPE_E    0x800UL    /* MTRRs enabled. */

/* 12.12.1 Detecting and Enabling x2APIC Mode. */
#define MSR_IA32_APICBASE   0x0000001B
#  define MSR_IA32_APICBASE_BSP (1UL << 8)             /* Processor is BSP. */
#  define MSR_IA32_APICBASE_X2APIC_ENABLE (1UL << 10)  /* Enable x2APIC mode. */
#  define MSR_IA32_APICBASE_XAPIC_ENABLE (1UL << 11)   /* xAPIC global enable/disable. */
#  define MSR_IA32_APICBASE_BASE (0xFFFFFFUL << 12)    /* Base physical address. */

/* 13.12.2 IA32_PAT MSR. */
#define MSR_IA32_CR_PAT     0x00000277

#ifndef __ASSEMBLY__

#include <cyanea/types.h>

static __always_inline unsigned long __rdmsr(unsigned int msr)
{
	unsigned long low, high;

	asm volatile("rdmsr" : "=a" (low), "=d" (high) : "c" (msr));

	return ((low) | (high) << 32);
}

static __always_inline void __wrmsr(unsigned int msr, u32 low, u32 high)
{
	asm volatile("wrmsr" : : "c" (msr), "a" (low), "d" (high) : "memory");
}

static inline unsigned long native_read_msr(unsigned int msr)
{
	return __rdmsr(msr);
}

static inline void native_write_msr(unsigned int msr, u32 low, u32 high)
{
	__wrmsr(msr, low, high);
}

#define rdmsrl(msr) native_read_msr((msr))

static inline void wrmsr(unsigned int msr, u32 low, u32 high)
{
	native_write_msr(msr, low, high);
}

static inline void wrmsrl(unsigned int msr, u64 value)
{
	native_write_msr(msr, (u32)(value & 0xFFFFFFFFULL), (u32)(value >> 32));
}

#endif /* __ASSEMBLY__ */
#endif /* __X86_ASM_MSR_H__ */
