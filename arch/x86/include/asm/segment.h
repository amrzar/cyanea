/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_SEGMENT_H__
#define __X86_ASM_SEGMENT_H__

#define GDT_ENTRY_BOOT32_CS 1
#define GDT_ENTRY_BOOT_CS   2
#define GDT_ENTRY_BOOT_DS   3
#define __BOOT32_CS      (GDT_ENTRY_BOOT32_CS * 8)
#define __BOOT_CS        (GDT_ENTRY_BOOT_CS * 8)
#define __BOOT_DS        (GDT_ENTRY_BOOT_DS * 8)

#ifdef CONFIG_X86_32
# define GDT_ENTRY_KERNEL_CS 1
# define GDT_ENTRY_KERNEL_DS 2
# define GDT_ENTRY_DEFAULT_USER_CS 3
# define GDT_ENTRY_DEFAULT_USER_DS 4

# define GDT_ENTRY_TSS 5
# define GDT_ENTRY_LDT 6
# define GDT_ENTRY_PERCPU 7

# define GDT_ENTRIES   16

# define __KERNEL_CS     (GDT_ENTRY_KERNEL_CS * 8)
# define __KERNEL_DS     (GDT_ENTRY_KERNEL_DS * 8)
# define __USER_DS       (GDT_ENTRY_DEFAULT_USER_DS * 8 + 3)
# define __USER_CS       (GDT_ENTRY_DEFAULT_USER_CS * 8 + 3)
# define __KERNEL_PERCPU (GDT_ENTRY_PERCPU * 8)

#else /* 64-bit Segments. */

# define GDT_ENTRY_KERNEL32_CS  1
# define GDT_ENTRY_KERNEL_CS    2
# define GDT_ENTRY_KERNEL_DS    3

/**
 * Supporting ''SYSCALL/SYSRET'' requires specific order in 'GDT'.
 * ''SYSRET'' hardcodes selectors:
 *
 *   cs = STAR.SYSRET_CS if returning to 32-bit userspace,
 *   cs = STAR.SYSRET_CS + 16 if returning to 64-bit userspace,
 *   ss = STAR.SYSRET_CS + 8 (in either case).
 * 
 * USER_DS should be between 32-bit and 64-bit code selectors.
 * 
 **/

# define GDT_ENTRY_DEFAULT_USER32_CS 4
# define GDT_ENTRY_DEFAULT_USER_DS   5
# define GDT_ENTRY_DEFAULT_USER_CS   6

/* System descriptors are occupying the space of two entries. */

# define GDT_ENTRY_TSS 8
# define GDT_ENTRY_LDT 10

# define GDT_ENTRIES   16

# define __KERNEL32_CS   (GDT_ENTRY_KERNEL32_CS * 8)
# define __KERNEL_CS     (GDT_ENTRY_KERNEL_CS * 8)
# define __KERNEL_DS     (GDT_ENTRY_KERNEL_DS * 8)
# define __USER32_CS     (GDT_ENTRY_DEFAULT_USER32_CS * 8 + 3)
# define __USER_DS       (GDT_ENTRY_DEFAULT_USER_DS * 8 + 3)
# define __USER_CS       (GDT_ENTRY_DEFAULT_USER_CS * 8 + 3)
#endif /* CONFIG_X86_32 */

#ifndef __ASSEMBLY__

#ifdef CONFIG_X86_32
#define __load_segment(seg, value) do { \
        unsigned short __value = (value); \
        asm volatile("movl %k0, %%" #seg : "+r" (__value) :: "memory"); \
    } while(0)

#endif /* CONFIG_X86_32 */

#define load_segment(seg, value) __load_segment(seg, value)
#define save_segment(seg, value) \
    asm volatile("mov %%" #seg ", %0" : "=r" (value) :: "memory")

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_SEGMENT_H__ */
