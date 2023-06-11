/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_BOOT_COMPRESSED_ELF_H__
#define __X86_BOOT_COMPRESSED_ELF_H__

#include <cyanea/types.h>

/* * See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format. * */

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

typedef struct elf64_hdr {
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;
# define ET_NONE 0              /* No file type. */
# define ET_REL 1               /* Relocatable file. */
# define ET_EXEC 2              /* Executable file. */
# define ET_DYN 3               /* Shared object file. */
# define ET_CORE 4              /* Core file. */

    Elf64_Half e_machine;
# define EM_NONE 0              /* No machine. */
# define EM_M32 1               /* AT&T WE 32100. */
# define EM_SPARC 2             /* SPARC. */
# define EM_386 3               /* Intel Architecture. */
# define EM_68K 4               /* Motorola 68000. */
# define EM_88K 5               /* Motorola 88000. */
# define EM_486 6               /* Intel 80486. */
# define EM_860 7               /* Intel 80860. */
# define EM_MIPS 8              /* MIPS RS3000 Big-Endian. */
# define EM_ARM 40              /* ARM 32-bit. */
# define EM_X86_64 62           /* AMD x86-64. */
# define EM_AARCH64 83          /* ARM 64-bit. */

    Elf64_Word e_version;
# define EV_NONE 0
# define EV_CURRENT 1

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

#define ELFMAG0 0x7F            /* ''Ehdr.e_ident[ELF_MAG0]''. */
#define ELFMAG1 'E'             /* ''Ehdr.e_ident[ELF_MAG1]''. */
#define ELFMAG2 'L'             /* ''Ehdr.e_ident[ELF_MAG2]''. */
#define ELFMAG3 'F'             /* ''Ehdr.e_ident[ELF_MAG3]''. */

/* ''Ehdr.e_ident[EI_CLASS]''. */
#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

/* ''Ehdr.e_ident[EI_DATA]''. */
#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

/* * Program Header. * */

typedef struct elf64_phdr {
    Elf64_Word p_type;
# define PT_NULL 0
# define PT_LOAD 1
# define PT_DYNAMIC 2
# define PT_INTERP 3
# define PT_NOTE 4
# define PT_SHLIB 5
# define PT_PHDR 6

    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

#endif /* __X86_BOOT_COMPRESSED_ELF_H__ */
