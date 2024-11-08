/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_IRQ_H__
#define __X86_ASM_IRQ_H__

#include <asm/desc_types.h>
#include <uapi/asm/eflags.h>

/* 6.2 EXCEPTION AND INTERRUPT VECTORS. */

/* The processor uses the vector number assigned to an exception or interrupt
 * as an index into the interrupt descriptor table (IDT). The table provides the
 * entry point to an exception or interrupt handler.
 */

#define IDT_ENTRIES 256
#define IDT_EXCEPTION_ENTRIES 32
#define IDT_EXCEPTION_WITH_ERRCODE_MASK 0x227D00

/* Architecture-defined exceptions and NMI interrupt. */

#define EXCEPTION_DE  0    /* #DE Divide-by-zero (Fault, No error code). */
#define EXCEPTION_DB  1    /* #DB Debug (Fault, No error code). */
#define EXCEPTION_NMI 2    /* NMI Interrupt. */
#define EXCEPTION_BP  3    /* #BP Breakpoint (Trap, No error code). */
#define EXCEPTION_OF  4    /* #OF Overflow (Trap, No error code). */
#define EXCEPTION_BR  5    /* #BR Bound Range Exceeded (Fault, No error code). */
#define EXCEPTION_UD  6    /* #UD Undefined Opcode (Fault, No error code). */
#define EXCEPTION_NM  7    /* #NM Device Not Available (Fault, No error code). */
#define EXCEPTION_DF  8    /* #DF Double Fault (Abort, Error code - zero). */
#define EXCEPTION_TS 10    /* #TS Invalid TSS (Fault, Error code). */
#define EXCEPTION_NP 11    /* #NP Segment Not Present (Fault, Error code). */
#define EXCEPTION_SS 12    /* #SS Stack Segment Fault (Fault, Error code). */
#define EXCEPTION_GP 13    /* #GP General Protection Fault (Fault, Error code). */
#define EXCEPTION_PF 14    /* #PF Page Fault (Fault, Error code). */
#define EXCEPTION_MF 16    /* #MF x87 Floating-Point Exception (Fault, No error code). */
#define EXCEPTION_AC 17    /* #AC Alignment Check (Fault, Error code - zero). */
#define EXCEPTION_MC 18    /* #MC Machine Check (Abort, No error code). */
#define EXCEPTION_XF 19    /* #XM SIMD Floating-Point Exception (Fault, No error code). */
#define EXCEPTION_VE 20    /* #VE Virtualization Exception (Fault, No error code). */
#define EXCEPTION_CP 21    /* #CP Control Protection Exception (Fault, Error code). */

/* VECTORS LAYOUT. */
/* The allowable range for vector numbers is 0 to 255. */

/*  - Vectors 0 .. IDT_EXCEPTION_ENTRIES - 1
 *    Reserved for architecture-defined exceptions and interrupts.
 *  - Vectors IDT_EXCEPTION_ENTRIES .. IDT_ENTRIES - 1
 *    User-defined and special interrupts.
 */

#define FIRST_EXTERNAL_VECTOR IDT_EXCEPTION_ENTRIES

#define NR_EXTERNAL_VECTORS (IDT_ENTRIES - FIRST_EXTERNAL_VECTOR)

#define SPURIOUS_APIC_VECTOR 255

#ifndef __ASSEMBLY__

static __always_inline unsigned long arch_local_save_flags(void)
{
    unsigned long flags;

    asm volatile(
        "pushf      ;"
        "pop    %0  ;"
        : "=rm" (flags)
        : : "memory"
    );

    return flags;
}

static __always_inline void arch_local_irq_disable(void)
{
    asm volatile("cli" : : : "memory");
}

static __always_inline void arch_local_irq_enable(void)
{
    asm volatile("sti" : : : "memory");
}

static __always_inline int arch_irqs_disabled_flags(unsigned long flags)
{
    return !(flags & _EFLAGS_IF);
}

static __always_inline int arch_irqs_disabled(void)
{
    unsigned long flags = arch_local_save_flags();

    return arch_irqs_disabled_flags(flags);
}

static __always_inline unsigned long arch_local_irq_save(void)
{
    unsigned long flags = arch_local_save_flags();
    arch_local_irq_disable();
    return flags;
}

static __always_inline void arch_local_irq_restore(unsigned long flags)
{
    if (!arch_irqs_disabled_flags(flags))
        arch_local_irq_enable();
}

static __always_inline void halt(void)
{
    asm volatile("hlt" : : : "memory");
}

#endif /* !__ASSEMBLY__ */

#endif /* __X86_ASM_IRQ_H__ */
