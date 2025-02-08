/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CPU_H__
#define __X86_ASM_CPU_H__

#include <cyanea/types.h>
#include <asm/cr_eflags_defs.h>
#include <uapi/asm/bitsperlong.h>

static inline unsigned long native_read_cr0(void)
{
	unsigned long val;
	asm volatile("mov %%cr0, %0" : "=r" (val));
	return val;
}

static inline void native_write_cr0(unsigned long val)
{
	asm volatile("mov %0, %%cr0" : : "r" (val) : "memory");
}

#define read_cr0()     native_read_cr0()
#define write_cr0(x)   native_write_cr0(x)

static inline unsigned long native_read_cr2(void)
{
	unsigned long val;
	asm volatile("mov %%cr2, %0" : "=r" (val));
	return val;
}

static inline void native_write_cr2(unsigned long val)
{
	asm volatile("mov %0, %%cr2" : : "r" (val) : "memory");
}

#define read_cr2()     native_read_cr2()
#define write_cr2(x)   native_write_cr2(x)

static inline unsigned long native_read_cr3(void)
{
	unsigned long val;
	asm volatile("mov %%cr3, %0" : "=r" (val));
	return val;
}

static inline void native_write_cr3(unsigned long val)
{
	asm volatile("mov %0, %%cr3" : : "r" (val) : "memory");
}

#define read_cr3()     native_read_cr3()
#define write_cr3(x)   native_write_cr3(x)

static inline unsigned long native_read_cr4(void)
{
	unsigned long val;
	asm volatile("mov %%cr4, %0" : "=r" (val));
	return val;
}

static inline void native_write_cr4(unsigned long val)
{
	asm volatile("mov %0, %%cr4" : : "r" (val) : "memory");
}

#define __read_cr4()   native_read_cr4()
#define __write_cr4(x) native_write_cr4(x)

/* Read and write CR4 with shadow register. */

void cr4_init_shadow(void);
void cr4_set_bits(unsigned long);
void cr4_clear_bits(unsigned long);
unsigned long read_cr4(void);

static __always_inline void native_wbinvd(void)
{
	asm volatile("wbinvd": : :"memory");
}

#define wbinvd() native_wbinvd()

/* 8.7 TASK MANAGEMENT IN 64-BIT MODE. */

struct x86_hw_tss {
	u32 reserved1;

	/* Stack pointers (RSP) for ring 0 .. 2. */
	u64 rsp0;
	u64 rsp1;
	u64 rsp2;

	u64 reserved2;

	/* Interrupt stack table (IST) pointers. */
	u64 ist[7];

	u32 reserved3;
	u32 reserved4;
	u16 reserved5;

	/* Offset to the I/O permission bit map from the 64-bit TSS base. */
	u16 io_bitmap_base;
} __packed;

#define IO_BITMAP_BITS  65536
#define IO_BITMAP_LONGS (IO_BITMAP_BITS / BITS_PER_LONG)

struct x86_io_bitmap {
	unsigned long bitmap[IO_BITMAP_LONGS + 1];
};

struct tss {
	struct x86_hw_tss tss;
	struct x86_io_bitmap io_bitmap;
};

#endif /* __X86_ASM_CPU_H__ */
