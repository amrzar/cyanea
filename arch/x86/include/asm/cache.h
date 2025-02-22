/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CACHE_H__
#define __X86_ASM_CACHE_H__

#ifndef __CYANEA_CACHE_H__
#  error "Use 'cyanea/cache.h'"
#endif

#if defined(CONFIG_L1_CACHE_4_SHIFT)
# define L1_CACHE_SHIFT 4
#elif defined(CONFIG_L1_CACHE_5_SHIFT)
# define L1_CACHE_SHIFT 5
#elif defined(CONFIG_L1_CACHE_6_SHIFT)
# define L1_CACHE_SHIFT 6
#elif defined(CONFIG_L1_CACHE_7_SHIFT)
# define L1_CACHE_SHIFT 7
#endif

#define L1_CACHE_BYTES (1 << L1_CACHE_SHIFT)

#endif /* __X86_ASM_CACHE_H__ */
