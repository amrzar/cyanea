/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_BITOPS_H__
#define __CYANEA_BITOPS_H__

#include <cyanea/const.h>

#ifndef __CHAR_BIT__
# error '__CHAR_BIT__'  is not defined.
#else
# define BITS_PER_TYPE(type) (sizeof(type) * __CHAR_BIT__)
# define BITS_TO_ULONGS(nr) __KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(unsigned long))
# define BITS_TO_BYTES(nr) __KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(char))
#endif

#include <asm/bitops.h>

/* 'X_bit' is atomic and '__X_bit' is non-atomic. */

#ifndef arch_set_bit
# error 'arch_set_bit' is not defined.
#else
# define set_bit arch_set_bit
#endif

#ifndef arch__set_bit
# error 'arch__set_bit' is not defined.
#else
# define __set_bit arch__set_bit
#endif

#ifndef arch_clear_bit
# error 'arch_clear_bit' is not defined.
#else
# define clear_bit arch_clear_bit
#endif

#ifndef arch__clear_bit
# error 'arch__clear_bit' is not defined.
#else
# define __clear_bit arch__clear_bit
#endif

#endif /* __CYANEA_BITOPS_H__ */
