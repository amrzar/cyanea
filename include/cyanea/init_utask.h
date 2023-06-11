/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_INIT_UTASK_H__
#define __CYANEA_INIT_UTASK_H__

#include <asm/utask.h>

extern struct utask init_utask;

# define TOP_OF_INIT_STACK ((unsigned long) __end_utask_init)

#endif /* __CYANEA_INIT_UTASK_H__ */
