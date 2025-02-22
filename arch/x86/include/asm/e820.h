/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_E820_H__
#define __X86_ASM_E820_H__

#include <cyanea/init.h>
#include <cyanea/stddef.h>
#include <cyanea/types.h>

/**
 * @file e820.h
 * @brief System memory map definitions using INT 15h, E820h.
 *
 * The "INT 15h, E820h" BIOS interrupt call returns a system memory map,
 * showing the various types of memory and reserved regions in a system:
 *
 *  - **RAM (Usable Memory)** is available to the OS.
 *  - **Reserved Memory** is reserved for system or firmware use.
 *  - **ACPI Reclaimable Memory** is reserved but can be reclaimed and used after
 *    the OS processes the ACPI tables.
 *  - **ACPI NVS (Non-Volatile Storage)** is reserved for ACPI non-volatile storage.
 *    The OS should not use this memory.
 *  - **Unusable Memory** is defective or unavailable.
 *  - **Persistent Memory (PMEM)** is non-volatile memory that retains data even
 *    when the power is turned off.
 */

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
