/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_CONTAINER_OF_H__
#define __CYANEA_CONTAINER_OF_H__

#define container_of(ptr, type, member) \
	({ \
		static_assert( \
		        __same_type(*(ptr), ((type *)(0))->member) || \
		        __same_type(*(ptr), void), \
		        "pointer type mismatch in ''container_of''."); \
		((type *)((void *)(ptr) - offsetof(type, member))); \
	})

#endif /* __CYANEA_CONTAINER_OF_H__ */
