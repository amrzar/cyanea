/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_REALMODE_H__
#define __X86_ASM_REALMODE_H__

#ifndef __ASSEMBLY__

#include <cyanea/types.h>

struct real_mode_header {
	u32 ap_config;              /* Pointer to 'ap_config' in 'trampoline_64.S'. */
	u32 trampoline_pgd;
} __attribute__((__packed__));

struct ap_config {
	u64 initial_code;
	u64 efer;
	u32 cr4;
} __attribute__((__packed__));

extern unsigned char real_mode_blob[];
extern unsigned char real_mode_blob_end[];
extern unsigned char real_mode_relocs[];

extern struct real_mode_header *real_mode_header;

#endif /* __ASSEMBLY__ */
#endif /* __X86_ASM_REALMODE_H__ */
