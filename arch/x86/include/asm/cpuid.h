/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CPUID_H__
#define __X86_ASM_CPUID_H__

static inline void cpuid(unsigned int *eax, unsigned int *ebx,
    unsigned int *ecx, unsigned int *edx)
{
    /* See 3.3 INSTRUCTIONS (A-L). */

    asm volatile("cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "0" (*eax), "2" (*ecx)
        : "memory");
}

/* Some CPUID calls require 'ECX', e.g. Structured Extended Feature Flags Enumeration Leaf. */

static inline void get_cpuid_with_ecx(unsigned int op, unsigned int ecx,
    unsigned int *__eax, unsigned int *__ebx, unsigned int *__ecx,
    unsigned int *__edx)
{
    *__eax = op;
    *__ecx = ecx;
    cpuid(__eax, __ebx, __ecx, __edx);
}

/* Clear 'ECX' since some cpus do not set or clear 'ECX' resulting in stale
 * register contents being returned.
 */

#define get_cpuid(op, a, b, c, d) get_cpuid_with_ecx(op, 0, a, b, c, d)

/* CPUID functions returning a single datum. **/

static inline unsigned int get_cpuid_eax(unsigned int op)
{
    unsigned int eax, ebx, ecx, edx;

    get_cpuid(op, &eax, &ebx, &ecx, &edx);

    return eax;
}

static inline unsigned int get_cpuid_ebx(unsigned int op)
{
    unsigned int eax, ebx, ecx, edx;

    get_cpuid(op, &eax, &ebx, &ecx, &edx);

    return ebx;
}

static inline unsigned int get_cpuid_ecx(unsigned int op)
{
    unsigned int eax, ebx, ecx, edx;

    get_cpuid(op, &eax, &ebx, &ecx, &edx);

    return ecx;
}

static inline unsigned int get_cpuid_edx(unsigned int op)
{
    unsigned int eax, ebx, ecx, edx;

    get_cpuid(op, &eax, &ebx, &ecx, &edx);

    return edx;
}

#endif /* __X86_ASM_CPUID_H__ */
