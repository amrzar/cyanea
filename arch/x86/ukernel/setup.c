/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/memblock.h>
#include <cyanea/sizes.h>
#include <cyanea/acpi.h>
#include <asm/setup.h>
#include <asm/e820.h>
#include <asm/ukernel.lds.h>
#include <asm/page.h>
#include <uapi/asm/bootparam.h>

struct boot_params boot_params;

void __init early_cpuinfo_init(void);
int __init reserve_real_mode(void);
void __init init_mem_mapping(void);
void __init acpi_x86_table_init(void);

# ifndef CONFIG_NUMA
static void __init init_numa(void)
{
    /* UMA. Set nid to 0. */
    memblock_set_node(0, -1, 0);
}
# else
void __init init_numa(void);
# endif /* CONFIG_NUMA */

static phys_addr_t __init get_ramdisk_image(void)
{
    phys_addr_t ramdisk_image = boot_params.hdr.ramdisk_image;

    ramdisk_image |= ((phys_addr_t) boot_params.ext_ramdisk_image << 32);

    return ramdisk_image;
}

static size_t __init get_ramdisk_size(void)
{
    size_t ramdisk_size = boot_params.hdr.ramdisk_size;

    ramdisk_size |= ((size_t) boot_params.ext_ramdisk_size << 32);

    return ramdisk_size;
}

static void __init reserve_initrd(void)
{
    phys_addr_t ramdisk_image = get_ramdisk_image();
    size_t ramdisk_size = get_ramdisk_size();

    if (!boot_params.hdr.type_of_loader ||
        !ramdisk_image || !ramdisk_size)
        return;

    /* Assume image is page aligned; round up the size to page. */

    ramdisk_size = ROUND_UP(ramdisk_size, PAGE_SIZE);

    ulog("RAMDISK: [%#018llx .. %#018llx]\n", ramdisk_image,
        ramdisk_image + ramdisk_size);

    memblock_reserve(ramdisk_image, ramdisk_size);
}

static void reserve_memory(void)
{
    /* Reserve the ukernel up to '__end_of_kernel_reserve'. */
    memblock_reserve(__pa_symbol(_text),
        (unsigned long)__end_of_kernel_reserve - (unsigned long)_text);

    reserve_initrd();
}

static void __arch_constructor setup_arch(void)
{
    ulog_info("[%s]\n", boot_command_line);

    early_cpuinfo_init();

    e820__memory_setup();
    e820__memblock_setup();

    reserve_memory();
    reserve_real_mode();

    init_mem_mapping();

    acpi_x86_table_init();

    init_numa();
}
