/* SPDX-License-Identifier: GPL-2.0-or-later */

/* [This code is position independent]. */

#include <cyanea/types.h>
#include <cyanea/errno.h>
#include <cyanea/stddef.h>
#include <asm-generic/unaligned.h>

#define MAX_BITS 16
#define MAX_SYMBOLS 288
#define MAX_DISTANCE 32

typedef u16 symbol_t;

/* The Huffman codes used for each alphabet in the "deflate" format have two
 * additional rules:
 *
 *   - All codes of a given bit length have lexicographically consecutive values,
 *     in the same order as the symbols they represent;
 *   - Shorter codes lexicographically precede longer codes.
 *
 * Given these rules, we can define the Huffman tree for an alphabet just by giving
 * the bit lengths of the codes for each symbol of the alphabet in order, i.e.
 * lengths in huffman_tree.
 *
 *   See https://www.rfc-editor.org/rfc/rfc1951.txt
 *
 * The code represents a Huffman tree using 'tree_t' where 'bl_count' counts the
 * number of symbols for each code length and 'sorted_symbols' is sorted symbols
 * based on code length.
 */

typedef struct tree {
    unsigned short bl_count[MAX_BITS];
    symbol_t sorted_symbols[MAX_SYMBOLS];
} *tree_t;

typedef struct deflate {
    const unsigned char *in_ptr, *in_end;
    unsigned char *dest;

#define ERR_NONE 0
#define ERR_OVERFLOW 1
#define ERR_SYMBOL 2
#define ERR_UNDEFINED 3
    int error;

    /* 'bit_accum' accumulates minimum bits to satisfy the next request.
     * 'u32' because the maximum request size is not larger than 'MAX_BITS' and maximum
     * of 23 bits ''15 + 8'' is enough to satisfy this request.
     */

    u32 bit_accum;
    size_t nr_bits, dest_size;

    struct tree ll, distance;
} *deflate_t;

static void huffman_tree(tree_t tree, const unsigned char lengths[], int nr_sym)
{
    int it;
    unsigned short offs[MAX_BITS], sum = 0;

    for (it = 0; it < MAX_BITS; it++)
        tree->bl_count[it] = 0;

    for (it = 0; it < nr_sym; it++)
        tree->bl_count[lengths[it]]++;

    tree->bl_count[0] = 0;

    for (it = 0; it < MAX_BITS; it++) {
        offs[it] = sum;
        sum += tree->bl_count[it];
    }

    for (it = 0; it < nr_sym; it++) {
        if (lengths[it] != 0)
            tree->sorted_symbols[offs[lengths[it]]++] = it;
    }
}

static u32 get_bits(deflate_t d, size_t nr_bits)
{
    u32 bits;

    if (d->error != ERR_NONE)
        return SUCCESS;

    while (d->nr_bits < nr_bits) {
        if (d->in_ptr == d->in_end) {
            d->error = ERR_OVERFLOW;

            return SUCCESS;
        }

        d->bit_accum |= ((u32)(*d->in_ptr++) << d->nr_bits);
        d->nr_bits += 8;
    }

    bits = d->bit_accum & ((1 << nr_bits) - 1);
    d->bit_accum >>= nr_bits;
    d->nr_bits -= nr_bits;

    return bits;
}

static u32 get_extra_bits(deflate_t d, size_t nr_bits, u32 base)
{
    u32 bits = nr_bits ? get_bits(d, nr_bits) : 0;

    return base + bits;
}

static symbol_t decode_symbol(deflate_t d, tree_t tree)
{
    int it;
    unsigned short sum = 0, offs = 0;

    /* In Huffman tree, each level adds one bit to the code length, i.e.
     * leaves on the first level have code length of single bit, leaves
     * on the second level have code length of two bits, etc.
     *
     * Following the rules for Huffman code in "deflate" format (see above)
     * at each level 'i', there are 'bl_count[i]' leaves on the left and
     * subtrees on the right for the greater code length, i.e. 'i + 1'.
     *
     * So, to convert the code to symbol, we go down the tree, one level
     * at a time looking only at the leaves.
     */

    for (it = 1; it < MAX_BITS; it++) {
        offs = 2 * offs + get_bits(d, 1);

        if (d->error != ERR_NONE)
            return SUCCESS;

        if (offs < tree->bl_count[it]) {

            /* Here, 'offs' is an index to a code with length 'it', and
             * 'sum' is total number of codes with length less than 'it',
             * 'sorted_symbols[sum + offs]' is the current symbol.
             *
             * we assume 'sum + offs' is in range, i.e. it is less than
             * the index in 'sorted_symbols' of last symbols that participate
             * in Huffman code generation.
             */

            return tree->sorted_symbols[sum + offs];
        }

        sum += tree->bl_count[it];
        offs -= tree->bl_count[it];
    }

    d->error = ERR_SYMBOL;
    return SUCCESS;
}

static void huffman_fixed_tree(tree_t lt, tree_t dt)
{
    int it;

    for (it = 0; it < MAX_BITS; it++)
        lt->bl_count[it] = 0;

    lt->bl_count[7] = 24;
    lt->bl_count[8] = 152;
    lt->bl_count[9] = 112;

    for (it = 0; it < 24; it++)
        lt->sorted_symbols[it] = 256 + it;

    for (it = 0; it < 144; it++)
        lt->sorted_symbols[24 + it] = it;

    for (it = 0; it < 8; it++)
        lt->sorted_symbols[24 + 144 + it] = 280 + it;

    for (it = 0; it < 112; it++)
        lt->sorted_symbols[24 + 144 + 8 + it] = 144 + it;

    for (it = 0; it < MAX_BITS; it++)
        dt->bl_count[it] = 0;

    dt->bl_count[5] = 32;

    for (it = 0; it < 32; it++)
        dt->sorted_symbols[it] = it;
}

static int huffman_dynamic_tree(deflate_t d, tree_t lt, tree_t dt)
{
    int it;

    static unsigned char __code_len_for_code_idx[19] = {
        16, 17, 18,  0,  8,  7,  9,  6, 10,  5,
        11,  4, 12,  3, 13,  2, 14,  1, 15,
    };

    unsigned char *code_len_for_code_idx = REL_REF(__code_len_for_code_idx);

    unsigned char lengths[MAX_SYMBOLS + MAX_DISTANCE] = { 0 };

    int hlit = get_extra_bits(d, 5, 257);       /* 5 bits for 'HLIT' (257 .. 286) */
    int hdist = get_extra_bits(d, 5, 1);        /* 5 bits for 'HDIST' (1 .. 32)   */
    int hclen = get_extra_bits(d, 4, 4);        /* 4 bits for 'HCLEN' (4 .. 19)   */

    if (d->error != ERR_NONE)
        goto out;

    for (it = 0; it < hclen; it++)
        lengths[code_len_for_code_idx[it]] = get_bits(d, 3);

    if (d->error != ERR_NONE)
        goto out;

    huffman_tree(lt, lengths, 19);

    it = 0;

    /* The Huffman codes for the two alphabets appear in the block immediately
     * after the header bits and before the actual compressed data, first the
     * ''literal/length code'' and then the ''distance code''.
     * ... retrieve and build appropriate trees.
     */

    while (it < hlit + hdist) {
#define SYM_CL_COPY_3_6 16
#define SYM_CL_REPEAT_3_10 17
#define SYN_CL_REPEAT_11_138 18

        int length;
        symbol_t symbol = decode_symbol(d, lt);

        if (d->error != ERR_NONE)
            goto out;

        /* Copy previous code length ''3 .. 6'' times. */
        if (symbol == SYM_CL_COPY_3_6) {
            if (it == 0) {
                d->error = ERR_SYMBOL;
                goto out;
            }

            symbol = lengths[it - 1];
            length = get_extra_bits(d, 2, 3);   /* Read 2 bits. */

            /* Repeat code length 0 for ''3 .. 10'' times. */
        } else if (symbol == SYM_CL_REPEAT_3_10) {
            symbol = 0;
            length = get_extra_bits(d, 3, 3);   /* Read 3 bits. */

            /* Repeat code length 0 for ''11 .. 138'' times. */
        } else if (symbol == SYN_CL_REPEAT_11_138) {
            symbol = 0;
            length = get_extra_bits(d, 7, 11);  /* Read 7 bits. */

        } else {
            length = 1;
        }

        if (d->error != ERR_NONE)
            goto out;

        while (length--) {
            lengths[it++] = symbol;
        }
    }

    huffman_tree(lt, lengths, hlit);
    huffman_tree(dt, lengths + hlit, hdist);

out:
    return SUCCESS;
}

static int deflate_block(deflate_t d)
{
    unsigned char *t_dest = d->dest;

    static unsigned char __length_bits[30] = {
        0,   0,   0,   0,   0,   0,   0,   0,   1,   1,
        1,   1,   2,   2,   2,   2,   3,   3,   3,   3,
        4,   4,   4,   4,   5,   5,   5,   5,   0, 127,
    };

    static unsigned short __length_base[30] = {
        3,   4,   5,   6,   7,   8,   9,  10,  11,  13,
        15,  17,  19,  23,  27,  31,  35,  43,  51,  59,
        67,  83,  99, 115, 131, 163, 195, 227, 258,   0,
    };

    static unsigned char __dist_bits[30] = {
        0,   0,   0,   0,   1,   1,   2,   2,   3,   3,
        4,   4,   5,   5,   6,   6,   7,   7,   8,   8,
        9,   9,  10,  10,  11,  11,  12,  12,  13,  13,
    };

    static unsigned short __dist_base[30] = {
        1,   2,   3,   4,   5,   7,   9,  13,  17,  25,
        33, 49,  65,  97, 129, 193, 257, 385, 513, 769,
        1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289,
        16385, 24577
    };

    unsigned char *length_bits = REL_REF(__length_bits);
    unsigned short *length_base = REL_REF(__length_base);
    unsigned char *dist_bits = REL_REF(__dist_bits);
    unsigned short *dist_base = REL_REF(__dist_base);

    while (1) {
#define SYM_LL_EOB 256
#define SYM_LL_LENGTH 257

        symbol_t symbol = decode_symbol(d, &d->ll);

        if (d->error != ERR_NONE)
            goto out;

        if (symbol == SYM_LL_EOB) {
            d->dest_size += d->dest - t_dest;
            goto out;
        }

        /* ... (0 .. 255): Represent literal bytes. */
        if (symbol < SYM_LL_EOB) {
            *d->dest++ = symbol;

        } else {
            symbol_t distance;
            int it, length, offset;

            /* Read ''[ length, backward offset ]'' pairs. */

            length = get_extra_bits(d, length_bits[symbol - SYM_LL_LENGTH],
                    length_base[symbol - SYM_LL_LENGTH]);

            distance = decode_symbol(d, &d->distance);
            offset =
                get_extra_bits(d, dist_bits[distance], dist_base[distance]);

            if (d->error != ERR_NONE)
                goto out;

            for (it = 0; it < length; it++)
                d->dest[it] = d->dest[it - offset];

            d->dest += length;
        }
    }

out:
    return SUCCESS;
}

static int deflate_uncompressed_block(deflate_t d)
{
    int it;

    if (d->in_end - d->in_ptr < 4) {
        d->error = ERR_OVERFLOW;
        goto out;
    }

    u16 length = get_unaligned_le16(d->in_ptr);

    /* Skip 'LEN' and 'NLEN'. */
    d->in_ptr += 4;

    if (d->in_end - d->in_ptr < length) {
        d->error = ERR_OVERFLOW;
        goto out;
    }

    for (it = length; it > 0; it--)
        *d->dest++ = *d->in_ptr++;

    d->dest_size += length;
    d->bit_accum = 0;
    d->nr_bits = 0;

out:
    return SUCCESS;
}

static int deflate_buffer(unsigned char *dest, size_t *dest_len, const unsigned char *source,
    size_t source_len)
{
    struct deflate d;

    d.in_ptr = source;
    d.in_end = d.in_ptr + source_len;
    d.dest = dest;
    d.dest_size = 0;
    d.bit_accum = 0;
    d.nr_bits = 0;
    d.error = ERR_NONE;

    unsigned char block_type, block_final;

    do {
        block_final = get_bits(&d, 1);
        block_type = get_bits(&d, 2);

        if (d.error != ERR_NONE)
            break;

#define BTYPE_NO_COMPRESSION 0
#define BTYPE_COMPRESSED_FIXED 1
#define BTYPE_COMPRESSED_DYNAMIC 2

        if (block_type == BTYPE_NO_COMPRESSION) {
            deflate_uncompressed_block(&d);

        } else if (block_type == BTYPE_COMPRESSED_FIXED) {
            huffman_fixed_tree(&d.ll, &d.distance);
            deflate_block(&d);

        } else if (block_type == BTYPE_COMPRESSED_DYNAMIC) {
            huffman_dynamic_tree(&d, &d.ll, &d.distance);
            if (d.error == ERR_NONE)
                deflate_block(&d);

        } else {
            d.error = ERR_UNDEFINED;
        }

    } while ((block_final == 0) && (d.error == ERR_NONE));

    if (d.error == ERR_NONE)
        *dest_len = d.dest_size;

    return d.error;
}

/* ''GZIP''. */

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
