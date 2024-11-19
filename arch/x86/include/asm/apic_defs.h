/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_APIC_DEFS_H__
#define __X86_ASM_APIC_DEFS_H__

/* 11.4.1 The Local APIC Block Diagram. */

/* APIC registers are memory-mapped to a 4-KByte page starting at 0xFEE00000 (UC). */

#define APIC_DEFAULT_PHYS_BASE 0xFEE00000

/* 0x000 .. 0x010. Reserved. */

/* 11.4.6 Local APIC ID. */
#define APIC_ID 0x020   /* Local APIC ID Register (R/W). */

/* 11.4.8 Local APIC Version Register. */
#define APIC_VER 0x030  /* Local APIC Version Register (RO). */
#  define APIC_VER_MASK                         0x00FF00FFU
#  define APIC_VER_Max_LVT(x) (((x) >> 16) & 0xFFU)
#  define APIC_VER_EOI_SUPPRESSION              0x01000000U

/* 0x040 .. 0x070. Reserved. */

/* 11.8.3.1 Task and Processor Priorities. */
#define APIC_TPR 0x080  /* Task Priority Register (R/W). */
#  define APIC_TPR_MASK                         0x000000FFU
#define APIC_PPR 0x0A0  /* Processor Priority Register (RO). */

/* 11.6.2.4 Lowest Priority Delivery Mode. */
#define APIC_APR 0x090  /* Arbitration Priority Register (RO). */

/* 11.8.5 Signaling Interrupt Servicing Completion. */
#define APIC_EOI 0x0B0  /* EOI Register (WO). */
#define APIC_RRR 0x0C0  /* Remote Read Register (RO). */

/* 11.6.2.2 Logical Destination Mode. */
#define APIC_LDR 0x0D0  /* Logical Destination Register (R/W). */
#define APIC_DFR 0x0E0  /* Destination Format Register (R/W). */
#  define APIC_DFR_CLUSTER                      0x0FFFFFFFU
#  define APIC_DFR_FLAT                         0xFFFFFFFFU

/* 11.9 SPURIOUS INTERRUPT. */
#define APIC_SVR 0x0F0  /* Spurious Interrupt Vector Register (R/W). */
#  define APIC_SVR_ACPI_ENABLED                 0x00000100U
#  define APIC_SVR_FOCUS_PROCESSOR              0x00000200U
#  define APIC_SVR_EOI_BROADCAST                0x00001000U
#  define APIC_SVR_VECTOR_MASK                  0x000000FFU

#define APIC_ISR 0x100          /* In-Service Register (RO) (0x100 .. 0x170). */
#define APIC_TMR 0x180          /* Trigger Mode Register (RO) (0x180 .. 0x1F0). */
#define APIC_IRR 0x200          /* Interrupt Request Register (RO) (0x200 .. 0x270). */
# define APIC_XXR_REGS 8        /* ISR, TMR, and IRR registers. */

/* 11.5.3 Error Handling. */
#define APIC_ESR 0x280  /* Error Status Register (R/W). */
#  define APIC_ESR_SEND_CHECKSUM                0x00000001U
#  define APIC_ESR_RECEIVE_CHECKSUM             0x00000002U
#  define APIC_ESR_SEND_ACCEPT                  0x00000004U
#  define APIC_ESR_RECEIVE_ACCEPT               0x00000008U
#  define APIC_ESR_REDIRECTABLE_IPI             0x00000010U
#  define APIC_ESR_SEND_ILLEGAL_VECTOR          0x00000020U
#  define APIC_ESR_RECEIVE_ILLEGAL_VECTOR       0x00000040U
#  define APIC_ESR_ILLEGAL_REGISTER_ADDRESS     0x00000080U

/* 0x290 .. 0x2E0. Reserved. */

#define APIC_CMCIR 0x2F0        /* LVT Corrected Machine Check Interrupt Register (R/W). */

/* 11.6.1 Interrupt Command Register (ICR). */
#define APIC_ICR 0x300  /* Interrupt Command Register (R/W). */
#  define APIC_ICR_DELIVERY_MODE_FIXED          0x00000000U
#  define APIC_ICR_DELIVERY_MODE_LOWEST_PRIO    0x00000100U
#  define APIC_ICR_DELIVERY_MODE_SMI            0x00000200U
#  define APIC_ICR_DELIVERY_MODE_NMI            0x00000400U
#  define APIC_ICR_DELIVERY_MODE_INIT           0x00000400U
#  define APIC_ICR_DELIVERY_MODE_STARTUP        0x00000600U
#  define APIC_ICR_DELIVERY_MODE_RESERVED       0x00000700U
#  define APIC_ICR_DESTINATION_MODE_LOGICAL     0x00000800U
#  define APIC_ICR_DELIVERY_STATUS_PENDING      0x00001000U
#  define APIC_ICR_LEVEL_ASSERT                 0x00004000U
#  define APIC_ICR_TRIGGER_MODE_LEVEL           0x00008000U
#  define APIC_ICR_DESTINATION_SELF             0x00040000U
#  define APIC_ICR_DESTINATION_ALL              0x00080000U
#  define APIC_ICR_DESTINATION_ALL_BUT_SELF     0x000C0000U
#define APIC_ICR2 0x310
#  define GET_XAPIC_DEST_FIELD(x) (((x) >> 24) & 0xFF)
#  define SET_XAPIC_DEST_FIELD(x) ((x) << 24)

/* 11.5.1 Local Vector Table. */
#define APIC_LVT_TIMER 0x320    /* LVT Timer Register (R/W). */
#  define APIC_LVT_TIMER_ONESHOT                (0 << 17)
#  define APIC_LVT_TIMER_PERIODIC               (1 << 17)
#  define APIC_LVT_TIMER_TSCDEADLINE            (2 << 17)
#define APIC_LVT_TSR   0x330    /* LVT Thermal Sensor Register (R/W). */
#define APIC_LVT_PMCR  0x340    /* LVT Performance Monitoring Counters Register (R/W). */
#define APIC_LVT_LINT0 0x350    /* LVT LINT0 Register (R/W). */
#define APIC_LVT_LINT1 0x360    /* LVT LINT1 Register (R/W). */
#  define APIC_LVT_LINTn_INPUT_POLARITY         (1 << 13)
#  define APIC_LVT_LINTn_REMOTE_IRR             (1 << 14)
#  define APIC_LVT_LINTn_LEVEL_TRIGGER          (1 << 15)
#define APIC_LVT_ERROR 0x370    /* LVT Error Register (R/W). */
#  define APIC_LVT_MASKED                       (1 << 16)
#  define APIC_LVT_SEND_PENDING                 (1 << 12)
#  define APIC_LVT_DELIVERY_MODE_MASK           0x00000700U

#define APIC_TMICT 0x380        /* Initial Count Register (for Timer) (R/W). */
#define APIC_TMCCT 0x390        /* Current Count Register (for Timer) (RO). */
/* 0x3A0 .. 0x3D0. Reserved. */
#define APIC_TDCR 0x3E0         /* Divide Configuration Register (for Timer) (R/W). */

#define APIC_BASE (fix_to_virt(FIX_APIC_BASE))
#define APIC_BASE_MSR 0x800

#endif /* __X86_ASM_APIC_DEFS_H__ */
