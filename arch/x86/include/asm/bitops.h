/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_BITOPS_H__
#define __X86_ASM_BITOPS_H__

#ifndef __CYANEA_BITOPS_H__
#  error "Use 'cyanea/bitops.h'"
#endif

# define arch_set_bit arch_set_bit
static __always_inline void arch_set_bit(long bit, volatile unsigned long *addr)
{
	asm volatile("lock; btsq %1, %0" : : "m" (*addr), "Ir" (bit) : "memory");
}

# define arch__set_bit arch__set_bit
static __always_inline void arch__set_bit(unsigned long bit, volatile unsigned long *addr)
{
	asm volatile("btsq %1, %0" : : "m" (*addr), "Ir" (bit) : "memory");
}

# define arch_clear_bit arch_clear_bit
static __always_inline void arch_clear_bit(long bit, volatile unsigned long *addr)
{
	asm volatile("lock; btrq %1, %0" : : "m" (*addr), "Ir" (bit) : "memory");
}

# define arch__clear_bit arch__clear_bit
static __always_inline void arch__clear_bit(unsigned long bit, volatile unsigned long *addr)
{
	asm volatile("btrq %1, %0" : : "m" (*addr), "Ir" (bit) : "memory");
}

#endif /* __X86_ASM_BITOPS_H__ */
