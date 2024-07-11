# SPDX-License-Identifier: GPL-2.0-or-later

#include "setup.h"
#include "../boot.h"

#include "../zoffset.h"

    .code16
    .section ".bstext", "ax"
    .fill   0x1EF, 1, 0x90

    .section ".header", "a"
    .byte   0xFF            # sentinel.
    .byte   0xFF

    #
    # Offset 497, 'hdr' starts at the end of boot-sector.
    #

    .global hdr
hdr:

    .byte   setup_sects - 1 # setup_sects.
    .word   1               # root_flags, default to readonly root (1) (DEPRECATED).
    .long   ZO__edata / 16  # syssize, size of the protected-mode code in units of
                            # 16-byte paragraphs.
    .word   0               # ram_size (OBSOLETE)
    .word   NORMAL_VGA      # vid_mode
    .word   0               # root_dev, default root device device number (DEPRECATED).

    .word   0xAA55          # boot_flag, signature at end of boot-sector.

    #
    # Offset 512, begining of second sector, entry from boot-loader, e.g. GRUB.
    #
    # Here, segment registers 'GS', 'FS', 'ES', 'DS', 'SS' have equal values, any
    # 64KiB segment between 0x1000 and 0xA0000. It covers real-mode image. 'SP' 
    # should be set up properly, normally pointing to the top of the heap, and
    # interrupts are disabled. 
    #
    # 'CS' is equal to 'DS + 0x20', i.e. the code segment starts at the begining
    # of the second sector '_start' and 'IP' is set to zero, real-mode linked in
    # 'setup.lds' at address 0x0.
    #

    .global _start
_start:

    .byte   0xeb                    # Explicitly enter short (2-byte) 'JMP' as bytes,
    .byte   start_of_setup - 1f     # or the assembler may generate a 3-byte 'JMP'.
1:

    .ascii  "HdrS"
    .word   0x020f          # Header version number (>= 0x0105)

    .long   0                       # realmode_swtch
    .word   0                       # start_sys_seg (OBSOLETE)
    .word   kernel_version - 512    # kernel_version, default to no version information.
    .byte   0                       # type_of_loader, 0x00 is pre-2.00 boot-loader.

loadflags:
    .byte   LOADED_HIGH     # loadflags, protected-mode code is loaded at 0x100000.

    .word   0x8000          # setup_move_size, size of real-mode code moved to 0x90000,
                            # in case it is not loaded there. If command-line should
                            # be moved, it must be located inside a same segment as
                            # the real-mode code.

    .long   0x100000        # code32_start, address to 'JMP' in protected-mode.
    .long   0               # ramdisk_image, 'initrd' load address set by boot-loader
    .long   0               # ramdisk_size, 'initrd' size set by boot-loader
    .long   0               # bootsect_kludge (OBSOLETE)

heap_end_ptr:
    .word   0               # Offset to the end of the real-mode heap, used by setup
                            # for local heap purposes, reserved by boot-loader.

    .byte   0, 0            # ext_loader_ver, ext_loader_type

    .long   0               # cmd_line_ptr, address of the command-line. Using this
                            # field, command-line can be located anywhere after the
                            # end of real-mode code heap up to 0xA0000. It does not
                            # have to be located in the same segment as the real-mode
                            # code itself.

    .long   0x7FFFFFFF      # initrd_addr_max, maximum address that may be occupied
                            # by the initial ramdisk/ramfs. Use Linux defaults.
    
    #
    # Boot-loaders who want to load the kernel at a nonstandard address should
    # fill in the fields marked (RELOC); other boot-loaders can ignore those fields.
    #

    .long   CONFIG_PHYSICAL_ALIGN   # kernel_alignment (RELOC), preferred alignment.
    
#ifdef CONFIG_RELOCATABLE
    .byte   1               # relocatable_kernel (RELOC), the protected-mode part of
                            # the ukernel can be loaded at any address that satisfies
                            # the 'kernel_alignment' field. The boot-loader must set 
                            # the 'code32_start' field.
#else
    .byte   0
#endif /* CONFIG_RELOCATABLE */
    .byte   BOOT_ALIGN_SHIFT        # min_alignment (RELOC), minimum alignment.
                                    # If a boot-loader uses this field, it should update
                                    # 'kernel_alignment' with the alignment unit used.

xloadflags:
    #ifdef CONFIG_X86_64
    #ifdef CONFIG_RELOCATABLE
#define XFLAG (XLF_CAN_BE_LOADED_ABOVE_4G | XLF_KERNEL_64)
    #else
#define XFLAG XLF_KERNEL_64
    #endif /* CONFIG_RELOCATABLE */
    #else
#define XFLAG 0
    #endif /* CONFIG_X86_64 */

    .word   XFLAG                   # xloadflags, 'XLF_EFI_KEXEC' is zero.

    .long   COMMAND_LINE_SIZE - 1   # cmdline_size, maximum size of the command-line.

    .long   0               # hardware_subarch, default to X86_SUBARCH_PC for x86 PC.
    .quad   0               # hardware_subarch_data, unused for the default x86 PC.

    .long   ZO_input_data           # payload_offset
    .long   ZO_z_input_len          # payload_length

    .quad   SETUP_NONE              # setup_data, linked list of 'struct setup_data'.
    .quad   CONFIG_PHYSICAL_START   # pref_address (RELOC)

    #
    # Calculate 'init_size' for 'ukernel.bin' in 'compressed directory'
    #
    # The compressed file consists of a short header and an arbitrary number of
    # compressed blocks. Three different block encoding are currently used in
    # deflate: 'stored', 'static', and 'dynamic'. The smallest block type encoding
    # is always used. 'stored' blocks contains uncompressed data with maximum
    # size of ''32768 - 1'' (minus one is for end-of-block literal).
    #
    # The worst case overhead is 18 bytes for the file header, plus 5 bytes every
    # 32KiB block -- assuming 'stored' block. As a result, to decompress in-place,
    # two cases should be considered:
    #
    #    (1) compressed data 'ZO_z_input_len' is larger than uncompressed data
    #        'ZO_z_output_len' or equal ''OFFSET = 18 + 5 bytes-per-32KiB + 32KiB''.
    #        Last 32KiB is added so after decompression of a block, it does not
    #        overrun the compressed data.
    #
    #    (2) compressed data 'ZO_z_input_len' is smaller then uncompressed data
    #        ''OFFSET + uncompressed data - compressed data''
    #
    # See https://www.rfc-editor.org/rfc/rfc1952.txt
    # See https://www.rfc-editor.org/rfc/rfc1951.txt
    #

#define ZO_z_extra_bytes (18 + (ZO_z_output_len >> 15 << 3) + 32768)
#if ZO_z_output_len > ZO_z_input_len
#define ZO_z_extract_offset (ZO_z_extra_bytes + ZO_z_output_len - ZO_z_input_len)
#else
#define ZO_z_extract_offset ZO_z_extra_bytes
#endif

    #
    # The boot-loader reserves 'init_size' linear contiguous memory starting at the
    # runtime start address of the 'ukernel.bin'.
    #
    # This buffer is used for in-place decompression of the ukernel. The code running
    # from the 'ukernel.bin''s head under 'ZO__ehead' address moves the 'ukernel.bin',
    # i.e. itself, to the end of this buffer. It copies memory from '_bss' backward
    # to the current instruction; see 'ukernel.lds.S' and 'head_64.S' in compressed directory.
    #
    # 'init_size' should be large enough so that the copy operation running form
    # the head does not overwrite the head ''itself''.
    #

#if (ZO__ehead - ZO_startup_32) > ZO_z_extract_offset
#define ZO_z_min_extract_offset (ZO__ehead - ZO_startup_32)
#else
#define ZO_z_min_extract_offset ZO_z_extract_offset
#endif

    #
    # 'init_size' should be large enough for the final decompressed ukernel.
    #

#define ZO_INIT_SIZE (ZO__end - ZO_startup_32 + ZO_z_min_extract_offset)
#define VO_INIT_SIZE (VO__end - VO__text)
#if ZO_INIT_SIZE > VO_INIT_SIZE
#define __INIT_SIZE ZO_INIT_SIZE
#else
#define __INIT_SIZE VO_INIT_SIZE
#endif

    #
    # Align 'init_size' to a page, so that when calculating the relocation address in
    # 'head_64.S', the result remains page aligned. It is required for initialising
    # identity mappings.
    #

#define INIT_SIZE ((__INIT_SIZE  + 0xFFF) & ~0xFFF)
    .long   INIT_SIZE       # init_size, amount of linear contiguous memory starting
                            # at the kernel runtime start address that the ukernel
                            # needs before it is capable of examining its memory map.

    .long   0               # handover_offset.
    .long   0               # kernel_info_offset, filled it at compile time.

    #
    # End of setup header.
    #

    .section ".entrytext", "ax"
start_of_setup:

    movw    %ds, %ax        # 'AX' stores 'DS' for reuse in following codes.
    movw    %ax, %es        # Set 'ES' to 'DS'.

    movw    %ss, %dx
    cmpw    %ax, %dx
    movw    %sp, %dx
    je      2f              # Success: 'SS' is equal to 'DS'

    #
    # Invalid 'SS' register, set up a new stack. If 'CAN_USE_HEAP' is set then
    # 'heap_end_ptr' contains the pointer to the end of real-mode heap.
    #

    movw    %ax, %ss        # Set 'SS' to 'DS'.

    movw    $_end, %dx
    testb   $CAN_USE_HEAP, loadflags
    jz      1f
    movw    heap_end_ptr, %dx
1:  addw    $SETUP_STACK_SIZE, %dx
    jnc     2f
    xorw    %dx, %dx        # Handle overflow!

2:  
    andw    $~3, %dx        # 'DX' points to the end of stack, dword align, and
    jnz     3f              # make sure it is not zero.
    movw    $0xFFFC, %dx
3:  movzwl  %dx, %esp

    sti

    pushw   %ds             # Normalise 'CS' to 'DS', see above.
    pushw   $4f
    lretw
4:

    # Reset BSS.
    cld
    xorl    %eax, %eax
    movw    $__bss_start, %di
    movw    $__bss_end, %cx
    subw    %di, %cx
    shrw    $2, %cx
    rep; stosl

    jmp     __c_setup       # Finally, 'JMP' to C setup code.
