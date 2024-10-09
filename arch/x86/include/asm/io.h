/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_IO_H__
#define __X86_ASM_IO_H__

#include <cyanea/types.h>

static inline void slow_down_io(void)
{
    asm volatile("outb %al, $0x80");
}

#define BUILDIO(bwl, bw, type) \
static __always_inline void out ## bwl (type value, u16 port) \
{ \
    asm volatile("out" #bwl " %" #bw "0, %w1"   \
        : : "a"(value), "Nd"(port));            \
} \
  \
static __always_inline type in ## bwl (u16 port)  \
{ \
    type value;                                 \
    asm volatile("in" #bwl " %w1, %" #bw "0"    \
        : "=a"(value) : "Nd"(port));            \
    return value;                               \
}

BUILDIO(b, b, u8);              /* ''inb'', ''outb''. */
BUILDIO(w, w, u16);             /* ''inw'', ''outw''. */
BUILDIO(l,  , u32);             /* ''inl'', ''outl''. */
#  undef BUILDIO

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

BUILDIO(b, u8);                 /* ''inb_p'', ''outb_p''. */
BUILDIO(w, u16);                /* ''inw_p'', ''outw_p''. */
BUILDIO(l, u32);                /* ''inl_p'', ''outl_p''. */
#  undef BUILDIO

#endif /* __X86_ASM_IO_H__ */
