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

#include <asm/io.h>

extern struct setup_header hdr;
extern struct boot_params boot_params;

typedef unsigned int offset_t;

/* GS/FS Get and Set. */

static inline u16 ds(void)
{
    u16 seg;
    asm volatile("movw %%ds, %0" : "=rm" (seg));
    return seg;
}

static inline void set_fs(u16 seg)
{
    asm volatile("movw %0, %%fs" : : "rm" (seg));
}

static inline void set_gs(u16 seg)
{
    asm volatile("movw %0, %%gs" : : "rm" (seg));
}

/* GS/FS Read and Write helpers. */

static inline u8 read_fs_8(offset_t off)
{
    u8 value;
    asm volatile("movb %%fs:%1, %0" : "=q" (value) : "m" (*(u8 *)(off)));
    return value;
}

static inline u16 read_fs_16(offset_t off)
{
    u16 value;
    asm volatile("movw %%fs:%1, %0" : "=r" (value) : "m" (*(u16 *)(off)));
    return value;
}

static inline u32 read_fs(offset_t off)
{
    u32 value;
    asm volatile("movl %%fs:%1, %0" : "=r" (value) : "m" (*(u32 *)(off)));
    return value;
}

static inline u32 read_gs(offset_t off)
{
    u32 value;
    asm volatile("movl %%gs:%1, %0" : "=r" (value) : "m" (*(u32 *)(off)));
    return value;
}

static inline void write_fs(u32 value, offset_t off)
{
    asm volatile("movl %1, %%fs:%0" : "+m" (*(u32 *)(off)) : "ri" (value));
}

/* 'ADDR' converts a ''logical address'' to a ''linear address'' in 'DS'. */
#define ADDR(seg, off) (((unsigned long)(ds()) << 4) + (offset_t)(off))

int early_param_parse(const char *, char *, int);

void early_console_init(void);
void early_serial_putchar(int);
void putchar(int);
void puts(const char *);

void __noreturn cpu_hlt(void);
#define error(msg) do { \
        puts(msg); \
        cpu_hlt(); \
    } while(0)

void detect_memory(void);
int enable_a20(void);
void __noreturn go_to_protected_mode(void);
void __noreturn protected_mode_jump(u32, u32);

#endif /* __ASSEMBLY__ */

#endif /* __X86_BOOT_SETUP_SETUP_H__ */
