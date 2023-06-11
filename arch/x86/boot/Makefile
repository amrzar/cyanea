# SPDX-License-Identifier: GPL-2.0-or-later

build: build.c
	@$(call cmd-info, HOSTCC, $<)
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

CLEAN_FILES += build

sed-voffset := 's/^\([0-9a-fA-F]*\) [ABCDGRSTVW] \(_text\|__bss_start\|_end\)$$/\#define VO_\2 0x\1/p'
voffset.h: FORCE
	$(call if-changed, $(NM) $(srctree)/ukernel.elf | sed -ne $(sed-voffset))

sed-zoffset := 's/^\([0-9a-fA-F]*\) [a-zA-Z] \(startup_32\|startup_64\|efi32_stub_entry\|efi64_stub_entry\|efi_pe_entry\|efi32_pe_entry\|input_data\|kernel_info\|_end\|_ehead\|_text\|z_.*\)$$/\#define ZO_\2 0x\1/p'
zoffset.h: compressed/ FORCE
	$(call if-changed, $(NM) compressed/ukernel.elf | sed -ne $(sed-zoffset))

build-zukernel: compressed/ setup/ build
	@$(call cmd-info, BUILD, zukernel)
	@./build setup/setup.bin compressed/ukernel.bin ./zukernel

CLEAN_FILES += zukernel

setup/: zoffset.h FORCE
	@printf '%-8s%s\n' DESCEND $@
	$(call descend-make, $@)

compressed/: voffset.h FORCE
	@printf '%-8s%s\n' DESCEND $@
	$(call descend-make, $@)

CLEAN_FILES += voffset.h zoffset.h

define clean-all
	$(call descend-make, setup rm-files)
	$(call descend-make, compressed rm-files)
endef

include $(srctree)/scripts/makefile.build
.DEFAULT_GOAL := build-zukernel
