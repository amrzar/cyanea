/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/acpi.h>
#include <cyanea/errno.h>
#include <cyanea/init.h>
#include <cyanea/memblock.h>
#include <asm/e820.h>
#include <asm/page.h>
#include <asm/setup.h>
#include <asm/ukernel.lds.h>
#include <uapi/asm/bootparam.h>

struct boot_params boot_params;

void __init early_cpuinfo_init(void);
void __init init_mem_mapping(void);
void __init cache_bp_init(void);
int __init efi_memblock_x86_reserve_range(void);
void __init acpi_boot_table_init(void);
void __init check_x2apic(void);
void __init setup_nr_cpu_ids(void);
int __init reserve_real_mode(void);

#ifndef CONFIG_NUMA
static void __init initmem_init(void)
{
	/* UMA. Set nid to 0. */
	memblock_set_node(0, -1, 0);
}
#else
# error "NUMA has not implemented."
#endif /* CONFIG_NUMA */

#ifdef CONFIG_EFI_STUB
int __init efi_add_memmap(void);
# define e820__memory_setup efi_add_memmap
#endif

int __init acpi_boot_init(void);

static void __init early_reserve_initrd(void)
{
	phys_addr_t ramdisk_image = boot_params.ramdisk_image;
	size_t ramdisk_size = boot_params.ramdisk_size;

	if (!ramdisk_image || !ramdisk_size)
		return;

	ramdisk_size = ROUND_UP(ramdisk_size, PAGE_SIZE);

	ulog("RAMDISK: [%#018llx .. %#018llx]\n", ramdisk_image,
	        ramdisk_image + ramdisk_size);

	memblock_reserve(ramdisk_image, ramdisk_size);
}

static void early_reserve_memory(void)
{
	/*
	 * Reserve the memory occupied by the kernel between '_text' and
	 * '__end_of_kernel_reserve' symbols. Any kernel sections after the
	 * '__end_of_kernel_reserve' symbol must be explicitly reserved with a
	 * separate memblock_reserve().
	 */
	memblock_reserve(__pa_symbol(_text),
	        (unsigned long)__end_of_kernel_reserve - (unsigned long)_text);

	early_reserve_initrd();
}

void __init setup_arch(void)
{
	ulog_info("[%s]\n", boot_command_line);

	early_cpuinfo_init();

	check_x2apic();
	e820__memory_setup();
	e820__memblock_setup();
	early_reserve_memory();
	efi_memblock_x86_reserve_range();
	cache_bp_init();
	reserve_real_mode();
	init_mem_mapping();
	acpi_boot_table_init();
	initmem_init();
	acpi_boot_init();
	setup_nr_cpu_ids();
}
