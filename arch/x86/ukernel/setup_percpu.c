/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/init.h>
#include <cyanea/percpu.h>

/* '__per_cpu_offset' is the offset that has to be added to a
 * per-cpu variable to get to the instance for a certain processor. **/

unsigned long __per_cpu_offset[NR_CPUS] __ro_after_init = { 0 };
