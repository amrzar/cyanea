#!/bin/bash
# SPDX-License-Identifier: GPL-2.0-or-later

[[ $# -lt 1 ]] && {
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

        if [[ -v DEBUG ]]; then
            set -x
            nohup qemu-system-${QEMU} \
                -kernel $KERNEL \
                -initrd $INITRD \
                -m 8192 \
                -smp ${NR_CPUS} \
                -cpu ${PROCESSOR%:*} \
                -nographic \
                -append "console=uart,io,0x3f8,9600 memblock=debug" \
                "$@" "${DEBUG_OPTIONS[@]}" &> /dev/null &
        elif [[ -v NUMA ]]; then
            #
            # ┌────────┐    ┌────────┐
            # | Node 0 ├─15─┤ Node 1 |
            # └───┬────┘    └───┬────┘
            #     |     \  /    |
            #     20     20     20
            #     |     /  \    |
            # ┌───┴────┐    ┌───┴────┐
            # | Node 2 ├─15─┤ Node 3 |
            # └────────┘    └────────┘
            #
            qemu-system-${QEMU} \
                -kernel $KERNEL \
                -initrd $INITRD \
                -m 8192 \
                -smp cpus=${NR_CPUS} \
                -numa node,cpus=0-3,nodeid=0 \
                -numa node,cpus=4-7,nodeid=1 \
                -numa node,cpus=8-11,nodeid=2 \
                -numa node,cpus=12-15,nodeid=3 \
                -numa dist,src=0,dst=1,val=15 \
                -numa dist,src=2,dst=3,val=15 \
                -numa dist,src=0,dst=2,val=20 \
                -numa dist,src=0,dst=3,val=20 \
                -numa dist,src=1,dst=2,val=20 \
                -numa dist,src=1,dst=3,val=20 \
                -cpu ${PROCESSOR%:*} \
                -nographic \
                -append "console=uart,io,0x3f8,9600 memblock=debug" \
                "$@" 
        else
            qemu-system-${QEMU} \
                -kernel $KERNEL \
                -initrd $INITRD \
                -m 8192 \
                -smp ${NR_CPUS} \
                -cpu ${PROCESSOR%:*} \
                -nographic \
                -append "console=uart,io,0x3f8,9600 memblock=debug" \
                "$@"            
        fi
        ;;

    *)
        echo "Unsupported architecture."
        exit 1
esac
