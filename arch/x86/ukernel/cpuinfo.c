/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/types.h>
#include <asm/cpuid.h>
#include <asm/cpuinfo.h>

struct cpuinfo_x86 cpu_info __percpu;

static inline unsigned int x86_family(u32 eax)
{
    unsigned int family;

    family = (eax >> 8) & 0xF;
    if (family == 0xF)
        family += (eax >> 20) & 0xFF;

    return family;
}

static inline unsigned int x86_model(u32 eax)
{
    unsigned int model;

    model = (eax >> 4) & 0xF;
    if (x86_family(eax) >= 0x6)
        model += ((eax >> 16) & 0xF) << 4;

    return model;
}

static inline unsigned int x86_stepping(u32 eax)
{
    return eax & 0xF;
}

void __init bsp_cpuinfo_init(void)
{
    u32 eax, ebx, ecx, edx;

    get_cpuid(0x00000000, (unsigned int *)(&cpu_info.x86_cpuid_level),
        (unsigned int *)(&cpu_info.x86_vendor_id[0]),
        (unsigned int *)(&cpu_info.x86_vendor_id[8]),
        (unsigned int *)(&cpu_info.x86_vendor_id[4]));

    get_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

    cpu_info.x86_family = x86_family(eax);
    cpu_info.x86_model = x86_model(eax);
    cpu_info.x86_stepping = x86_stepping(eax);

    cpu_info.x86_capability[CPUID_1_ECX] = ecx;
    cpu_info.x86_capability[CPUID_1_EDX] = edx;
}
