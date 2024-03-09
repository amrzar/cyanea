# SPDX-License-Identifier: GPL-2.0-or-later

obj-y += setup.o
obj-y += setup_percpu.o
obj-y += cpuinfo.o
obj-y += e820.o
obj-y += cache.o
obj-y += mm.o
obj-y += irq.o
obj-y += entry_64.o
obj-y += __entry_64.o
obj-y += cpu.o
obj-$(NUMA) += numa.o

obj-y += apic/
obj-y += acpi/

include $(srctree)/scripts/makefile.build
