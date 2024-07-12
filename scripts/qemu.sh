#!/bin/bash

[[ $# -lt 2 ]] && {
    echo "Usage: ${0##*/} KERNEL INITRD OPTIONS"
    exit 1
}

KERNEL="${1}"
shift 1
INITRD="${1}"
shift 1

DEBUG_OPTIONS=("-s" "-S")

echo "(Starting QEMU)"

case ${ARCH} in
    x86)
        [[ -v X86_64 ]] && QEMU=x86_64
            qemu-system-${QEMU} \
                -bios ${OVMF:-/usr/share/ovmf/OVMF.fd} \
                -kernel ${KERNEL} \
                -initrd ${INITRD} \
                -m 8192 \
                -smp ${NR_CPUS} \
                -cpu ${PROCESSOR%:*} \
                -serial pty \
                "$@"            
        ;;

    *)
        echo "Unsupported architecture."
        exit 1
esac
