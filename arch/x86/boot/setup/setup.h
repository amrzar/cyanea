/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_BOOT_SETUP_SETUP_H__
#define __X86_BOOT_SETUP_SETUP_H__

#include <asm/setup.h>
#include <uapi/asm/bootparam.h>

#define SETUP_STACK_SIZE 1024   /* Size of real-mode stack. */

#define NORMAL_VGA 0xFFFF       /* 80x25 mode */
#define EXTENDED_VGA 0xFFFE     /* 80x50 mode */
#define ASK_VGA 0xFFFD          /* Ask for video mode at boot time. */

#ifndef __ASSEMBLY__

#include <cyanea/types.h>
#include <cyanea/compiler.h>
#include <asm/io.h>

extern struct setup_header hdr;
extern struct boot_params boot_params;

typedef unsigned int offset_t;

#define get_segment(seg) ({ \
        u16 value; \
        asm volatile ("movw %%" #seg ", %0":"=rm" (value)); \
        value; \
    })

#define set_segmant(seg, v) \
    asm volatile ("movw %0, %%" #seg ::"rm" ((u16) (v)))

#define set_fs(v) set_segmant(fs, (v))
#define set_gs(v) set_segmant(gs, (v))

#define ADDR(seg, off) \
    ((unsigned long)(get_segment(seg)) << 4) + (offset_t)(off)

static inline u8 rdfs8(offset_t off)
{
    u8 value;
    asm volatile ("movb %%fs:%1, %0":"=q" (value)
        :"m"(*(u8 *) (off))
        );
    return value;
}

static inline u16 rdfs16(offset_t off)
{
    u16 value;
    asm volatile ("movw %%fs:%1, %0":"=r" (value)
        :"m"(*(u16 *) (off))
        );
    return value;
}

static inline u32 rdfs32(offset_t off)
{
    u32 value;
    asm volatile ("movl %%fs:%1, %0":"=r" (value)
        :"m"(*(u32 *) (off))
        );
    return value;
}

static inline u8 rdgs8(offset_t off)
{
    u8 value;
    asm volatile ("movb %%gs:%1, %0":"=q" (value)
        :"m"(*(u8 *) (off))
        );
    return value;
}

static inline u16 rdgs16(offset_t off)
{
    u16 value;
    asm volatile ("movw %%gs:%1, %0":"=r" (value)
        :"m"(*(u16 *) (off))
        );
    return value;
}

static inline u32 rdgs32(offset_t off)
{
    u32 value;
    asm volatile ("movl %%gs:%1, %0":"=r" (value)
        :"m"(*(u32 *) (off))
        );
    return value;
}

static inline void wrfs8(u8 value, offset_t off)
{
    asm volatile ("movb %1, %%fs:%0":"+m" (*(u8 *) (off))
        :"qi"(value)
        );
}

static inline void wrfs16(u16 value, offset_t off)
{
    asm volatile ("movw %1, %%fs:%0":"+m" (*(u16 *) (off))
        :"ri"(value)
        );
}

static inline void wrfs32(u32 value, offset_t off)
{
    asm volatile ("movl %1, %%fs:%0":"+m" (*(u32 *) (off))
        :"ri"(value)
        );
}

extern int early_param_parse(const char *, char *, int);

extern void early_console_init(void);
extern void early_serial_putchar(int);
extern void putchar(int);
extern void puts(const char *);

extern void __noreturn cpu_hlt(void);
#define error(msg) do { \
        puts(msg); \
        cpu_hlt(); \
    } while(0)

extern void detect_memory(void);
extern int enable_a20(void);
extern void __noreturn go_to_protected_mode(void);
extern void __noreturn protected_mode_jump(u32, u32);

#endif /* __ASSEMBLY__ */

#endif /* __X86_BOOT_SETUP_SETUP_H__ */
