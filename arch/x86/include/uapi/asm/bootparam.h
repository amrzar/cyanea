/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_UAPI_ASM_BOOTPARAM_H__
#define __X86_UAPI_ASM_BOOTPARAM_H__

#ifndef __ASSEMBLY__

#include <uapi/cyanea/types.h>
#include <uapi/cyanea/screen_info.h>

/* 'BIOS_E820_MAX_ENTRIES' is the maximum number of entries in 'e820_table'. */
# define BIOS_E820_MAX_ENTRIES 128
struct bios_e820_entry {
    __u64 addr;
    __u64 size;
    __u32 type;
} __attribute__((__packed__));

struct efi_info {
    __u64 efi_systab;
    __u64 efi_memdesc_size;
    __u64 efi_memmap_size;
    __u64 efi_memmap;
} __attribute__((__packed__));

# define BOOT_PARAMS_SIZE 0x1000
struct boot_params {
    __u64 ramdisk_image;                /* 0x000. */
    __u64 ramdisk_size;                 /* 0x008. */
    __u64 cmd_line_ptr;                 /* 0x010. */

    struct screen_info screen_info;     /* 0x018. */

    __u8 e820_entries;                  /* 0x03C. */
    struct bios_e820_entry e820_table[BIOS_E820_MAX_ENTRIES];   /* 0x03D. */
    struct efi_info efi_info;           /* 0xA3D. */

    __u8 __pad[0x1000 - 0xA5D];         /* 0xA5D. */
} __attribute__((__packed__));

_Static_assert((sizeof(struct boot_params) == BOOT_PARAMS_SIZE),
    "boot_params is not 4KiB.");

#endif /* __ASSEMBLY__ */

#endif /* __X86_UAPI_ASM_BOOTPARAM_H__ */
