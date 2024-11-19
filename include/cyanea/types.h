/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_TYPES_H__
#define __CYANEA_TYPES_H__

#ifndef __ASSEMBLY__

#include <asm-generic/int-ll64.h>
#include <uapi/cyanea/types.h>

#define DECLARE_BITMAP(name, bits) \
	unsigned long name[BITS_TO_ULONGS(bits)]

typedef _Bool bool;

typedef u64 phys_addr_t;

#endif /*  __ASSEMBLY__ */
#endif /* __CYANEA_TYPES_H__ */
