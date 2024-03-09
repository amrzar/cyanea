/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_CACHE_H__
#define __CYANEA_CACHE_H__

#include <asm/cache.h>

#ifndef SMP_CACHE_BYTES
# define SMP_CACHE_BYTES L1_CACHE_BYTES
#endif

#ifndef ____cacheline_aligned
# define ____cacheline_aligned __aligned(SMP_CACHE_BYTES)
#endif

#endif /* __CYANEA_CACHE_H__ */
