# SPDX-License-Identifier: GPL-2.0-or-later

#include "setup.h"
#include "../boot.h"

#include "../zoffset.h"

    #ifndef SVGA_MODE
#define SVGA_MODE ASK_VGA   #
    #endif
 
    #ifndef ROOT_RDONLY
#define ROOT_RDONLY 1       #
    #endif

BOOTSEG     = 0x07c0        # Original boot-sector segment
BIOSSEG     = 0xf000        # BIOS reset segment

    .code16
    .section ".bstext", "ax"

    #
    # Offset 0, beginning of boot-sector.
    #

    .global bootsect_start
bootsect_start:

#
# There are two methods to boot the EFI-compatible ukernel image
#
# Without EFI boot-loader - The ukernel is compiled as a PE/COFF executable.
# It can be run as a standard EFI application with 'efi_pe_entry' as entry point.
#
# With EFI boot-loader - The boot-loader uses the handover code specified
# with 'handover_offset' field in the ukernel header. Here, the EFI boot-loader
# uses two separate entry points to enter the kernel based on the architecture
# it was compiled for,
#
#    (1) 32-bit loader: handover_offset points to 'efi32_stub_entry'
#    (2) 64-bit loader: handover_offset + 512 points to 'efi64_stub_entry'
#
# By making (1) and (2) valid entry points for both 'CONFIG_X86_32' and
# 'CONFIG_X86_64', the image can be used with both 32-bit and 64-bit EFI.
#

#
# See https://docs.microsoft.com/en-us/windows/win32/debug/pe-format
# for specification describing the structure of PE format.
#

#ifdef CONFIG_EFI_STUB

#define EFI_STUB_MAJOR_VERSION 0x1
#define EFI_STUB_MINOR_VERSION 0x0

    .set    pe_machine, 0x8664              # IMAGE_FILE_MACHINE_AMD64
    .set    image_file_extra_flags, 0       # None!
    .set    pe_opt_magic, 0x20b             # Magic: PE32+ format

    .byte   0x4d              # "MZ", MS-DOS header
    .byte   0x5a

#endif /* CONFIG_EFI_STUB */

    #
    # BIOS 'JMP' to the boot-sector at 0000:7C00. However, the code linked
    # 'setup.lds' at address 0x0. 'LJMP' to 07c0:0000 to normalise the address.
    # See https://en.wikibooks.org/wiki/X86_Assembly/Bootloaders
    #

    ljmp    $BOOTSEG, $start2

start2:
    movw    %cs, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss
    xorw    %sp, %sp
    sti

    cld
    movw    $bugger_off_msg, %si

msg_loop:
    lodsb
    andb    %al, %al
    jz      bs_die
    movb    $0xe, %ah
    movw    $7, %bx
    int     $0x10
    jmp     msg_loop

bs_die:
    xorw    %ax, %ax
    int     $0x16           # Wait for user to press any key, and
    int     $0x19           # reboot the computer.

    #
    # 'int $0x19' should never return. In case it does anyway,
    # invoke the BIOS reset code ...
    #

    ljmp    $BIOSSEG, $0xfff0

#ifdef CONFIG_EFI_STUB
    .org    0x3c            # End of MS-DOS header, with pointer
    .long   pe_header       # to begining of PE header.

#endif /* CONFIG_EFI_STUB */

    .section ".bsdata", "a"
bugger_off_msg:
    .ascii  "Use a boot-loader.\r\n"
    .ascii  "\n"
    .ascii  "Remove disk and press any key to reboot ...\r\n"
    .byte   0

#ifdef CONFIG_EFI_STUB
pe_header:
    .ascii  "PE"
    .word   0

    .word   pe_machine
    .word   section_count   # NumberOfSections
    .long   0               # TimeDateStamp
    .long   0               # PointerToSymbolTable, deprecated
    .long   1               # NumberOfSymbols, deprecated
    .word   section_table - optional_header # SizeOfOptionalHeader
    .word   image_file_extra_flags | 0x206  # Characteristics:
                                            # 'image_file_extra_flags' |
                                            # IMAGE_FILE_DEBUG_STRIPPED |
                                            # IMAGE_FILE_EXECUTABLE_IMAGE |
                                            # IMAGE_FILE_LINE_NUMS_STRIPPED

optional_header:
    .word   pe_opt_magic

    .byte   0x02            # MajorLinkerVersion
    .byte   0x14            # MinorLinkerVersion
    .long   0               # SizeOfCode, filled it at compile time.
    .long   0               # SizeOfInitializedData
    .long   0               # SizeOfUninitializedData
    .long   0x0000          # AddressOfEntryPoint, filled it at compile time.
    .long   0x0200          # BaseOfCode, '_start' at begining of second sector.

extra_header_fields:
    .quad   CONFIG_PHYSICAL_START

    .long   0x20            # SectionAlignment
    .long   0x20            # FileAlignment
    .word   0               # MajorOperatingSystemVersion
    .word   0               # MinorOperatingSystemVersion
    .word   EFI_STUB_MAJOR_VERSION  # MajorImageVersion
    .word   EFI_STUB_MINOR_VERSION  # MinorImageVersion
    .word   0               # MajorSubsystemVersion
    .word   0               # MinorSubsystemVersion
    .long   0               # Win32VersionValue
    .long   0               # SizeOfImage, filled it at compile time.
    .long   0x200           # SizeOfHeaders
    .long   0               # CheckSum
    .word   0xa             # IMAGE_SUBSYSTEM_EFI_APPLICATION
    .word   0               # DllCharacteristics

    .quad   0               # SizeOfStackReserve
    .quad   0               # SizeOfStackCommit
    .quad   0               # SizeOfHeapReserve
    .quad   0               # SizeOfHeapCommit

    .long   0               # LoaderFlags
    .long   0x6             # NumberOfRvaAndSizes

    .quad   0               # ExportTable, SizeOfExportTable
    .quad   0               # ImportTable, SizeOfImportTable
    .quad   0               # ResourceTable, SizeOfResourceTable
    .quad   0               # ExceptionTable, SizeOfExceptionTable
    .quad   0               # CertificationTable, SizeOfCertificateTable
    .quad   0               # BaseRelocationTable, SizeOfBaseRelocationTable

section_table:

    #
    # For all sections, 'VirtualSize', total size of the section when loaded
    # into memory, 'VirtualAddress', the address of the section's first byte
    # relative to 'ImageBase' when loaded into memory, 'SizeOfRawData', size
    # of the section for image file, 'PointerToRawData', pointer to the first
    # byte of the section within the image file, are filled at compile time.
    # 

    .ascii  ".setup\0\0"
    .long   0               # VirtualSize
    .long   0x0             # VirtualAddress
    .long   0               # SizeOfRawData
    .long   0x0             # PointerToRawData

    .long   0               # PointerToRelocations
    .long   0               # PointerToLineNumbers
    .word   0               # NumberOfRelocations
    .word   0               # NumberOfLineNumbers
    .long   0x60500020      # Characteristics:
                            # IMAGE_SCN_CNT_CODE |
                            # IMAGE_SCN_ALIGN_16BYTES |
                            # IMAGE_SCN_MEM_EXECUTE |
                            # IMAGE_SCN_MEM_READ

    #
    # The EFI application loader requires a relocation section because
    # EFI applications must be relocatable.
    #

    .ascii  ".reloc\0\0"
    .long   0               # VirtualSize
    .long   0               # VirtualAddress
    .long   0               # SizeOfRawData
    .long   0               # PointerToRawData

    .long   0               # PointerToRelocations
    .long   0               # PointerToLineNumbers
    .word   0               # NumberOfRelocations
    .word   0               # NumberOfLineNumbers
    .long   0x42100040      # Characteristics:
                            # IMAGE_SCN_CNT_INITIALIZED_DATA |
                            # IMAGE_SCN_ALIGN_1BYTES |
                            # IMAGE_SCN_MEM_DISCARDABLE |
                            # IMAGE_SCN_MEM_READ

#ifdef CONFIG_EFI_MIXED

    #
    # The '.compat' section contains the PE machine type and entry point
    # address, allowing firmware implementations to match it to their native
    # machine type, and invoke non-native binaries using a secondary entry point.
    #

    .ascii  ".compat\0"
    .long   0               # VirtualSize
    .long   0x0             # VirtualAddress
    .long   0               # SizeOfRawData
    .long   0x0             # PointerToRawData

    .long   0               # PointerToRelocations
    .long   0               # PointerToLineNumbers
    .word   0               # NumberOfRelocations
    .word   0               # NumberOfLineNumbers
    .long   0x42100040      # Characteristics:
                            # IMAGE_SCN_CNT_INITIALIZED_DATA |
                            # IMAGE_SCN_ALIGN_1BYTES |
                            # IMAGE_SCN_MEM_DISCARDABLE |
                            # IMAGE_SCN_MEM_READ

#endif /* CONFIG_EFI_MIXED */

    .ascii  ".text\0\0\0"
    .long   0               # VirtualSize
    .long   0x0             # VirtualAddress
    .long   0               # SizeOfRawData
    .long   0x0             # PointerToRawData

    .long   0               # PointerToRelocations
    .long   0               # PointerToLineNumbers
    .word   0               # NumberOfRelocations
    .word   0               # NumberOfLineNumbers
    .long   0x60500020      # Characteristics:
                            # IMAGE_SCN_CNT_CODE |
                            # IMAGE_SCN_ALIGN_16BYTES |
                            # IMAGE_SCN_MEM_EXECUTE |
                            # IMAGE_SCN_MEM_READ
 
    .set    section_count, (. - section_table) / 0x28
#endif /* CONFIG_EFI_STUB */

    .section ".header", "a"
    .byte   0xFF
    .byte   0xFF

    #
    # Offset 497, 'hdr' starts at the end of boot-sector.
    #

    .global hdr
hdr:

    .byte   0               # setup_sects, filled it at compile time.
    .word   ROOT_RDONLY     # root_flags, default to readonly root (DEPRECATED).

    .long   0               # syssize, size of the protected-mode code in units of
                            # 16-byte paragraphs, filled it at compile time.

    .word   0               # ram_size (OBSOLETE)
    .word   SVGA_MODE       # vid_mode
    .word   0               # root_dev, default root device device number (DEPRECATED).

    .word   0xAA55          # boot_flag, signature at end of boot-sector.

    #
    # Offset 512, begining of second sector, entry from bootloader, e.g. GRUB.
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
    .byte   0                       # type_of_loader, 0x00 is pre-2.00 bootloader.

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
#define XLF01 (XLF_CAN_BE_LOADED_ABOVE_4G | XLF_KERNEL_64)
    #else
#define XLF01 XLF_KERNEL_64
    #endif /* CONFIG_RELOCATABLE */
    #else
#define XLF01 0
    #endif /* CONFIG_X86_64 */

    #ifdef CONFIG_EFI_STUB
    #ifdef CONFIG_EFI_MIXED
#define XLF23 (XLF_EFI_HANDOVER_32 | XLF_EFI_HANDOVER_64)
    #else
    #ifdef CONFIG_X86_64
#define XLF23 XLF_EFI_HANDOVER_64
    #else
#define XLF23 XLF_EFI_HANDOVER_32
    #endif /* CONFIG_X86_64 */
    #endif /* CONFIG_EFI_MIXED */
    #else
#define XLF23 0
    #endif /* CONFIG_EFI_STUB */
    .word   XLF01 | XLF23           # xloadflags, 'XLF_EFI_KEXEC' is zero.

    .long   COMMAND_LINE_SIZE - 1   # cmdline_size, maximum size of the command-line.

    .long   0               # hardware_subarch, default to X86_SUBARCH_PC for x86 PC.
    .quad   0               # hardware_subarch_data, unused for the default x86 PC.

    .long   ZO_input_data           # payload_offset
    .long   ZO_z_input_len          # payload_length

    .quad   SETUP_NONE              # setup_data
    .quad   CONFIG_PHYSICAL_START   # pref_address (RELOC)

    .long   0               # init_size, amount of linear contiguous memory starting
                            # at the kernel runtime start address that the ukernel
                            # needs before it is capable of examining its memory map,
                            # filled it at compile time.

    .long   0               # handover_offset, filled it at compile time.
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

    #
    # Reset BSS.
    #

    cld
    xorl    %eax, %eax
    movw    $__bss_start, %di
    movw    $__bss_end, %cx
    subw    %di, %cx
    shrw    $2, %cx
    rep; stosl

    jmp     __c_setup       # Finally, 'JMP' to C setup code.
