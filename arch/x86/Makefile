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

include/generated/asm-offsets.h: arch/x86/asm-offsets.s FORCE
	$(call if-changed, $(asm-offset))

ukernel.elf: arch/x86/ukernel.lds

LDFLAGS-ukernel.elf := -T arch/x86/ukernel.lds

# Instruct linker to use 2 MiB page size, i.e. segment's alignment.
LDFLAGS-ukernel.elf += -z max-page-size=0x200000

CLEAN_FILES += arch/x86/asm-offsets.s
CLEAN_FILES += arch/x86/ukernel.lds ukernel.elf

# 'ukernel' is default target.

ukernel: include/generated/asm-offsets.h ukernel.elf

define archhelp
	echo  'Architecture specific targets ($(ARCH)):'
	echo  '  [ukernel]       - Build ukernel image'
endef

run: ukernel
	$(Q)$(srctree)/scripts/qemu.sh arch/x86/boot/zukernel /home/amrzar/initrd

.PHONY: run
