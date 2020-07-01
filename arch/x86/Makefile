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

ifdef X86_32
	BITS := 32
	UTS_MACHINE := i386

	AFLAGS += -m32
	CFLAGS += -m32 -msoft-float -mregparm=3 -freg-struct-return

else
	BITS := 64
	UTS_MACHINE := x86_64

	AFLAGS += -m64
	CFLAGS += -m64 -mno-red-zone -mcmodel=kernel
	
endif # X86_32

LDFLAGS := -m elf_$(UTS_MACHINE)

obj-y += arch/x86/ukernel/
obj-y += arch/x86/head_$(BITS).o arch/x86/__head_$(BITS).o

# ... and extra files need to be built.

extra-y += arch/x86/asm-offsets.s
extra-y += arch/x86/ukernel.lds

LDFLAGS-ukernel.elf := -T arch/x86/ukernel.lds

ifdef X86_NEED_RELOCS

#
# 'x86_32' uses single mapping form 'PAGE_OFFSET' to begining of memory. 
# If boot-loader loads the ukernel at any address other than 'CONFIG_PHYSICAL_START',
# this offset is also used for the virtual address relocation.
#
# 'x86_64' uses a mapping from '__START_KERNEL_map' to begining of memory.
# If boot-loader loads the ukernel at physical address 'physaddr' other than
# 'CONFIG_PHYSICAL_START', we update the page-table to map '__START_KERNEL_map'
# to 'physaddr - CONFIG_PHYSICAL_START' without virtual address relocation.
#
# 'X86_NEED_RELOCS' is not set for 'x86_64' relocation.
#

LDFLAGS-ukernel.elf += --emit-relocs --discard-none
endif # X86_NEED_RELOCS

include/generated/asm-offsets.h: arch/x86/asm-offsets.s FORCE
	$(call if-changed,$(asm-offset))

#
# 'ukernel' is default target.
# 'ukernel.elf' is the main ukernel ELF image.
#

ukernel: include/generated/asm-offsets.h ukernel.elf
	$(call descend-make,arch/x86/boot)

CLEAN_FILES += ukernel.elf

define archclean
	$(call descend-make,arch/x86/boot clean)
endef

define archhelp
	echo  'Architecture specific targets ($(ARCH)):'
	echo  '  [ukernel]       - Build ukernel image'
endef

run: ukernel
	$(Q)$(srctree)/scripts/qemu.sh arch/x86/boot/zukernel

.PHONY: run
