/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __X86_ASM_PERCPU_H__
#define __X86_ASM_PERCPU_H__

#ifndef __ASSEMBLY__

/* PER-CPU SECTIONS. */

#define __percpu __section(".data..percpu")
#define __percpu_cache_aligned \
    __section(".data..percpu") __aligned(L1_CACHE_BYTES)
#define __percpu_page_aligned \
    __section(".data..percpu..page_aligned") __aligned(PAGE_SIZE)

extern unsigned long this_cpu_off __percpu;

# define arch_this_cpu_ptr(ptr) ({ \
        unsigned long pcpu_ptr; \
        \
        asm ("add %%gs:%1, %0" \
            : "=r" (pcpu_ptr) \
            : "m" (this_cpu_off), "0" (ptr)); \
        \
        (typeof(ptr))(pcpu_ptr); \
    })

void __bad_percpu_size(void);

# define percpu_to_op(op, var, val) do { \
        switch(sizeof(var)) { \
        case 1: \
            asm volatile(op "b %1, %%gs:%0" \
                : "+m" (var) \
                : "qi" ((typeof(var))(val))); \
            break; \
        case 2: \
            asm volatile(op "w %1, %%gs:%0" \
                : "+m" (var) \
                : "ri" ((typeof(var))(val))); \
            break; \
        case 4: \
            asm volatile(op "l %1, %%gs:%0" \
                : "+m" (var) \
                : "ri" ((typeof(var))(val))); \
            break; \
        case 8: \
            asm volatile(op "q %1, %%gs:%0" \
                : "+m" (var) \
                : "re" ((typeof(var))(val))); \
            break; \
        default: __bad_percpu_size(); \
        } \
    } while (0)

# define percpu_from_op(op, var) ({ \
        typeof(var) pcp_from_val__; \
        switch(sizeof(var)) { \
        case 1: \
            asm volatile(op "b %%gs:%1, %0" \
                : "=q" (pcp_from_val__) \
                : "m" (var)); \
            break; \
        case 2: \
            asm volatile(op "w %%gs:%1, %0" \
                : "=r" (pcp_from_val__) \
                : "m" (var)); \
            break; \
        case 4: \
            asm volatile(op "l %%gs:%1, %0" \
                : "=r" (pcp_from_val__) \
                : "m" (var)); \
            break; \
        case 8: \
            asm volatile(op "q %%gs:%1, %0" \
                : "=r" (pcp_from_val__) \
                : "m" (var)); \
            break; \
        default: __bad_percpu_size(); \
        } \
        pcp_from_val__; \
    })

/* For X86_64 all these operations are atomic. */
/* Keep 'this_cpu_*' operations as volative. */

# define this_cpu_read(pcp) percpu_from_op("mov", (pcp))
# define this_cpu_write(pcp, val) percpu_to_op("mov", (pcp), val)

#else /* !__ASSEMBLY__ */

#endif /* __ASSEMBLY__ */

#endif /* __X86_ASM_PERCPU_H__ */
