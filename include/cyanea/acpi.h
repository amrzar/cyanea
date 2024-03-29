/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CYANEA_ACPI_H__
#define __CYANEA_ACPI_H__

#include <cyanea/init.h>

#ifndef ACPI_USE_BUILTIN_STDARG
# define ACPI_USE_BUILTIN_STDARG
#endif
# undef __linux__
# include "../../drivers/acpi/acpica/source/include/acpi.h"

extern int acpi_disabled;

int __init acpi_table_init(void);
int __init acpi_table_parse(char *, int (*)(struct acpi_table_header *table));

#endif /* __CYANEA_ACPI_H__ */
