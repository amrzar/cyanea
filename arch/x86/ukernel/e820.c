/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/memblock.h>
#include <cyanea/init.h>

#include <asm/page.h>
#include <asm/e820.h>

/* Keep it in __initdata; We assume we do not need this information after boot. */
static struct e820_table e820_table __initdata;

static void __init e820_print_type(enum e820_type type)
{
    switch (type) {
    case E820_TYPE_RAM:
        ulog_info("usable (normal).");

        break;
    case E820_TYPE_RESERVED:
        ulog_info("reserved.");

        break;
    case E820_TYPE_ACPI:
        ulog_info("ACPI (reclaimable).");

        break;
    case E820_TYPE_NVS:
        ulog_info("ACPI NVS.");

        break;
    case E820_TYPE_UNUSABLE:
        ulog_info("unusable.");

        break;
    default:
        ulog_info("type %u", type);
    }
}

static void __init e820__print_table(void)
{
    int e;

    for (e = 0; e < e820_table.nr_entries; e++) {
        ulog_info("[mem %#018llx .. %#018llx] ", e820_table.entries[e].start,
            e820_table.entries[e].start + e820_table.entries[e].size - 1);
        e820_print_type(e820_table.entries[e].type);
        ulog_info("\n");
    }

    ulog_info("\n");
}

static void __init __e820__range_add(struct e820_table *table,
    phys_addr_t start, size_t size, enum e820_type type)
{
    int e = table->nr_entries;

    if (e < E820_MAX_ENTRIES) {
        table->entries[e].start = start;
        table->entries[e].size = size;
        table->entries[e].type = type;

        table->nr_entries++;
    } else {
        ulog_err("not enough space.\n");
    }
}

void __init e820__range_add(phys_addr_t start, size_t size, enum e820_type type)
{
    __e820__range_add(&e820_table, start, size, type);
}

static void __init append_e820_table(struct bios_e820_entry *entries,
    u8 nr_entries)
{
    struct bios_e820_entry *entry = entries;

    if (nr_entries < 2) {

        /* In x86, we expect at least 2 memory range [0 .. 0x9F000] '640 KiB RAM
         * ("Low memory")', and some extra usable memory range.
         */

        /* TODO. Simulate e820 table using other technique. */

    }

    while (nr_entries) {
        e820__range_add(entry->addr, entry->size, entry->type);

        entry++;
        nr_entries--;
    }
}

void __init e820__memory_setup(void)
{
    append_e820_table(boot_params.e820_table, boot_params.e820_entries);

    /* TODO. Sanitize entries in 'e820_table'. */

    ulog_info("BIOS-provided physical RAM map:\n");
    e820__print_table();
}

void __init e820__memblock_setup(void)
{
    int i;
    struct e820_entry *entry;

    for (i = 0; i < e820_table.nr_entries; i++) {
        entry = &e820_table.entries[i];

        if (entry->type == E820_TYPE_RAM)
            memblock_add(entry->start, entry->size, -1, 0);
    }
}
