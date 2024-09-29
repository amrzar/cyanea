/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CPUINFO_H__
#define __X86_ASM_CPUINFO_H__

#include <cyanea/types.h>
#include <cyanea/percpu.h>

#define CAP_BIT(word, x) (((word) << 5) + (x))

enum cpu_leafs {
    /* Basic CPUID Information. */
    CPUID_1_ECX,
    CPUID_1_EDX,
# define CPUID_1_ECX_BIT(x) CAP_BIT(CPUID_1_ECX, x)
# define CPUID_1_EDX_BIT(x) CAP_BIT(CPUID_1_EDX, x)

    /* Structured Extended Feature Flags. */
    CPUID_7_ECX_0_EBX,
    CPUID_7_ECX_0_ECX,
    CPUID_7_ECX_0_EDX,
# define CPUID_7_ECX_0_EBX_BIT(x) CAP_BIT(CPUID_7_ECX_0_EBX, x)
# define CPUID_7_ECX_0_ECX_BIT(x) CAP_BIT(CPUID_7_ECX_0_ECX, x)
# define CPUID_7_ECX_0_EDX_BIT(x) CAP_BIT(CPUID_7_ECX_0_EDX, x)

    CPUID_LEAFS
};

/* Structured Extended Feature Flags. */
/* ''EAX = 0x07, ECX = 0x0''. */
#define _CPUID_BIT_INVPCID CPUID_7_ECX_0_EBX_BIT(10)

struct cpuinfo_x86 {
    char x86_vendor_id[16];

    u8 x86_family;
    u8 x86_model;
    u8 x86_stepping;

    u8 x86_virt_bits;
    u8 x86_phys_bits;

    u32 x86_cpuid_level;
    u32 extended_cpuid_level;

    u32 x86_capability[CPUID_LEAFS];
};

extern struct cpuinfo_x86 cpu_info __percpu;

#define this_cpu_has(bit) \
    ((1UL << (bit & 0x1F)) & this_cpu_read(cpu_info.x86_capability[bit >> 5]))

#endif /* __X86_ASM_CPUINFO_H__ */
