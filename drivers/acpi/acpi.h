/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ACPI_H__
#define __ACPI_H__

#include <cyanea/acpi.h>

/* RSDP - Root System Description Pointer. */

#define ACPI_SIG_RSDP "RSD PTR "
#define ACPI_RSDP_CHECKSUM_LENGTH 20
#define ACPI_RSDP_XCHECKSUM_LENGTH 36

struct acpi_table_rsdp {
    char signature[8];      /* ACPI signature. */
    u8 checksum;            /* ACPI 1.0 checksum. */
    char oem_id[6];
    u8 revision;            /* Must be (0) for ACPI 1.0 or (2) for ACPI 2.0+. */
    u32 rsdt_address;       /* 32-bit physical address of the RSDT. */
    u32 length;             /* Table length in bytes, including header (ACPI 2.0+). */
    u64 xsdt_address;       /* 64-bit physical address of the XSDT (ACPI 2.0+). */
    u8 extended_checksum;   /* ACPI 2.0+ checksum. */
    u8 reserved[3];
};

/* RSDT/XSDT - Root System Description Tables. */

struct acpi_table_rsdt {
    struct acpi_table_header header;
    u32 table_offset_entry[1];
};

struct acpi_table_xsdt {
    struct acpi_table_header header;
    u64 table_offset_entry[1];
};

#define ACPI_RSDT_ENTRY_SIZE (sizeof(u32))
#define ACPI_XSDT_ENTRY_SIZE (sizeof(u64))

#endif /* __ACPI_H__ */
