/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CPUID_H__
#define __X86_ASM_CPUID_H__

static inline void cpuid(unsigned int *eax, unsigned int *ebx,
    unsigned int *ecx, unsigned int *edx)
{
    /* Returns processor identification and feature information to the 'EAX',
     * 'EBX', 'ECX', and 'EDX' registers, as determined by input entered in
     * 'EAX' (in some cases, 'ECX' as well). **/

    asm volatile ("cpuid":"=a" (*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        :"0"(*eax), "2"(*ecx)
        :"memory");
}

/* Some CPUID calls require 'ECX', e.g. Structured Extended Feature Flags Enumeration Leaf. */

static inline void __get_cpuid_with_ecx(unsigned int op, unsigned int ecx,
    unsigned int *__eax, unsigned int *__ebx, unsigned int *__ecx,
    unsigned int *__edx)
{
    *__eax = op;
    *__ecx = ecx;
    cpuid(__eax, __ebx, __ecx, __edx);
}

/* Clear 'ECX' since some cpus do not set or clear 'ECX' resulting in stale
 * register contents being returned. **/

#define get_cpuid(op, a, b, c, d) __get_cpuid_with_ecx(op, 0, a, b, c, d)

#define get_cpuid_reg(reg) \
static inline unsigned int get_cpuid_##reg(unsigned int op) \
{ \
    unsigned int eax, ebx, ecx, edx; \
  \
    get_cpuid(op, &eax, &ebx, &ecx, &edx); \
  \
    return reg; \
}

/* CPUID functions returning a single datum. **/

get_cpuid_reg(eax);
get_cpuid_reg(ebx);
get_cpuid_reg(ecx);
get_cpuid_reg(edx);

#endif /* __X86_ASM_CPUID_H__ */
