/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASM_OFFSET_H__
#define __ASM_OFFSET_H__

/* **/
/* 'DEFINE', 'BLANK', and 'COMMENT' generate '.ascii' assembly directive that
 * is processed by build system, see 'sed-offsets'.
 *
 * */

#define DEFINE(sym, val) \
    asm volatile("\n.ascii \"->" #sym " %0 " #val "\"" :: "i" (val))
#define BLANK() asm volatile("\n.ascii \"->\"" :: )
#define COMMENT(x) asm volatile("\n.ascii \"->#" x "\"")

#define OFFSET(sym, str, mem) DEFINE(sym, offsetof(struct str, mem))
#define SIZE_OF(sym, str) DEFINE(sym, sizeof(struct str))

#endif /* __ASM_OFFSET_H__ */
