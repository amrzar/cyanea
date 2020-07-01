/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_ELF_H__
#define __CYANEA_ELF_H__

#include <asm/elf.h>
#include <cyanea/types.h>

/* * See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format. * */

typedef u32 Elf32_Addr;
typedef u16 Elf32_Half;
typedef u32 Elf32_Off;
typedef s32 Elf32_Sword;
typedef u32 Elf32_Word;

typedef u64 Elf64_Addr;
typedef u16 Elf64_Half;
typedef s16 Elf64_SHalf;
typedef u64 Elf64_Off;
typedef s32 Elf64_Sword;
typedef u32 Elf64_Word;
typedef u64 Elf64_Xword;
typedef s64 Elf64_Sxword;

/* * ELF Header. * */

#define EI_NIDENT 16

typedef struct elf32_hdr {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct elf64_hdr {
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_PAD 7
#define EI_NIDENT 16

#define ELFMAG0 0x7F            /* ''Ehdr.e_ident[ELFMAG0]''. */
#define ELFMAG1 'E'             /* ''Ehdr.e_ident[ELFMAG1]''. */
#define ELFMAG2 'L'             /* ''Ehdr.e_ident[ELFMAG2]''. */
#define ELFMAG3 'F'             /* ''Ehdr.e_ident[ELFMAG3]''. */

/* ''Ehdr.e_ident[EI_CLASS]''. */
#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

/* ''Ehdr.e_ident[EI_DATA]''. */
#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

/* ''Ehdr.e_type''. */
#define ET_NONE 0               /* No file type. */
#define ET_REL 1                /* Relocatable file. */
#define ET_EXEC 2               /* Executable file. */
#define ET_DYN 3                /* Shared object file. */
#define ET_CORE 4               /* Core file. */

/* ''Ehdr.e_machine''. */
#define EM_NONE 0               /* No machine. */
#define EM_M32 1                /* AT&T WE 32100. */
#define EM_SPARC 2              /* SPARC. */
#define EM_386 3                /* Intel Architecture. */
#define EM_68K 4                /* Motorola 68000. */
#define EM_88K 5                /* Motorola 88000. */
#define EM_486 6                /* Intel 80486. */
#define EM_860 7                /* Intel 80860. */
#define EM_MIPS 8               /* MIPS RS3000 Big-Endian. */
#define EM_ARM 40               /* ARM 32-bit. */
#define EM_X86_64 62            /* AMD x86-64. */
#define EM_AARCH64 83           /* ARM 64-bit. */

/* ''Ehdr.e_version''. */
#define EV_NONE 0
#define EV_CURRENT 1

/* * Section Header. * */

#define SHN_UNDEF 0
#define SHN_ABS 0xFFF1
#define SHN_COMMON 0xFFF2

typedef struct elf32_shdr {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct elf64_shdr {
    Elf64_Word sh_name;
    Elf64_Word sh_type;
    Elf64_Xword sh_flags;
    Elf64_Addr sh_addr;
    Elf64_Off sh_offset;
    Elf64_Xword sh_size;
    Elf64_Word sh_link;
    Elf64_Word sh_info;
    Elf64_Xword sh_addralign;
    Elf64_Xword sh_entsize;
} Elf64_Shdr;

/* ''Shdr.sh_type''. */
#define SHT_NULL 0              /* Inactive section. */
#define SHT_PROGBITS 1          /* Section holds information defined by the program. */
#define SHT_SYMTAB 2            /* Symbol table. 'SHT_SYMTAB' provides symbols for link
                                 * editing, though it may also be used for dynamic
                                 * linking. As a complete symbol table, it may contain
                                 * many symbols unnecessary for dynamic linking. An
                                 * object file can also contain a 'SHT_DYNSYM' section. */
#define SHT_STRTAB 3            /* String table. */
#define SHT_RELA 4              /* Relocation entries with explicit addends. */
#define SHT_HASH 5              /* Symbol hash table. */
#define SHT_DYNAMIC 6           /* Information for dynamic linking. */
#define SHT_NOTE 7              /* Information that marks the file in some way. */
#define SHT_NOBITS 8            /* A section of this type occupies no space in the file
                                 * but otherwise resembles 'SHT_PROGBITS'. */
#define SHT_REL 9               /* Relocation entries without explicit addends. */
#define SHT_SHLIB 10            /* Reserved but has unspecified semantics. */
#define SHT_DYNSYM 11           /* Minimal set of dynamic linking symbols. */

/* ''Shdr.sh_flags''. */
#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKPROC 0xF0000000

/* ... ''SHT_SYMTAB''. */
#define STN_UNDEF 0

typedef struct elf32_sym {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct elf64_sym {
    Elf64_Word st_name;
    unsigned char st_info;
    unsigned char st_other;
    Elf64_Half st_shndx;
    Elf64_Addr st_value;
    Elf64_Xword st_size;
} Elf64_Sym;

/* ''Sym.st_info'' specifies the symbol's type and binding attributes. */

#define STB_LOCAL 0             /* Symbols are not visible outside the object file. */
#define STB_GLOBAL 1            /* Symbols are visible to all object files. */
#define STB_WEAK 2              /* Weak symbols resemble global symbols, with lower precedence. */

#define STT_NOTYPE 0            /* Symbol's type is not defined. */
#define STT_OBJECT 1            /* Symbol is associated with a data object. */
#define STT_FUNC 2              /* Symbol is associated with a function. */
#define STT_SECTION 3           /* Symbol is associated with a section. */
#define STT_FILE 4              /* The symbol's name gives the name of the source file
                                 * associated with the object file. */

#define ELF_ST_BIND(x) ((x) >> 4)
#define ELF_ST_TYPE(x) ((x) & 0xf)
#define ELF32_ST_BIND(x) ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x) ELF_ST_TYPE(x)
#define ELF64_ST_BIND(x) ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x) ELF_ST_TYPE(x)

/* ... ''SHT_RELA'' or ''SHT_REL''.  */
typedef struct elf32_rela {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
    Elf32_Sword r_addend;
} Elf32_Rela;

typedef struct elf64_rela {
    Elf64_Addr r_offset;
    Elf64_Xword r_info;
    Elf64_Sxword r_addend;
} Elf64_Rela;

typedef struct elf32_rel {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct elf64_rel {
    Elf64_Addr r_offset;
    Elf64_Xword r_info;
} Elf64_Rel;

#define ELF32_R_SYM(x)  ((x) >> 8)
#define ELF32_R_TYPE(x) ((x) & 0xFF)
#define ELF64_R_SYM(x)  ((x) >> 32)
#define ELF64_R_TYPE(x) ((x) & 0xFFFFFFFF)

/* * Program Header. * */
typedef struct elf32_phdr {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

typedef struct elf64_phdr {
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

/* ''Phdr.p_type''. */
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6

#endif /* __CYANEA_ELF_H__ */
