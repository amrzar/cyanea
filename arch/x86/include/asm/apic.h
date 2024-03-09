/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_APIC_H__
#define __X86_ASM_APIC_H__

#include <cyanea/types.h>

#include <asm/apic_defs.h>

struct apic {

};

# define apic_driver(sym) \
    static const struct apic *__apic_driver_ ## sym __used \
    __aligned(__alignof__(struct apic *)) \
    __section(".apic.drivers") = { &sym }

extern struct apic *__apic_drivers_start[], *__apic_drivers_end[];

extern struct apic *apic;


#endif /* __X86_ASM_APIC_H__ */
