
                         voffset.h ... arch/x86/ukernel.img
                                                │
                                                ├─ compress (gzip)
                                                │
                                                ▼
                                         ukernel.img.gz
                                                │
                                                ├─ mkpiggy (mkpiggy.S)
                                                │
                                                ▼
                    Objects from >>>  *.o +  piggy.o
                   boot/compress      │         │
                                      └────┬────┘
                                           │
                        zoffset.h ... ukernel.elf (-pie)
                                           │
         Objects from >>> setup.elf        ├─ Stripped
           boot/setup       │              │
                            │              ▼
                  Stripped ─┤        ukernel.bin
                            │              │
                            ▼              │
                         setup.bin         │
                            │              │
                            └──────┬───────┘
                                   │
                                   ├─ img-build
                                   │
                                   ▼
                               zukernel

There are two 'ukerne.bin' created in this process. One as position indipendant code which is stored in compresse directory, let's call it compressed ukernel.bin.
The other is the main kernel. The bootloader will starts the compressed ukernel.bin which will move itself and decompress the main kernel into it's place.
setup.elf is lined at address 0 and is relocated simply by to normalising the address, see header.S.
