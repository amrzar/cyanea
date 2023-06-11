/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_E820_H__
#define __X86_ASM_E820_H__

#include <cyanea/types.h>

#include <uapi/asm/bootparam.h>

enum e820_type {
    E820_TYPE_RAM = 1,          /* Usable (normal) RAM. */
    E820_TYPE_RESERVED = 2,     /* Reserved. */
    E820_TYPE_ACPI = 3,         /* ACPI reclaimable memory. */
    E820_TYPE_NVS = 4,          /* ACPI Non-volatile Storage memory. */
    E820_TYPE_UNUSABLE = 5      /* Unusable memory. */
};

struct e820_entry {
    phys_addr_t start;
    size_t size;
    enum e820_type type;
};

#define E820_MAX_ENTRIES BIOS_E820_MAX_ENTRIES

struct e820_table {
    int nr_entries;

    struct e820_entry entries[E820_MAX_ENTRIES];
};

unsigned long __init e820_end_pfn(unsigned long, enum e820_type);
void __init e820__range_add(phys_addr_t, size_t, enum e820_type);
void __init e820__memory_setup(void);
void __init e820__memblock_setup(void);

#endif /* __X86_ASM_E820_H__ */
