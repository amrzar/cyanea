/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASM_GENERIC_EARLY_IOREMAP_H__
#define __ASM_GENERIC_EARLY_IOREMAP_H__

#include <cyanea/init.h>
#include <cyanea/types.h>
#include <cyanea/stddef.h>
#include <cyanea/pgtable.h>

void __init *early_ioremap(phys_addr_t, size_t, pgprot_t);
void __init early_iounmap(void *, size_t);

#endif /* __ASM_GENERIC_EARLY_IOREMAP_H__ */
