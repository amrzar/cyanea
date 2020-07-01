/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/types.h>
#include <cyanea/errno.h>
#include <cyanea/stddef.h>

#include "compressed.h"

static unsigned long relocs_count, relocs_next;
static uintptr_t *relocs;

#define ElfW(type) _ElfW(ELF_BITS, type)
#define _ElfW(bits, type) __ElfW(bits, type)
#define __ElfW(bits, type) Elf##bits##_##type

#ifdef CONFIG_X86_32
#define ELF_BITS 32
#define SHT_REL_TYPE SHT_REL
#define ELF_R_SYM ELF32_R_SYM
#define Elf_Rel Elf32_Rel

static int __do_reloc_32(Elf32_Rel * rel, Elf32_Sym * sym,
    int (*process)(Elf32_Rel * rel, Elf32_Sym * sym))
{
    int ret = SUCCESS;
    unsigned int r_type = ELF32_R_TYPE(rel->r_info);

    if (sym->st_shndx == SHN_ABS)
        return SUCCESS;

    switch (r_type) {
    case R_386_NONE:
    case R_386_PC32:

        break;

    case R_386_32:
        process(rel, sym);
        break;

    default:
        error("Unsupported relocation type: %u\n", r_type);
    }

    return SUCCESS;
}

#define do_reloc __do_reloc_32
#else
#define ELF_BITS 64
#define SHT_REL_TYPE SHT_RELA
#define ELF_R_SYM ELF64_R_SYM
#define Elf_Rel Elf64_Rela

static int __do_reloc_64(Elf64_Rel * rel, Elf64_Sym * sym,
    int (*process)(Elf64_Rela * rel, Elf64_Sym * sym))
{
    assert(!"''__do_reloc_64'' not implemented!\n");
}

#define do_reloc __do_reloc_64
#endif /* CONFIG_X86_32 */

#define Elf_Ehdr ElfW(Ehdr)
#define Elf_Shdr ElfW(Shdr)
#define Elf_Sym  ElfW(Sym)

static int count_reloc(Elf_Rel * rel, Elf_Sym * sym)
{
    relocs_count++;

    return SUCCESS;
}

static int insert_reloc(Elf_Rel * rel, Elf_Sym * sym)
{
    relocs[relocs_next++] = rel->r_offset;

    return SUCCESS;
}

/* **/

static inline Elf_Shdr *get_shdr(Elf_Ehdr * ehdr)
{
    return (Elf_Shdr *) ((uintptr_t) (ehdr) + ehdr->e_shoff);
}

static inline uintptr_t get_section(Elf_Ehdr * ehdr, int i)
{
    return (uintptr_t) (ehdr) + get_shdr(ehdr)[i].sh_offset;
}

static void walk_relocs(Elf_Ehdr * ehdr,
    int (*process)(Elf_Rel * rel, Elf_Sym * sym))
{
    int i, n;
    unsigned int r_symtab, r_applies;
    Elf_Shdr *shdrs = get_shdr(ehdr);

    for (i = 0; i < ehdr->e_shnum; i++) {
        if (shdrs[i].sh_type != SHT_REL_TYPE)
            continue;

        r_symtab = shdrs[i].sh_link;
        r_applies = shdrs[i].sh_info;

        if (!(shdrs[r_applies].sh_flags & SHF_ALLOC))
            continue;

        for (n = 0; n < shdrs[i].sh_size / shdrs[i].sh_entsize; n++) {
            Elf_Rel *rel = &((Elf_Rel *) (get_section(ehdr, i)))[n];
            Elf_Sym *sym = &((Elf_Sym *) (get_section(ehdr, r_symtab)))
                [ELF_R_SYM(rel->r_info)];
            do_reloc(rel, sym, process);
        }
    }
}

void emit_relocs(void *output)
{
    relocs_count = 0;
    walk_relocs((Elf_Ehdr *) (output), count_reloc);

    if (relocs_count != 0) {
        if ((relocs = malloc(relocs_count * sizeof(relocs[0]))) == NULL)
            error("''emit_relocs''failed.\n");

        relocs_next = 0;
        walk_relocs((Elf_Ehdr *) (output), insert_reloc);
    }
}

void handle_relocations(void *output, unsigned long output_len)
{

}
