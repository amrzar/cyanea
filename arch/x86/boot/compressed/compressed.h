/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_BOOT_COMPRESSED_COMPRESSED_H__
#define __X86_BOOT_COMPRESSED_COMPRESSED_H__

#include <cyanea/compiler.h>
#include <cyanea/elf.h>
#include <uapi/asm/bootparam.h>

extern struct boot_params *boot_params;

extern int early_param_parse(const char *, char *, int);

extern void early_console_init(void);
extern void putchar(int);
extern void puts(const char *);
extern __attribute__((format(printf, 1, 2)))
int printf(const char *, ...);

extern void __noreturn cpu_hlt(void);

#define error(...) do { \
        printf(__VA_ARGS__); \
        cpu_hlt(); \
    } while(0)

#define assert(x) do { \
        if (!(x)) { \
            printf("%s:%d: %s: Assertion `" #x "' failed.", \
                __FILE__, __LINE__, __func__); \
            cpu_hlt(); \
        } \
    } while(0)

extern void *malloc(size_t);
extern void *memdup(const void *, size_t);
extern void free(void *);

#endif /* __X86_BOOT_COMPRESSED_COMPRESSED_H__ */
