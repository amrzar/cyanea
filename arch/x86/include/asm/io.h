/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_IO_H__
#define __X86_ASM_IO_H__

#include <cyanea/init.h>
#include <cyanea/pgtable.h>
#include <cyanea/stddef.h>
#include <cyanea/types.h>

void __init *ioremap_prot(phys_addr_t phys_addr, size_t size, pgprot_t prot);

void __init *ioremap(phys_addr_t phys_addr, size_t size);
void __init iounmap(void *virt_addr, size_t size);

void __init *memremap(phys_addr_t phys_addr, size_t size);
void __init memunmap(void *virt_addr, size_t size);

#define build_mmio_read(name, bwl, bw, type, barrier) \
	static inline type name(const volatile void *addr) \
	{ \
		type value; \
		asm volatile("mov" #bwl " %1, %" #bw "0" \
		        : "=r" (value) : "m" (*(volatile type *)(addr)) barrier); \
		return value; \
	}

#define build_mmio_write(name, bwl, bw, type, barrier) \
	static inline void name(type value, volatile void *addr) \
	{ \
		asm volatile("mov" #bwl " %" #bw "0, %1" \
		        : : "r" (value), "m" (*(volatile type *)(addr)) barrier); \
	}

build_mmio_read(readb, b, b, u8,    : "memory")
build_mmio_read(readw, w, w, u16,   : "memory")
build_mmio_read(readl, l,, u32,     : "memory")
# define readb readb
# define readw readw
# define readl readl

build_mmio_write(writeb, b, b, u8,  : "memory")
build_mmio_write(writew, w, w, u16, : "memory")
build_mmio_write(writel, l,, u32,   : "memory")
# define writeb writeb
# define writew writew
# define writel writel

static inline void slow_down_io(void)
{
	asm volatile("outb %al, $0x80");
}

#define BUILDIO(bwl, bw, type) \
	static __always_inline void out ## bwl (type value, u16 port) \
	{ \
		asm volatile("out" #bwl " %" #bw "0, %w1" \
		        : : "a" (value), "Nd" (port)); \
	} \
	\
	static __always_inline type in ## bwl (u16 port) \
	{ \
		type value; \
		asm volatile("in" #bwl " %w1, %" #bw "0" \
		        : "=a" (value) : "Nd" (port)); \
		return value; \
	}

BUILDIO(b, b, u8)       /* ''inb'', ''outb''. */
BUILDIO(w, w, u16)      /* ''inw'', ''outw''. */
BUILDIO(l,, u32)        /* ''inl'', ''outl''. */
#  undef BUILDIO

# define inb inb
# define inw inw
# define inl inl

# define outb outb
# define outw outw
# define outl outl

#define BUILDIO(bwl, type) \
	static inline void out ## bwl ## _p (type value, u16 port) \
	{ \
		out ## bwl(value, port);                    \
		slow_down_io();                             \
	} \
	\
	static inline type in ## bwl ## _p (u16 port)   \
	{ \
		type value = in ## bwl(port);               \
		slow_down_io();                             \
		return value;                               \
	}

BUILDIO(b, u8)          /* ''inb_p'', ''outb_p''. */
BUILDIO(w, u16)         /* ''inw_p'', ''outw_p''. */
BUILDIO(l, u32)         /* ''inl_p'', ''outl_p''. */
#  undef BUILDIO

# define inb_p inb_p
# define inw_p inw_p
# define inl_p inl_p

# define outb_p outb_p
# define outw_p outw_p
# define outl_p outl_p

#endif /* __X86_ASM_IO_H__ */
