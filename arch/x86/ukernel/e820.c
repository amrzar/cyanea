/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/memblock.h>
#include <cyanea/init.h>

#include <asm/page.h>
#include <asm/e820.h>

struct e820_entry {
    phys_addr_t start;
    size_t size;
    enum e820_type type;
};

struct e820_table {
    int nr_entries;
# define E820_MAX_ENTRIES 128
    struct e820_entry entries[E820_MAX_ENTRIES];
};

/* Keep it in __initdata; We assume we do not need this information after boot. */
static struct e820_table e820_table __initdata;

static void __init e820_print_type(enum e820_type type)
{
    switch (type) {
    case E820_TYPE_RAM:
        ulog_debug("usable (normal).");
        break;
    case E820_TYPE_RESERVED:
        ulog_debug("reserved.");
        break;
    case E820_TYPE_ACPI:
        ulog_debug("ACPI (reclaimable).");
        break;
    case E820_TYPE_NVS:
        ulog_debug("ACPI NVS.");
        break;
    case E820_TYPE_UNUSABLE:
        ulog_debug("unusable.");
        break;
    default:
        ulog_debug("type %u", type);
    }
}

void __init e820__print_table(void)
{
    int e;

    for (e = 0; e < e820_table.nr_entries; e++) {
        ulog_debug("[mem %#018llx .. %#018llx] [%#10lu] ", e820_table.entries[e].start,
            e820_table.entries[e].start + e820_table.entries[e].size - 1,
            e820_table.entries[e].size >> PAGE_SHIFT);
        e820_print_type(e820_table.entries[e].type);
        ulog_debug("\n");
    }
}

static void __init __e820__range_add(struct e820_table *table, phys_addr_t start, size_t size,
    enum e820_type type)
{
    if (table->nr_entries == E820_MAX_ENTRIES) {
        ulog_err("ignoring [mem %#010llx .. %#010llx]", start, start + size - 1);

    } else {

        /* INSERT entry. */
        table->entries[table->nr_entries].start = start;
        table->entries[table->nr_entries].size = size;
        table->entries[table->nr_entries].type = type;
        table->nr_entries++;
    }
}

static void __init __e820__update_table(struct e820_table *table)
{
    /* TODO. Some E820 memory maps include overlapping entries; check that?! */

    int cur_idx, idx = 0;

    if (!table->nr_entries)
        return;

    for (cur_idx = 1; cur_idx < table->nr_entries; cur_idx++) {
        /* MERGE entries. */
        if (table->entries[idx].type == table->entries[cur_idx].type &&
            table->entries[idx].start + table->entries[idx].size == table->entries[cur_idx].start)
            table->entries[idx].size += table->entries[cur_idx].size;
        else
            table->entries[++idx] = table->entries[cur_idx];
    }

    table->nr_entries = idx + 1;
}

void __init e820__range_add(phys_addr_t start, size_t size, enum e820_type type)
{
    __e820__range_add(&e820_table, start, size, type);
}

void __init e820__update_table(void)
{
    __e820__update_table(&e820_table);
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
