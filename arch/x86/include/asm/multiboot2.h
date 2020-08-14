/*
 * Copyright 2020 Amirreza Zarrabi <amrzar@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __ASM_X86_MULTIBOOT2_H
#define __ASM_X86_MULTIBOOT2_H

#define MB_HEADER_MAGIC 0xe85250d6

#define MB_HEADER_ARCHITECTURE_I386 0
#define MB_HEADER_ARCHITECTURE_MIPS32 4

#define MB_HEADER_TAG_END 0
#define MB_HEADER_TAG_INFORMATION_REQUEST 1
#define MB_HEADER_TAG_ADDRESS 2
#define MB_HEADER_TAG_ENTRY_ADDRESS 3

#define MB_HEADER_TAG_CONSOLE_FLAGS 4
#define MB_CONSOLE_FLAGS_CONSOLE_REQUIRED 1
#define MB_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED 2

#define MB_HEADER_TAG_FRAMEBUFFER 5
#define MB_HEADER_TAG_MODULE_ALIGN 6
#define MB_HEADER_TAG_EFI_BS 7
#define MB_HEADER_TAG_ENTRY_ADDRESS_EFI32 8
#define MB_HEADER_TAG_ENTRY_ADDRESS_EFI64 9

#define MB_HEADER_TAG_RELOCATABLE 10
#define MB_LOAD_PREFERENCE_NONE 0
#define MB_LOAD_PREFERENCE_LOW 1
#define MB_LOAD_PREFERENCE_HIGH 2

#define MB_HEADER_FLAG_OPTIONAL 1

#ifndef __ASSEMBLY__
#include <cyanea/types.h>
#include <cyanea/compiler.h>

#define MB_BOOTLOADER_MAGIC 0x36d76289

struct __tag_header {
    uint32_t type;
    uint32_t size;
} __packed;

#define MB_TAG_TYPE_END 0
#define MB_TAG_TYPE_EFI_BS 18

#define MB_TAG_TYPE_CMDLINE 1
#define MB_TAG_TYPE_BOOT_LOADER_NAME 2
struct mb_tag_string {
    struct __tag_header tag_header;
    char string[];
};

#define MB_TAG_TYPE_MODULE 3
struct mb_tag_module {
    struct __tag_header tag_header;
    uint32_t mod_start;
    uint32_t mod_end;
    char cmdline[];
} __packed;

#define MB_TAG_TYPE_BASIC_MEMINFO 4
struct mb_tag_basic_meminfo {
    struct __tag_header tag_header;
    uint32_t mem_lower;
    uint32_t mem_upper;
} __packed;

#define MB_TAG_TYPE_BOOTDEV 5
struct mb_tag_bootdev {
    struct __tag_header tag_header;
    uint32_t biosdev;
    uint32_t partition;
    uint32_t sub_parition;
} __packed;

struct mb_mmap_entry {
    uint64_t addr;
    uint64_t len;
#define MB_MEMORY_AVAILABLE 1
#define MB_MEMORY_RESERVED 2
#define MB_MEMORY_ACPI_RECLAIMABLE 3
#define MB_MEMORY_NVS 4
#define MB_MEMORY_BADRAM 5
    uint32_t type;
    uint32_t zero;
} __packed;

#define MB_TAG_TYPE_MMAP 6
struct mb_tag_mmap {
    struct __tag_header tag_header;
    uint32_t entry_size;
    uint32_t entry_version;
    struct mb_mmap_entry entries[];
} __packed;

struct mb_vbe_info_block {
    uint8_t external_specification[512];
};

struct mb_vbe_mode_info_block {
    uint8_t external_specification[256];
};

#define MB_TAG_TYPE_VBE 7
struct mb_tag_vbe {
    struct __tag_header tag_header;

    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    struct mb_vbe_info_block vbe_control_info;
    struct mb_vbe_mode_info_block vbe_mode_info;
} __packed;

#define MB_TAG_TYPE_FRAMEBUFFER 8
struct mb_tag_fb_common {
    struct __tag_header tag_header;

    uint64_t fb_addr;
    uint32_t fb_pitch;
    uint32_t fb_width;
    uint32_t fb_height;
    uint8_t fb_bpp;
#define MB_FRAMEBUFFER_TYPE_INDEXED 0
#define MB_FRAMEBUFFER_TYPE_RGB 1
#define MB_FRAMEBUFFER_TYPE_EGA_TEXT 2
    uint8_t fb_type;
    uint16_t reserved;
} __packed;

struct mb_color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} __packed;

struct mb_tag_framebuffer {
    struct mb_tag_fb_common common;

    union {
        struct {
            uint16_t fb_palette_num_colors;
            struct mb_color fb_palette[];
        } __packed;
        struct {
            uint8_t fb_red_field_position;
            uint8_t fb_red_mask_size;
            uint8_t fb_green_field_position;
            uint8_t fb_green_mask_size;
            uint8_t fb_blue_field_position;
            uint8_t fb_blue_mask_size;
        } __packed;
    };
} __packed;

#define MB_TAG_TYPE_ELF_SECTIONS 9
struct mb_tag_elf_sections {
    struct __tag_header tag_header;
    uint32_t e_shnum;
    uint32_t e_shentsize;
    uint32_t e_shstrndx;
    uint8_t sections[];
} __packed;

#define MB_TAG_TYPE_EFI32 11
#define MB_TAG_TYPE_EFI32_IH 19
struct mb_tag_efi32 {
    struct __tag_header tag_header;
    uint32_t pointer;
} __packed;

#define MB_TAG_TYPE_EFI64 12
#define MB_TAG_TYPE_EFI64_IH 20
struct mb_tag_efi64 {
    struct __tag_header tag_header;
    uint64_t pointer;
} __packed;

#define MB_TAG_TYPE_SMBIOS 13
struct mb_tag_smbios {
    struct __tag_header tag_header;
    uint8_t major;
    uint8_t minor;
    uint8_t reserved[6];
    uint8_t tables[];
} __packed;

#define MB_TAG_TYPE_ACPI_OLD 14
#define MB_TAG_TYPE_ACPI_NEW 15
struct mb_tag_acpi {
    struct __tag_header tag_header;
    uint8_t rsdp[];
};

#define MB_TAG_TYPE_NETWORK 16
struct mb_tag_network {
    struct __tag_header tag_header;
    uint8_t dhcpack[];
};

#define MB_TAG_TYPE_EFI_MMAP 17
struct mb_tag_efi_mmap {
    struct __tag_header tag_header;
    uint32_t descr_size;
    uint32_t descr_vers;
    uint8_t efi_mmap[];
} __packed;

#define MB_TAG_TYPE_LOAD_BASE_ADDR 21
struct mb_tag_load_base_addr {
    struct __tag_header tag_header;
    uint32_t load_base_addr;
} __packed;

#endif /* __ASSEMBLY__ */
#endif /* __ASM_X86_MULTIBOOT2_H */