# SPDX-License-Identifier: GPL-2.0-or-later

#include "pe.h"

    .section ".head", "a"

    .global     __efi_header
__efi_header:
.Ldoshdr:
    .short   MZ_MAGIC               # (''MZ''), MS-DOS header.

    .org    .Ldoshdr + 0x38
    .long   LINUX_PE_MAGIC          # EFI bootable Linux kernel image.
    .long   .Lpehdr - .Ldoshdr      # Pointer to PE header.

.Lpehdr:
    .long   PE_MAGIC                # (''PE\0\0''), COFF header. 
    .short  MACHINE_TYPE            # Machine.
    .short  .Lsection_count         # NumberOfSection. 
    .long   0                       # TimeDateStamp.
    .long   0                       # PointerToSymbolTable (DEPRECATED).
    .long   0                       # NumberOfSymbolTable (DEPRECATED).
    .short  .Lsection_table - .Loptional_header     # SizeOptionalHeader. 
    .short  IMAGE_FILE_DEBUG_STRIPPED | \
            IMAGE_FILE_EXECUTABLE_IMAGE | \
            IMAGE_FILE_LINE_NUMS_STRIPPED           # Characteristics.

.Loptional_header:
## Standard COFF Fields.
    .short  PE_OPT_MAGIC_PE32PLUS
    .byte   0       # MajorLinkerVersion.
    .byte   0       # MinorLinkerVersion.
    .long   _etext - .Lefi_header_end               # SizOfCode.
    .long   __data_size                             # SizeOfInitialisedData (see README.md).
    .long   0                                       # SizeOfUninitialisedData.
    .long   __efi_entry - .Ldoshdr                  # AddressOfEntryPoint (RVA).
    .long   .Lefi_header_end - .Ldoshdr             # BaseOfCode (RVA).
    .quad   0       # ImageBase.

## Windows Specific Fields.
    .long   4096    # SectionAlignment.
    .long   512     # FileAlignment.
    .short  0       # MajorOperatingSystemVersion.
    .short  0       # MinorOperatingSystemVersion.
    .short  LINUX_EFISTUB_MAJOR_VERSION             # MajorImageVersion.
    .short  LINUX_EFISTUB_MINOR_VERSION             # MinorImageVersion.
    .short  0       # MajorSubsystemVersion.
    .short  0       # MinorSubsystemVersion.
    .long   0       # Win32VersionValue (zeros filled).
    .long   _end - .Ldoshdr                         # SizeOfImage.
    .long   .Lefi_header_end - .Ldoshdr             # SizeOfHeaders.
    .long   0       # Checksum.
    .short  IMAGE_SUBSYSTEM_EFI_APPLICATION         # Subsystem.
    .short  IMAGE_DLL_CHARACTERISTICS_NX_COMPAT     # DllCharacteristics.
    .quad   0       # SizeOfStackReserve.
    .quad   0       # SizeOfStackCommit.
    .quad   0       # SizeOfHeapReserve.
    .quad   0       # SizeOfHeapCommit.
    .long   0       # LoaderFlags (zeros filled).
    .long   (.Lsection_table - .) / 8               # NumberOfRvaAndSizes

## Data Directories.
    .quad   0       # ExportTable (RVA), SizeOfExportTable.
    .quad   0       # ImportTable (RVA), SizeOfImportTable.
    .quad   0       # ResourceTable (RVA), SizeOfResourceTable.
    .quad   0       # ExceptionTable (RVA), SizeOfExceptionTable.
    .quad   0       # CertificationTable (RVA), SizeOfCertificationTable.
    .quad   0       # BaseRelocationTable (RVA), SizeOfBaseRelocationTable.

.Lsection_table:
    .ascii  ".text\0\0\0"
    .long   _etext - .Lefi_header_end               # VirtualSize.
    .long   .Lefi_header_end - .Ldoshdr             # VirtualAddress (RVA).
    .long   _etext - .Lefi_header_end               # SizeOfRawData.
    .long   .Lefi_header_end - .Ldoshdr             # PointerToRawData.

    .long   0, 0    # PointerToRelocations, PointerToLineNumbers.
    .short  0, 0    # NumberOfRelocations, NumberOfLineNumbers.
    .long   IMAGE_SCN_CNT_CODE | \
            IMAGE_SCN_MEM_READ | \
            IMAGE_SCN_MEM_EXECUTE                   # Characteristics.
    
    .ascii  ".data\0\0\0"
    .long   __data_size                             # VirtualSize (see README.md).
    .long   _etext - .Ldoshdr                       # VirtualAddress (RVA).
    .long   __data_rawsize                          # SizeOfRawData (see README.md).
    .long   _etext - .Ldoshdr                       # PointerToRawData.
    
    .long   0, 0    # PointerToRelocations, PointerToLineNumbers.
    .short  0, 0    # NumberOfRelocations, NumberOfLineNumbers.
    .long   IMAGE_SCN_CNT_INITIALIZED_DATA | \
            IMAGE_SCN_MEM_READ | \
            IMAGE_SCN_MEM_WRITE                     # Characteristics.

    .set    .Lsection_count, (. - .Lsection_table) / 40

.p2align    12
    .Lefi_header_end:
