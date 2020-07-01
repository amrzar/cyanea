#!/bin/bash
# SPDX-License-Identifier: GPL-2.0-or-later

[[ $# -lt 1 ]] && {
    echo "Usage: ${0##*/} KERNEL OPTIONS"
    exit 1
}

KERNEL="${1}"
shift 1

DEBUG_OPTIONS=("-s" "-S")

echo "(Starting QEMU)"
case ${ARCH} in
    x86)
        [[ -v X86_32 ]] && QEMU=i386
        [[ -v X86_64 ]] && QEMU=x86_64

        if [[ -v DEBUG ]]; then
            nohup qemu-system-${QEMU} \
                -kernel $KERNEL \
                -m 1024 \
                -nographic \
                -append console=uart,io,0x3f8,9600 \
                "$@" "${DEBUG_OPTIONS[@]}" &> /dev/null &
        else
            qemu-system-${QEMU} \
                -kernel $KERNEL \
                -m 1024 \
                -nographic \
                -append console=uart,io,0x3f8,9600 \
                "$@"            
        fi
        ;;

    *)
        echo "Unsupported architecture."
        exit 1
esac
