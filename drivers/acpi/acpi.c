/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/acpi.h>
#include <cyanea/string.h>
#include <cyanea/errno.h>
#include <cyanea/minmax.h>
#include <asm/io.h>

#include <asm/setup.h>

#include "acpi.h"

# define acpi_compare_sig(a, sig) (!strncmp((a), (sig), 4))

# define acpi_unmap(a, b) memunmap((a), (b))
static void __init *acpi_mmap(phys_addr_t phys_addr, size_t size)
{
	void *virt_addr;

	virt_addr = memremap(phys_addr, size);
	if (!virt_addr) {
		ulog_err("ACPI: Could not map memory at 0x%8.8X for length %u.\n",
		        phys_addr, size);
	}

	return virt_addr;
}

static char __init acpi_calc_checksum(char *mem, size_t size)
{
	int i;
	char sum = 0;

	for (i = 0; i < size; i++)
		sum += mem[i];

	return sum;
}

static int __init acpi_validate(struct acpi_table_header *table)
{
	if (acpi_calc_checksum((char *)table, table->length))
		return -EINVAL;

	return SUCCESS;
}

static phys_addr_t __init acpi_find_root_pointer(void)
{
	/* 5.2.5.2. Finding the RSDP on UEFI Enabled Systems. */
	/* https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html#finding-the-rsdp-on-uefi-enabled-systems. */

	if (boot_params.acpi_rsdp_addr)
		return boot_params.acpi_rsdp_addr;

	ulog_err("ACPI: A valid RSDP was not found.\n");

	return 0;
}

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

/* Undo acpi__acquire_table(); suitable for __cleanup. */
static void __init acpi__release_table(struct acpi_table_header **cleanup_ptr)
{
	struct acpi_table_header *table = *cleanup_ptr;

	if (table != NULL)
		acpi_unmap(table, table->length);
}

static int __init acpi_add_table(phys_addr_t);
static int __init acpi_parse_root_table(phys_addr_t rsdp_paddr)
{
	int i;
	struct acpi_table_rsdp *rsdp;
	struct acpi_table_header *table_header __cleanup(acpi__release_table) = NULL;
	struct acpi_table_xsdt *xsdt;
	phys_addr_t paddr;

	rsdp = acpi_mmap(rsdp_paddr, sizeof(*rsdp));
	if (!rsdp)
		return -ENOMEM;

	if (rsdp->revision == 2) {
		paddr = rsdp->xsdt_address;
		acpi_unmap(rsdp, sizeof(*rsdp));
	} else {
		acpi_unmap(rsdp, sizeof(*rsdp));
		ulog_err("ACPI version 1.0 not supported.");

		return -EINVAL;
	}

	table_header = acpi__acquire_table(paddr);
	if (!table_header)
		return -ENOMEM;

	xsdt = (struct acpi_table_xsdt *)table_header;

	/* Parse XSDT. */
	for (i = 0; i < ((xsdt->header.length - sizeof(xsdt->header)) / sizeof(xsdt->entry[0])); i++) {
		paddr = xsdt->entry[i];
		if (!paddr)
			continue;

		acpi_add_table(paddr);
	}

	return SUCCESS;
}

/* Store entries for RSDT/XSDT in easier to access format. */

# define ACPI_MAX_INIT_TABLES 32
struct acpi_table_desc {
	phys_addr_t address;
	size_t length;
	char signature[4];
};

static int __initdata acpi_tables_count;
static struct acpi_table_desc __initdata acpi_tables[ACPI_MAX_INIT_TABLES];

/* Add a table to 'acpi_tables'. */
static int __init acpi_add_table(phys_addr_t paddr)
{
	struct acpi_table_header *table __cleanup(acpi__release_table);

	table = acpi__acquire_table(paddr);
	if (table) {
		if (acpi_tables_count == ACPI_MAX_INIT_TABLES) {
			ulog_err("acpi_tables has no space %d.", acpi_tables_count);

			return -ENOSPC;
		}

		acpi_tables[acpi_tables_count].address = paddr;
		acpi_tables[acpi_tables_count].length = table->length;
		memcpy(acpi_tables[acpi_tables_count].signature, table->signature, 4);

		acpi_tables_count++;
	}

	return SUCCESS;
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

		return SUCCESS;
	}

	return -EINVAL;
}

void __init acpi_put_table(struct acpi_table_header *table)
{
	acpi_unmap(table, table->length);
}

int __init acpi_table_init(void)
{
	phys_addr_t paddr;

	paddr = acpi_find_root_pointer();
	if (!paddr)
		return -ENODEV;

	return acpi_parse_root_table(paddr);
}

/**
 * acpi_table_parse() - find table with @signature, run @handler on it.
 * @signature: table signature to find.
 * @handler: handler to run.
 *
 * Scan the ACPI System Descriptor Table (STD) for a table matching @signature,
 * run @handler on it.
 *
 * Return SUCCESS if table found. Otherwise -ENODEV is returned.
 */
int __init acpi_table_parse(char *signature,
        int (*handler)(struct acpi_table_header *))
{
	struct acpi_table_header *table;

	if (acpi_get_table(signature, 1, &table))
		return -ENODEV;

	handler(table);

	acpi_put_table(table);

	return SUCCESS;
}

/**
 * acpi_subtable_parse() - find subtable with @type, run @handler on it.
 * @table: root table.
 * @table_size: size of the root table.
 * @type: type of subtable.
 * @handler: handler to run.
 *
 * @table_size is not the size of ACPI table -- the length field in the header,
 * but only the size of the root table, i.e. the offset from the beginning of
 * the ACPI table, to the beginning of the subtables.
 *
 * Return sum of all matching entries. Otherwise -EINVAL if @handler failed.
 */
int __init acpi_subtable_parse(struct acpi_table_header *table, size_t table_size,
        int type, int (*handler)(struct acpi_subtable_header *))
{
	int err, count = 0;
	struct acpi_subtable_header *subtable = (struct acpi_subtable_header *)
	        ((unsigned long)table + table_size);

	while ((unsigned long)(subtable + 1) < ((unsigned long)table + table->length)) {
		if (subtable->type == type) {
			err = handler(subtable);
			if (err)
				break;

			count++;
		}

		/* NEXT subtable. */
		subtable = (struct acpi_subtable_header *)
		        ((unsigned long)subtable + subtable->length);
	}

	return err ? -EINVAL : count;
}
