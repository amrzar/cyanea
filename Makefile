
 #
 # Copyright 2020 Amirreza Zarrabi <amrzar@gmail.com>
 #
 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation; either version 2 of the License, or
 # (at your option) any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #

MAKEFLAGS += --no-print-directory

VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERION =

srctree := $(CURDIR)
uconfig = ../tools/uconfig

-include .sys.config.in # ... try building '.sys.config.in'

AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
CC = $(CROSS_COMPILE)gcc
CPP = $(CC) -E
OBJCOPY = $(CROSS_COMPILE)objcopy
NOSTDINC_FLAGS = -nostdinc

HOSTCC = gcc
HOSTCFLAGS = -Wall -Wstrict-prototypes -Wno-unused-function -O2

export CROSS_COMPILE CPP AS LD CC OBJCOPY NOSTDINC_FLAGS
export HOSTCC HOSTCFLAGS
export srctree Q

UKERNELINCLUDE := $(srctree)/include

CPPFLAGS := -D__KERNEL__ -I$(UKERNELINCLUDE)
CFLAGS := -Wall -Wundef -Wstrict-prototypes -fno-common -std=gnu89 -fno-PIE \
	-Werror -Wmissing-prototypes -Wnested-externs -Wmissing-declarations \
	-Wpointer-arith -ffreestanding
AFLAGS := -D__ASSEMBLY__

export CPPFLAGS CFLAGS AFLAGS LDFLAGS ARFLAGS UKERNELINCLUDE

UKERNELRELEASE = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)

 # ... SHELL used to run scripts.
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; else echo /bin/bash; fi)

export VERSION PATCHLEVEL SUBLEVEL EXTRAVERION UKERNELRELEASE CONFIG_SHELL

ifdef DEBUG_BUILD
CFLAGS += -g -ggdb
endif # DEBUG_BUILD

 # ... disable stack protector if enabled by default.
CFLAGS += -fno-stack-protector

 # ... architecture dependant.
-include $(srctree)/arch/$(ARCH)/Makefile

objects += drivers/

include Rules.make

 # ... 'sys.config.h' is system autoconfig. 
sys-config-h = $(UKERNELINCLUDE)/sys.config.h

menuconfig:
	$(Q)$(MAKE) -C $(uconfig) I=$(srctree)/configs.in \
		OUT=$(sys-config-h) menuconfig

silentoldconfig:
	$(Q)$(MAKE) -C $(uconfig) I=$(srctree)/configs.in \
		OUT=$(sys-config-h) silentoldconfig

defconfig:
	$(Q)$(MAKE) -C $(uconfig) I=$(srctree)/configs.in defconfig

.PHONY: menuconfig silentoldconfig oldconfig

.sys.config.in: $(sys-config-h)
	$(Q)echo "[BUILD] $@" 
	$(Q)sed -n 's/^#define CONFIG_\([^ ]*\) "\?\([^"]*\)"\? *$$/export \1=\2/w $@' $(sys-config-h)

clean:
	$(Q)echo "[CLEAN]"
	$(Q)find $(srctree) \
		\( -name "*.o" -o -name "*.d" \) \
		-type f -delete
	$(Q)rm -f $(CLEAN_FILES) # ... delete whatever there.

mrproper: clean
	$(Q)$(MAKE) -C $(uconfig) clean
	$(Q)rm -f $(sys-config-h)
	$(Q)rm -f .old.config
	$(Q)rm -f .sys.config.in

help:
 # ... brief documentation of the typical targets used
	echo 'Cleaning targets:'
	echo '  clean           - remove generated files but keep config files'
	echo '  mrproper        - remove generated files and config files'
	echo ''
	echo 'Configuration targets:'
	echo '  ukernel         - build the cyanea ukernel'
	echo ''
	echo '  menuconfig      - start uconfig gui'
	echo '  silentoldconfig - generate sys.config.h form existing config files'
	echo '  defconfig       - generate default config files'

.SILENT: help

.PHONY: ukernel mrproper clean help
.DEFAULT_GOAL := ukernel

style:
	$(Q)find $(srctree) \
		\( -name "*.c" -o -name "*.h" \) -exec \
		astyle \
		--style=attach \
		--indent-after-parens \
		--indent-preproc-define \
		--convert-tabs \
		--indent-labels \
		--indent-preproc-cond \
		--break-blocks \
		--pad-oper \
		--pad-comma \
		--pad-header \
		--align-pointer=name \
		--break-one-line-headers \
		{} +

.PHONY: style
