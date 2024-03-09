/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/acpi.h>
#include <cyanea/errno.h>

/* ''Default'' memory affinity init. */
int __init __weak acpi_memory_affinity_init(struct acpi_srat_mem_affinity
    *memory_affinity)
{
    ulog_debug("Use default acpi_memory_affinity_init.");

    return SUCCESS;
}

/* ''Default'' cpu affinity init. */
int __init __weak acpi_cpu_affinity_init(struct acpi_srat_cpu_affinity
    *cpu_affinity)
{
    ulog_debug("Use default acpi_cpu_affinity_init.");

    return SUCCESS;
}

static int __init acpi_parse_cpu_affinity(struct acpi_subtable_header
    *subtable_header)
{
    struct acpi_srat_cpu_affinity *cpu_affinity;

    cpu_affinity = (struct acpi_srat_cpu_affinity *)(subtable_header);

    return acpi_cpu_affinity_init(cpu_affinity);
}

static int __init acpi_parse_memory_affinity(struct acpi_subtable_header
    *subtable_header)
{
    struct acpi_srat_mem_affinity *memory_affinity;

    memory_affinity = (struct acpi_srat_mem_affinity *)(subtable_header);

    return acpi_memory_affinity_init(memory_affinity);
}

static int __init acpi_parse_srat(struct acpi_table_header *table_header)
{
    acpi_subtable_parse(table_header, sizeof(struct acpi_table_srat),
        ACPI_SRAT_TYPE_CPU_AFFINITY, acpi_parse_cpu_affinity);

    acpi_subtable_parse(table_header, sizeof(struct acpi_table_srat),
        ACPI_SRAT_TYPE_MEMORY_AFFINITY, acpi_parse_memory_affinity);

    return SUCCESS;
}

int __init acpi_numa_init(void)
{
    if (acpi_disabled)
        return -EINVAL;

    acpi_table_parse(ACPI_SIG_SRAT, acpi_parse_srat);

    return SUCCESS;
}
