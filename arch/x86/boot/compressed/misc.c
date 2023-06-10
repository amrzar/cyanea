/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/types.h>
#include <cyanea/errno.h>
#include <cyanea/stddef.h>

#include <asm-generic/unaligned.h>

#include "compressed.h"
#include "string.h"
#include "../boot.h"

struct boot_params *boot_params __aligned(16);

void __noreturn cpu_hlt(void)
{
    while (1) {
        asm volatile ("hlt");
    }
}

static unsigned long HEAP;
static int malloc_count = 0;
static unsigned long malloc_buffer = 0;

void *malloc(size_t size)
{
    void *ptr;

    if (malloc_buffer == 0)
        malloc_buffer = HEAP;

    malloc_buffer = __ALIGN_KERNEL(malloc_buffer, 4);

    ptr = (void *)(malloc_buffer);
    malloc_buffer += size;

    if (malloc_buffer >= HEAP + BOOT_HEAP_SIZE)
        return NULL;

    malloc_count++;
    return ptr;
}

void free(void *ptr)
{
    malloc_count--;

    if (malloc_count == 0)
        malloc_buffer = HEAP;
}

static const u32 crctab32[] = {
    0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190,
    0x6B6B51F4, 0x4DB26158, 0x5005713C, 0xEDB88320, 0xF00F9344,
    0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278,
    0xBDBDF21C
};

static u32 update_crc32(const u8 buffer[], size_t buf_size)
{
    size_t n;
    u32 crc = 0xFFFFFFFF;

    if (buf_size == 0)
        return 0;

    for (n = 0; n < buf_size; n++) {
        crc ^= buffer[n];
        crc = crctab32[crc & 0x0F] ^ (crc >> 4);
        crc = crctab32[crc & 0x0F] ^ (crc >> 4);
    }

    return (crc ^ 0xFFFFFFFF);
}

extern int deflate_buffer(char *, size_t *, const char *, size_t);
static int decompress_gzip(unsigned char *dest, size_t *dest_len,
    const unsigned char *source, size_t source_len)
{
    int ret = 0;

    /* * See https://www.rfc-editor.org/rfc/rfc1952.txt * */

    size_t input_size;
    const unsigned char *start = source + 10;
    const unsigned char *end = source + source_len;

    if ((source_len < 18) ||    /* Size of empty compressed file. */
        (source[0] != 0x1F) ||  /* Identification 1. */
        (source[1] != 0x8B) ||  /* Identification 2. */
        (source[2] != 0x8) ||   /* ''deflate'' compression method. */
        (source[3] & 0xE0))     /* Reserved FLG bits must be zero. */
        return -1;

#define F_TEXT    (1 << 0)
#define F_HCRC    (1 << 1)
#define F_EXTRA   (1 << 2)
#define F_NAME    (1 << 3)
#define F_COMMENT (1 << 4)
    u8 flag = source[3];

    /* ... and skip 'F_EXTRA', 'F_NAME', and 'F_COMMENT'. */

    if (flag & F_EXTRA) {
        start += get_unaligned_le16(start) + 2;

        if (start > end)
            return -1;
    }

    if (flag & F_NAME) {
        do {
            if (start == end)
                return -1;
        } while (*start++);
    }

    if (flag & F_COMMENT) {
        do {
            if (start == end)
                return -1;
        } while (*start++);
    }

    if (flag & F_HCRC) {
        if (start + 2 > end)
            return -1;

        u16 crc16 = update_crc32(source, start - source) & 0xFFFF;

        if (crc16 != get_unaligned_le16(start)) {
            printf("Invalid header CRC for compressed file.\n");
            return -1;
        }

        start += 2;
    }

    if (start + 8 > end)
        return -1;

    input_size = get_unaligned_le32(&source[source_len - 4]);

    if (input_size > *dest_len) {
        printf("Not enough space %zu needs %zu.\n", *dest_len, input_size);
        return -1;
    }

    if ((ret = deflate_buffer(dest, dest_len, start, end - start - 8)) != 0) {
        printf("''deflate_buffer'' failed (ret = %d).\n", ret);
        return -1;
    }

    if ((*dest_len != input_size) ||
        (update_crc32(dest, input_size) !=
            get_unaligned_le32(&source[source_len - 8]))) {
        printf("Invalid CRC for compressed file.\n");
        return -1;
    }

    return SUCCESS;
}

static void parse_elf(void *output)
{
    Elf64_Ehdr ehdr;
    Elf64_Phdr *phdrs, *phdr;

    int i;
    void *dest;

    memcpy(&ehdr, output, sizeof(ehdr));

    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3)
        error("Kernel is not a valid ELF file");

    printf("Parsing ELF ...\n");

    phdrs = malloc(sizeof(*phdr) * ehdr.e_phnum);

    if (phdrs == NULL)
        error("''parse_elf'' failed.\n");

    memcpy(phdrs, output + ehdr.e_phoff, sizeof(*phdr) * ehdr.e_phnum);

    for (i = 0; i < ehdr.e_phnum; i++) {
        switch (phdrs[i].p_type) {
        case PT_LOAD:
            dest = (void *)(phdrs[i].p_paddr);

            /*
             * If 'CONFIG_RELOCATABLE' is set the 'output' can be different from
             * 'CONFIG_PHYSICAL_START'. We move segments with 'PT_LOAD' flag set.
             *
             * The ELF file is linked at physical address 'CONFIG_PHYSICAL_START'.
             * So for the initial segment 'p_paddr' is 'CONFIG_PHYSICAL_START' and,
             * 'memmove' copies the segment to 'p_paddr + output - CONFIG_PHYSICAL_START',
             * i.e. 'output'. There is ELF header at 'output'.
             *
             * */

            dest += ((uintptr_t) (output) - CONFIG_PHYSICAL_START);
            memmove(dest, output + phdrs[i].p_offset, phdrs[i].p_filesz);

            break;

        default:
            break;
        }
    }

    free(phdrs);
}

__attribute__((regparm(0)))
void *decompress_ukernel(void *bp, void *heap,
    unsigned char *input, unsigned long input_len,
    unsigned char *output, unsigned long output_len)
{
    int ret;

    boot_params = bp;
    HEAP = (unsigned long)(heap);

    early_console_init();

#ifdef CONFIG_X86_64
#ifdef CONFIG_RELOCATABLE

    /*
     * For x86_64 if we do not relocate the virtual addresses then we rely on
     * page-table updates in order to keep virtual address unmodified after the
     * boot-loader physical address relocation.
     *
     * This requires mapping '__START_KERNEL_map' to an offset -- rather than
     * the beginning of the physical memory -- which is the relocation offset,
     * i.e. 'output - CONFIG_PHYSICAL_START'. Offset should be positive!
     *
     * */

    if ((unsigned long)(output) < CONFIG_PHYSICAL_START) {
        error("We are loaded bellow CONFIG_PHYSICAL_START (%p:%#x)\n",
            output, CONFIG_PHYSICAL_START);
    }

#endif /* CONFIG_RELOCATABLE */
#endif /* CONFIG_X86_64 */

    printf("Decompressing ukernel ...\n");
    ret = decompress_gzip(output, (size_t *)(&output_len), input, input_len);

    if (ret != SUCCESS)
        error("''decompress_gzip'' failed (ret = %d).\n", ret);

    parse_elf((void *)(output));

    printf("Done.\nBooting the ukernel (output = %p).\n", output);

    return output;
}
