/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_E820_H__
#define __X86_ASM_E820_H__

#include <cyanea/init.h>
#include <cyanea/stddef.h>
#include <cyanea/types.h>

/**
 * @brief The types of memory regions in the system as returned by the
 * ''INT 15h, E820h'' BIOS interrupt.
 */
enum e820_type {
	E820_TYPE_RAM = 1,      /**< Usable memory available for the OS. */
	E820_TYPE_RESERVED = 2, /**< Memory reserved for the system or firmware. */
	E820_TYPE_ACPI = 3,     /**< ACPI reclaimable memory. */
	E820_TYPE_NVS = 4,      /**< ACPI non-volatile storage. */
	E820_TYPE_UNUSABLE = 5, /**< Memory that is defective or unavailable for use. */
	E820_TYPE_PMEM = 7      /**< Persistent memory. */
};

void __init e820__range_add(phys_addr_t start, size_t size,
        enum e820_type type);
void __init e820__update_table(void);
void __init e820__memblock_setup(void);
void __init e820__print_table(void);

#endif /* __X86_ASM_E820_H__ */
