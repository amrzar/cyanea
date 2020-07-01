/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <asm-offset.h>

#include <cyanea/stddef.h>
#include <cyanea/compiler.h>
#include <uapi/asm/bootparam.h>

static void __always_unused unused_function(void)
{
    SIZE_OF(BP_size, boot_params);
    OFFSET(BP_scratch, boot_params, scratch);
    OFFSET(BP_kernel_alignment, boot_params, hdr.kernel_alignment);
    OFFSET(BP_init_size, boot_params, hdr.init_size);
    OFFSET(BP_cmd_line_ptr, boot_params, hdr.cmd_line_ptr);
}
