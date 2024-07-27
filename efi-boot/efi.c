/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/types.h>
#include <cyanea/stdarg.h>
#include <cyanea/stddef.h>
#include <cyanea/const.h>

#include <asm-generic/unaligned.h>

# define EFI_PAGE_SIZE BIT_UL(12)

/* [This code should be position independent]. */

# define __efiapi __attribute__((ms_abi))

# define UUID_SIZE 16
typedef struct {
    u8 b[UUID_SIZE];
} efi_guid_t __aligned(__alignof__(u32));
# define EFI_GUID(a, b, c, d...) ((efi_guid_t){ {                \
            (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff,  \
            (b) & 0xff, ((b) >> 8) & 0xff,                      \
            (c) & 0xff, ((c) >> 8) & 0xff, d } })

# define LOADED_IMAGE_PROTOCOL_GUID \
    EFI_GUID(0x5b1b31a1, 0x9562, 0x11d2,  0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b)

typedef u64 efi_status_t;
typedef u64 efi_handle_t;
typedef u16 efi_char16_t;

typedef struct efi_table_hdr {
    u64 signature;
    u32 revision;
    u32 headersize;
    u32 crc32;
    u32 reserved;
} efi_table_hdr_t;

typedef struct efi_simple_text_output_protocol {
    u64 reset;
    efi_status_t (__efiapi *output_string)(struct efi_simple_text_output_protocol *, efi_char16_t *);
} efi_simple_text_output_protocol_t;

typedef struct efi_runtime_services {
    efi_table_hdr_t hdr;
    u64 __pad[14];
} efi_runtime_services_t;

typedef struct efi_boot_services {
    efi_table_hdr_t hdr;
    u64 __pad1[5];
    efi_status_t (__efiapi *allocate_pool)(int, unsigned long, void **);
    u64 __pad2[10];
    efi_status_t (__efiapi *handle_protocol)(efi_handle_t, efi_guid_t *, void **);
    u64 __pad3[27];
} efi_boot_services_t;

typedef struct efi_system_table {
    efi_table_hdr_t hdr;
    u64 fw_vendor;
    u32 fw_revision;
    u32 __pad1;
    u64 con_in_handle;      /* efi_handle_t. */
    u64 con_in;             /* efi_simple_text_input_protocol_t. */
    efi_handle_t con_out_handle;
    efi_simple_text_output_protocol_t *con_out;
    u64 stderr_handle;      /* efi_handle_t. */
    u64 stderr;             /* efi_simple_text_output_protocol_t. */
    efi_runtime_services_t *runtime;
# define EFI_BOOT_SERVICE(func, ...) EFI_SYSTABLE->boottime->func(__VA_ARGS__)
    efi_boot_services_t *boottime;
    u32 nr_tables;
    u32 __pad2;
    u64 tables;
} efi_system_table_t;

typedef struct efi_loaded_image {
    u32 revision;
    efi_handle_t parent_handle;
    efi_system_table_t *system_table;
    efi_handle_t device_handle;
    void *file_path;                /* efi_device_path_protocol. */
    void *reserved;
    u32 load_options_size;
    void *load_options;
    void *image_base;
    u64 image_size;
    unsigned int image_code_type;   /* efi_memory_type. */
    unsigned int image_data_type;   /* efi_memory_type. */
    efi_status_t (__efiapi *unload)(efi_handle_t);
} efi_loaded_image_t;

# define EFI_SUCCESS 0

static efi_system_table_t *__efi_systab;
# define EFI_SYSTABLE REL_REF(__efi_systab)

# define EFI_RESERVED_TYPE 0
# define EFI_LOADER_CODE 1
# define EFI_LOADER_DATA 2

#ifndef __pic__
# define REL_REF(x) (*(typeof(&(x)))(rip_rel_ptr(&(x))))
# ifdef CONFIG_X86_64
static always_inline __pure void *rip_rel_ptr(void *p)
{
    asm("leaq %c1(%%rip), %0" : "=r"(p) : "i"(p));

    return p;
}
# endif
#else
# define REL_REF(x) (x)
#endif /* __pic__ */

# define ASCII_TO_USC2(x) ((efi_char16_t)(x))
# define USC2_TO_ASCII(x) ((char)(x))

static void usc2_to_ascii(const efi_char16_t *usc2, char *ascii, size_t len)
{
    int i;

    for (i = 0; usc2[i] != '\0' && i < len - 1; i++)
        ascii[i] = USC2_TO_ASCII(usc2[i]);
    ascii[i] = USC2_TO_ASCII('\0');
}

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

    EFI_SYSTABLE->con_out->output_string(EFI_SYSTABLE->con_out, usc2);
}

/* WAIT indefinitely; firmware reboot. */
# define efi_error(s) ({ efi_puts(s); while (1); })

extern char _binary_UKERNEL_gz_start[], _binary_UKERNEL_gz_end[];

#include "deflate.c"

static u32 update_crc32(const unsigned char *buffer, size_t size)
{
    size_t n;
    static u32 __crctab32[] = {
        0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190,
        0x6B6B51F4, 0x4DB26158, 0x5005713C, 0xEDB88320, 0xF00F9344,
        0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278,
        0xBDBDF21C
    };

    u32 *crctab32 = REL_REF(__crctab32);
    u32 crc = 0xFFFFFFFF;

    if (!size)
        return 0;

    for (n = 0; n < size; n++) {
        crc ^= buffer[n];
        crc = crctab32[crc & 0x0F] ^ (crc >> 4);
        crc = crctab32[crc & 0x0F] ^ (crc >> 4);
    }

    return (crc ^ 0xFFFFFFFF);
}

static int decompress_gzip(unsigned char *dest, size_t *dest_len, unsigned char *source,
    size_t source_len)
{
    /* See https://www.rfc-editor.org/rfc/rfc1952.txt */

    const unsigned char *start = source + 10;
    const unsigned char *end = source + source_len;

    if ((source_len < 18) ||    /* Size of empty compressed file.  */
        (source[0] != 0x1F) ||  /* Identification 1. */
        (source[1] != 0x8B) ||  /* Identification 2. */
        (source[2] != 0x8) ||   /* ''deflate'' compression method. */
        (source[3] & 0xE0))     /* Reserved FLG bits must be zero. */
        return -EINVAL;

#define F_TEXT    (1 << 0)
#define F_HCRC    (1 << 1)
#define F_EXTRA   (1 << 2)
#define F_NAME    (1 << 3)
#define F_COMMENT (1 << 4)
    u8 flag = source[3];

    /* Skip 'F_EXTRA', 'F_NAME', and 'F_COMMENT'. */

    if (flag & F_EXTRA) {
        start += get_unaligned_le16(start) + 2;
        if (start > end)
            return -EINVAL;
    }

    if (flag & F_NAME) {
        do {
            if (start == end)
                return -EINVAL;
        } while (*start++);
    }

    if (flag & F_COMMENT) {
        do {
            if (start == end)
                return -EINVAL;
        } while (*start++);
    }

    if (flag & F_HCRC) {
        if (start + 2 > end)
            return -EINVAL;

        u16 crc16 = update_crc32(source, start - source) & 0xFFFF;
        if (crc16 != get_unaligned_le16(start)) {
            efi_puts("Invalid header CRC for compressed file.\n");

            return -EINVAL;
        }

        start += 2;
    }

    if (start + 8 > end)
        return -EINVAL;

    if (deflate_buffer(dest, dest_len, start, end - start - 8)) {
        efi_puts("'deflate_buffer' failed.\n");

        return -EINVAL;
    }

    if (update_crc32(dest, *dest_len) != get_unaligned_le32(&source[source_len - 8])) {
        efi_puts("Invalid CRC for compressed file.\n");

        return -EINVAL;
    }

    return SUCCESS;
}

#ifndef EFI_ALLOC_ALIGN
# define EFI_ALLOC_ALIGN EFI_PAGE_SIZE
#endif

# define EFI_HLT while (1)

efi_status_t __efiapi __efi_entry(efi_handle_t handle, efi_system_table_t *systab)
{
    char *cmd_line, *ukernel_base;
    size_t cmd_line_size, alloc_size;

    efi_loaded_image_t *image;

    EFI_SYSTABLE = systab;

    /* ''Copy command-line params''. */

    if (EFI_BOOT_SERVICE(handle_protocol, handle, &LOADED_IMAGE_PROTOCOL_GUID, (void **)&image))
        efi_error("Unable to get parent's loaded image protocol.");

    cmd_line_size = image->load_options_size / sizeof(efi_char16_t) + 1 /* for trialing '\0'. */;

    if (EFI_BOOT_SERVICE(allocate_pool, EFI_LOADER_DATA, cmd_line_size, (void **)&cmd_line))
        efi_error("Unable to get command-line.");

    usc2_to_ascii(image->load_options, cmd_line, cmd_line_size);

    /* ''Decompressing ukernel''. */

    alloc_size = ROUND_UP(get_unaligned_le32(REL_REF(_binary_UKERNEL_gz_end) - 4), EFI_ALLOC_ALIGN);

    if (EFI_BOOT_SERVICE(allocate_pool, EFI_LOADER_CODE, alloc_size + CONFIG_PHYSICAL_ALIGN - 1,
            (void **)&ukernel_base)) /* Use (2). */
        efi_error("Unable to allocate memory for ukernel.\n");

    ukernel_base = PTR_ALIGN(ukernel_base, CONFIG_PHYSICAL_ALIGN);

    efi_puts("Decompressing UKERNEL ...\n");

    decompress_gzip((unsigned char *)ukernel_base, &alloc_size,
        (unsigned char *)REL_REF(_binary_UKERNEL_gz_start),
        _binary_UKERNEL_gz_end - _binary_UKERNEL_gz_start);




    efi_error("Something went wrong, We should not get here!");

    return EFI_SUCCESS;
}
