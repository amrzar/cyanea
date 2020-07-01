# SPDX-License-Identifier: GPL-2.0-or-later

#include <uapi/asm/bootparam.h>

#
# All 'kernel_info' data should be part of this structure. Fixed size data have to
# be put before 'kernel_info_var_len_data' label. Variable size data have to be put
# after 'kernel_info_var_len_data' label. Each chunk of variable size data has to
# be prefixed with header/magic and its size.
#
#   kernel_info:
#           .ascii  "LToP"
#           .long   kernel_info_var_len_data - kernel_info
#           .long   kernel_info_end - kernel_info
#           .long   0x01234567
#   kernel_info_var_len_data:
#   example_struct:
#           .ascii  "0123"
#           .long   example_struct_end - example_struct
#           .ascii  "Struct"
#           .long   0x89012345
#   example_struct_end:
#   example_strings:
#           .ascii  "ABCD"
#           .long   example_strings_end - example_strings
#           .asciz  "String_0"
#           .asciz  "String_1"
#   example_strings_end:
#   kernel_info_end:
# 
# Each variable size data header/magic can be any 4-character string, without \0
# at the end of the string.
#

    .section ".rodata.kernel_info", "a"

    .global kernel_info
kernel_info:

    .ascii  "LToP"
    .long   kernel_info_var_len_data - kernel_info
    .long   kernel_info_end - kernel_info
    .long   SETUP_TYPE_MAX

kernel_info_var_len_data:
kernel_info_end:
