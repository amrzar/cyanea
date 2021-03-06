
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

LDFLAGS := -melf_x86_64
LDFLAGS_ukernel :=

CFLAGS += -march=$(X86_MICROARCHITECTURE)
CFLAGS += -mno-red-zone
CFLAGS += -mcmodel=kernel
CFLAGS += -mno-sse -mno-mmx -mno-sse2 -mno-3dnow
CFLAGS += -mno-avx
CFLAGS += -m64

AFLAGS += -m64

CPPFLAGS += -I$(srctree)/arch/x86/include

head := arch/x86/head.o

arch/x86/.linker.lds: arch/x86/linker.lds.S FORCE
	$(Q)$(CPP) $(CPPFLAGS) -D__ASSEMBLY__ -P -C -o $@ $<

ukernel.image: $(head) arch/x86/.linker.lds .output.o
	$(Q)echo "[BUILD] $@" 
	$(Q)$(LD) $(LDFLAGS) $(LDFLAGS_ukernel) -o arch/x86/.$@.tmp \
		-T arch/x86/.linker.lds $(head) .output.o
	$(Q)$(OBJCOPY) -O elf32-i386 arch/x86/.$@.tmp arch/x86/$@
	$(Q)rm -f arch/x86/.$@.tmp

ukernel: ukernel.image

CLEAN_FILES += arch/x86/ukernel.image
CLEAN_FILES += arch/x86/.linker.lds
