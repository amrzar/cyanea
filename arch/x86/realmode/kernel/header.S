# SPDX-License-Identifier: GPL-2.0-or-later

    .section ".header", "a"

#
# Header to export details of the real-mode kernel 'struct real_mode_header { ... }'.
#

    .global real_mode_header
real_mode_header:

    .long   pa_ap_config
    .long   pa_trampoline_pgd

    .type   real_mode_header, @object
    .size   real_mode_header, . - real_mode_header
