/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_BOOT_COMPRESSED_COMPRESSED_H__
#define __X86_BOOT_COMPRESSED_COMPRESSED_H__

#include <cyanea/compiler.h>
#include <uapi/asm/bootparam.h>

extern struct boot_params *boot_params;

int early_param_parse(const char *, char *, int);

void early_console_init(void);
void putchar(int);
void puts(const char *);
__attribute__((format(printf, 1, 2))) int printf(const char *, ...);

void __noreturn cpu_hlt(void);

#define error(...) do { \
        printf("%s: ", __func__); \
        printf(__VA_ARGS__); \
        cpu_hlt(); \
    } while(0)

#endif /* __X86_BOOT_COMPRESSED_COMPRESSED_H__ */
