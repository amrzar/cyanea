# SPDX-License-Identifier: GPL-2.0-or-later

CFLAGS := $(REALMODE_CFLAGS) -D_SETUP -D__USE_GENERIC_ULIB
CFLAGS += -fno-asynchronous-unwind-tables
AFLAGS := $(CFLAGS) -D__ASSEMBLY__
LDFLAGS := -m elf_i386

obj-y += setup.o
obj-y += header.o
obj-y += puts.o
obj-y += early_params.o
obj-y += string.o
obj-y += version.o
obj-y += memory.o
obj-y += a20.o pm.o pmjump.o

LDFLAGS-setup.elf := -T setup.lds

OBJCOPYFLAGS-setup.bin := -O binary
CLEAN_FILES += setup.elf setup.bin

include $(srctree)/scripts/makefile.build
.DEFAULT_GOAL := setup.bin
