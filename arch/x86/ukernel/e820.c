/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm/e820.h>

static struct e820_table e820_table __initdata;

static void __init __e820__range_add(struct e820_table *table,
    struct e820_entry e)
{
    int i = table->nr_entries;

    if (i < E820_MAX_ENTRIES) {
        table->entries[i] = e;
        table->nr_entries++;
    }
}

void __init e820__range_add(u64 start, u64 size, enum e820_type type)
{
    __e820__range_add(&e820_table, (struct e820_entry) { start, size, type });
}

/* Copy the BIOS E820 map into a safe place. */

static void __init append_e820_table(struct __e820_entry *entries,
    u32 nr_entries)
{
    struct __e820_entry *entry = entries;

    if (nr_entries < 2) {

        /* In x86, we expect at least 2 memory range [0 .. 0x9F000] '640 KiB RAM
         * ("Low memory")', and some extra usable memory range. */

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
}
