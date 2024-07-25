/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BUILDx_H__
#define __BUILDx_H__

#define DEFINE(sym, val) \
    asm volatile("\n.ascii \"->" #sym " %0 " #val "\"" : : "i" (val))

#define BLANK() \
    asm volatile("\n.ascii \"->\"" : : )

#define COMMENT(x) \
    asm volatile("\n.ascii \"->#" x "\"")

#endif /* __BUILDx_H__ */
