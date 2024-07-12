# SPDX-License-Identifier: GPL-2.0-or-later

# Copied from 'x86/boot/setup/Makefile'.

CFLAGS := $(REALMODE_CFLAGS) -D_SETUP -D__USE_GENERIC_ULIB
CFLAGS += -fno-asynchronous-unwind-tables
AFLAGS := $(CFLAGS) -D__ASSEMBLY__
LDFLAGS := -m elf_i386

obj-y += header.o
obj-y += trampoline_64.o

# Bild real-mode kernel for APs boot.

# The real-mode kernel is compiled with '-m16'. For relocation, we update segment registers.
# For codes in 32-bit section, we reference only to global symbols as 'pa_X'.

# Extract global symbols that are not 'UND' and export them with 'pa_X' symbols.

sed-pasyms := -n -r -e 's/^([0-9a-fA-F]+) [ABCDGRSTVW] (.+)$$/pa_\2 = \2;/p'

pasyms.h: .builtin.o
	@$(call cmd-info, PASYMS, $@)
	$(Q)$(NM) $< | sed $(sed-pasyms) | sort | uniq > $@

realmode.lds: pasyms.h

CLEAN_FILES += pasyms.h
CLEAN_FILES += realmode.lds

kernel.elf: realmode.lds

LDFLAGS-kernel.elf := -T realmode.lds --emit-relocs --discard-none

kernel.bin: kernel.elf
	@$(call cmd-info, OBJCOPY, $<)
	$(Q)$(OBJCOPY) -O binary $< $@

CLEAN_FILES += kernel.elf kernel.bin

include $(srctree)/scripts/makefile.build
.DEFAULT_GOAL := kernel.bin
