/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/types.h>
#include <cyanea/percpu.h>

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

void __init early_cpuinfo_init(void)
{
    u32 eax, ebx, ecx, edx;

    /* Get pointer for BSP's 'cpu_info'. */
    struct cpuinfo_x86 *c = this_cpu_ptr(&cpu_info);

    /* Get vendor ID. */
    get_cpuid(0x00000000, (unsigned int *)(&c->x86_cpuid_level),
        (unsigned int *)(&c->x86_vendor_id[0]),
        (unsigned int *)(&c->x86_vendor_id[8]),
        (unsigned int *)(&c->x86_vendor_id[4]));

    /* Basic CPUID Information. */
    if (c->x86_cpuid_level >= 0x00000001) {
        get_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

        c->x86_family = x86_family(eax);
        c->x86_model = x86_model(eax);
        c->x86_stepping = x86_stepping(eax);

        c->x86_capability[CPUID_1_ECX] = ecx;
        c->x86_capability[CPUID_1_EDX] = edx;
    }

    /* Structured Extended Feature. */
    if (c->x86_cpuid_level >= 0x00000007) {
        get_cpuid_with_ecx(0x00000007, 0, &eax, &ebx, &ecx, &edx);

        c->x86_capability[CPUID_7_ECX_0_EBX] = ebx;
        c->x86_capability[CPUID_7_ECX_0_ECX] = ecx;
        c->x86_capability[CPUID_7_ECX_0_EDX] = edx;
    }

    /* Extended Function CPUID Information. */
    eax = get_cpuid_eax(0x80000000);
    c->extended_cpuid_level = eax;

    if (c->extended_cpuid_level >= 0x80000008) {
        get_cpuid(0x80000008, &eax, &ebx, &ecx, &edx);

        c->x86_virt_bits = (eax >> 8) & 0xFF;
        c->x86_phys_bits = eax & 0xFF;
    } else {
        c->x86_phys_bits = 36;
        c->x86_virt_bits = 48;
    }
}
