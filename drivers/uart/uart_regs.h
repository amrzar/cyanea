/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UART_REGS_H__
#define __UART_REGS_H__

/** ''Universal Asynchronous Receiver/Transmitter (UART)'' * */

#define UART_THR        0       /* W.   Transmit buffer    'UART_LCR_DLAB = 0'. */
#define UART_RBR        0       /* R.   Receive buffer     'UART_LCR_DLAB = 0'. */
#define UART_DLL        0       /* R/W. Devisor Latch Low  'UART_LCR_DLAB = 1'. */
#define UART_DLH        1       /* R/W. Devisor Latch High 'UART_LCR_DLAB = 1'. */

#define UART_SCR        7       /* R/W. Scratch Register. */

/** 'UART_IER' : R/W. Interrupt Enable Register. **/
#define UART_IER        1
#define UART_IER_MSI    0x08    /* Enable modem status interrupt. */
#define UART_IER_RLSI   0x04    /* Enable receiver line status interrupt. */
#define UART_IER_THRI   0x02    /* Enable transmitter holding register interrupt. */
#define UART_IER_RDI    0x01    /* Enable receiver data interrupt. */

/** 'UART_IIR' : R. Interrupt ID Register. **/
#define UART_IIR        2
#define UART_IIR_PEND   0x01    /* Interrupt pending. */
#define UART_IIR_ID     0x0E    /* Mask for the interrupt ID. */
#define UART_IIR_MSI    0x00    /* Modem status interrupt. */
#define UART_IIR_THRI   0x02    /* Transmitter holding register empty. */
#define UART_IIR_RDI    0x04    /* Receiver data interrupt. */
#define UART_IIR_RLSI   0x06    /* Receiver line status interrupt. */

/** 'UART_FCR' : W. FIFO Control Register. **/
#define UART_FCR        2
#define UART_FCR_FIFO   0x01    /* Enable the FIFO -- 8520 compatibility mode. */
#define UART_FCR_CLR_RX 0x02    /* Clear the receive FIFO. */
#define UART_FCR_CLR_TX 0x04    /* Clear the transmit FIFO. */
#define UART_FCR_DMA    0x08    /* Enable DMA. */
#define UART_FCR_TRG    0xC0    /* Mask for the FIFO trigger range. */
#define UART_FCR_TRG_1  0x00    /* Mask for trigger set at 1. */
#define UART_FCR_TRG_4  0x40    /* Mask for trigger set at 4. */
#define UART_FCR_TRG_8  0x80    /* Mask for trigger set at 8. */
#define UART_FCR_TRG_14 0xC0    /* Mask for trigger set at 14. */

/** 'UART_LCR' : R/W. Line Control Register. **/
#define UART_LCR        3
#define UART_LCR_DLAB   0x80    /* DLL and DLH access bit. */
#define UART_LCR_SBC    0x40    /* Set break control. */
#define UART_LCR_SPAR   0x20    /* Use a fix parity bit '0' or '1' based on parity type. */
#define UART_LCR_PSEL   0x10    /* Parity select: '0' = odd, '1' = even. */
#define UART_LCR_PARITY 0x08    /* Parity enable. */
#define UART_LCR_STOP   0x04    /* Stop bits: '0' = 1 stop bit, '1' = 2 stop bits. */
#define UART_LCR_WLEN5  0x00    /* Word length: 5 bits. */
#define UART_LCR_WLEN6  0x01    /* Word length: 6 bits. */
#define UART_LCR_WLEN7  0x02    /* Word length: 7 bits. */
#define UART_LCR_WLEN8  0x03    /* Word length: 8 bits. */

/** 'UART_MCR' : R/W. Modem Control Register. **/
#define UART_MCR        4
#define UART_MCR_LOOP   0x10    /* Enable loopback mode. */
#define UART_MCR_OUT2   0x08    /* Enable Aux Output 2. */
#define UART_MCR_OUT1   0x04    /* Enable Aux Output 1. */
#define UART_MCR_RTS    0x02    /* Set/Clear Request To Send (RTS). */
#define UART_MCR_DTR    0x01    /* Set/Clear Data Terminal Ready (DTR). */

/** 'UART_LSR' : R. Line Status Register. **/
#define UART_LSR        5
#define UART_LSR_FIFOE  0x80    /* FIFO error. */
#define UART_LSR_TEMT   0x40    /* Transmitter empty, FIFO and shift register. */
#define UART_LSR_THRE   0x20    /* THR empty, ready to accept more characters. */
#define UART_LSR_BI     0x10    /* Break interrupt indicator. */
#define UART_LSR_FE     0x08    /* Frame error indicator. */
#define UART_LSR_PE     0x04    /* Parity error indicator. */
#define UART_LSR_OE     0x02    /* Overrun error indicator. */
#define UART_LSR_DR     0x01    /* Receiver data ready. */

/** 'UART_MSR' : R. Modem Status Register. **/
#define UART_MSR        6
#define UART_MSR_DCD    0x80    /* Carrier Detect. */
#define UART_MSR_RI     0x40    /* Ring Indicator. */
#define UART_MSR_DSR    0x20    /* Data Set Ready (DSR). */
#define UART_MSR_CTS    0x10    /* Clear to Send (CTS). */
#define UART_MSR_DDCD   0x08    /* Delta Data Carrier Detect (DCD). */
#define UART_MSR_TERI   0x04    /* Trailing edge ring indicator. */
#define UART_MSR_DDSR   0x02    /* Delta DSR. */
#define UART_MSR_DCTS   0x01    /* Delta CTS. */

#endif /* __UART_REGS_H__ */
