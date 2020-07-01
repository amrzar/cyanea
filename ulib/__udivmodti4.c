/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/types.h>
#include <cyanea/stddef.h>

typedef union {
    u64 U;
    struct {                    /* Split 'u64'. */
#ifdef CONFIG_CPU_LITTLE_ENDIAN
        u32 lo, hi;
#else
        u32 hi, lo;
#endif
    } U32;

    s64 S;
    struct {                    /* Split 's64'. */
#ifdef CONFIG_CPU_LITTLE_ENDIAN
        s32 lo, hi;
#else
        s32 hi, lo;
#endif
    } S32;
} arith64_t;

/* **/

unsigned long long __udivmodti4(unsigned long long a, unsigned long long b,
    unsigned long long *c)
{
    if (b > a) {
        if (c != NULL)
            *c = a;
        return 0;
    }

    arith64_t nu_a = {.U = a };
    arith64_t di_b = {.U = b };

    if (di_b.U32.hi == 0) {
        if (b == 0) {

        }
        if (b == 1) {
            if (c != NULL)
                *c = 0;
            return a;
        }
        if (nu_a.U32.hi == 0) {
            if (c != NULL)
                *c = nu_a.U32.lo % di_b.U32.lo;
            return nu_a.U32.lo / di_b.U32.lo;
        }
    }

    /* Let's do ''binary'' long division. */

    int lz = __builtin_clzll(b);
    unsigned long long quot = 0;
    unsigned long long q = (1 << lz);
    b <<= lz;

    while (q > 0) {
        if (b <= a) {
            a -= b;
            quot += q;
        }
        b >>= 1;
        q >>= 1;
    }

    if (c != NULL)
        *c = a;

    return quot;
}
