# SPDX-License-Identifier: GPL-2.0-or-later

CFLAGS := -m$(BITS) -O2
CFLAGS += -fno-strict-aliasing
CFLAGS += -mno-mmx -mno-sse
CFLAGS += -ffreestanding -fno-stack-protector
CFLAGS += -fno-asynchronous-unwind-tables
CFLAGS += -D__USE_GENERIC_ULIB

ifdef X86_32
CLAFGS += -march=i386
else
CFLAGS += -mcmodel=small
CFLAGS += -mno-red-zone
endif # CONFIG_X86_32

AFLAGS := $(CFLAGS) -D__ASSEMBLY__
LDFLAGS := -m elf_$(UTS_MACHINE)

mkpiggy: mkpiggy.c
	@$(call cmd-info, HOSTCC, $<)
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

ukernel.elf.gz: FORCE
	$(Q)gzip -cfn $(srctree)/ukernel.elf > ukernel.elf.gz

piggy.S: mkpiggy ukernel.elf.gz
	@./mkpiggy ukernel.elf.gz > piggy.S

CLEAN_FILES += mkpiggy
CLEAN_FILES += ukernel.elf.gz piggy.S

obj-y += head_$(BITS).o
obj-y += piggy.o
obj-y += kernel_info.o
obj-y += deflate.o
obj-y += misc.o
obj-y += string.o
obj-y += early_params.o
obj-y += printf.o
obj-$(X86_NEED_RELOCS) += relocs.o

extra-y += ukernel.lds

LDFLAGS-ukernel.elf := -T ukernel.lds -pie -z noexecstack
LDFLAGS-ukernel.elf += --no-dynamic-linker --orphan-handling=error
LDFLAGS-ukernel.elf += --no-ld-generated-unwind-info

OBJCOPYFLAGS-ukernel.bin := -O binary -S
CLEAN_FILES += ukernel.elf ukernel.bin

include $(srctree)/scripts/makefile.build
.DEFAULT_GOAL := ukernel.bin
