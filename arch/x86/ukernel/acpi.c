/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/acpi.h>
#include <cyanea/string.h>
#include <cyanea/errno.h>

static int acpi_disabled;

int __init acpi_boot_table_init(void)
{
    if (acpi_disabled)
        return SUCCESS;

    return SUCCESS;
}

static int __init acpi_setup(char *arg)
{
    if (!strcmp(arg, "off")) {
        acpi_disabled = 1;

        return SUCCESS;
    }

    return -EINVAL;
}

early_param("acpi", acpi_setup);
