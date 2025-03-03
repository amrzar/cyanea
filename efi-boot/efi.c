/* SPDX-License-Identifier: GPL-2.0-or-later */

/* [This code is position independent]. */

#include <cyanea/const.h>
#include <uapi/cyanea/screen_info.h>

#include "efi.h"

#ifdef CONFIG_X86
# define REL_REF(x) (*(typeof(&(x)))(rip_rel_ptr(&(x))))
static __always_inline __pure void *rip_rel_ptr(void *p)
{
	asm("leaq %c1(%%rip), %0" : "=r"(p) : "i"(p));

	return p;
}
#endif /* CONFIG_X86 */

static struct efi_system_table *____efi_systab;
#define __efi_systab REL_REF(____efi_systab)

extern char _binary_UKERNEL_gz_start[], _binary_UKERNEL_gz_end[];
#define __UKERNEL_gz_start REL_REF(_binary_UKERNEL_gz_start)
#define __UKERNEL_gz_end   REL_REF(_binary_UKERNEL_gz_end)
#define __UKERNEL_gz_size  (_binary_UKERNEL_gz_end - _binary_UKERNEL_gz_start)

#define ASCII_TO_USC2(x) ((efi_char16_t)(x))
#define USC2_TO_ASCII(x) ((char)(x))

#define EFI_PUTS_LEN 128
#define efi_puts(s) __efi_puts(REL_REF(s))
static efi_char16_t __usc2[EFI_PUTS_LEN];
static void __efi_puts(const char *s)
{
	int i, n;
	efi_char16_t *usc2 = REL_REF(__usc2);

	for (i = 0, n = 0; s[i] != '\0' && n < EFI_PUTS_LEN - 3; i++, n++) {
		usc2[n] = ASCII_TO_USC2(s[i]);
		if (s[i] == '\n')
			usc2[++n] = ASCII_TO_USC2('\r');
	}

	usc2[n] = ASCII_TO_USC2('\0');

	__efi_systab->con_out->output_string(__efi_systab->con_out, usc2);
}

static u64 efi_find_config_table(efi_guid_t guid)
{
	int i;
	struct efi_config_table *ct;

	for (i = 0; i < __efi_systab->nr_config_tables; i++) {
		ct = &__efi_systab->config_tables[i];

		if (!__builtin_memcmp(&ct->guid, &guid, UUID_SIZE))
			return ct->table;
	}

	return 0;
}

/* Support 'ACPI_20_TABLE_GUID'. */
static efi_physical_addr_t efi_get_rsdp_addr(void)
{
	efi_physical_addr_t rsdp_addr;

	rsdp_addr = efi_find_config_table(ACPI_20_TABLE_GUID);
	if (rsdp_addr) {
		efi_puts("'ACPI_20_TABLE_GUID' found.\n");

		return rsdp_addr;
	}

	return 0;
}

#include "deflate.c"

#define efi_bs_call(func, ...) __efi_systab->boottime->func(__VA_ARGS__)

static void __noreturn efi_exit(efi_handle_t handle, efi_status_t status)
{
	efi_bs_call(exit, handle, status, 0, NULL);
}

/* 7.2 Memory Allocation Services. */
/* https://uefi.org/specs/UEFI/2.10/07_Services_Boot_Services.html#memory-allocation-services. */

#ifndef EFI_ALLOC_ALIGN
# define EFI_ALLOC_ALIGN EFI_PAGE_SIZE
#endif

struct efi_boot_memmap {
	u64 map_size;
	u64 desc_size;
	u32 desc_ver;
	u64 map_key;
# define EFI_MMAP_NR_SLACK_SLOTS 8
	struct efi_memory_desc descs[];
};

static efi_status_t efi_allocate_pages(size_t size,
        efi_physical_addr_t *alloc_addr,
        unsigned long align, int memory_type)
{
	efi_status_t status;
	efi_physical_addr_t addr;
	int e, extra;

	if (align < EFI_ALLOC_ALIGN)
		align = EFI_ALLOC_ALIGN;
	extra = align / EFI_PAGE_SIZE - 1;

	size = ROUND_UP(size, EFI_ALLOC_ALIGN);

	/* 7.2.1. EFI_BOOT_SERVICES.AllocatePages. */
	status = efi_bs_call(allocate_pages, EFI_ALLOCATE_ANY_PAGES, memory_type,
	                size / EFI_PAGE_SIZE + extra, &addr);
	if (status != EFI_SUCCESS)
		return status;

	*alloc_addr = PTR_ALIGN(addr, align);

	if (extra) {
		e = (addr & (align - 1)) / EFI_PAGE_SIZE;
		if (e) {
			efi_bs_call(free_pages, addr, extra - e + 1);
			extra = e - 1;
		}

		if (extra)
			efi_bs_call(free_pages, *alloc_addr + size, extra);
	}

	return EFI_SUCCESS;
}

static efi_status_t efi_allocate(int memory_type, size_t size,
        void **alloc_addr)
{
	/* 7.2.4. EFI_BOOT_SERVICES.AllocatePool. */
	return efi_bs_call(allocate_pool, memory_type, size, alloc_addr);
}

static efi_status_t efi_free(void *addr)
{
	/* 7.2.5. EFI_BOOT_SERVICES.FreePool. */
	return efi_bs_call(free_pool, addr);
}

static efi_status_t efi_get_memory_map(struct efi_boot_memmap *memmap,
        struct efi_memory_desc *memmap_desc)
{
	/* 7.2.3. EFI_BOOT_SERVICES.GetMemoryMap. */
	return efi_bs_call(get_memory_map, &memmap->map_size, memmap_desc,
	                &memmap->map_key, &memmap->desc_size, &memmap->desc_ver);
}

/* 7.4 Image Services. */
/* https://uefi.org/specs/UEFI/2.10/07_Services_Boot_Services.html#image-services. */

static efi_status_t efi_exit_boot_services(efi_handle_t handle,
        struct efi_boot_memmap **memmap)
{
	efi_status_t status;

	struct efi_boot_memmap *m, tmp;
	size_t size;

	tmp.map_size = 0;
	status = efi_get_memory_map(&tmp, NULL);
	if (status != EFI_BUFFER_TOO_SMALL)
		return EFI_LOAD_ERROR;

	size = tmp.map_size + tmp.desc_size * EFI_MMAP_NR_SLACK_SLOTS;

	status = efi_allocate(EFI_LOADER_DATA, sizeof(*m) + size, (void **)&m);
	if (status != EFI_SUCCESS)
		return status;

	m->map_size = size;
	status = efi_get_memory_map(m, m->descs);
	if (status != EFI_SUCCESS) {
		efi_free(m);

		return status;
	}

	/* 7.4.6. EFI_BOOT_SERVICES.ExitBootServices. */
	status = efi_bs_call(exit_boot_services, handle, m->map_key);
	if (status == EFI_INVALID_PARAMETER) {

		m->map_size = size;
		status = efi_get_memory_map(m, m->descs);
		if (status != EFI_SUCCESS)
			/* 'exit_boot_services' already called, cannot call 'efi_free'. */
			return status;

		status = efi_bs_call(exit_boot_services, handle, m->map_key);
		if (status != EFI_SUCCESS)
			/* 'exit_boot_services' already called, cannot call 'efi_free'. */
			return status;
	}

	if (memmap)
		*memmap = m;

	return EFI_SUCCESS;
}

/* 12.9 Graphics Output Protocol. */
/* https://uefi.org/specs/UEFI/2.10/12_Protocols_Console_Support.html#graphics-output-protocol. */

static struct efi_graphics_output_protocol *efi_find_gop(efi_guid_t *proto,
        efi_handle_t *gop_handles, size_t gop_handles_len)
{
	int i;

	struct efi_graphics_output_protocol *first_gop = NULL;

	for (i = 0; i < gop_handles_len; i++) {
		efi_status_t status;

		efi_handle_t handle;
		struct efi_graphics_output_protocol *gop;
		struct efi_graphics_output_mode_info *info;

		void *con_out;
		efi_guid_t con_out_proto = EFI_CONSOLE_OUT_DEVICE_GUID;

		/*  Found a handle that support 'EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID' and 'EFI_CONSOLE_OUT_DEVICE_GUID'. */

		handle = gop_handles[i];

		/* 7.3.6. EFI_BOOT_SERVICES.LocateHandle. */
		status = efi_bs_call(handle_protocol, handle, proto, (void **)&gop);
		if (status != EFI_SUCCESS)
			continue;

		info = gop->mode->info;
		if (info->pixel_format == PIXEL_BLT_ONLY ||
		        info->pixel_format >= PIXEL_FORMAT_MAX)
			continue;

		status = efi_bs_call(handle_protocol, handle, &con_out_proto, &con_out);
		if (status == EFI_SUCCESS)
			return gop;

		if (!first_gop)
			first_gop = gop;
	}

	return first_gop;
}

static void efi_set_mode(struct efi_graphics_output_protocol *gop)
{
	int i;

	struct efi_graphics_output_protocol_mode *mode = gop->mode;
	struct efi_graphics_output_mode_info *qi, *info = mode->info;

	u64 info_size;
	u64 area, next_area = info->horizontal_resolution * info->vertical_resolution;
	u32 next_mode = mode->mode;

	for (i = 0; i < mode->max_mode; i++) {
		if (gop->query_mode(gop, i, &info_size, &qi) != EFI_SUCCESS)
			continue;

		area = qi->horizontal_resolution * qi->vertical_resolution;
		if (area < next_area) {
			next_area = area;
			next_mode = i;
		}
	}

	if (mode->mode != next_mode)
		gop->set_mode(gop, next_mode);
}

static efi_status_t efi_setup_gop(struct screen_info *si, efi_guid_t *proto,
        efi_handle_t *gop_handles, size_t gop_handles_len)
{
	struct efi_graphics_output_protocol *gop;
	struct efi_graphics_output_protocol_mode *mode;
	struct efi_graphics_output_mode_info *info;

	gop = efi_find_gop(proto, gop_handles, gop_handles_len);
	if (!gop)
		return EFI_NOT_FOUND;

	efi_set_mode(gop);

	mode = gop->mode;
	info = mode->info;

	si->framebuffer = mode->frame_buffer_base;
	si->framebuffer_size = mode->frame_buffer_size;
	si->screen_width = info->horizontal_resolution;
	si->screen_height = info->vertical_resolution;
	si->pixels_per_scan_line = info->pixels_per_scan_line;

	return EFI_SUCCESS;
}

#ifdef CONFIG_X86
#include <cyanea/pgtable.h>
#include <uapi/asm/bootparam.h>

#define MIN_KERNEL_ALIGN PMD_SIZE
_Static_assert(!((CONFIG_PHYSICAL_START & (CONFIG_PHYSICAL_ALIGN - 1))),
        "'CONFIG_PHYSICAL_START' is not aligned to 'CONFIG_PHYSICAL_ALIGN'.");

static efi_status_t setup_graphics(struct boot_params *boot_params)
{
	efi_status_t status;
	efi_guid_t graphics_proto = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	efi_handle_t *gop_handles;
	u64 gop_handles_len;

	struct screen_info *si = &boot_params->screen_info;

	si->framebuffer = 0;
	/* 7.3.15. EFI_BOOT_SERVICES.LocateHandleBuffer. */
	status = efi_bs_call(locate_handle_buffer, EFI_LOCATE_BY_PROTOCOL,
	                &graphics_proto, NULL, &gop_handles_len, &gop_handles);
	if (status == EFI_SUCCESS) {
		status = efi_setup_gop(si, &graphics_proto, gop_handles, gop_handles_len);

		efi_free(gop_handles);
	} else {
		/* TODO. SUPPORT ''Universal Graphics Adapter protocol''. */
	}

	return status;
}

static void __noreturn enter_kernel(unsigned long kernel_addr,
        struct boot_params *boot_params)
{
	/* Enter decompressed kernel with ''boot_params'' pointer in RSI. */
	asm("jmp *%0"::"r"(kernel_addr), "S"(boot_params));

	__builtin_unreachable();
}

/* '''ARCHITECTURE EFI ENTRY'''. */
#define arch_efi_entry __x86_efi_entry
static efi_status_t __noreturn __x86_efi_entry(efi_handle_t handle,
        struct efi_system_table *systab)
{
	efi_status_t status;

	struct efi_boot_memmap *memmap;

	struct boot_params *boot_params;
	/* ''UKERNEL @ (ukernel_addr, ukernel_size)''. */
	unsigned long ukernel_addr;
	size_t ukernel_size;

	__efi_systab = systab;

	status = efi_allocate_pages(BOOT_PARAMS_SIZE,
	                (efi_physical_addr_t *)&boot_params, 0, EFI_LOADER_DATA);
	if (status != EFI_SUCCESS)
		efi_exit(handle, status);

	boot_params->ramdisk_image = 0;
	boot_params->ramdisk_size = 0;
	boot_params->cmd_line_ptr = 0;
	boot_params->acpi_rsdp_addr = efi_get_rsdp_addr();

	/* ''Decompressing ukernel''. */

	ukernel_size = ROUND_UP(get_unaligned_le32(__UKERNEL_gz_end - 4),
	                MIN_KERNEL_ALIGN);

	status = efi_allocate_pages(ukernel_size, (efi_physical_addr_t *)&ukernel_addr,
	                CONFIG_PHYSICAL_ALIGN, EFI_LOADER_CODE);
	if (status != EFI_SUCCESS)
		efi_exit(handle, status);

	efi_puts("Decompressing UKERNEL ...\n");
	if (decompress_gzip((unsigned char *)ukernel_addr, &ukernel_size,
	                (unsigned char *)__UKERNEL_gz_start, __UKERNEL_gz_size))
		efi_exit(handle, EFI_LOAD_ERROR);

	status = setup_graphics(boot_params);
	if (status != EFI_SUCCESS)
		efi_puts("err. 'setup_graphics'.\n");

	status = efi_exit_boot_services(handle, &memmap);
	if (status != EFI_SUCCESS) {
		efi_puts("err. 'efi_exit_boot_services'.\n");

		efi_exit(handle, status);
	}

	boot_params->efi_info.efi_systab = (u64)systab;
	boot_params->efi_info.efi_memdesc_size = memmap->desc_size;
	boot_params->efi_info.efi_memmap_size = memmap->map_size;
	boot_params->efi_info.efi_memmap = (u64)memmap->descs;

	enter_kernel(ukernel_addr, boot_params);

	/* NEVER GET HERE! */
	efi_exit(handle, status);
}
#endif /* CONFIG_X86 */

efi_status_t __efiapi __efi_entry(efi_handle_t handle,
        struct efi_system_table *systab)
{
	arch_efi_entry(handle, systab);
}
