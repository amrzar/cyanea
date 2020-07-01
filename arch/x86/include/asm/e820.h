/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_E820_H__
#define __X86_ASM_E820_H__

#include <cyanea/types.h>
#include <cyanea/init.h>
#include <uapi/asm/bootparam.h>

enum e820_type {
    E820_TYPE_RAM = 1,          /* Usable (normal) RAM. */
    E820_TYPE_RESERVED = 2,     /* Reserved. */
    E820_TYPE_ACPI = 3,         /* ACPI reclaimable memory. */
    E820_TYPE_NVS = 4,          /* ACPI Non-volatile Storage memory. */
    E820_TYPE_UNUSABLE = 5      /* Unusable memory. */
};

struct e820_entry {
    u64 start;
    u64 size;
    enum e820_type type;
};

/* We assume legacy E820 BIOS memory as reported in 'boot_params.e820_table'. */
/* 'boot_params' is defined in 'x86/ukernel/setup.c'. */

#define E820_MAX_ENTRIES __E820_MAX_ENTRIES

struct e820_table {
    int nr_entries;

    struct e820_entry entries[E820_MAX_ENTRIES];
};

extern void __init e820__memory_setup(void);

#endif /* __X86_ASM_E820_H__ */
