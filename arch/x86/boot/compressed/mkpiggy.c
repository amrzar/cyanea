/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdio.h>
#include <inttypes.h>
#include <endian.h>

static void print_help(char *app_name)
{
    fprintf(stderr, "Usage: %s compressed_ukernel\n", app_name);
    fprintf(stderr,
        "compressed_ukernel should be a ''GZIP'' compressed file.\n");
}

int main(int argc, char **argv)
{
    FILE *file;
    int retval = 1;

    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    if ((file = fopen(argv[1], "r")) == NULL) {
        perror(argv[1]);
        goto out;
    }

    uint8_t ID[2];

    if (fread(ID, sizeof(uint8_t), 2, file) != 2) {
        perror(argv[1]);
        goto out;
    }

    if (ID[0] != 0x1F || ID[1] != 0x8B) {
        fprintf(stderr, "[%s] is not a ''GZIP'' compressed file\n", argv[0]);
        goto out;
    }

    uint32_t orig_len;

    /* For ''GZIP'' file the size of original (uncompressed) 'orig_len' data stored
     * as little-endian value at ''ISIZE'' field.
     *
     * See https://www.rfc-editor.org/rfc/rfc1952.txt
     */

    if ((fseek(file, -4L, SEEK_END) != 0) ||
        (fread(&orig_len, sizeof(uint32_t), 1, file) != 1)) {
        perror(argv[1]);
        goto out;
    }

    long compressed_len = ftell(file);
    orig_len = le32toh(orig_len);

    printf(".section \".rodata..compressed\",\"a\",@progbits\n");
    printf(".global z_input_len\n");
    printf("z_input_len = %lu\n", compressed_len);
    printf(".global z_output_len\n");
    printf("z_output_len = %lu\n", (unsigned long)(orig_len));

    printf(".global input_data, input_data_end\n");
    printf("input_data:\n");
    printf(".incbin \"%s\"\n", argv[1]);
    printf("input_data_end:\n");

    /* The 'head_{32,64}.S' in compressed directory needs size of original and
     * compressed files. However, using 'z_input_len' and 'z_output_len' directy
     * in the assembly may cause the linker -- BFD linker -- to generate runtime
     * relocation entry in '.rel.dyn', even though they are absolute symbols.
     *
     * Instead, use 'input_len' and 'output_len' variables in '.rodata' section.
     * They are accessed using 'symbole@GOTOFF' as position-independent references
     * to eliminate any runtime relocations.
     *
     * See 'head_{32,64}.S' in compressed directory.
     */

    printf(".section \".rodata\",\"a\",@progbits\n");
    printf(".global input_len\n");
    printf("input_len:\n\t.long %lu\n", compressed_len);
    printf(".global output_len\n");
    printf("output_len:\n\t.long %lu\n", (unsigned long)(orig_len));

    retval = 0;

out:

    if (file != NULL) {
        fclose(file);
    }

    return retval;
}
