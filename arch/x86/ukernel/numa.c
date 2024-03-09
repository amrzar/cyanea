/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/acpi.h>
#include <cyanea/errno.h>

static int __init x86_acpi_numa_init(void)
{
    int err;

    err = acpi_numa_init();
    if (err)
        return err;

    return SUCCESS;
}

static int __init numa_init(void)
{
    int err;

    err = x86_acpi_numa_init();
    if (err)
        return err;

    return SUCCESS;
}

void __init x86_numa_init(void)
{
    numa_init();
}
