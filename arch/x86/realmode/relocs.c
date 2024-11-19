/* SPDX-License-Identifier: GPL-2.0-or-later */

/* THIS FILE HAS BEEN DERIVED FROM LINUX 'arch/x86/tools/relocs.c'. */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <regex.h>
#include <errno.h>

#include <elf.h>

#define RELOCS_MAX 100

static void die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	exit(1);
}

enum symtype {
	S_ABS,
	S_REL,
	S_SEG,
	S_LIN,
	S_NSYMTYPES
};

static const char *const sym_regex[S_NSYMTYPES] = {

	/* These symbols are known to be relative, even if the linker marks them
	 * as absolute (typically defined outside any section in the linker script).
	 */

	[S_REL] = "^pa_",

	/* These are 16-bit segment symbols when compiling 16-bit code. */

	[S_SEG] = "^real_mode_seg$",

	/* These are offsets belonging to segments when compiling 16-bit code. */

	[S_LIN] = "^pa_",
};

static regex_t sym_regex_c[S_NSYMTYPES];
static int is_reloc(enum symtype type, const char *sym_name)
{
	return sym_regex[type] &&
	        !regexec(&sym_regex_c[type], sym_name, 0, NULL, 0);
}

static void regex_init(void)
{
	char errbuf[128];
	int i, err;

	for (i = 0; i < S_NSYMTYPES; i++) {
		if (!sym_regex[i])
			continue;

		err = regcomp(&sym_regex_c[i], sym_regex[i], REG_EXTENDED | REG_NOSUB);
		if (err) {
			regerror(err, &sym_regex_c[i], errbuf, sizeof(errbuf));

			die("%s\n", errbuf);
		}
	}
}

struct relocs {
	Elf32_Addr r_offset[RELOCS_MAX];
	unsigned int count;
};

static struct relocs relocs16 = { 0 };
static struct relocs relocs32 = { 0 };

static void add_reloc(struct relocs *r, Elf32_Addr r_offset)
{
	if (r->count == RELOCS_MAX)
		die("'add_reloc' failed.\n");

	r->r_offset[r->count++] = r_offset;
}

/* Fill relocation tables. */

static void do_reloc(Elf32_Rel *rel, Elf32_Sym *sym, const char *symname)
{
	unsigned r_type = ELF32_R_TYPE(rel->r_info);

	int shn_abs = (sym->st_shndx == SHN_ABS) && !is_reloc(S_REL, symname);

	switch (r_type) {
	case R_386_NONE:

	case R_386_PC32:

		/* PC relative relocation; probably in 64-bit text section. */

		break;
	case R_386_16:
		if (shn_abs) {

			if (is_reloc(S_ABS, symname))
				break;

			if (is_reloc(S_SEG, symname)) {
				add_reloc(&relocs16, rel->r_offset);

				break;
			}

		} else {

			/* We should not have relocation entry for offset in 16-bit code. */

			if (!is_reloc(S_LIN, symname))
				break;
		}

		die("Invalid %s %d relocation: %s\n",
		        shn_abs ? "absolute" : "relative", r_type, symname);

		break;
	case R_386_32:
		if (shn_abs) {

			if (is_reloc(S_ABS, symname))
				break;

		} else {
			if (is_reloc(S_REL, symname))
				add_reloc(&relocs32, rel->r_offset);

			break;
		}

		die("Invalid %s %d relocation: %s\n",
		        shn_abs ? "absolute" : "relative", r_type, symname);

		break;
	default:

		die("Unsupported relocation type: %d.\n", r_type);
	}
}

# define get_section_hdrs(e) ((Elf32_Shdr *) ((uintptr_t) (e) + (e)->e_shoff))
# define get_section(e, i) ((void *) ((uintptr_t) (e) + get_section_hdrs(e)[i].sh_offset))

static const char *sym_name(const char *sym_strtab, Elf32_Sym *sym)
{
	return (sym->st_name) ? (sym_strtab + sym->st_name) : "<no-name>";
}

static void walk_relocs(Elf32_Ehdr *ehdr,
        void process(Elf32_Rel *, Elf32_Sym *, const char *))
{
	int i, j;

	Elf32_Shdr *shdrs = get_section_hdrs(ehdr);

	for (i = 0; i < ehdr->e_shnum; i++) {

		if (shdrs[i].sh_type != SHT_REL)
			continue;

		/* 'shdrs[i].sh_info' is the section index to apply the relocation. */

		if (!(shdrs[shdrs[i].sh_info].sh_flags & SHF_ALLOC))
			continue;

		Elf32_Rel *reltab = get_section(ehdr, i);

		/* 'shdrs[i].sh_link' is section index of the associated symbol table. */

		Elf32_Sym *symtab = get_section(ehdr, shdrs[i].sh_link);

		/* 'shdrs[shdrs[i].sh_link].sh_link' is section index of the associated string table. */

		const char *sym_strtab =
		        get_section(ehdr, shdrs[shdrs[i].sh_link].sh_link);

		for (j = 0; j < shdrs[i].sh_size / shdrs[i].sh_entsize; j++) {

			Elf32_Rel *rel = &reltab[j];
			Elf32_Sym *sym = &symtab[ELF32_R_SYM(rel->r_info)];

			process(rel, sym, sym_name(sym_strtab, sym));
		}
	}
}

int main(int argc, char **argv)
{
	int i, fd;

	struct stat s;
	const char *file;

	if (argc != 2)
		die("Usage: %s KERNEL\n"
		        "  KERNEL is 'ukernel.elf' real-mode kernel.\n", argv[0]);

	regex_init();

	if ((fd = open(argv[1], O_RDONLY)) == -1) {
		perror(argv[1]);

		return -1;
	}

	if (fstat(fd, &s)) {
		perror(argv[1]);

		return -1;
	}

	file = mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (file == MAP_FAILED) {
		perror(argv[1]);

		return -1;
	}

	Elf32_Ehdr *ehdr = (Elf32_Ehdr *) file;

	if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0)
		die("%s is not a valid ELF file.\n", argv[1]);

	/* Populate 'relocs16' and 'relocs32'. */

	walk_relocs(ehdr, do_reloc);

	/* Generate the relocation information. */

	printf("\t.long 0x%08" PRIx32 "\t# 'relocs16.count'.\n", relocs16.count);
	for (i = 0; i < relocs16.count; i++)
		printf("\t.long 0x%08" PRIx32 "\n", relocs16.r_offset[i]);

	printf("\t.long 0x%08" PRIx32 "\t# 'relocs32.count'.\n", relocs32.count);
	for (i = 0; i < relocs32.count; i++)
		printf("\t.long 0x%08" PRIx32 "\n", relocs32.r_offset[i]);

	return 0;
}
