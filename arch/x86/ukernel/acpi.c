/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/acpi.h>
#include <cyanea/errno.h>

#include <string.h>

void __init acpi_x86_table_init(void)
{
    if (acpi_disabled)
        return;

    if (acpi_table_init()) {
        acpi_disabled = 1;

    }
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
