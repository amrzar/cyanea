/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CPU_H__
#define __X86_ASM_CPU_H__

#include <cyanea/const.h>
#include <cyanea/types.h>
#include <cyanea/compiler.h>

/* EFLAGS bits. */

#include <uapi/asm/eflags.h>

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

/* CPU model specific register (MSR) numbers. */

/* x86-64 specific MSRs */
#define MSR_EFER            0xC0000080  /* extended feature register */
#define MSR_FS_BASE         0xC0000100  /* 64bit FS base */
#define MSR_GS_BASE         0xC0000101  /* 64bit GS base */
#define MSR_KERNEL_GS_BASE  0xC0000102  /* SwapGS GS shadow */

/* Extended Feature Enable Register. */
#define _EFER_SCE           0   /* SYSCALL/SYSRET instructions in 64-bit mode. */
#define _EFER_LME           8   /* IA-32e Mode Enable. */
#define _EFER_LMA           10  /* IA-32e Mode Active. */
#define _EFER_NXE           11  /* Execute Disable Bit Enable. */

#define MSR_MTRRcap         0x000000FE
#define MSR_MTRRdefType     0x000002FF

#ifndef __ASSEMBLY__

/* Notes on CRx registers accessors:
 *
 *   - Use dummy input operand '__FORCE_ORDER' for read functions to prevent
 *     reordering CRx write across this read while allowing memory read to be
 *     cached or reordered across this read.
 *   - Use 'memory' clobber for the write functions to prevent reordering and
 *     caching memory access across CRx (it is needed as CRx writes affect the
 *     semantics of memory accesses).
 */

#define __FORCE_ORDER "m"(*(unsigned int *)0x1000UL)

static inline unsigned long native_read_cr0(void)
{
    unsigned long val;
    asm volatile("mov %%cr0, %0" : "=r" (val) : __FORCE_ORDER);
    return val;
}

static inline void native_write_cr0(unsigned long val)
{
    asm volatile("mov %0, %%cr0" : : "r" (val) : "memory");
}

# define read_cr0()   native_read_cr0()
# define write_cr0(x) native_write_cr0(x)

static inline unsigned long native_read_cr2(void)
{
    unsigned long val;
    asm volatile("mov %%cr2, %0" : "=r" (val) : __FORCE_ORDER);
    return val;
}

static inline void native_write_cr2(unsigned long val)
{
    asm volatile("mov %0, %%cr2" : : "r" (val) : "memory");
}

# define read_cr2()   native_read_cr2()
# define write_cr2(x) native_write_cr2(x)

static inline unsigned long native_read_cr3(void)
{
    unsigned long val;
    asm volatile("mov %%cr3, %0" : "=r" (val) : __FORCE_ORDER);
    return val;
}

static inline void native_write_cr3(unsigned long val)
{
    asm volatile("mov %0, %%cr3" : : "r" (val) : "memory");
}

# define read_cr3()   native_read_cr3()
# define write_cr3(x) native_write_cr3(x)

static inline unsigned long native_read_cr4(void)
{
    unsigned long val;
    asm volatile("mov %%cr4, %0" : "=r" (val) : __FORCE_ORDER);
    return val;
}

static inline void native_write_cr4(unsigned long val)
{
    asm volatile("mov %0, %%cr4" : : "r" (val) : "memory");
}

# define __read_cr4()   native_read_cr4()
# define __write_cr4(x) native_write_cr4(x)

/* Read and write CR4 with shadow register; use instead of '__read/__write_cr4'. */

void cr4_init_shadow(void);
void cr4_set_bits(unsigned long);
void cr4_clear_bits(unsigned long);
unsigned long read_cr4(void);

/* 8.7 TASK MANAGEMENT IN 64-BIT MODE. */

struct task_state_segment {
    u32 reserved1;

    /* Stack pointers (RSP) for ring 0 .. 2. */
    u64 rsp0;
    u64 rsp1;
    u64 rsp2;

    u64 reserved2;

    /* Interrupt stack table (IST) pointers. */
    u64 ist[7];

    u32 reserved3;
    u32 reserved4;
    u16 reserved5;

    /* Offset to the I/O permission bit map from the 64-bit TSS base. */
    u16 io_bitmap_base;
} __packed;

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_CPU_H__ */
