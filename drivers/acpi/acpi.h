/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ACPI_H__
#define __ACPI_H__

#include <cyanea/acpi.h>

/* 5.2.5. Root System Description Pointer (RSDP). */
/* https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html#root-system-description-pointer-rsdp. */

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

/* 5.2.7. Root System Description Table (RSDT). */
/* https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html#root-system-description-table-rsdt. */

struct acpi_table_rsdt {
	struct acpi_table_header header;
	u32 entry[];
} __packed;

/* 5.2.8. Extended System Description Table (XSDT). */
/* https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html#extended-system-description-table-xsdt. */

struct acpi_table_xsdt {
	struct acpi_table_header header;
	u64 entry[];
} __packed;

#endif /* __ACPI_H__ */
