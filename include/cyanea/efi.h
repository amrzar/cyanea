/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_EFI_H__
#define __CYANEA_EFI_H__

#include <cyanea/types.h>

/* Unified Extensible Firmware Interface (UEFI). */
/* https://uefi.org/specs/UEFI/2.10/index.html. */

#define EFI_PAGE_SIZE BIT_UL(12)

#define UUID_SIZE 16
typedef struct {
    u8 b[UUID_SIZE];
} efi_guid_t __aligned(__alignof__(u32));
#define EFI_GUID(a, b, c, d...) ((efi_guid_t){ {    \
            (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff,  \
            (b) & 0xff, ((b) >> 8) & 0xff,          \
            (c) & 0xff, ((c) >> 8) & 0xff, d } })

typedef u64 efi_physical_addr_t;

/* 7.2.3. EFI_BOOT_SERVICES.GetMemoryMap(). */
/* https://uefi.org/specs/UEFI/2.10/07_Services_Boot_Services.html#efi-boot-services-getmemorymap. */

struct efi_memory_desc {
# define EFI_RESERVED_TYPE 0
# define EFI_LOADER_CODE 1
# define EFI_LOADER_DATA 2
# define EFI_BOOT_SERVICES_CODE 3
# define EFI_BOOT_SERVICES_DATA 4
# define EFI_RUNTIME_SERVICES_CODE 5
# define EFI_RUNTIME_SERVICES_DATA 6
# define EFI_CONVENTIONAL_MEMORY 7
# define EFI_UNUSABLE_MEMORY 8
# define EFI_ACPI_RECLAIM_MEMORY 9
# define EFI_ACPI_MEMORY_NVS 10
# define EFI_PERSISTENT_MEMORY 14
    u32 type;
    u32 pad;
    efi_physical_addr_t phys_addr;
    u64 virt_addr;
    u64 num_pages;
# define EFI_MEMORY_UC  ((u64)0x0000000000000001ULL)    /* Uncached. */
# define EFI_MEMORY_WC  ((u64)0x0000000000000002ULL)    /* Write-Coalescing. */
# define EFI_MEMORY_WT  ((u64)0x0000000000000004ULL)    /* Write-Through. */
# define EFI_MEMORY_WB  ((u64)0x0000000000000008ULL)    /* Write-Back. */
    u64 attribute;
};

#endif /* __CYANEA_EFI_H__ */
