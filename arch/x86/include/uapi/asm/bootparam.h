/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_UAPI_ASM_BOOTPARAM_H__
#define __X86_UAPI_ASM_BOOTPARAM_H__

#define SETUP_NONE 0
#define SETUP_INDIRECT (1 << 31)

/* 'SETUP_INDIRECT | max(SETUP_*)'. */
#define SETUP_TYPE_MAX (SETUP_INDIRECT | SETUP_NONE)

/* 'loadflags'. */
#define LOADED_HIGH   (1 << 0)
#define KASLR_FLAG    (1 << 1)
#define QUIET_FLAG    (1 << 5)
#define KEEP_SEGMENTS (1 << 6)
#define CAN_USE_HEAP  (1 << 7)

/* 'xloadflags'. */
#define XLF_KERNEL_64 (1 << 0)
#define XLF_CAN_BE_LOADED_ABOVE_4G (1 << 1)
#define XLF_EFI_HANDOVER_32 (1 << 2)
#define XLF_EFI_HANDOVER_64 (1 << 3)
#define XLF_EFI_KEXEC (1 << 4)
#define XLF_5LEVEL    (1 << 5)
#define XLF_5LEVEL_ENABLED  (1 << 6)

#ifndef __ASSEMBLY__

#include <uapi/cyanea/types.h>

struct setup_data {
    __u64 next;
    __u32 type;
    __u32 len;
    __u8 data[0];
};

struct setup_indirect {
    __u32 type;
    __u32 reserved;
    __u64 len;
    __u64 addr;
};

struct setup_header {
    __u8 setup_sects;
    __u16 root_flags;
    __u32 syssize;
    __u16 ram_size;
    __u16 vid_mode;
    __u16 root_dev;
    __u16 boot_flag;
    __u16 jump_inst;
    __u32 header;
    __u16 version;
    __u32 realmode_swtch;
    __u16 start_sys_seg;
    __u16 kernel_version;
    __u8 type_of_loader;
    __u8 loadflags;
    __u16 setup_move_size;
    __u32 code32_start;
    __u32 ramdisk_image;
    __u32 ramdisk_size;
    __u32 bootsect_kludge;
    __u16 heap_end_ptr;
    __u8 ext_loader_ver;
    __u8 ext_loader_type;
    __u32 cmd_line_ptr;
    __u32 initrd_addr_max;
    __u32 kernel_alignment;
    __u8 relocatable_kernel;
    __u8 min_alignment;
    __u16 xloadflags;
    __u32 cmdline_size;
    __u32 hardware_subarch;
    __u64 hardware_subarch_data;
    __u32 payload_offset;
    __u32 payload_length;
    __u64 setup_data;
    __u64 pref_address;
    __u32 init_size;
    __u32 handover_offset;
    __u32 kernel_info_offset;
} __attribute__((__packed__));

/* '__E820_MAX_ENTRIES' is the maximum number of entries in 'boot_params''s
 * 'e820_table', which is part of the x86 boot protocol. * */

#define __E820_MAX_ENTRIES 128
struct __e820_entry {
    __u64 addr;
    __u64 size;
    __u32 type;
} __attribute__((__packed__));

struct boot_params {
    __u8 _pad[0x0C8];           /* 0x000 */
    __u32 ext_cmd_line_ptr;     /* 0x0C8 */
    __u8 _pad1[0x118];          /* 0x0CC */
    __u32 scratch;              /* 0x1E4 */
    __u8 e820_entries;          /* 0x1E8 */
    __u8 _pad2[6];              /* 0x1E9 */

    /*
     * 'sentinel' is set to a nonzero value (0xFF) in header.S.
     *
     * The purpose of this field is to guarantee compliance with the x86
     * boot spec. The 'boot_params' structure should be cleared by the
     * boot-loader before it updates the 'setup_header' portion of the
     * 'boot_param'. The fact that this field is nonzero means other fields
     * in ''boot_params'' may be invalid and should be initialised.
     *
     * */

    __u8 sentinel;              /* 0x1EF */
    __u8 _pad3[1];              /* 0x1F0 */

    struct setup_header hdr;    /* 0x1F1 */
    __u8 _pad4[0x2D0 - 0x1F1 - sizeof(struct setup_header)];

    struct __e820_entry e820_table[__E820_MAX_ENTRIES]; /* 0x2D0 */
    __u8 _pad5[0x330];          /* 0xCD0 */
} __attribute__((__packed__));

_Static_assert((sizeof(struct boot_params) == 0x1000),
    "Size of 'boot_params' is not 4KiB.");

#endif /* __ASSEMBLY__ */

#endif /* __X86_UAPI_ASM_BOOTPARAM_H__ */
