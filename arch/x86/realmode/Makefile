# SPDX-License-Identifier: GPL-2.0-or-later

relocs: relocs.c
	@$(call cmd-info, HOSTCC, $<)
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

rm-piggy.S: relocs kernel/
	@./relocs kernel/kernel.elf > kernel.reloc

CLEAN_FILES += relocs
CLEAN_FILES += kernel.reloc

obj-y += init.o
obj-y += rm-piggy.o

# Build the real-mode kernel (.elf and .bin).

kernel/: FORCE
	@printf '%-8s%s\n' DESCEND $@
	$(call descend-make, $@)

define clean-all
	$(call descend-make, kernel rm-files)
endef

include $(srctree)/Makefile.build
