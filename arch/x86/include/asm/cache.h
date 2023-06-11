/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_CACHE_H__
#define __X86_ASM_CACHE_H__

#if defined(CONFIG_L1_CACHE_4_SHIFT)
# define L1_CACHE_SHIFT 4
#elif defined(CONFIG_L1_CACHE_5_SHIFT)
# define L1_CACHE_SHIFT 5
#elif defined(CONFIG_L1_CACHE_6_SHIFT)
# define L1_CACHE_SHIFT 6
#elif defined(CONFIG_L1_CACHE_7_SHIFT)
# define L1_CACHE_SHIFT 7
#else

/* Cache Line is not defined. Let's guess something !!! */

# define L1_CACHE_SHIFT 6
#endif /* CONFIG_L1_CACHE_X_SHIFT */

#define L1_CACHE_BYTES  (1 << L1_CACHE_SHIFT)

#endif /* __X86_ASM_CACHE_H__ */
