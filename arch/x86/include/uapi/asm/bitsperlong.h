/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_UAPI_ASM_BITSPERLONG_H__
#define __X86_UAPI_ASM_BITSPERLONG_H__

#ifdef __x86_64__
#define __BITS_PER_LONG 64
#else
#define __BITS_PER_LONG 32
#endif

#ifdef CONFIG_64BIT
#if __BITS_PER_LONG != 64
#  error inconsistent word size.
#endif
#else
#if __BITS_PER_LONG != 32
#  error inconsistent word size.
#endif
#endif /* CONFIG_64BIT */

#define BITS_PER_LONG __BITS_PER_LONG
#define BITS_PER_LONG_LONG 64

#endif /* __X86_UAPI_ASM_BITSPERLONG_H__ */
