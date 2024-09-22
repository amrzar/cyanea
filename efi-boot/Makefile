# SPDX-License-Identifier: GPL-2.0-or-later

UKERNEL.gz: FORCE
	$(Q)gzip -cfn $(srctree)/UKERNEL.bin > $@

UKERNEL.gz.o: UKERNEL.gz
	$(Q)$(OBJCOPY) -I binary -O elf64-x86-64 $< $@ \
		--rename-section .data=.gz..data,load,alloc,readonly,contents

ifdef X86_64
AFLAGS-header.o += -DMACHINE_TYPE=IMAGE_FILE_MACHINE_AMD64
ifdef DEBUG
CFLAGS-efi.o += -O2
endif # DEBUG
else
$(error undefined architecture.)
endif #X86_64

obj-y += header.o
obj-y += efi.o

UKERNEL.efi.elf: UKERNEL.gz.o .builtin.o
	$(Q)$(LD) $(LDFLAGS) -T boot.lds -o $@ $^

UKERNEL.efi: UKERNEL.efi.elf
	$(Q)$(OBJCOPY) -O binary $< $@

CLEAN_FILES += UKERNEL.gz
CLEAN_FILES += UKERNEL.gz.o
CLEAN_FILES += UKERNEL.efi.elf
CLEAN_FILES += UKERNEL.efi

include $(srctree)/Makefile.build
.DEFAULT_GOAL := UKERNEL.efi
