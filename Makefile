# SPDX-License-Identifier: GPL-2.0-or-later

MAKEFLAGS += --no-print-directory
Q = $(if $(V),,@)

$(shell mkdir -p include/generated)

srctree := $(CURDIR)
sysconfig = $(srctree)/include/sys.config.h
ulib = $(srctree)/ulib
scripts = ../tools/scripts

export srctree sysconfig ulib scripts Q

uconfig = ../tools/uconfig

-include .sys.config.in

AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
CC = $(CROSS_COMPILE)gcc
CPP = $(CC) -E -x c
NM = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy

export CROSS_COMPILE CPP AS LD CC NM OBJCOPY

HOSTCC = gcc
HOSTCFLAGS = -Wall -Wstrict-prototypes -Wno-unused-function -O2

export HOSTCC HOSTCFLAGS

UKERINCLUDE := -I$(srctree)/arch/$(ARCH)/include \
			   -I$(srctree)/include/generated \
			   -I$(srctree)/include \
			   -include $(sysconfig) \
			   -include $(srctree)/include/ulog.h \
			   -include $(srctree)/include/compiler.h

CPPFLAGS := -nostdinc -D__UKERNEL__ -D__CYANEA__
CFLAGS := -nostdinc -std=gnu11 -fno-common -fno-PIE -ffreestanding -fno-strict-aliasing \
	-Wall -Wundef -Wstrict-prototypes -ftls-model=local-exec -Wnested-externs   \
	-Werror
AFLAGS := -nostdinc -D__ASSEMBLY__ -fno-PIE

export CPPFLAGS CFLAGS AFLAGS LDFLAGS UKERINCLUDE

CFLAGS += -fno-stack-protector
CFLAGS += -Wimplicit-fallthrough=5
ifdef DEBUG
CFLAGS += -g -Wno-unused-function
endif

include/generated/compile-info.h: FORCE
	$(Q)$(srctree)/mkcompile-info.sh $@

CFLAGS-version.o = -include compile-info.h
version.o: include/generated/compile-info.h
obj-y += version.o

obj-y += init/ uklib/ ukernel/ drivers/

-include $(srctree)/arch/$(ARCH)/Makefile
include $(srctree)/Makefile.build

LDFLAGS-ukernel.elf += --orphan-handling=error

menuconfig silentoldconfig defconfig: FORCE
	$(call descend-make, $(uconfig) $@)

.sys.config.in: $(sysconfig)
	@$(call cmd-info, GEN, $@)
	$(Q)sed -ne 's/^#define CONFIG_\([^ ]*\) "\?\([^"]*\)"\? *$$/export \1=\2/w $@' $(sysconfig)

clean: rm-files
	@echo "CLEAN"
	$(Q)find . \( -name '*.o' -o -name '*.d' \) -delete
ifdef EFI_STUB
	$(call descend-make, efi-boot rm-files)
endif # EFI_STUB

mrproper: clean
	$(call descend-make, $(uconfig) clean)
	$(Q)rm -rf include/generated
	$(Q)rm -f $(sysconfig)
	$(Q)rm -f .old.config
	$(Q)rm -f .sys.config.in

help:
	echo 'Cleaning targets:'
	echo '  clean           - remove compiled and generated files but keep config files'
	echo '  mrproper        - remove everything'
	echo ''
	echo 'Configuration targets:'
	echo '  menuconfig      - start uconfig GUI'
	echo '  silentoldconfig - generate sys.config.h form existing config files'
	echo '  defconfig       - generate default config files'
	echo ''
	$(archhelp)

.SILENT: help

ukernel: ukernel.arch
ifdef EFI_STUB
	$(call descend-make, efi-boot)
endif # EFI_STUB

# Uncomment if use astyle.
export USE_ASTYLE = 1

style:
	$(Q)find $(srctree) \
		\( -name '*.c' -o -name '*.h' \) \
		\( -exec $(scripts)/style.sh {} \; -o -quit \)

.PHONY: help style ukernel.arch mrproper clean
.DEFAULT_GOAL := ukernel
