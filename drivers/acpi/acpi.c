/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/acpi.h>
#include <cyanea/errno.h>

# define ACPI_MAX_INIT_TABLES 128
static struct acpi_table_desc initial_tables[ACPI_MAX_INIT_TABLES];

static int __init acpi_locate_initial_tables(void)
{
    ACPI_STATUS status;

    /* Initialize the ACPICA table manager and get ACPI tables. */

    status = AcpiInitializeTables(initial_tables, ACPI_MAX_INIT_TABLES, FALSE);
    if (ACPI_FAILURE(status))
        return -EINVAL;

    return SUCCESS;
}

static void __init acpi_table_init_complete(void)
{
    /* TODO. Support ACPI table override. */
}

int __init acpi_table_init(void)
{
    int err;

    err = acpi_locate_initial_tables();
    if (err)
        return err;

    acpi_table_init_complete();

    return 0;
}
