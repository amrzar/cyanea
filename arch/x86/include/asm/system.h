/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_SYSTEM_H__
#define __X86_ASM_SYSTEM_H__

#ifndef __ASSEMBLY__

/**
 * Use dummy input operand '__FORCE_ORDER' for read functions to prevent
 * reordering CRn write across this read while allowing memory read to be
 * cached or reordered across this read. Use 'memory' clobber for the write
 * functions to prevent reordering and caching memory access across CRn
 * (it is needed as CRn writes affect the semantics of memory accesses).
 * 
 */

#define __FORCE_ORDER "m"(*(unsigned int *)0x1000UL)

static inline unsigned long native_read_cr0(void)
{
    unsigned long val;
    asm volatile ("mov %%cr0, %0":"=r" (val):__FORCE_ORDER);
    return val;
}

static inline void native_write_cr0(unsigned long val)
{
    asm volatile ("mov %0, %%cr0"::"r" (val):"memory");
}

static inline unsigned long native_read_cr2(void)
{
    unsigned long val;
    asm volatile ("mov %%cr2, %0":"=r" (val):__FORCE_ORDER);
    return val;
}

static inline void native_write_cr2(unsigned long val)
{
    asm volatile ("mov %0, %%cr2"::"r" (val):"memory");
}

static inline unsigned long native_read_cr3(void)
{
    unsigned long val;
    asm volatile ("mov %%cr3, %0":"=r" (val):__FORCE_ORDER);
    return val;
}

static inline void native_write_cr3(unsigned long val)
{
    asm volatile ("mov %0, %%cr3"::"r" (val):"memory");
}

static inline unsigned long native_read_cr4(void)
{
    unsigned long val;
    asm volatile ("mov %%cr4, %0":"=r" (val):__FORCE_ORDER);
    return val;
}

static inline void native_write_cr4(unsigned long val)
{
    asm volatile ("mov %0, %%cr4"::"r" (val):"memory");
}

#define read_cr0()      native_read_cr0()
#define write_cr0(x)    native_write_cr0(x)
#define read_cr2()      native_read_cr2()
#define write_cr2(x)    native_write_cr2(x)
#define read_cr3()      native_read_cr3()
#define write_cr3(x)    native_write_cr3(x)
#define read_cr4()      native_read_cr4()
#define write_cr4(x)    native_write_cr4(x)

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_SYSTEM_H__ */
