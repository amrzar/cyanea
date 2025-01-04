/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_PERCPU_H__
#define __CYANEA_PERCPU_H__

#include <cyanea/init.h>
#include <cyanea/pgtable.h>

#define NR_CPUS CONFIG_NR_CPUS

#include <asm/percpu.h>

#define this_cpu_ptr(ptr) arch_raw_cpu_ptr(ptr)

#endif /* __CYANEA_PERCPU_H__ */
