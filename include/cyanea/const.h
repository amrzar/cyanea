/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_CONST_H__
#define __CYANEA_CONST_H__

#include <uapi/cyanea/const.h>

#define UL(x)  _UL(x)
#define ULL(x) _ULL(x)

#define BIT_UL(x)  _BITUL(x)
#define BIT_ULL(x) _BITULL(x)

#define IS_ALIGNED(x, a) (((x) & ((typeof(x))(a) - 1)) == 0)

#endif /* __CYANEA_CONST_H__ */
