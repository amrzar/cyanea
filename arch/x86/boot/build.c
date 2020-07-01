/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <endian.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "voffset.h"
#include "zoffset.h"

static const uint32_t crctab32[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419,
    0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
    0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07,
    0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
    0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
    0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
    0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
    0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
    0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
    0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
    0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e,
    0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed,
    0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3,
    0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
    0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
    0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
    0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17,
    0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
    0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
    0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
    0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a,
    0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
    0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
    0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
    0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe,
    0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
    0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
    0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
    0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1,
    0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
    0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
    0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
    0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
    0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
    0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b,
    0x2d02ef8d
};

/*
 * The real-mode code consists of the boot-sector (always one sector) plus the
 * setup code (at least 4 sectors), i.e. 'SETUP_SECT_MIN' is 5. The maximum size
 * of real-mode code is 32KiB so that there is enough space for stack and heap
 * in a 64KiB segment.
 *
 * See 'setup_sects' and 'setup_move_size' in 'header.S'.
 *
 * */

#define SETUP_SECT_MIN 5
#define SETUP_SECT_MAX 64

static uint8_t buffer[SETUP_SECT_MAX * 512] = { 0 };

#define OFFSET_PE_HEADER_PTR 0x3C
#define OFFSET_SETUP_SECTS 0x1F1
#define OFFSET_SYSSIZE 0x1F4
#define OFFSET_BOOT_FLAG 0x1FE
#define OFFSET_INIT_SIZE 0x260
#define OFFSET_HANDOVER_OFFSET 0x264
#define OFFSET_KERNEL_INFO_OFFSET 0x268

#define read_le16(p) le16toh(*((uint16_t *)(p)))
#define read_le32(p) le32toh(*((uint32_t *)(p)))
#define store_le16(p, value) (*((uint16_t *)(p)) = htole16((uint16_t)(value)))
#define store_le32(p, value) (*((uint32_t *)(p)) = htole32((uint32_t)(value)))

#ifdef CONFIG_EFI_STUB

/*
 * 'PECOFF_COMPAT_RESERVE' is default '.compat' size. EFI stub version 1.0 allows
 * the firmware that targets a diffrent machine type to start the kernel via the
 * entry point exposed in the '.compat' PE/COFF section.
 *
 * */

#ifdef CONFIG_EFI_MIXED
#define PECOFF_COMPAT_RESERVE 0x20
#else
#define PECOFF_COMPAT_RESERVE 0x0
#endif /* CONFIG_EFI_MIXED */

/*
 * 'PECOFF_RELOC_RESERVE' is default '.reloc' size. The EFI application loader
 * requires a relocation section as EFI applications must be relocatable.
 *
 * */

#define PECOFF_RELOC_RESERVE 0x20

static void update_pecoff_section_header(char *section_name,
    uint32_t virtual_offset, size_t virtual_size,
    uint32_t raw_data_offset, size_t raw_data_size)
{

    uintptr_t pe_header = read_le32(&buffer[OFFSET_PE_HEADER_PTR]);

#define OFFSET_PECOFF_NR_SECTIONS 0x6
#ifdef CONFIG_X86_32
#define OFFSET_PECOFF_SECTION_TABLE 0xA8
#else
#define OFFSET_PECOFF_SECTION_TABLE 0xB8
#endif /* CONFIG_X86_32 */

    int num_sections =
        read_le32(&buffer[pe_header + OFFSET_PECOFF_NR_SECTIONS]);
    uintptr_t section =
        (uintptr_t) (&buffer[pe_header + OFFSET_PECOFF_SECTION_TABLE]);

    while (num_sections > 0) {
        if (strncmp((char *)section, section_name, 8) == 0) {
            store_le32(section + 0x8, virtual_size);
            store_le32(section + 0xC, virtual_offset);
            store_le32(section + 0x10, raw_data_size);
            store_le32(section + 0x14, raw_data_offset);

            break;
        }

        section += 0x28;        /* ... move to next section. */
        num_sections--;
    }
}

static void update_pecoff_setup(size_t size)
{
    uint32_t setup_size =
        size - 0x200 - PECOFF_RELOC_RESERVE - PECOFF_COMPAT_RESERVE;

    /* '.setup' section covers setup code following the first sector. */
    update_pecoff_section_header(".setup", 0x200, setup_size, 0x200,
        setup_size);
}

static void update_pecoff_reloc(size_t size)
{
    uint32_t reloc_offset = size - PECOFF_RELOC_RESERVE - PECOFF_COMPAT_RESERVE;

    update_pecoff_section_header(".reloc", reloc_offset, PECOFF_RELOC_RESERVE,
        reloc_offset, PECOFF_RELOC_RESERVE);

    /* '.reloc' section contains single dummy relocation entry.
     * The relocation is applied to offset 10 of the relocation section. */

    store_le32(&buffer[reloc_offset], reloc_offset + 10);
    store_le32(&buffer[reloc_offset + 0x4], 10);
}

static void update_pecoff_compat(size_t size)
{
    uint32_t compat_offset = size - PECOFF_COMPAT_RESERVE;

    update_pecoff_section_header(".compat", compat_offset,
        PECOFF_COMPAT_RESERVE, compat_offset, PECOFF_COMPAT_RESERVE);

    /* ... and store the IA-32 machine type (0x14c) and the associated entry point.
     * The boot-loader uses '.compat' to figure out suppoeted execution modes in
     * the image. */

    buffer[compat_offset] = 0x1;
    buffer[compat_offset + 0x1] = 0x8;
    store_le16(&buffer[compat_offset + 0x2], 0x14C);
    store_le32(&buffer[compat_offset + 0x4], efi32_pe_entry + size);
}

static void update_pecoff_text(uintptr_t text_start,
    size_t sys_size, size_t init_size)
{
    uintptr_t pe_header = read_le32(&buffer[OFFSET_PE_HEADER_PTR]);

#define OFFSET_PECOFF_SIZE_OF_CODE 0x1C
#define OFFSET_PECOFF_ADDRESS_OF_ENTRY_POINT 0x28
#define OFFSET_PECOFF_SIZE_OF_IMAGE 0x50

    /*
     * The PE/COFF loader can load 'ukernel.bin' at an address which is misaligned
     * with respect to the 'kernel_alignment' in the setup header.
     *
     * Add 'CONFIG_PHYSICAL_ALIGN' so that 'ukernel.bin' can be aligned within the
     * declared size of the buffer.
     *
     * */

    init_sz += CONFIG_PHYSICAL_ALIGN;

    /* 'SizeOfCode' is sum of ''code'' sections ... assuming 'init_sz' except the initial sector. */
    store_le32(&buffer[pe_header + OFFSET_PECOFF_SIZE_OF_CODE],
        init_sz - 0x200);

    /* 'SizeOfImage' is size of sections, including headers ... assuming 'init_sz'. */
    store_le32(&buffer[pe_header + OFFSET_PECOFF_SIZE_OF_IMAGE], init_sz);

    store_le32(&buffer[pe_header + OFFSET_PECOFF_ADDRESS_OF_ENTRY_POINT],
        text_start + efi_pe_entry);

    update_pecoff_section_header(".text", text_start,
        init_sz - text_start, text_start, sys_size);
}

static int update_setup_handover(void)
{
    uintptr_t stub;

#ifdef CONFIG_X86_32
    stub = efi32_stub_entry;
#else
    stub = efi64_stub_entry - 0x200;
#ifdef CONFIG_EFI_MIXED

    if (efi32_stub_entry != stub) {
        fprintf(stderr,
            "'efi32_stub_entry' and 'efi64_stub_entry' should be 512-byte apart.\n");
        return -1;
    }

#endif /* CONFIG_EFI_MIXED */
#endif /* CONFIG_X86_32 */

    store_le32(&buffer[OFFSET_HANDOVER_OFFSET], stub);
    return 0;
}

#else

static void update_pecoff_setup(size_t size)
{
}

static void update_pecoff_reloc(size_t size)
{
}

static void update_pecoff_compat(size_t size)
{
}

static void update_pecoff_text(uintptr_t text_start,
    size_t sys_size, size_t init_sz)
{
}

static int update_setup_handover(void)
{
    return 0;
}

#endif /* CONFIG_EFI_STUB */

static uint32_t partial_crc32_one(uint8_t c, uint32_t crc)
{
    return crctab32[(crc ^ c) & 0xFF] ^ (crc >> 8);
}

static uint32_t partial_crc32(const uint8_t buf[], size_t length, uint32_t crc)
{
    for (size_t i = 0; i < length; i++) {
        crc = partial_crc32_one(buf[i], crc);
    }

    return crc;
}

#define die(s) do{          \
        perror(s);          \
        return -1;          \
    } while(0)

#define align_mask(x, mask) (((x) + (mask)) & ~(mask))

int main(int argc, char **argv)
{
    int fd;
    ssize_t setup_len;
    size_t sys_size, kernel_size, init_size;

    void *kernel;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s SETUP KERNEL OUTPUT\n"
            "  SETUP is 'setup.bin' from boot directory.\n"
            "  KERNEL is 'ukernel.bin' from compressed directory.\n"
            "  OUTPUT is 'zukernel'.\n", argv[0]);
        return -1;
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1)
        die(argv[1]);

    if ((setup_len = read(fd, buffer, SETUP_SECT_MAX * 512)) == -1)
        die(argv[1]);

    if (setup_len < 1024) {
        fprintf(stderr, "%s must be at least two sectors.\n", argv[1]);
        return -1;
    }

    if (read_le16(&buffer[OFFSET_BOOT_FLAG]) != 0xAA55) {
        fprintf(stderr, "'boot_flag' is not '0xAA55'.\n");
        return -1;
    }

    close(fd);                  /* ... 'SETUP'. */

#ifdef CONFIG_EFI_STUB
    setup_len += PECOFF_COMPAT_RESERVE;
    setup_len += PECOFF_RELOC_RESERVE;
#endif

    setup_len = align_mask(setup_len, 0x1FF);

    if (setup_len < SETUP_SECT_MIN * 512)
        setup_len = SETUP_SECT_MIN * 512;

    buffer[OFFSET_SETUP_SECTS] = (setup_len / 512) - 1;

    /* Updating '.setup', '.reloc', and '.compat' PE/COFF sections ... */

    update_pecoff_setup(setup_len);
    update_pecoff_reloc(setup_len);
    update_pecoff_compat(setup_len);

    if ((fd = open(argv[2], O_RDONLY)) == -1)
        die(argv[1]);

    struct stat sb;

    if (fstat(fd, &sb) == -1)
        die(argv[1]);

    kernel_size = sb.st_size;
    kernel = mmap(NULL, kernel_size, PROT_READ, MAP_SHARED, fd, 0);

    if (kernel == MAP_FAILED)
        die(argv[1]);

    /* 'sys_size' must be 16-byte paragraphs aligned, add a 4-byte for CRC. */
    sys_size = (kernel_size + 15 + 4) / 16;

#ifdef CONFIG_EFI_STUB

    /*
     * Here, end of the PE/COFF executable is 16-byte aligned. However, the PE/COFF
     * header in 'header.S' specifies 'FileAlignment' as 32 bytes, and when adding
     * an EFI signature to the file it must first be padded to this alignment.
     *
     * */

    sys_size = align_mask(sys_size, 0x1);
#endif /* CONFIG_EFI_STUB */

    store_le32(&buffer[OFFSET_SYSSIZE], sys_size);

    /*
     * Calculate 'init_size' for 'ukernel.bin' in 'compressed directory'
     *
     * The compressed file consists of a short header and an arbitrary number of
     * compressed blocks. Three different block encoding are currently used in
     * deflate: 'stored', 'static', and 'dynamic'. The smallest block type encoding
     * is always used. 'stored' blocks contains uncompressed data with maximum
     * size of ''32768 - 1'' (minus one is for end-of-block literal).
     *
     * The worst case overhead is 18 bytes for the file header, plus 5 bytes every
     * 32KiB block -- assuming 'stored' block. As a result, to decompress in-place,
     * two cases should be considered:
     *
     *    (1) compressed data 'ZO_z_input_len' is larger than uncompressed data
     *        'ZO_z_output_len' or equal ''OFSSET = 18 + 5 bytes-per-32KiB + 32KiB''.
     *        Last 32KiB is added so after decompression of a block, it does not
     *        overrun the compressed data.
     *
     *    (2) compressed data smaller then uncompressed data
     *        ''OFFSET + uncompressed data - compressed data''
     *
     * See https://www.rfc-editor.org/rfc/rfc1952.txt
     * See https://www.rfc-editor.org/rfc/rfc1951.txt
     *
     * */

#define ZO_z_extra_bytes (18 + (ZO_z_output_len >> 15 << 3) + 32768)
#if ZO_z_output_len > ZO_z_input_len
#define ZO_z_extract_offset (ZO_z_extra_bytes + ZO_z_output_len - ZO_z_input_len)
#else
#define ZO_z_extract_offset ZO_z_extra_bytes
#endif

    /*
     * The boot-loader reserves 'init_size' linear contiguous memory starting at
     * the runtime start address of the 'ukernel.bin'.
     *
     * This buffer is used for in-place decompression of the ukernel. The code
     * running from the 'ukernel.bin''s head under 'ZO__ehead' address moves the
     * 'ukernel.bin', i.e. ''itself'', to the end of this buffer. It copies memory
     * from '_bss' backward to the current instruction; see 'ukernel.lds.S' and
     * in 'head_{32, 64}.S' compress directory.
     *
     * 'init_size' should be large enough so that the copy operation running form
     * the head does not overwrite the head ''itself''.
     *
     * */

#if (ZO__ehead - ZO_startup_32) > ZO_z_extract_offset
#define ZO_z_min_extract_offset (ZO__ehead - ZO_startup_32)
#else
#define ZO_z_min_extract_offset ZO_z_extract_offset
#endif

    /* 'init_size' should be large enough for the final decompressed ukernel. */

#define ZO_INIT_SIZE (ZO__end - ZO_startup_32 + ZO_z_min_extract_offset)
#define VO_INIT_SIZE (VO__end - VO__text)
#if ZO_INIT_SIZE > VO_INIT_SIZE
    init_size = ZO_INIT_SIZE;
#else
    init_size = VO_INIT_SIZE;
#endif

#ifdef CONFIG_EFI_STUB

    /*
     * There are two types of bootable image for 'zukernel':
     *
     *    (1) standard Linux image. 'setup_sectors' is not part of the 'init_size'.
     *        'init_size' defines the static footprint of the 'ukernel.bin' in
     *        compressed directory. 'setup.bin''s memory is allocated separately
     *        by the boot-loader.
     *
     *    (2) PE/COFF executable. 'setup_sectors' is part of the 'init_size'.
     *        '.setup' and '.text' section are loaded by UEFI as part of the
     *        PE/COFF execution.
     *
     * */

    init_size += setup_len;
#endif /* CONFIG_EFI_STUB */

    /* 
     * Here, page align 'init_size' so that when calculating the relocation address
     * in 'head_64.S', the result remains page aligned. It is required for initialising
     * 'ident-mapping' page-table.
     * 
     * */

    init_size = align_mask(init_size, 0xFFF);

    store_le32(&buffer[OFFSET_INIT_SIZE], init_size);
    store_le32(&buffer[OFFSET_KERNEL_INFO_OFFSET], ZO_kernel_info);

    /* ... start of '.text' section is after '.setup' section. */
    update_pecoff_text(setup_len, sys_size * 16, init_size);

    if (update_setup_handover() == -1)
        return -1;

    uint32_t crc = 0xFFFFFFFFUL;

    /*
     * Store 'OUTPUT' ...
     *
     * CRC-32 is created over 'SETUP' and 'KERNEL' files.
     * End of 'OUTPUT' is padded with '\0' after 'kernel_size' and CRC is stored
     * in the last 4 bytes. Therefore, CRC of the 'OUTPUT' up to the limit specified
     * in the 'sys_size' is always 0.
     *
     * */

    FILE *file = fopen(argv[3], "w");

    if (file == NULL)
        die(argv[3]);

    crc = partial_crc32(buffer, setup_len, crc);

    if (fwrite(buffer, 1, setup_len, file) != setup_len)
        die(argv[3]);

    crc = partial_crc32(kernel, kernel_size, crc);

    if (fwrite(kernel, 1, kernel_size, file) != kernel_size)
        die(argv[3]);

    while (kernel_size++ < (sys_size * 16) - 4) {
        crc = partial_crc32_one('\0', crc);

        if (fwrite("\0", 1, 1, file) != 1)
            die(argv[3]);
    }

    fprintf(file, "%u", htole32(crc));

    fclose(file);
    close(fd);                  /* ... 'KERNEL'. */

    return 0;
}
