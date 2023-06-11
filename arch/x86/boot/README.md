
                            [voffset.h] ... ukernel.elf
                                                │
                                                ├─ compress (gzip)
                                                │
                                                ▼
                                         ukernel.elf.gz
                                                │
                                                ├─ mkpiggy (mkpiggy.S)
                                                │
                                                ▼
        Objects from compressed >>>  *.o +  piggy.o
                                      │         │
                                      └────┬────┘
                                           │
                      [zoffset.h] ... ukernel.elf (-pie)
                                           │
                         setup.elf         ├─ Stripped
                            │              │
                            │              ▼
                  Stripped ─┤        ukernel.bin (1)
                            │              │
                            ▼              │
                        setup.bin (2)      │
                            │              │
                            └──────┬───────┘
                                   │
                                   ├─ img-build
                                   │
                                   ▼
                               zukernel

There are two 'ukernel.bin' created in this process. One as position independent
code which is stored in compresses directory (1), let's call it compressed ukernel.bin.
The other (2) is the setup directory.

The boot-loader will starts the compressed ukernel.bin which will move itself
and decompress the main kernel 'ukernel.elf.gz' into it's own place.
