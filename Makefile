# SPDX-License-Identifier: GPL-2.0-or-later

MAKEFLAGS += --no-print-directory
Q = $(if $(V),,@)

srctree := $(CURDIR)
$(shell mkdir -p include/generated)

uconfig = ../tools/uconfig
scripts = ../tools/scripts
sysconfig = $(srctree)/include/sys.config.h
ulib = $(srctree)/ulib

-include .sys.config.in # ... try building '.sys.config.in'

AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
CC = $(CROSS_COMPILE)gcc
CPP = $(CC) -E -x c
NM = $(CROSS_COMPILE)nm
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy
NOSTDINC_FLAGS = -nostdinc

HOSTCC = gcc
HOSTCFLAGS = -Wall -Wstrict-prototypes -Wno-unused-function -O2

export CROSS_COMPILE CPP AS LD CC NM AR OBJCOPY
export NOSTDINC_FLAGS OBJCOPYFLAGS
export HOSTCC HOSTCFLAGS
export srctree sysconfig ulib scripts Q

UAPIINCLUDE := -I$(srctree)/arch/$(ARCH)/include/uapi \
			   -I$(srctree)/include/uapi

UKERINCLUDE := -I$(srctree)/arch/$(ARCH)/include \
			   -I$(srctree)/include/generated \
			   -I$(srctree)/include \
			   -I$(srctree)/ulib/include \
			   $(UAPIINCLUDE) \
			   -include $(sysconfig) \
			   -include $(ulib)/include/ulib/ulog.h

#
# The '-nostdlib' forces GCC not to use standard system startup files or
# libraries. It is only effective when $(CC) is used as linker driver.
# Here, we do not set '-nostdlib' as $(LD) is always called explicitly.
#

CPPFLAGS := -D__UKERNEL__ -D__CYANEA__
CFLAGS   := -std=gnu11 -fno-common -fno-PIE -ffreestanding -fno-strict-aliasing \
	-Wall -Wundef -Wstrict-prototypes -ftls-model=local-exec -Wnested-externs   \
	-Werror
AFLAGS   := -D__ASSEMBLY__ -fno-PIE

export CPPFLAGS CFLAGS AFLAGS LDFLAGS ARFLAGS UKERINCLUDE UAPIINCLUDE

CFLAGS += -fno-stack-protector
CFLAGS += -Wimplicit-fallthrough=5

ifdef DEBUG
	CFLAGS += -g
	CFLAGS += -Wno-unused-function
endif # DEBUG

# Build compile information header 'compile-info.h'.
include/generated/compile-info.h: FORCE
	$(Q)$(srctree)/scripts/mkcompile-info.sh $@

CFLAGS-version.o = -include compile-info.h
version.o: include/generated/compile-info.h
obj-y += version.o

obj-y += init/ ulib/ ukernel/ drivers/

-include $(srctree)/arch/$(ARCH)/Makefile
include $(srctree)/scripts/makefile.build

# Just make sure sections are not placed heuristically by the linker.
# All sections should be explicitly named in the linker script.
LDFLAGS-ukernel.elf += --orphan-handling=error

menuconfig silentoldconfig defconfig: FORCE
	$(call descend-make, $(uconfig) $@)

.sys.config.in: $(sysconfig)
	@$(call cmd-info, GEN, $@)
	$(Q)sed -ne 's/^#define CONFIG_\([^ ]*\) "\?\([^"]*\)"\? *$$/export \1=\2/w $@' $(sysconfig)

clean: rm-files
	@echo "CLEAN"
	$(Q)find . \( -name '*.o' -o -name '*.d' \) -delete

mrproper: clean
	$(call descend-make, $(uconfig) clean)
	$(Q)rm -rf include/generated
	$(Q)rm -f $(sysconfig)
	$(Q)rm -f .old.config
	$(Q)rm -f .sys.config.in

.PHONY: mrproper clean

help:
	echo 'Cleaning targets:'
	echo '  clean           - remove compiled and generated files but keep config files'
	echo '  mrproper        - remove everything'
	echo ''
	echo 'Configuration targets:'
	echo '  menuconfig      - start uconfig gui'
	echo '  silentoldconfig - generate sys.config.h form existing config files'
	echo '  defconfig       - generate default config files'
	echo ''
	$(archhelp)

.SILENT: help

# Uncommnet if use astyle.
export USE_ASTYLE = 1

style:
	$(Q)find $(srctree) \
		\( -name '*.c' -o -name '*.h' \) \
		\( -exec $(scripts)/style.sh {} .style \; -o -quit \)

.PHONY: help style
.DEFAULT_GOAL := ukernel
