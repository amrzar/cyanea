/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_E820_H__
#define __X86_ASM_E820_H__

#include <cyanea/init.h>
#include <cyanea/stddef.h>
#include <cyanea/types.h>

/* The ''INT 15h, E820h'' BIOS interrupt call returns a system memory map, showing the
 * various types of memory and reserved regions in a system.
 * Here are the different types of memory that are commonly reported:
 *
 *  - RAM (Usable Memory) is the memory available to the OS and  is used for
 *    running programs and storing data.
 *  - Reserved Memory.
 *  - ACPI Reclaimable Memory is initially reserved but can be reclaimed and
 *    used after the OS has processed the ACPI tables.
 *  - ACPI NVS (Non-Volatile Storage) Memory is reserved for ACPI non-volatile
 *    storage. The OS should not use this memory.
 *  - Unusable.
 *  - Persistent Memory (PMEM) refers to non-volatile memory that retains data
 *    even when the power is turned off.
 *
 **/

enum e820_type {
	E820_TYPE_RAM = 1,
	E820_TYPE_RESERVED = 2,
	E820_TYPE_ACPI = 3,
	E820_TYPE_NVS = 4,
	E820_TYPE_UNUSABLE = 5,
	E820_TYPE_PMEM = 7
};

void __init e820__range_add(phys_addr_t start, size_t size,
        enum e820_type type);
void __init e820__update_table(void);
void __init e820__memblock_setup(void);
void __init e820__print_table(void);

#endif /* __X86_ASM_E820_H__ */
