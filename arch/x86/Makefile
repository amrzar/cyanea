# SPDX-License-Identifier: GPL-2.0-or-later

REALMODE_CFLAGS := -m16 -Os -Wall -Wstrict-prototypes -march=i386 \
	-mregparm=3 -fno-strict-aliasing -fomit-frame-pointer -fno-pic -mno-mmx \
	-mno-sse -fcf-protection=none  

REALMODE_CFLAGS += -ffreestanding	
REALMODE_CFLAGS += -fno-stack-protector
REALMODE_CFLAGS += -mpreferred-stack-boundary=2
export REALMODE_CFLAGS

export BITS UTS_MACHINE

CFLAGS += -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx
CFLAGS += -fcf-protection=none
CFLAGS += -fno-asynchronous-unwind-tables

BITS := 64
UTS_MACHINE := x86_64

AFLAGS += -m64
CFLAGS += -m64 -mno-red-zone -mcmodel=kernel

LDFLAGS := -m elf_$(UTS_MACHINE)

obj-y += arch/x86/ukernel/
obj-y += arch/x86/realmode/
obj-y += arch/x86/head_$(BITS).o arch/x86/__head_$(BITS).o

# Building 'ukernel.elf'.

arch/x86/asm-offsets.s: arch/x86/asm-offsets.c
	$(Q)$(CC) $(UKERINCLUDE) -S -o $@ $<

include/generated/asm-offsets.h: arch/x86/asm-offsets.s FORCE
	$(call if-changed, $(asm-offset))

UKERNEL.bin: ukernel.elf
	$(Q)$(OBJCOPY) -O binary $< $@
	$(Q)truncate -s $$(nm --radix=d $< | awk '/ __ukernel_code_size$$/{print $$1}') $@

ukernel.arch: include/generated/asm-offsets.h arch/x86/ukernel.lds UKERNEL.bin

# Use 2MiB page size (i.e. segment alignment).
LDFLAGS-ukernel.elf += -z max-page-size=0x200000
LDFLAGS-ukernel.elf += -T arch/x86/ukernel.lds

CLEAN_FILES += arch/x86/asm-offsets.s
CLEAN_FILES += arch/x86/ukernel.lds
CLEAN_FILES += ukernel.elf
CLEAN_FILES += UKERNEL.bin

define archhelp
	echo  'Architecture specific targets ($(ARCH)):'
	echo  '  [ukernel]       - Build ukernel image'
endef
