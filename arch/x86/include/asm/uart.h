/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_UART_H__
#define __X86_ASM_UART_H__

#define BASE_BAUD (1843200 / 16)        /* ''1.8432 MHz''. */

#define UART_PORTS \
	{ BASE_BAUD, 0x3F8 }, /* ''ttyS0'' */ \
	{ BASE_BAUD, 0x2F8 }, /* ''ttyS1'' */ \
	{ BASE_BAUD, 0x3E8 }, /* ''ttyS2'' */ \
	{ BASE_BAUD, 0x2E8 },       /* ''ttyS3'' */

#endif /* __X86_ASM_UART_H__ */
