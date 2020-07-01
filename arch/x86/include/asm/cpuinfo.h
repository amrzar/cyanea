/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CPUINFO_H__
#define __X86_ASM_CPUINFO_H__

#include <cyanea/init.h>
#include <cyanea/types.h>

#define CAP_BIT(word, x) ((word) * 32 + (x))

enum cpu_leafs {
    CPUID_1_ECX,
#define CPUID_1_ECX_BIT(x) CAP_BIT(CPUID_1_ECX, x)
    CPUID_1_EDX,
#define CPUID_1_EDX_BIT(x) CAP_BIT(CPUID_1_EDX, x)

    __N_CPUID
};

/* Intel-defined CPU features, CPUID level 0x00000001 (ECX). */

#define _CPUID_BIT_SSE3   CPUID_1_ECX_BIT(0)    /* Streaming SIMD Extensions 3 (SSE3). */
#define _CPUID_BIT_PCLMULQDQ CPUID_1_ECX_BIT(1) /* 'PCLMULQDQ'. */
#define _CPUID_BIT_DTES64 CPUID_1_ECX_BIT(2)    /* 64-bit DS Area. */
#define _CPUID_BIT_MONITOR CPUID_1_ECX_BIT(3)   /* 'MONITOR'/'MWAIT'. */
#define _CPUID_BIT_DSCPL  CPUID_1_ECX_BIT(4)    /* CPL Qualified Debug Store. */
#define _CPUID_BIT_VMX    CPUID_1_ECX_BIT(5)    /* Virtual Machine Extensions. */
#define _CPUID_BIT_SMX    CPUID_1_ECX_BIT(6)    /* Safer Mode Extensions. */
#define _CPUID_BIT_EIST   CPUID_1_ECX_BIT(7)    /* Enhanced Intel SpeedStep technology. */
#define _CPUID_BIT_TM2    CPUID_1_ECX_BIT(8)    /* Thermal Monitor 2. */
#define _CPUID_BIT_SSSE3  CPUID_1_ECX_BIT(9)    /* Supplemental Streaming SIMD Extensions 3 (SSSE3). */
#define _CPUID_BIT_CNXTID CPUID_1_ECX_BIT(10)   /* L1 Context ID. */
#define _CPUID_BIT_SDBG   CPUID_1_ECX_BIT(11)   /* IA32_DEBUG_INTERFACE MSR for silicon debug. */
#define _CPUID_BIT_FMA    CPUID_1_ECX_BIT(12)   /* FMA extensions using YMM state. */
#define _CPUID_BIT_CMPXCHG16B CPUID_1_ECX_BIT(13)       /* CMPXCHG16B Available. */
#define _CPUID_BIT_XTPR   CPUID_1_ECX_BIT(14)   /* xTPR Update Control. */
#define _CPUID_BIT_PDCM   CPUID_1_ECX_BIT(15)   /* Perfmon and Debug Capability. */
#define _CPUID_BIT_PCID   CPUID_1_ECX_BIT(17)   /* Process-context identifiers. */
#define _CPUID_BIT_DCA    CPUID_1_ECX_BIT(18)   /* Supports prefetch data from a memory mapped device. */
#define _CPUID_BIT_SSE4_1 CPUID_1_ECX_BIT(19)   /* Supports SSE4.1. */
#define _CPUID_BIT_SSE4_2 CPUID_1_ECX_BIT(20)   /* Supports SSE4.2. */
#define _CPUID_BIT_X2APIC CPUID_1_ECX_BIT(21)   /* Supports x2APIC feature. */
#define _CPUID_BIT_MOVBE  CPUID_1_ECX_BIT(22)   /* Supports MOVBE instruction. */
#define _CPUID_BIT_POPCNT CPUID_1_ECX_BIT(23)   /* Supports the POPCNT instruction. */
#define _CPUID_BIT_TSC_DEADLINE CPUID_1_ECX_BIT(24)     /* Local APIC timer supports one-shot operation using a TSC deadline value. */
#define _CPUID_BIT_AESNI  CPUID_1_ECX_BIT(25)   /* Supports the AESNI instruction extensions. */
#define _CPUID_BIT_XSAVE  CPUID_1_ECX_BIT(26)   /* XSAVE/XRSTOR/XSETBV/XGETBV instructions instructions. */
#define _CPUID_BIT_OSXSAVE CPUID_1_ECX_BIT(27)  /* XSAVE instruction enabled in the OS. */
#define _CPUID_BIT_AVX    CPUID_1_ECX_BIT(28)   /* AVX instruction extensions. */
#define _CPUID_BIT_F16C   CPUID_1_ECX_BIT(29)   /* Supports 16-bit floating-point conversion instructions. */
#define _CPUID_BIT_RDRAND CPUID_1_ECX_BIT(30)   /* Supports RDRAND instruction. */

/* Intel-defined CPU features, CPUID level 0x00000001 (EDX). */

#define _CPUID_BIT_FPU    CPUID_1_EDX_BIT(0)    /* Floating-Point Unit On-Chip. */
#define _CPUID_BIT_VME    CPUID_1_EDX_BIT(1)    /* Virtual 8086 Mode Enhancements. */
#define _CPUID_BIT_DE     CPUID_1_EDX_BIT(2)    /* Debugging Extensions. */
#define _CPUID_BIT_PSE    CPUID_1_EDX_BIT(3)    /* Page Size Extension. */
#define _CPUID_BIT_TSC    CPUID_1_EDX_BIT(4)    /* Time Stamp Counter. */
#define _CPUID_BIT_MSR    CPUID_1_EDX_BIT(5)    /* Model Specific Registers RDMSR and WRMSR Instructions. */
#define _CPUID_BIT_PAE    CPUID_1_EDX_BIT(6)    /* Physical Address Extension. */
#define _CPUID_BIT_MCE    CPUID_1_EDX_BIT(7)    /* Machine Check Exception. */
#define _CPUID_BIT_CX8    CPUID_1_EDX_BIT(8)    /* CMPXCHG8B Instruction. */
#define _CPUID_BIT_APIC   CPUID_1_EDX_BIT(9)    /* APIC On-Chip. */
#define _CPUID_BIT_SEP    CPUID_1_EDX_BIT(11)   /* SYSENTER and SYSEXIT Instructions. */
#define _CPUID_BIT_MTRR   CPUID_1_EDX_BIT(12)   /* Memory Type Range Registers. */
#define _CPUID_BIT_PGE    CPUID_1_EDX_BIT(13)   /* Page Global Bit. */
#define _CPUID_BIT_MCA    CPUID_1_EDX_BIT(14)   /* Machine Check Architecture. */
#define _CPUID_BIT_CMOV   CPUID_1_EDX_BIT(15)   /* Conditional Move Instructions. */
#define _CPUID_BIT_PAT    CPUID_1_EDX_BIT(16)   /* Page Attribute Table. */
#define _CPUID_BIT_PSE36  CPUID_1_EDX_BIT(17)   /* 36-Bit Page Size Extension. */
#define _CPUID_BIT_PSN    CPUID_1_EDX_BIT(18)   /* Processor Serial Number. */
#define _CPUID_BIT_CLFSH  CPUID_1_EDX_BIT(19)   /* CLFLUSH Instruction. */
#define _CPUID_BIT_DS     CPUID_1_EDX_BIT(21)   /* Debug Store. */
#define _CPUID_BIT_ACPI   CPUID_1_EDX_BIT(22)   /* Thermal Monitor and Software Controlled Clock Facilities. */
#define _CPUID_BIT_MMX    CPUID_1_EDX_BIT(23)   /* Intel MMX Technology. */
#define _CPUID_BIT_FXSR   CPUID_1_EDX_BIT(24)   /* FXSAVE and FXRSTOR Instructions. */
#define _CPUID_BIT_SSE    CPUID_1_EDX_BIT(25)   /* SSE. */
#define _CPUID_BIT_SSE2   CPUID_1_EDX_BIT(26)   /* SSE2. */
#define _CPUID_BIT_SS     CPUID_1_EDX_BIT(27)   /* Self Snoop. */
#define _CPUID_BIT_HTT    CPUID_1_EDX_BIT(28)   /* Max APIC IDs reserved field is Valid. */
#define _CPUID_BIT_TM     CPUID_1_EDX_BIT(29)   /* Thermal Monitor. */
#define _CPUID_BIT_PBE    CPUID_1_EDX_BIT(31)   /* Pending Break Enable. */

struct cpuinfo_x86 {
    u32 x86_cpuid_level;

    char x86_vendor_id[16];

    u8 x86_family;
    u8 x86_model;
    u8 x86_stepping;

    /* Use union to force 'unsigned long' alignment.
     * If 'x86_capability' is not aligned then any atomic bit operation (set or
     * clear) can end up crossing a cache line boundary, which causes the CPU
     * to do a full bus lock (it can not lock both cache lines at once). **/

    union {
        u32 x86_capability[__N_CPUID];
        unsigned long UNUSED;
    };
};

extern struct cpuinfo_x86 cpu_info __percpu;

/* We assume 'bit' is always constant! **/
#define __test_cpu_cap(c, bit) \
    ((1UL << ((bit) & 31)) & ((c)->x86_capability[(bit) >> 5]))

#endif /* __X86_ASM_CPUINFO_H__ */
