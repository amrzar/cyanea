/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_DESC_DEFS_H__
#define __X86_ASM_DESC_DEFS_H__

#include <cyanea/const.h>
#include <cyanea/types.h>
#include <cyanea/compiler.h>

#define GDT_ENTRY(attribute, base, limit) \
    ((((base) & ULL(0xFF000000)) << (56 - 24)) | \
        (((attribute) & ULL(0x0000F0FF)) << 40) | \
        (((limit) & ULL(0x000F0000)) << (48 - 16)) | \
        (((base) & ULL(0x00FFFFFF)) << 16) | \
        (((limit) & ULL(0x0000FFFF))))

#ifndef __ASSEMBLY__

union desc_struct {
    struct {
        u16 limit0;
        u16 base0;
        u16 base1:8, type:4, s:1, dpl:2, p:1;
        u16 limit1:4, avl:1, l:1, d:1, g:1, base2:8;
    };

    u64 __raw_desc;
} __packed;

typedef union desc_struct desc_struct_t;

#define GDT_ENTRY_INIT(a, b, limit) \
    { .__raw_desc = GDT_ENTRY(a, b, limit) }

/** ''SYSTEM DESCRIPTOR TYPES'' **/

struct ldttss_desc {
    u16 limit0;
    u16 base0;
    u16 base1:8, type:5, dpl:2, p:1;
    u16 limit1:4, zero0:3, g:1, base2:8;
    u32 base3;
    u32 zero1;
} __packed;

typedef struct ldttss_desc ldt_desc_t;
typedef struct ldttss_desc tss_desc_t;

enum {
    DESC_LDT = 0x2,             /* Local descriptor-table (LDT) segment. */
    DESC_TSS = 0x9,             /* Task-state segment (TSS) (Available). */
};

struct gate_desc {
    u16 offset_low;
    u16 segment;
    u16 ist:3, zero:5, type:5, dpl:2, p:1;
    u16 offset_middle;
    u32 offset_high;
    u32 reserved;
} __packed;

typedef struct gate_desc gate_desc_t;

enum {
    GATE_TASK = 0x5,            /* Task Gate -- only 32-bit. */
    GATE_CALL = 0xC,            /* Call Gate. */
    GATE_INTERRUPT = 0xE,       /* Interrupt Gate. */
    GATE_TRAP = 0xF,            /* Trap Gate. */
};

struct gdt_ptr {
    unsigned short size;
    unsigned long base_address; /* 8-byte aligned. */
} __packed;

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_DESC_DEFS_H__ */
