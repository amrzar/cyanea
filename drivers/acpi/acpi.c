/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/acpi.h>
#include <cyanea/minmax.h>
#include <cyanea/string.h>
#include <cyanea/errno.h>

#include <asm-generic/ioremap.h>

#include "acpi.h"

int acpi_disabled;

# define ACPI_DIFF_PTR(a, b) ((unsigned long)(a) - (unsigned long)(b))
# define ACPI_ADD_PTR(a, b) ((void *)((unsigned long)(a) + (b)))

# define acpi_compare_sig(a, sig) (!strncmp((a), (sig), 4))

# define acpi_unmap(a, b) iounmap((a), (b))
static void __init *acpi_mmap(phys_addr_t phys_addr, size_t size)
{
    void *virt_addr;

    virt_addr = ioremap(phys_addr, size, PAGE_KERNEL);
    if (!virt_addr) {
        ulog_err("ACPI: Could not map memory at 0x%8.8X for length %u.\n",
            phys_addr, size);
    }

    return virt_addr;
}

/* acpi_calc_checksum
 * acpi_validate_rsdp
 * acpi_validate
 */

static char __init acpi_calc_checksum(char *mem, size_t size)
{
    int i;
    char sum = 0;

    for (i = 0; i < size; i++)
        sum += mem[i];

    return sum;
}

static int __init acpi_validate_rsdp(struct acpi_table_rsdp *rsdp)
{
    if (strncmp(rsdp->signature, ACPI_SIG_RSDP, 8))
        return -EINVAL;

    if (acpi_calc_checksum((char *)rsdp, ACPI_RSDP_CHECKSUM_LENGTH))
        return -EINVAL;

    if (rsdp->revision >= 2 &&
        acpi_calc_checksum((char *)rsdp, ACPI_RSDP_XCHECKSUM_LENGTH))
        return -EINVAL;

    return SUCCESS;
}

static int __init acpi_validate(struct acpi_table_header *table)
{
    if (acpi_calc_checksum((char *)table, table->length))
        return -EINVAL;

    return SUCCESS;
}

# define ACPI_HI_RSDP_WINDOW_BASE ((phys_addr_t)(0x000E0000))
# define ACPI_HI_RSDP_WINDOW_SIZE 0x00020000

# define ACPI_RSDP_SCAN_STEP 16
static void __init *acpi_scan_mem_for_rsdp(void *start_addr, size_t size)
{
    void *addr, *end_addr = start_addr + size;

    for (addr = start_addr; addr < end_addr; addr += ACPI_RSDP_SCAN_STEP) {
        if (!acpi_validate_rsdp(addr))
            return addr;
    }

    return NULL;
}

static phys_addr_t __init acpi_find_root_pointer(void)
{
    void *addr, *rsdp;

    /* (1) TODO. Use Extended BIOS Data Area (EBDA). */
    /* (2) Search upper memory: 16-byte boundaries in [0xE0000h .. 0xFFFFFh]. */

    addr = acpi_mmap(ACPI_HI_RSDP_WINDOW_BASE, ACPI_HI_RSDP_WINDOW_SIZE);
    if (!addr)
        return 0;

    rsdp = acpi_scan_mem_for_rsdp(addr, ACPI_HI_RSDP_WINDOW_SIZE);
    acpi_unmap(addr, ACPI_HI_RSDP_WINDOW_SIZE);

    if (rsdp)
        return (phys_addr_t)(ACPI_HI_RSDP_WINDOW_BASE + ACPI_DIFF_PTR(rsdp, addr));

    ulog_err("ACPI: A valid RSDP was not found.\n");

    return 0;
}

/* acpi__acquire_table
 * acpi__release_table
 *
 */

static struct acpi_table_header __init *acpi__acquire_table(phys_addr_t paddr)
{
    struct acpi_table_header *table;
    size_t table_length;

    table = acpi_mmap(paddr, sizeof(*table));
    if (!table)
        return NULL;

    table_length = table->length;

    acpi_unmap(table, sizeof(*table));

    table = acpi_mmap(paddr, table_length);
    if (!table)
        return NULL;

    if (acpi_validate(table)) {
        acpi_unmap(table, table_length);

        return NULL;
    }

    return table;
}

static void __init acpi__release_table(struct acpi_table_header *table)
{
    acpi_unmap(table, table->length);
}

static void __init acpi_add_table(phys_addr_t);
static int __init acpi_parse_root_table(phys_addr_t rsdp_paddr)
{
    int i;
    struct acpi_table_rsdp *rsdp;

    phys_addr_t paddr;
    size_t entry_size;

    /* RSDT/XSDT. */
    struct acpi_table_header *table;
    void *table_entry;
    int table_count;

    rsdp = acpi_mmap(rsdp_paddr, sizeof(*rsdp));
    if (!rsdp)
        return -ENOMEM;

    if (rsdp->revision > 1 && rsdp->xsdt_address) {
        paddr = rsdp->xsdt_address;
        entry_size = ACPI_XSDT_ENTRY_SIZE;
    } else {
        paddr = rsdp->rsdt_address;
        entry_size = ACPI_RSDT_ENTRY_SIZE;
    }

    acpi_unmap(rsdp, sizeof(*rsdp));

    /* Get RSDT/XSDT. */
    table = acpi__acquire_table(paddr);
    if (!table)
        return -ENOMEM;

    /* Parse RSDT/XSDT. */
    /* ''table_offset_entry[]''. */
    table_count = ((table->length - sizeof(*table)) / entry_size);
    table_entry = ACPI_ADD_PTR(table, sizeof(*table));

    for (i = 0; i < table_count; i++) {

        if (entry_size == ACPI_XSDT_ENTRY_SIZE)
            paddr = *(u64 *)(table_entry);

        else /* 'ACPI_RSDT_ENTRY_SIZE'. */
            paddr = *(u32 *)(table_entry);

        /* NULL entries in RSDT/XSDT?! Skip. */
        if (!paddr)
            goto next_entry;

        acpi_add_table(paddr);

next_entry:

        table_entry += entry_size;
    }

    acpi__release_table(table);

    return SUCCESS;
}

/* Store entries for RSDT/XSDT in easier to access format. */

# define ACPI_MAX_INIT_TABLES 128
struct acpi_table_desc {
    phys_addr_t address;
    size_t length;
    char signature[4];
};

static int __initdata acpi_tables_count;
static struct acpi_table_desc __initdata acpi_tables[ACPI_MAX_INIT_TABLES];

static void __init acpi_add_table(phys_addr_t paddr)
{
    struct acpi_table_header *table;

    table = acpi__acquire_table(paddr);
    if (table) {
        acpi_tables[acpi_tables_count].address = paddr;
        acpi_tables[acpi_tables_count].length = table->length;

        memcpy(acpi_tables[acpi_tables_count].signature, table->signature, 4);

        acpi__release_table(table);
        acpi_tables_count++;
    }
}

int __init acpi_get_table(char *signature, int instance,
    struct acpi_table_header **table)
{
    int i, n = 0;

    if (!signature || !table)
        return -EINVAL;

    for (i = 0; i < acpi_tables_count; i++) {
        if (!acpi_compare_sig(acpi_tables[i].signature, signature))
            continue;

        if (++n < instance)
            continue;

        *table = acpi_mmap(acpi_tables[i].address, acpi_tables[i].length);
        if (!*table)
            return -ENOMEM;
    }

    return SUCCESS;
}

void __init acpi_put_table(struct acpi_table_header *table)
{
    acpi_unmap(table, table->length);
}

/* acpi_initialise_tables
 * acpi_table_parse
 * acpi_subtable_parse
 *
 */

int __init acpi_table_init(void)
{
    phys_addr_t paddr;

    paddr = acpi_find_root_pointer();
    if (!paddr)
        return -ENODEV;

    return acpi_parse_root_table(paddr);
}

int __init acpi_table_parse(char *signature,
    int (*handler)(struct acpi_table_header *))
{
    struct acpi_table_header *table;

    if (acpi_get_table(signature, 1, &table))
        return -ENODEV;

    else {
        handler(table);
        acpi_put_table(table);
    }

    return SUCCESS;
}

int __init acpi_subtable_parse(struct acpi_table_header *table,
    size_t table_size, int type, int (*handler)(struct acpi_subtable_header *))
{
    int err, count = 0;
    void *table_end = ACPI_ADD_PTR(table, table->length);

    struct acpi_subtable_header *sub;

    sub = ACPI_ADD_PTR(table, table_size);
    while (ACPI_ADD_PTR(sub, sizeof(*sub)) < table_end) {
        if (sub->type == type) {
            err = handler(sub);
            if (err)
                return err;

            count++;
        }

        sub = ACPI_ADD_PTR(sub, sub->length);
    }

    return count;
}
