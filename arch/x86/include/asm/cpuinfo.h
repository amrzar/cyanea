/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CPUINFO_H__
#define __X86_ASM_CPUINFO_H__

#include <cyanea/types.h>
#include <cyanea/percpu.h>

#define CAP_BIT(word, x) (((word) << 5) + (x))

enum cpu_leafs {
    CPUID_1_ECX,
    CPUID_1_EDX,
    CPUID_7_ECX_0_EBX,
    CPUID_7_ECX_0_ECX,
    CPUID_7_ECX_0_EDX,
    CPUID_LEAFS
};

#define CPUID_1_ECX_BIT(x) CAP_BIT(CPUID_1_ECX, x)
#define CPUID_1_EDX_BIT(x) CAP_BIT(CPUID_1_EDX, x)
#define CPUID_7_ECX_0_EBX_BIT(x) CAP_BIT(CPUID_7_ECX_0_EBX, x)
#define CPUID_7_ECX_0_ECX_BIT(x) CAP_BIT(CPUID_7_ECX_0_ECX, x)
#define CPUID_7_ECX_0_EDX_BIT(x) CAP_BIT(CPUID_7_ECX_0_EDX, x)

/* Basic CPUID Information, CPUID level 0x00000001 (ECX). */

#define _CPUID_BIT_SSE3 CPUID_1_ECX_BIT(0)      /* Streaming SIMD Extensions 3 (SSE3). */
#define _CPUID_BIT_PCLMULQDQ CPUID_1_ECX_BIT(1) /* 'PCLMULQDQ'. */
#define _CPUID_BIT_DTES64 CPUID_1_ECX_BIT(2)    /* 64-bit DS Area. */
#define _CPUID_BIT_MONITOR CPUID_1_ECX_BIT(3)   /* 'MONITOR'/'MWAIT'. */
#define _CPUID_BIT_DSCPL CPUID_1_ECX_BIT(4)     /* CPL Qualified Debug Store. */
#define _CPUID_BIT_VMX CPUID_1_ECX_BIT(5)       /* Virtual Machine Extensions. */
#define _CPUID_BIT_SMX CPUID_1_ECX_BIT(6)       /* Safer Mode Extensions. */
#define _CPUID_BIT_EIST CPUID_1_ECX_BIT(7)      /* Enhanced Intel SpeedStep technology. */
#define _CPUID_BIT_TM2 CPUID_1_ECX_BIT(8)       /* Thermal Monitor 2. */
#define _CPUID_BIT_SSSE3 CPUID_1_ECX_BIT(9)     /* Supplemental Streaming SIMD Extensions 3 (SSSE3). */
#define _CPUID_BIT_CNXTID CPUID_1_ECX_BIT(10)   /* L1 Context ID. */
#define _CPUID_BIT_SDBG CPUID_1_ECX_BIT(11)     /* IA32_DEBUG_INTERFACE MSR for silicon debug. */
#define _CPUID_BIT_FMA CPUID_1_ECX_BIT(12)      /* FMA extensions using YMM state. */
#define _CPUID_BIT_CMPXCHG16B CPUID_1_ECX_BIT(13)       /* CMPXCHG16B Available. */
#define _CPUID_BIT_XTPR CPUID_1_ECX_BIT(14)     /* xTPR Update Control. */
#define _CPUID_BIT_PDCM CPUID_1_ECX_BIT(15)     /* Perfmon and Debug Capability. */
#define _CPUID_BIT_PCID CPUID_1_ECX_BIT(17)     /* Process-context identifiers. */
#define _CPUID_BIT_DCA CPUID_1_ECX_BIT(18)      /* Supports prefetch data from a memory mapped device. */
#define _CPUID_BIT_SSE4_1 CPUID_1_ECX_BIT(19)   /* Supports SSE4.1. */
#define _CPUID_BIT_SSE4_2 CPUID_1_ECX_BIT(20)   /* Supports SSE4.2. */
#define _CPUID_BIT_X2APIC CPUID_1_ECX_BIT(21)   /* Supports x2APIC feature. */
#define _CPUID_BIT_MOVBE CPUID_1_ECX_BIT(22)    /* Supports MOVBE instruction. */
#define _CPUID_BIT_POPCNT CPUID_1_ECX_BIT(23)   /* Supports the POPCNT instruction. */
#define _CPUID_BIT_TSC_DEADLINE CPUID_1_ECX_BIT(24)     /* Local APIC timer supports one-shot operation using a TSC deadline value. */
#define _CPUID_BIT_AESNI CPUID_1_ECX_BIT(25)    /* Supports the AESNI instruction extensions. */
#define _CPUID_BIT_XSAVE CPUID_1_ECX_BIT(26)    /* XSAVE/XRSTOR/XSETBV/XGETBV instructions instructions. */
#define _CPUID_BIT_OSXSAVE CPUID_1_ECX_BIT(27)  /* XSAVE instruction enabled in the OS. */
#define _CPUID_BIT_AVX CPUID_1_ECX_BIT(28)      /* AVX instruction extensions. */
#define _CPUID_BIT_F16C CPUID_1_ECX_BIT(29)     /* Supports 16-bit floating-point conversion instructions. */
#define _CPUID_BIT_RDRAND CPUID_1_ECX_BIT(30)   /* Supports RDRAND instruction. */

/* Basic CPUID Information, CPUID level 0x00000001 (EDX). */

#define _CPUID_BIT_FPU CPUID_1_EDX_BIT(0)       /* Floating-Point Unit On-Chip. */
#define _CPUID_BIT_VME CPUID_1_EDX_BIT(1)       /* Virtual 8086 Mode Enhancements. */
#define _CPUID_BIT_DE CPUID_1_EDX_BIT(2)        /* Debugging Extensions. */
#define _CPUID_BIT_PSE CPUID_1_EDX_BIT(3)       /* Page Size Extension. */
#define _CPUID_BIT_TSC CPUID_1_EDX_BIT(4)       /* Time Stamp Counter. */
#define _CPUID_BIT_MSR CPUID_1_EDX_BIT(5)       /* Model Specific Registers RDMSR and WRMSR Instructions. */
#define _CPUID_BIT_PAE CPUID_1_EDX_BIT(6)       /* Physical Address Extension. */
#define _CPUID_BIT_MCE CPUID_1_EDX_BIT(7)       /* Machine Check Exception. */
#define _CPUID_BIT_CX8 CPUID_1_EDX_BIT(8)       /* CMPXCHG8B Instruction. */
#define _CPUID_BIT_APIC CPUID_1_EDX_BIT(9)      /* APIC On-Chip. */
#define _CPUID_BIT_SEP CPUID_1_EDX_BIT(11)      /* SYSENTER and SYSEXIT Instructions. */
#define _CPUID_BIT_MTRR CPUID_1_EDX_BIT(12)     /* Memory Type Range Registers. */
#define _CPUID_BIT_PGE CPUID_1_EDX_BIT(13)      /* Page Global Bit. */
#define _CPUID_BIT_MCA CPUID_1_EDX_BIT(14)      /* Machine Check Architecture. */
#define _CPUID_BIT_CMOV CPUID_1_EDX_BIT(15)     /* Conditional Move Instructions. */
#define _CPUID_BIT_PAT CPUID_1_EDX_BIT(16)      /* Page Attribute Table. */
#define _CPUID_BIT_PSE36 CPUID_1_EDX_BIT(17)    /* 36-Bit Page Size Extension. */
#define _CPUID_BIT_PSN CPUID_1_EDX_BIT(18)      /* Processor Serial Number. */
#define _CPUID_BIT_CLFSH CPUID_1_EDX_BIT(19)    /* CLFLUSH Instruction. */
#define _CPUID_BIT_DS CPUID_1_EDX_BIT(21)       /* Debug Store. */
#define _CPUID_BIT_ACPI CPUID_1_EDX_BIT(22)     /* Thermal Monitor and Software Controlled Clock Facilities. */
#define _CPUID_BIT_MMX CPUID_1_EDX_BIT(23)      /* Intel MMX Technology. */
#define _CPUID_BIT_FXSR CPUID_1_EDX_BIT(24)     /* FXSAVE and FXRSTOR Instructions. */
#define _CPUID_BIT_SSE CPUID_1_EDX_BIT(25)      /* SSE. */
#define _CPUID_BIT_SSE2 CPUID_1_EDX_BIT(26)     /* SSE2. */
#define _CPUID_BIT_SS CPUID_1_EDX_BIT(27)       /* Self Snoop. */
#define _CPUID_BIT_HTT CPUID_1_EDX_BIT(28)      /* Max APIC IDs reserved field is valid. */
#define _CPUID_BIT_TM CPUID_1_EDX_BIT(29)       /* Thermal Monitor. */
#define _CPUID_BIT_PBE CPUID_1_EDX_BIT(31)      /* Pending Break Enable. */

/* Structured Extended Feature Flags Enumeration Leaf, CPUID level 0x00000007:0 (EBX). */

#define _CPUID_BIT_FSGSBASE CPUID_7_ECX_0_EBX_BIT(0)    /* Supports RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE. */
#define _CPUID_BIT_TSC_ADJUST CPUID_7_ECX_0_EBX_BIT(1)  /* IA32_TSC_ADJUST MSR is supported. */
#define _CPUID_BIT_SGX  CPUID_7_ECX_0_EBX_BIT(2)        /* Supports Intel Software Guard Extensions (Intel® SGX Extensions). */
#define _CPUID_BIT_BMI1 CPUID_7_ECX_0_EBX_BIT(3)        /* 1st group bit manipulation extensions. */
#define _CPUID_BIT_HLE  CPUID_7_ECX_0_EBX_BIT(4)        /* Hardware Lock Elision. */
#define _CPUID_BIT_AVX2 CPUID_7_ECX_0_EBX_BIT(5)        /* Supports Intel Advanced Vector Extensions 2 (Intel® AVX2). */
#define _CPUID_BIT_FDP_EXCPTN_ONLY CPUID_7_ECX_0_EBX_BIT(6)     /* x87 FPU Data Pointer updated only on x87 exceptions. */
#define _CPUID_BIT_SMEP CPUID_7_ECX_0_EBX_BIT(7)        /* Supervisor Mode Execution Protection. */
#define _CPUID_BIT_BMI2 CPUID_7_ECX_0_EBX_BIT(8)        /* 2nd group bit manipulation extensions. */
#define _CPUID_BIT_ERMS CPUID_7_ECX_0_EBX_BIT(9)        /* Enhanced REP MOVSB/STOSB instructions. */
#define _CPUID_BIT_INVPCID CPUID_7_ECX_0_EBX_BIT(10)    /* Invalidate Processor Context ID. */
#define _CPUID_BIT_RTM CPUID_7_ECX_0_EBX_BIT(11)        /* Restricted Transactional Memory. */
#define _CPUID_BIT_CQM CPUID_7_ECX_0_EBX_BIT(12)        /* Cache QoS Monitoring. */
#define _CPUID_BIT_ZERO_FCS_FDS CPUID_7_ECX_0_EBX_BIT(13)       /* Deprecates FPU CS and FPU DS values. */
#define _CPUID_BIT_MPX CPUID_7_ECX_0_EBX_BIT(14)        /* Memory Protection Extension. */
#define _CPUID_BIT_RDT_A CPUID_7_ECX_0_EBX_BIT(15)      /* Resource Director Technology Allocation. */
#define _CPUID_BIT_AVX512F CPUID_7_ECX_0_EBX_BIT(16)    /* AVX-512 Foundation. */
#define _CPUID_BIT_AVX512DQ CPUID_7_ECX_0_EBX_BIT(17)   /* AVX-512 DQ (Double/Quad granular) Instructions. */
#define _CPUID_BIT_RDSEED CPUID_7_ECX_0_EBX_BIT(18)     /* RDSEED instruction. */
#define _CPUID_BIT_ADX CPUID_7_ECX_0_EBX_BIT(19)        /* ADCX and ADOX instructions. */
#define _CPUID_BIT_SMAP CPUID_7_ECX_0_EBX_BIT(20)       /* Supervisor Mode Access Prevention. */
#define _CPUID_BIT_AVX512IFMA CPUID_7_ECX_0_EBX_BIT(21) /* AVX-512 Integer Fused Multiply-Add instructions. */
#define _CPUID_BIT_CLFLUSHOPT CPUID_7_ECX_0_EBX_BIT(23) /* CLFLUSHOPT instruction. */
#define _CPUID_BIT_CLWB CPUID_7_ECX_0_EBX_BIT(24)       /* CLWB instruction. */
#define _CPUID_BIT_INTEL_PT CPUID_7_ECX_0_EBX_BIT(25)   /* Intel Processor Trace. */
#define _CPUID_BIT_AVX512PF CPUID_7_ECX_0_EBX_BIT(26)   /* AVX-512 Prefetch. */
#define _CPUID_BIT_AVX512ER CPUID_7_ECX_0_EBX_BIT(27)   /* AVX-512 Exponential and Reciprocal. */
#define _CPUID_BIT_AVX512CD CPUID_7_ECX_0_EBX_BIT(28)   /* AVX-512 Conflict Detection. */
#define _CPUID_BIT_SHA_NI CPUID_7_ECX_0_EBX_BIT(29)     /* SHA1/SHA256 Instruction Extensions. */
#define _CPUID_BIT_AVX512BW CPUID_7_ECX_0_EBX_BIT(30)   /* AVX-512 BW (Byte/Word granular) Instructions. */
#define _CPUID_BIT_AVX512VL CPUID_7_ECX_0_EBX_BIT(31)   /* AVX-512 VL (128/256 Vector Length) Extensions. */

/* Structured Extended Feature Flags Enumeration Leaf, CPUID level 0x00000007:0 (ECX). */

#define _CPUID_BIT_AVX512VBMI CPUID_7_ECX_0_ECX_BIT(1)  /* AVX512 Vector Bit Manipulation instructions. */
#define _CPUID_BIT_UMIP CPUID_7_ECX_0_ECX_BIT(2)        /* User Mode Instruction Protection . */
#define _CPUID_BIT_PKU CPUID_7_ECX_0_ECX_BIT(3) /* Protection Keys for Userspace. */
#define _CPUID_BIT_OSPKE CPUID_7_ECX_0_ECX_BIT(4)       /* OS Protection Keys Enable. */
#define _CPUID_BIT_WAITPKG CPUID_7_ECX_0_ECX_BIT(5)     /* UMONITOR/UMWAIT/TPAUSE Instructions. */
#define _CPUID_BIT_AVX512_VBMI2 CPUID_7_ECX_0_ECX_BIT(6)        /* Additional AVX512 Vector Bit Manipulation Instructions. */
#define _CPUID_BIT_SHSTK CPUID_7_ECX_0_ECX_BIT(7)       /* Supports CET shadow stack features. */
#define _CPUID_BIT_GFNI CPUID_7_ECX_0_ECX_BIT(8)        /* Galois Field New Instructions. */
#define _CPUID_BIT_VAES CPUID_7_ECX_0_ECX_BIT(9)        /* Vector AES. */
#define _CPUID_BIT_VPCLMULQDQ CPUID_7_ECX_0_ECX_BIT(10) /* Carry-Less Multiplication Double Quadword. */
#define _CPUID_BIT_AVX512_VNNI CPUID_7_ECX_0_ECX_BIT(11)        /* Vector Neural Network Instructions . */
#define _CPUID_BIT_AVX512_BITALG CPUID_7_ECX_0_ECX_BIT(12)      /* Support for VPOPCNT[B,W] and VPSHUF-BITQMB instructions. */
#define _CPUID_BIT_TME CPUID_7_ECX_0_ECX_BIT(13)        /* Intel Total Memory Encryption. */
#define _CPUID_BIT_AVX512_VPOPCNTDQ CPUID_7_ECX_0_ECX_BIT(14)   /* POPCNT for vectors of DW/QW. */
#define _CPUID_BIT_LA57 CPUID_7_ECX_0_ECX_BIT(16)       /* 5-level page tables. */
#define _CPUID_BIT_RDPID CPUID_7_ECX_0_ECX_BIT(22)      /* RDPID instruction. */
#define _CPUID_BIT_BUS_LOCK_DETECT CPUID_7_ECX_0_ECX_BIT(24)    /* Bus Lock detect. */
#define _CPUID_BIT_CLDEMOTE CPUID_7_ECX_0_ECX_BIT(25)   /* CLDEMOTE instruction. */
#define _CPUID_BIT_MOVDIRI CPUID_7_ECX_0_ECX_BIT(27)    /* MOVDIRI instruction. */
#define _CPUID_BIT_MOVDIR64B CPUID_7_ECX_0_ECX_BIT(28)  /* MOVDIR64B instruction. */
#define _CPUID_BIT_ENQCMD CPUID_7_ECX_0_ECX_BIT(29)     /* ENQCMD and ENQCMDS instructions. */
#define _CPUID_BIT_SGX_LC CPUID_7_ECX_0_ECX_BIT(30)     /* Software Guard Extensions Launch Control. */

/* Structured Extended Feature Flags Enumeration Leaf, CPUID level 0x00000007:0 (EDX). */

#define _CPUID_BIT_AVX512_4VNNIW CPUID_7_ECX_0_EDX_BIT(2)       /* AVX-512 Neural Network Instructions. */
#define _CPUID_BIT_AVX512_4FMAPS CPUID_7_ECX_0_EDX_BIT(3)       /* AVX-512 Multiply Accumulation Single precision. */
#define _CPUID_BIT_FSRM CPUID_7_ECX_0_EDX_BIT(4)        /* Fast Short Rep Mov. */
#define _CPUID_BIT_AVX512_VP2INTERSECT CPUID_7_ECX_0_EDX_BIT(8) /* AVX-512 Intersect for D/Q. */
#define _CPUID_BIT_SRBDS_CTRL CPUID_7_ECX_0_EDX_BIT(9)  /* SRBDS mitigation MSR available. */
#define _CPUID_BIT_MD_CLEAR CPUID_7_ECX_0_EDX_BIT(10)   /* VERW clears CPU buffers. */
#define _CPUID_BIT_RTM_ALWAYS_ABORT CPUID_7_ECX_0_EDX_BIT(11)   /* RTM transaction always aborts. */
#define _CPUID_BIT_TSX_FORCE_ABORT CPUID_7_ECX_0_EDX_BIT(13)    /* TSX_FORCE_ABORT. */
#define _CPUID_BIT_SERIALIZE CPUID_7_ECX_0_EDX_BIT(14)  /* SERIALIZE instruction. */
#define _CPUID_BIT_HYBRID_CPU CPUID_7_ECX_0_EDX_BIT(15)
#define _CPUID_BIT_TSXLDTRK CPUID_7_ECX_0_EDX_BIT(16)   /* TSX Suspend Load Address Tracking. */
#define _CPUID_BIT_PCONFIG CPUID_7_ECX_0_EDX_BIT(18)    /* Intel PCONFIG. */
#define _CPUID_BIT_ARCH_LBR CPUID_7_ECX_0_EDX_BIT(19)   /* Intel ARCH LBR. */
#define _CPUID_BIT_IBT CPUID_7_ECX_0_EDX_BIT(20)        /* Indirect Branch Tracking. */
#define _CPUID_BIT_AMX_BF16 CPUID_7_ECX_0_EDX_BIT(22)   /* AMX bf16 Support. */
#define _CPUID_BIT_AVX512_FP16 CPUID_7_ECX_0_EDX_BIT(23)        /* AVX512 FP16. */
#define _CPUID_BIT_AMX_TILE CPUID_7_ECX_0_EDX_BIT(24)   /* AMX tile Support. */
#define _CPUID_BIT_AMX_INT8 CPUID_7_ECX_0_EDX_BIT(25)   /* AMX int8 Support. */
#define _CPUID_BIT_SPEC_CTRL CPUID_7_ECX_0_EDX_BIT(26)  /* Speculation Control (IBRS + IBPB). */
#define _CPUID_BIT_INTEL_STIBP CPUID_7_ECX_0_EDX_BIT(27)        /* Single Thread Indirect Branch Predictors (STIBP). */
#define _CPUID_BIT_FLUSH_L1D CPUID_7_ECX_0_EDX_BIT(28)  /* L1D_FLUSH. */
#define _CPUID_BIT_ARCH_CAPABILITIES CPUID_7_ECX_0_EDX_BIT(29)  /* IA32_ARCH_CAPABILITIES MSR. */
#define _CPUID_BIT_CORE_CAPABILITIES CPUID_7_ECX_0_EDX_BIT(30)  /* IA32_CORE_CAPABILITIES MSR. */
#define _CPUID_BIT_SPEC_CTRL_SSBD CPUID_7_ECX_0_EDX_BIT(31)     /* Speculative Store Bypass Disable (SSBD). */

struct cpuinfo_x86 {
    u32 x86_cpuid_level;

    u32 extended_cpuid_level;

    char x86_vendor_id[16];

    u8 x86_family;
    u8 x86_model;
    u8 x86_stepping;

    u8 x86_virt_bits;
    u8 x86_phys_bits;

    u32 x86_capability[CPUID_LEAFS];
};

extern struct cpuinfo_x86 cpu_info __percpu;

/* Hopefully, 'bit' is constant. */

#define cpu_has(c, bit)   \
    ((1UL << (bit & 0x1F)) & ((c)->x86_capability[bit >> 5]))

#define this_cpu_has(bit) \
    ((1UL << (bit & 0x1F)) & this_cpu_read(cpu_info.x86_capability[bit >> 5]))

#endif /* __X86_ASM_CPUINFO_H__ */
