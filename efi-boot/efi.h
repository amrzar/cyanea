/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __EFI_H__
#define __EFI_H__

#include <cyanea/types.h>

/* Unified Extensible Firmware Interface (UEFI). */
/* https://uefi.org/specs/UEFI/2.10/index.html. */

#define EFI_PAGE_SIZE BIT_UL(12)

#define __efiapi __attribute__((ms_abi))

#define UUID_SIZE 16
typedef struct {
    u8 b[UUID_SIZE];
} efi_guid_t __aligned(__alignof__(u32));
#define EFI_GUID(a, b, c, d...) ((efi_guid_t){ {   \
            (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff,  \
            (b) & 0xff, ((b) >> 8) & 0xff,          \
            (c) & 0xff, ((c) >> 8) & 0xff, d } })

#define ACPI_TABLE_GUID                     EFI_GUID(0xeb9d2d30, 0x2d88, 0x11d3, 0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d)
#define ACPI_20_TABLE_GUID                  EFI_GUID(0x8868e871, 0xe4f1, 0x11d3, 0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81)
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID   EFI_GUID(0x9042a9de, 0x23dc, 0x4a38, 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a)
#define EFI_CONSOLE_OUT_DEVICE_GUID         EFI_GUID(0xd3b36f2c, 0xd551, 0x11d4, 0x9a, 0x46, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d)

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

struct efi_config_table {
    efi_guid_t guid;
    u64 table;
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

/* 7. Services - Boot Services. */
/* https://uefi.org/specs/UEFI/2.10/07_Services_Boot_Services.html#services-boot-services. */

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

/* 8. Services - Runtime Services. */
/* https://uefi.org/specs/UEFI/2.10/08_Services_Runtime_Services.html#services-runtime-services. */

struct efi_runtime_services {
    struct efi_table_hdr hdr;
    u64 __pad[14];
};

/* 4. EFI System Table. */
/* https://uefi.org/specs/UEFI/2.10/04_EFI_System_Table.html#efi-system-table. */

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
    u32 nr_config_tables;
    u32 __pad2;
    struct efi_config_table *config_tables;
};

/* 12. Protocols - Console Support. */
/* https://uefi.org/specs/UEFI/2.10/12_Protocols_Console_Support.html#protocols-console-support. */

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

#define EFI_SUCCESS            0
#define EFI_LOAD_ERROR         (1 | BIT_UL(BITS_PER_LONG - 1))
#define EFI_INVALID_PARAMETER  (2 | BIT_UL(BITS_PER_LONG - 1))
#define EFI_BUFFER_TOO_SMALL   (5 | BIT_UL(BITS_PER_LONG - 1))
#define EFI_NOT_FOUND          (14 | BIT_UL(BITS_PER_LONG - 1))

#endif /* __EFI_H__ */
