/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_ACPI_H__
#define __CYANEA_ACPI_H__

#include <cyanea/init.h>
#include <cyanea/stddef.h>
#include <cyanea/types.h>

/* Advanced Configuration and Power Interface (ACPI). */
/* https://uefi.org/specs/ACPI/6.5/index.html. */

#define ACPI_SIG_SRAT "SRAT"    /* System Resource Affinity Table. */
#define ACPI_SIG_FADT "FACP"    /* Fixed ACPI Description Table. */
#define ACPI_SIG_MADT "APIC"    /* Multiple APIC Description Table. */
#define ACPI_SIG_HPET "HPET"    /* High Precision Event Timer table. */
#define ACPI_SIG_SPCR "SPCR"    /* Serial Port Console Redirection table. */

/* 5.2.6. System Description Table Header. */
/* https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html#system-description-table-header. */
/* Header is used by all ACPI tables except the RSDP and FACS. */

struct acpi_table_header {
	char signature[4];      /* ASCII signature. */
	u32 length;             /* Length of table in bytes, including this header. */
	u8 revision;            /* ACPI Specification minor version number. */
	u8 checksum;
	char oem_id[6];
	char oem_table_id[8];
	u32 oem_revision;
	char creator_id[4];
	u32 creator_revision;
};

/* ''Generic Subtable header'' (used in MADT, SRAT, etc.). */

struct acpi_subtable_header {
	u8 type;
	u8 length;
};

int __init acpi_get_table(char *signature, int instance,
        struct acpi_table_header **table);
void acpi_put_table(struct acpi_table_header *table);

int __init acpi_table_init(void);
int __init acpi_table_parse(char *signature,
        int (*handler)(struct acpi_table_header *));
int __init acpi_subtable_parse(struct acpi_table_header *table,
        size_t table_size, int type,
        int (*handler)(struct acpi_subtable_header *));

/* ACPI TABLES: */

/* 5.2.12. Multiple APIC Description Table (MADT). */
/* https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html#multiple-apic-description-table-madt. */

struct acpi_table_madt {
	struct acpi_table_header header;
	u32 address;
# define ACPI_MADT_PCAT_COMPAT 1
#  define ACPI_MADT_DUAL_PIC 1
#  define ACPI_MADT_MULTIPLE_APIC 0
	u32 flags;
};

enum acpi_madt_type {
	ACPI_MADT_TYPE_LOCAL_APIC  = 0,
	ACPI_MADT_TYPE_IO_APIC = 1,
	ACPI_MADT_TYPE_INTERRUPT_OVERRIDE = 2,
	ACPI_MADT_TYPE_NMI_SOURCE = 3,
	ACPI_MADT_TYPE_LOCAL_APIC_NMI = 4,
	ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE = 5,
	ACPI_MADT_TYPE_INTERRUPT_SOURCE = 8,
	ACPI_MADT_TYPE_LOCAL_X2APIC = 9,
	ACPI_MADT_TYPE_LOCAL_X2APIC_NMI = 10,
	ACPI_MADT_TYPE_MULTIPROC_WAKEUP = 16,
};

/* MADT Subtables: */

/* 'ACPI_MADT_TYPE_LOCAL_APIC'. */
struct acpi_madt_local_apic {
	struct acpi_subtable_header header;
	u8 uid;         /* ACPI Processor UID. */
	u8 id;          /* Processor's local APIC id. */
# define ACPI_MADT_ENABLED 1            /* Processor is enabled. */
# define ACPI_MADT_ONLINE_CAPABLE 2     /* System HW supports enabling processor at runtime. */
	u32 lapic_flags;
};

/* 'ACPI_MADT_TYPE_IO_APIC'. */
struct acpi_madt_io_apic {
	struct acpi_subtable_header header;
	u8 id;          /* I/O APIC ID. */
	u8 reserved;
	u32 address;
	u32 global_irq_base;
};

/* 'ACPI_MADT_TYPE_INTERRUPT_OVERRIDE'. */
struct acpi_madt_interrupt_override {
	struct acpi_subtable_header header;
	u8 bus;
	u8 source_irq;
	u32 global_irq;
	u16 inti_flags;
};

/* 'ACPI_MADT_TYPE_NMI_SOURCE'. */
struct acpi_madt_nmi_source {
	struct acpi_subtable_header header;
	u16 inti_flags;
	u32 global_irq;
};

/* 'ACPI_MADT_TYPE_LOCAL_APIC_NMI'. */
struct acpi_madt_local_apic_nmi {
	struct acpi_subtable_header header;
	u8 uid;         /* ACPI Processor UID. */
	u16 inti_flags;
	u8 lint;        /* LINTn to which NMI is connected. */
};

/* 'ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE'. */
struct acpi_madt_local_apic_override {
	struct acpi_subtable_header header;
	u16 reserved;
	u64 address;
};

/* 'ACPI_MADT_TYPE_LOCAL_X2APIC'. */
struct acpi_madt_local_x2apic {
	struct acpi_subtable_header header;
	u16 reserved;
	u32 id;         /* Processor x2APIC ID. */
	u32 lapic_flags;
	u32 uid;        /* ACPI processor UID. */
};

/* 'ACPI_MADT_TYPE_LOCAL_X2APIC_NMI'. */
struct acpi_madt_local_x2apic_nmi {
	struct acpi_subtable_header header;
	u16 inti_flags;
	u32 uid;        /* ACPI processor UID. */
	u8 lint;        /* LINTn to which NMI is connected. */
	u8 reserved[3];
};

#endif /* __CYANEA_ACPI_H__ */
