# SPDX-License-Identifier: GPL-2.0-or-later

#include <cyanea/pgtable.h>

    .section ".init.data", "aw"
 
    .balign PAGE_SIZE

    .global real_mode_blob
    .global real_mode_blob_end
    .global real_mode_relocs

real_mode_blob:
    .incbin "kernel/kernel.bin"
real_mode_blob_end:
real_mode_relocs:
    .incbin "kernel.reloc"
