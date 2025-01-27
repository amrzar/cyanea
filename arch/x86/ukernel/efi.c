/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/efi.h>
#include <cyanea/errno.h>
#include <cyanea/init.h>
#include <cyanea/types.h>
#include <asm/e820.h>
#include <asm/io.h>
#include <uapi/asm/bootparam.h>

#define efi_memmap_unmap(a, b) memunmap((a), (b))
static void __init *efi_memmap_init(phys_addr_t phys_addr, size_t size)
{
	void *virt_addr;

	virt_addr = memremap(phys_addr, size);
	if (!virt_addr) {
		ulog_err("EFI: Could not map memory at 0x%8.8X for length %u.\n",
		        phys_addr, size);
	}

	return virt_addr;
}

/**
 * @brief Iterate over each EFI memory descriptor in the memory map.
 *
 * The descriptor size (`e->efi_memdesc_size`) represents the size in bytes of
 * an EFI_MEMORY_DESCRIPTOR element. The size is returned to allow for future
 * expansion of the EFI_MEMORY_DESCRIPTOR. Use the descriptor size to iterate
 * over the map.
 *
 * @param md Pointer to the current EFI memory descriptor.
 * @param descs Pointer to the start of the EFI memory descriptor array.
 * @param e Pointer to the EFI system table containing memory map information.
 *
 * @see https://uefi.org/specs/UEFI/2.10/07_Services_Boot_Services.html#efi-boot-services-getmemorymap
 */
#define for_each_efi_memory_desc_in_map(md, descs, e) \
	for ((md) = (descs); \
	        ((void *)(md) + (e)->efi_memdesc_size) <= ((void *)(descs) + (e)->efi_memmap_size); \
	        (md) = ((void *)(md) + (e)->efi_memdesc_size))

static void __init __used efi_memory_print_type(u32 type)
{
	switch (type) {
	case EFI_RESERVED_TYPE:
		ulog_info("EFI_RESERVED_TYPE");
		break;
	case EFI_LOADER_CODE:
		ulog_info("EFI_LOADER_CODE");
		break;
	case EFI_LOADER_DATA:
		ulog_info("EFI_LOADER_DATA");
		break;
	case EFI_BOOT_SERVICES_CODE:
		ulog_info("EFI_BOOT_SERVICES_CODE");
		break;
	case EFI_BOOT_SERVICES_DATA:
		ulog_info("EFI_BOOT_SERVICES_DATA");
		break;
	case EFI_RUNTIME_SERVICES_CODE:
		ulog_info("EFI_RUNTIME_SERVICES_CODE");
		break;
	case EFI_RUNTIME_SERVICES_DATA:
		ulog_info("EFI_RUNTIME_SERVICES_DATA");
		break;
	default:
		ulog_info("type %u", type);
	}
}

int __init efi_add_memmap(void)
{
	struct efi_memory_desc *descs, *md;
	struct efi_info *e = &boot_params.efi_info;

	descs = efi_memmap_init(e->efi_memmap, e->efi_memmap_size);
	if (!descs)
		return -ENOMEM;

	for_each_efi_memory_desc_in_map(md, descs, e) {
		int e820_type;

		switch (md->type) {
		case EFI_LOADER_CODE:
		case EFI_LOADER_DATA:
		case EFI_BOOT_SERVICES_CODE:
		case EFI_BOOT_SERVICES_DATA:
		case EFI_CONVENTIONAL_MEMORY:
			e820_type = E820_TYPE_RAM;
			break;
		case EFI_ACPI_RECLAIM_MEMORY:
			e820_type = E820_TYPE_ACPI;
			break;
		case EFI_ACPI_MEMORY_NVS:
			e820_type = E820_TYPE_NVS;
			break;
		case EFI_UNUSABLE_MEMORY:
			e820_type = E820_TYPE_UNUSABLE;
			break;
		case EFI_PERSISTENT_MEMORY:
			e820_type = E820_TYPE_PMEM;
			break;
		default:
			/* EFI_RESERVED_TYPE. */
			/* EFI_RUNTIME_SERVICES_CODE. */
			/* EFI_RUNTIME_SERVICES_DATA. */
			/* EFI_MEMORY_MAPPED_IO. */
			/* EFI_MEMORY_MAPPED_IO_PORT_SPACE. */
			/* EFI_PAL_CODE. */
			/* EFI_UNACCEPTED_MEMORY. */
			e820_type = E820_TYPE_RESERVED;
		}

		e820__range_add(md->phys_addr, md->num_pages * EFI_PAGE_SIZE,
		        e820_type);
	}

	efi_memmap_unmap(descs, e->efi_memmap_size);

	e820__update_table();
	e820__print_table();

	return SUCCESS;
}
