#!/bin/bash
# SPDX-License-Identifier: GPL-2.0-or-later 

[[ $# -ne 1 ]] && {
    echo "${0##*/}: output file is missing."
    exit 1
}

{
    echo "#ifndef __COMPILE_INFO_H__"
    echo "#define __COMPILE_INFO_H__"
    echo ""
    echo "#define TIMESTAMP \"$(date)\""
    echo "#define COMPILE_BY \"$(whoami | sed 's/\\/\\\\/')\""
    echo "#define COMPILE_HOST \"$(uname -n)\""
    echo ""
    echo "#endif /* __COMPILE_INFO_H__ */"
} > "${1}"
