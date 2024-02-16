# SPDX-License-Identifier: GPL-2.0-or-later

acpica = acpica
components = $(acpica)/source/components

cflags := -I$(acpica)/source/include
cflags += -U__linux__
cflags += -Wno-unused-but-set-variable

# List of files in the ''acpica''

## - DEBUGGER.
# obj-y := $(components)/debugger/dbcmds.o
# obj-y := $(components)/debugger/dbconvert.o
# obj-y := $(components)/debugger/dbdisply.o
# obj-y := $(components)/debugger/dbexec.o
# obj-y := $(components)/debugger/dbfileio.o
# obj-y := $(components)/debugger/dbhistry.o
# obj-y := $(components)/debugger/dbinput.o
# obj-y := $(components)/debugger/dbmethod.o
# obj-y := $(components)/debugger/dbnames.o
# obj-y := $(components)/debugger/dbobject.o
# obj-y := $(components)/debugger/dbstats.o
# obj-y := $(components)/debugger/dbtest.o
# obj-y := $(components)/debugger/dbutils.o
# obj-y := $(components)/debugger/dbxface.o

## - DISASSEMBLER.
# obj-y := $(components)/disassembler/dmbuffer.o
# obj-y := $(components)/disassembler/dmcstyle.o
# obj-y := $(components)/disassembler/dmdeferred.o
# obj-y := $(components)/disassembler/dmnames.o
# obj-y := $(components)/disassembler/dmopcode.o
# obj-y := $(components)/disassembler/dmresrc.o
# obj-y := $(components)/disassembler/dmresrcl.o
# obj-y := $(components)/disassembler/dmresrcl2.o
# obj-y := $(components)/disassembler/dmresrcs.o
# obj-y := $(components)/disassembler/dmutils.o
# obj-y := $(components)/disassembler/dmwalk.o

##  - DISPATCHER.
obj-y := $(components)/dispatcher/dsargs.o
obj-y += $(components)/dispatcher/dscontrol.o
obj-y += $(components)/dispatcher/dsdebug.o
obj-y += $(components)/dispatcher/dsfield.o
obj-y += $(components)/dispatcher/dsinit.o
obj-y += $(components)/dispatcher/dsmethod.o
obj-y += $(components)/dispatcher/dsmthdat.o
obj-y += $(components)/dispatcher/dsobject.o
obj-y += $(components)/dispatcher/dsopcode.o
obj-y += $(components)/dispatcher/dspkginit.o
obj-y += $(components)/dispatcher/dsutils.o
obj-y += $(components)/dispatcher/dswexec.o
obj-y += $(components)/dispatcher/dswload.o
obj-y += $(components)/dispatcher/dswload2.o
obj-y += $(components)/dispatcher/dswscope.o
obj-y += $(components)/dispatcher/dswstate.o

##  - EVENTS.
obj-y += $(components)/events/evevent.o
obj-y += $(components)/events/evglock.o
obj-y += $(components)/events/evgpe.o
obj-y += $(components)/events/evgpeblk.o
obj-y += $(components)/events/evgpeinit.o
obj-y += $(components)/events/evgpeutil.o
obj-y += $(components)/events/evhandler.o
obj-y += $(components)/events/evmisc.o
obj-y += $(components)/events/evregion.o
obj-y += $(components)/events/evrgnini.o
obj-y += $(components)/events/evsci.o
obj-y += $(components)/events/evxface.o
obj-y += $(components)/events/evxfevnt.o
# obj-y += $(components)/events/evxfgpe.o
obj-y += $(components)/events/evxfregn.o

##  - EXECUTER.
obj-y += $(components)/executer/exconcat.o
obj-y += $(components)/executer/exconfig.o
obj-y += $(components)/executer/exconvrt.o
obj-y += $(components)/executer/excreate.o
# obj-y += $(components)/executer/exdebug.o
# obj-y += $(components)/executer/exdump.o
obj-y += $(components)/executer/exfield.o
obj-y += $(components)/executer/exfldio.o
obj-y += $(components)/executer/exmisc.o
obj-y += $(components)/executer/exmutex.o
obj-y += $(components)/executer/exnames.o
obj-y += $(components)/executer/exoparg1.o
obj-y += $(components)/executer/exoparg2.o
obj-y += $(components)/executer/exoparg3.o
obj-y += $(components)/executer/exoparg6.o
obj-y += $(components)/executer/exprep.o
obj-y += $(components)/executer/exregion.o
obj-y += $(components)/executer/exresnte.o
obj-y += $(components)/executer/exresolv.o
obj-y += $(components)/executer/exresop.o
obj-y += $(components)/executer/exserial.o
obj-y += $(components)/executer/exstore.o
obj-y += $(components)/executer/exstoren.o
obj-y += $(components)/executer/exstorob.o
obj-y += $(components)/executer/exsystem.o
obj-y += $(components)/executer/extrace.o
obj-y += $(components)/executer/exutils.o

##  - HARDWARE.
obj-y += $(components)/hardware/hwacpi.o
# obj-y += $(components)/hardware/hwesleep.o
obj-y += $(components)/hardware/hwgpe.o
obj-y += $(components)/hardware/hwpci.o
obj-y += $(components)/hardware/hwregs.o
# obj-y += $(components)/hardware/hwsleep.o
# obj-y += $(components)/hardware/hwtimer.o
obj-y += $(components)/hardware/hwvalid.o
obj-y += $(components)/hardware/hwxface.o
# obj-y += $(components)/hardware/hwxfsleep.o

##  - NAMESPACE.
obj-y += $(components)/namespace/nsaccess.o
obj-y += $(components)/namespace/nsalloc.o
obj-y += $(components)/namespace/nsarguments.o
obj-y += $(components)/namespace/nsconvert.o
# obj-y += $(components)/namespace/nsdump.o
# obj-y += $(components)/namespace/nsdumpdv.o
obj-y += $(components)/namespace/nseval.o
obj-y += $(components)/namespace/nsinit.o
obj-y += $(components)/namespace/nsload.o
obj-y += $(components)/namespace/nsnames.o
obj-y += $(components)/namespace/nsobject.o
obj-y += $(components)/namespace/nsparse.o
obj-y += $(components)/namespace/nspredef.o
obj-y += $(components)/namespace/nsprepkg.o
obj-y += $(components)/namespace/nsrepair.o
obj-y += $(components)/namespace/nsrepair2.o
obj-y += $(components)/namespace/nssearch.o
obj-y += $(components)/namespace/nsutils.o
obj-y += $(components)/namespace/nswalk.o
obj-y += $(components)/namespace/nsxfeval.o
obj-y += $(components)/namespace/nsxfname.o
obj-y += $(components)/namespace/nsxfobj.o

##  - PARSER.
obj-y += $(components)/parser/psargs.o
obj-y += $(components)/parser/psloop.o
obj-y += $(components)/parser/psobject.o
obj-y += $(components)/parser/psopcode.o
obj-y += $(components)/parser/psopinfo.o
obj-y += $(components)/parser/psparse.o
obj-y += $(components)/parser/psscope.o
obj-y += $(components)/parser/pstree.o
obj-y += $(components)/parser/psutils.o
obj-y += $(components)/parser/pswalk.o
obj-y += $(components)/parser/psxface.o

##  - RESOURCES.
# obj-y += $(components)/resources/rsaddr.o
# obj-y += $(components)/resources/rscalc.o
# obj-y += $(components)/resources/rscreate.o
# obj-y += $(components)/resources/rsdump.o
# obj-y += $(components)/resources/rsdumpinfo.o
# obj-y += $(components)/resources/rsinfo.o
# obj-y += $(components)/resources/rsio.o
# obj-y += $(components)/resources/rsirq.o
# obj-y += $(components)/resources/rslist.o
# obj-y += $(components)/resources/rsmemory.o
# obj-y += $(components)/resources/rsmisc.o
# obj-y += $(components)/resources/rsserial.o
# obj-y += $(components)/resources/rsutils.o
# obj-y += $(components)/resources/rsxface.o

##  - TABLES.
obj-y += $(components)/tables/tbdata.o
obj-y += $(components)/tables/tbfadt.o
obj-y += $(components)/tables/tbfind.o
obj-y += $(components)/tables/tbinstal.o
obj-y += $(components)/tables/tbprint.o
obj-y += $(components)/tables/tbutils.o
obj-y += $(components)/tables/tbxface.o
# obj-y += $(components)/tables/tbxfload.o
obj-y += $(components)/tables/tbxfroot.o

##  - UTILITIES.
obj-y += $(components)/utilities/utaddress.o
obj-y += $(components)/utilities/utalloc.o
obj-y += $(components)/utilities/utascii.o
obj-y += $(components)/utilities/utbuffer.o
obj-y += $(components)/utilities/utcache.o
obj-y += $(components)/utilities/utcksum.o
obj-y += $(components)/utilities/utclib.o
obj-y += $(components)/utilities/utcopy.o
obj-y += $(components)/utilities/utdebug.o
obj-y += $(components)/utilities/utdecode.o
obj-y += $(components)/utilities/utdelete.o
obj-y += $(components)/utilities/uterror.o
obj-y += $(components)/utilities/uteval.o
obj-y += $(components)/utilities/utexcep.o
obj-y += $(components)/utilities/utglobal.o
obj-y += $(components)/utilities/uthex.o
obj-y += $(components)/utilities/utids.o
obj-y += $(components)/utilities/utinit.o
obj-y += $(components)/utilities/utlock.o
obj-y += $(components)/utilities/utmath.o
obj-y += $(components)/utilities/utmisc.o
obj-y += $(components)/utilities/utmutex.o
obj-y += $(components)/utilities/utnonansi.o
obj-y += $(components)/utilities/utobject.o
obj-y += $(components)/utilities/utosi.o
obj-y += $(components)/utilities/utownerid.o
obj-y += $(components)/utilities/utpredef.o
obj-y += $(components)/utilities/utprint.o
obj-y += $(components)/utilities/utresdecode.o
obj-y += $(components)/utilities/utresrc.o
obj-y += $(components)/utilities/utstate.o
obj-y += $(components)/utilities/utstring.o
obj-y += $(components)/utilities/utstrsuppt.o
obj-y += $(components)/utilities/utstrtoul64.o
obj-y += $(components)/utilities/uttrack.o
obj-y += $(components)/utilities/utuuid.o
obj-y += $(components)/utilities/utxface.o
obj-y += $(components)/utilities/utxferror.o
obj-y += $(components)/utilities/utxfinit.o
obj-y += $(components)/utilities/utxfmutex.o

obj-y += $(acpica)/source/os_specific/service_layers/oscyanea.o

obj-y += acpi.o

include $(srctree)/scripts/makefile.build
cmd-info =
