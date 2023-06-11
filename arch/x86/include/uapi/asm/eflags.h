/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_UAPI_ASM_EFLAGS_H__
#define __X86_UAPI_ASM_EFLAGS_H__

#include <uapi/cyanea/const.h>

#define _EFLAGS_BIT_CF      0   /* Carry Flag. */
#define _EFLAGS_CF    _BITUL(_EFLAGS_BIT_CF)
#define _EFLAGS_BIT_FIXED   1   /* Bit 1 - always on. */
#define _EFLAGS_FIXED _BITUL(_EFLAGS_BIT_FIXED)
#define _EFLAGS_BIT_PF      2   /* Parity Flag . */
#define _EFLAGS_PF    _BITUL(_EFLAGS_BIT_PF)
#define _EFLAGS_BIT_AF      4   /* Auxiliary carry Flag. */
#define _EFLAGS_AF    _BITUL(_EFLAGS_BIT_AF)
#define _EFLAGS_BIT_ZF      6   /* Zero Flag. */
#define _EFLAGS_ZF    _BITUL(_EFLAGS_BIT_ZF)
#define _EFLAGS_BIT_SF      7   /* Sign Flag. */
#define _EFLAGS_SF    _BITUL(_EFLAGS_BIT_SF)
#define _EFLAGS_BIT_TF      8   /* Trap Flag. */
#define _EFLAGS_TF    _BITUL(_EFLAGS_BIT_TF)
#define _EFLAGS_BIT_IF      9   /* Interrupt Flag. */
#define _EFLAGS_IF    _BITUL(_EFLAGS_BIT_IF)
#define _EFLAGS_BIT_DF      10  /* Direction Flag. */
#define _EFLAGS_DF    _BITUL(_EFLAGS_BIT_DF)
#define _EFLAGS_BIT_OF      11  /* Overflow Flag. */
#define _EFLAGS_OF    _BITUL(_EFLAGS_BIT_OF)
#define _EFLAGS_BIT_IOPL    12  /* I/O Privilege Level (2 bits). */
#define _EFLAGS_IOPL  (_UL(3) << _EFLAGS_BIT_IOPL)
#define _EFLAGS_BIT_NT      14  /* Nested Task. */
#define _EFLAGS_NT    _BITUL(_EFLAGS_BIT_NT)
#define _EFLAGS_BIT_RF      16  /* Resume Flag. */
#define _EFLAGS_RF    _BITUL(_EFLAGS_BIT_RF)
#define _EFLAGS_BIT_VM      17  /* Virtual Mode. */
#define _EFLAGS_VM    _BITUL(_EFLAGS_BIT_VM)
#define _EFLAGS_BIT_AC      18  /* Alignment Check/Access Control. */
#define _EFLAGS_AC    _BITUL(_EFLAGS_BIT_AC)
#define _EFLAGS_BIT_VIF     19  /* Virtual Interrupt Flag. */
#define _EFLAGS_VIF   _BITUL(_EFLAGS_BIT_VIF)
#define _EFLAGS_BIT_VIP     20  /* Virtual Interrupt Pending. */
#define _EFLAGS_VIP   _BITUL(_EFLAGS_BIT_VIP)
#define _EFLAGS_BIT_ID      21  /* CPUID detection. */
#define _EFLAGS_ID    _BITUL(_EFLAGS_BIT_ID)

#endif /* __X86_UAPI_ASM_EFLAGS_H__ */
