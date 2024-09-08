/* SPDX-License-Identifier: GPL-2.0-or-later */

/* UEFI Spec. https://uefi.org/specs/UEFI/2.10/. */

/* [This code is position independent]. */

#include <cyanea/types.h>
#include <cyanea/stdarg.h>
#include <cyanea/stddef.h>
#include <cyanea/limits.h>
#include <cyanea/const.h>

#include <asm-generic/unaligned.h>
#include <uapi/cyanea/screen_info.h>

# define EFI_PAGE_SIZE BIT_UL(12)

# define __efiapi __attribute__((ms_abi))

# define UUID_SIZE 16
typedef struct {
    u8 b[UUID_SIZE];
} efi_guid_t __aligned(__alignof__(u32));
# define EFI_GUID(a, b, c, d...) ((efi_guid_t){ {   \
            (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff,  \
            (b) & 0xff, ((b) >> 8) & 0xff,          \
            (c) & 0xff, ((c) >> 8) & 0xff, d } })

# define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    EFI_GUID(0x9042a9de, 0x23dc, 0x4a38, 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a)
#define EFI_CONSOLE_OUT_DEVICE_GUID \
    EFI_GUID(0xd3b36f2c, 0xd551, 0x11d4, 0x9a, 0x46, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d)

typedef u64 efi_status_t;
typedef u64 efi_handle_t;
typedef u16 efi_char16_t;
typedef u64 efi_physical_addr_t;

struct efi_table_hdr {
    u64 signature;
    u32 revision;
    u32 header_size;
    u32 crc32;
    u32 reserved;
};

struct efi_simple_text_output_protocol {
    u64 reset;
    efi_status_t (__efiapi *output_string)(struct efi_simple_text_output_protocol *, efi_char16_t *);
};

struct efi_runtime_services {
    struct efi_table_hdr hdr;
    u64 __pad[14];
};

struct efi_memory_desc {
# define EFI_RESERVED_TYPE          0
# define EFI_LOADER_CODE            1
# define EFI_LOADER_DATA            2
    u32 type;
    u32 pad;
    efi_physical_addr_t phys_addr;
    u64 virt_addr;
    u64 num_pages;
    u64 attribute;
};

struct efi_boot_services {
    struct efi_table_hdr hdr;
    u64 __pad1[2];
# define EFI_ALLOCATE_ANY_PAGES 0
# define EFI_ALLOCATE_MAX_ADDRESS 1
# define EFI_ALLOCATE_ADDRESS 2
    efi_status_t (__efiapi *allocate_pages)(int, int, u64, efi_physical_addr_t *);
    efi_status_t (__efiapi *free_pages)(efi_physical_addr_t, u64);
    efi_status_t (__efiapi *get_memory_map)(u64 *, struct efi_memory_desc *, u64 *, u64 *, u32 *);
    efi_status_t (__efiapi *allocate_pool)(int, u64, void **);
    efi_status_t (__efiapi *free_pool)(void *);
    u64 __pad2[9];
    efi_status_t (__efiapi *handle_protocol)(efi_handle_t, efi_guid_t *, void **);
    u64 __pad3[2];
# define EFI_LOCATE_ALL_HANDLES 0
# define EFI_LOCATE_BY_REGISTER_NOTIFY 1
# define EFI_LOCATE_BY_PROTOCOL 2
    efi_status_t (__efiapi *locate_handle)(int, efi_guid_t *, void *, u64 *, efi_handle_t *);
    u64 __pad4[4];
    efi_status_t __noreturn (__efiapi *exit)(efi_handle_t, efi_status_t, u64, efi_char16_t *);
    efi_status_t (__efiapi *unload_image)(efi_handle_t);
    efi_status_t (__efiapi *exit_boot_services)(efi_handle_t, u64);
    u64 __pad5[9];
    efi_status_t (__efiapi *locate_handle_buffer)(int, efi_guid_t *, void *, u64 *, efi_handle_t **);
    u64 __pad6[7];
};

struct efi_system_table {
    struct efi_table_hdr hdr;
    u64 fw_vendor;
    u32 fw_revision;
    u32 __pad1;
    u64 con_in_handle;      /* efi_handle_t. */
    u64 con_in;             /* struct efi_simple_text_input_protocol. */
    efi_handle_t con_out_handle;
    struct efi_simple_text_output_protocol *con_out;
    u64 stderr_handle;      /* efi_handle_t. */
    u64 stderr;             /* struct efi_simple_text_output_protocol. */
    struct efi_runtime_services *runtime;
    struct efi_boot_services *boottime;
    u32 nr_tables;
    u32 __pad2;
    u64 tables;
};

# define efi_bs_call(func, ...) __efi_systab->boottime->func(__VA_ARGS__)
# define efi_proto_call(proto, func, ...) ({    \
        __typeof__(proto) __proto = (proto);    \
        __proto->func(__proto, __VA_ARGS__);    \
    })

# define EFI_SUCCESS            (0)
# define EFI_LOAD_ERROR         (1  | BIT_UL(BITS_PER_LONG - 1))
# define EFI_INVALID_PARAMETER  (2  | BIT_UL(BITS_PER_LONG - 1))
# define EFI_BUFFER_TOO_SMALL   (5  | BIT_UL(BITS_PER_LONG - 1))
# define EFI_NOT_FOUND          (14 | BIT_UL(BITS_PER_LONG - 1))

#ifdef CONFIG_X86
# define REL_REF(x) (*(typeof(&(x)))(rip_rel_ptr(&(x))))
static always_inline __pure void *rip_rel_ptr(void *p)
{
    asm("leaq %c1(%%rip), %0" : "=r"(p) : "i"(p));

    return p;
}
#endif /* CONFIG_X86 */

static struct efi_system_table *____efi_systab;
# define __efi_systab REL_REF(____efi_systab)

extern char _binary_UKERNEL_gz_start[], _binary_UKERNEL_gz_end[];
# define __UKERNEL_gz_start REL_REF(_binary_UKERNEL_gz_start)
# define __UKERNEL_gz_end   REL_REF(_binary_UKERNEL_gz_end)
# define __UKERNEL_gz_size  (_binary_UKERNEL_gz_end - _binary_UKERNEL_gz_start)

# define ASCII_TO_USC2(x) ((efi_char16_t)(x))
# define USC2_TO_ASCII(x) ((char)(x))

# define EFI_PUTS_LEN 128
# define efi_puts(s) __efi_puts(REL_REF(s))
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

/* 'decompress_gzip'. */
#include "deflate.c"

static void __noreturn efi_exit(efi_handle_t handle, efi_status_t status)
{
    efi_bs_call(exit, handle, status, 0, NULL);
}

/* 7.2. Memory Allocation Services. */
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
    struct efi_memory_desc memmap[];
};

static efi_status_t efi_allocate_pages(size_t size, efi_physical_addr_t *alloc_addr,
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

static efi_status_t efi_allocate(int memory_type, size_t size, void **alloc_addr)
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
    return efi_bs_call(get_memory_map, &memmap->map_size, memmap_desc, &memmap->map_key,
            &memmap->desc_size, &memmap->desc_ver);
}

/* 7.4. Image Services. */
/* https://uefi.org/specs/UEFI/2.10/07_Services_Boot_Services.html#image-services. */

static efi_status_t efi_exit_boot_services(efi_handle_t handle, struct efi_boot_memmap **memmap)
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
    status = efi_get_memory_map(m, m->memmap);
    if (status != EFI_SUCCESS) {
        efi_free(m);

        return status;
    }

    /* 7.4.6. EFI_BOOT_SERVICES.ExitBootServices. */
    status = efi_bs_call(exit_boot_services, handle, m->map_key);
    if (status == EFI_INVALID_PARAMETER) {

        m->map_size = size;
        status = efi_get_memory_map(m, m->memmap);
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

struct efi_pixel_bitmask {
    u32 red_mask;
    u32 green_mask;
    u32 blue_mask;
    u32 reserved_mask;
};

struct efi_graphics_output_mode_info {
    u32 version;
    u32 horizontal_resolution;
    u32 vertical_resolution;
# define PIXEL_RGB_RESERVED_8BIT_PER_COLOR 0
# define PIXEL_BGR_RESERVED_8BIT_PER_COLOR 1
# define PIXEL_BIT_MASK 2
# define PIXEL_BLT_ONLY 3
# define PIXEL_FORMAT_MAX 4
    int pixel_format;
    struct efi_pixel_bitmask pixel_information;
    u32 pixels_per_scan_line;
};

struct efi_graphics_output_protocol_mode {
    u32 max_mode;
    u32 mode;
    struct efi_graphics_output_mode_info *info;
    u64 size_of_info;
    efi_physical_addr_t frame_buffer_base;
    u64 frame_buffer_size;
};

struct efi_graphics_output_protocol {
    efi_status_t (__efiapi *query_mode)(struct efi_graphics_output_protocol *, u32, u64 *,
        struct efi_graphics_output_mode_info **);
    efi_status_t (__efiapi *set_mode)(struct efi_graphics_output_protocol *, u32);
    u64 pad;    /* ''efi_status_t (__efiapi *blt)(...)''. */
    struct efi_graphics_output_protocol_mode *mode;
};

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
        if (efi_proto_call(gop, query_mode, i, &info_size, &qi) != EFI_SUCCESS)
            continue;

        area = qi->horizontal_resolution * qi->vertical_resolution;
        if (area < next_area) {
            next_area = area;
            next_mode = i;
        }
    }

    if (mode->mode != next_mode)
        efi_proto_call(gop, set_mode, next_mode);
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

# define MIN_KERNEL_ALIGN PMD_SIZE
_Static_assert((CONFIG_PHYSICAL_ALIGN >= MIN_KERNEL_ALIGN),
    "'CONFIG_PHYSICAL_ALIGN' is less than minimum kernel alignment.");

static efi_status_t setup_graphics(struct boot_params *boot_params)
{
    efi_status_t status;
    efi_guid_t graphics_proto = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    efi_handle_t *gop_handles;
    u64 gop_handles_len;

    struct screen_info *si = &boot_params->screen_info;

    si->framebuffer = 0;
    /* 7.3.15. EFI_BOOT_SERVICES.LocateHandleBuffer. */
    status = efi_bs_call(locate_handle_buffer, EFI_LOCATE_BY_PROTOCOL, &graphics_proto, NULL,
            &gop_handles_len, &gop_handles);
    if (status == EFI_SUCCESS) {
        status = efi_setup_gop(si, &graphics_proto, gop_handles, gop_handles_len);

        efi_free(gop_handles);
    } else {
        /* TODO. SUPPORT ''Universal Graphics Adapter protocol''. */
    }

    return status;
}

static void __noreturn enter_kernel(unsigned long kernel_addr, struct boot_params *boot_params)
{
    /* Enter decompressed kernel with ''boot_params'' pointer in RSI. */
    asm("jmp *%0"::"r"(kernel_addr), "S"(boot_params));

    __builtin_unreachable();
}

/* '''ARCHITECTURE EFI ENTRY'''. */
# define arch_efi_entry __x86_efi_entry
static efi_status_t __noreturn __x86_efi_entry(efi_handle_t handle, struct efi_system_table *systab)
{
    efi_status_t status;

    struct efi_boot_memmap *mmap;

    struct boot_params *boot_params;
    /* ''UKERNEL @ (ukernel_addr, ukernel_size)''. */
    unsigned long ukernel_addr;
    size_t ukernel_size;

    __efi_systab = systab;

    status = efi_allocate_pages(BOOT_PARAMS_SIZE, (efi_physical_addr_t *)&boot_params, 0,
            EFI_LOADER_DATA);
    if (status != EFI_SUCCESS)
        efi_exit(handle, status);

    boot_params->ramdisk_image = 0;
    boot_params->ramdisk_size = 0;
    boot_params->cmd_line_ptr = 0;
    boot_params->e820_entries = 0;

    /* ''Decompressing ukernel''. */

    ukernel_size = ROUND_UP(get_unaligned_le32(__UKERNEL_gz_end - 4), MIN_KERNEL_ALIGN);
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

    status = efi_exit_boot_services(handle, &mmap);
    if (status != EFI_SUCCESS) {
        efi_puts("err. 'efi_exit_boot_services'.\n");

        efi_exit(handle, status);
    }

    boot_params->efi_info.efi_systab = (u64)systab;
    boot_params->efi_info.efi_memdesc_size = mmap->desc_size;
    boot_params->efi_info.efi_memmap_size = mmap->map_size;
    boot_params->efi_info.efi_memmap = (u64)mmap->memmap;

    enter_kernel(ukernel_addr, boot_params);

    /* NEVER GET HERE! */
    efi_exit(handle, status);
}
#endif /* CONFIG_X86 */

efi_status_t __efiapi __efi_entry(efi_handle_t handle, struct efi_system_table *systab)
{
    arch_efi_entry(handle, systab);
}
