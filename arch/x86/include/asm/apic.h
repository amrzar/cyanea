/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_APIC_H__
#define __X86_ASM_APIC_H__

#include <cyanea/types.h>
#include <asm/cpuinfo.h>
#include <asm/fixmap.h>
#include <asm/io.h>
#include <asm/msr.h>

#include "apic_defs.h"

/* ''x2APIC'' Support. */

/* 11.12.1 Detecting and Enabling x2APIC Mode. */

static inline int apic_is_x2apic_enabled(void)
{
	return !!(rdmsrl(MSR_IA32_APICBASE) & MSR_IA32_APICBASE_X2APIC_ENABLE);
}

static inline int x2apic_enabled(void)
{
	return this_cpu_has(_CPUID_BIT_X2APIC) && apic_is_x2apic_enabled();
}

/* 11.12.1.2 x2APIC Register Address Space. */

static inline void native_apic_msr_write(u32 reg, u32 value)
{
	wrmsr(APIC_BASE_MSR + (reg >> 4), value, 0);
}

static inline u32 native_apic_msr_read(u32 reg)
{
	return (u32)(rdmsrl(APIC_BASE_MSR + (reg >> 4)));
}

/* ''xAPIC'' Support. */

static inline void native_apic_mem_write(u32 reg, u32 value)
{
	writel(value, (volatile void *)(APIC_BASE + reg));
}

static inline u32 native_apic_mem_read(u32 reg)
{
	return readl((const volatile void *)(APIC_BASE + reg));
}

struct apic {
	u32 (*read)(u32 reg);
	void (*write)(u32 reg, u32 value);

	u32 (*get_apic_id)(void);
	char *name;
};

extern struct apic *apic;

static __always_inline u32 apic_read(u32 reg)
{
	return apic->read(reg);
}

static __always_inline void apic_write(u32 reg, u32 value)
{
	apic->write(reg, value);
}

static __always_inline u32 read_apic_id(void)
{
	return apic->get_apic_id();
}

extern struct apic *__apic_drivers_start[], *__apic_drivers_end[];

#define apic_driver(sym) \
	static const struct apic *__apic_driver_ ## sym __used  \
	__aligned(__alignof__(struct apic *))               \
	__section(".apic.drivers") = &sym

#endif /* __X86_ASM_APIC_H__ */
