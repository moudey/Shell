##############################################################################
##
##  Establish build target type for Detours.
##
##  Microsoft Research Detours Package
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##

############################################## Determine Processor Build Type.
#
# Default the detours architecture to match the compiler target that
# has been selected by the user via the VS Developer Command Prompt
# they launched.

DETOURS_TARGET_PROCESSOR=arm64

DETOURS_DEFINITION=""

!IF "$(DETOURS_TARGET_PROCESSOR)" == "" && "$(VSCMD_ARG_TGT_ARCH)" != ""
DETOURS_TARGET_PROCESSOR = $(VSCMD_ARG_TGT_ARCH)
!ENDIF

!IF "$(DETOURS_TARGET_PROCESSOR)" == "" && "$(PROCESSOR_ARCHITEW6432)" != ""
DETOURS_TARGET_PROCESSOR = x86
!ENDIF

!IF "$(DETOURS_TARGET_PROCESSOR)" == ""
DETOURS_TARGET_PROCESSOR = $(PROCESSOR_ARCHITECTURE)
!ENDIF

# Uppercase DETOURS_TARGET_PROCESSOR
DETOURS_TARGET_PROCESSOR=$(DETOURS_TARGET_PROCESSOR:A=a)
DETOURS_TARGET_PROCESSOR=$(DETOURS_TARGET_PROCESSOR:D=d)
DETOURS_TARGET_PROCESSOR=$(DETOURS_TARGET_PROCESSOR:I=i)
DETOURS_TARGET_PROCESSOR=$(DETOURS_TARGET_PROCESSOR:M=m)
DETOURS_TARGET_PROCESSOR=$(DETOURS_TARGET_PROCESSOR:R=r)
DETOURS_TARGET_PROCESSOR=$(DETOURS_TARGET_PROCESSOR:X=x)


!IF "$(DETOURS_TARGET_PROCESSOR)" == "amd64"
DETOURS_TARGET_PROCESSOR = x64
!ENDIF


!if "$(DETOURS_TARGET_PROCESSOR:64=)" == "$(DETOURS_TARGET_PROCESSOR)"
DETOURS_32BIT=1
DETOURS_BITS=32
!else
DETOURS_64BIT=1
DETOURS_BITS=64
!endif

########################################## Configure build based on Processor.
##
## DETOURS_OPTION_PROCESSOR: Set this macro if the processor *will* run code
##                           from another ISA (i.e. x86 on x64).
##
##      DETOURS_OPTION_BITS: Set this macro if the processor *may* have
##                           an alternative word size.
##
!IF "$(DETOURS_TARGET_PROCESSOR)" == "x64"
#!MESSAGE Building for 64-bit x64.
DETOURS_SOURCE_BROWSING = 0
DETOURS_OPTION_PROCESSOR=x86
DETOURS_OPTION_BITS=32
DETOURS_DEFINITION=/DDETOURS_X64_OFFLINE_LIBRARY
!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "ia64"
#!MESSAGE Building for 64-bit IA64.
DETOURS_OPTION_PROCESSOR=x86
DETOURS_OPTION_BITS=32
DETOURS_DEFINITION=/DDETOURS_X86_OFFLINE_LIBRARY
!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "x86"
#!MESSAGE Building for 32-bit x86.
DETOURS_OPTION_BITS=64
# Don't set DETOURS_OPTION_PROCESSOR for x64 because we don't *know* that
# we'll run on a 64-bit machine.
!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "arm"
#!MESSAGE Building for 32-bit ARM.
DETOURS_DEFINITION=/DDETOURS_ARM_OFFLINE_LIBRARY
!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "arm64"
#!MESSAGE Building for 64-bit ARM.
DETOURS_DEFINITION=/DDETOURS_ARM64_OFFLINE_LIBRARY
!ELSE
!MESSAGE Note: To select the target processor architecture set either
!MESSAGE       PROCESSOR_ARCHITECTURE or DETOURS_TARGET_PROCESSOR.
!MESSAGE
!ERROR Unknown target processor: "$(DETOURS_TARGET_PROCESSOR)"
!ENDIF

##############################################################################
##
!IF "$(DETOURS_CONFIG)" == "debug"
DETOURS_DEBUG=1
DETOURS_CONFIG="-debug"
!ELSE
DETOURS_DEBUG=0
!ENDIF

BIND = $(ROOT)\bin
OBJD = $(BIND)\obj\$(DETOURS_TARGET_PROCESSOR)$(DETOURS_CONFIG)

BINDS = \
        $(ROOT)\bin

OBJDS = \
        $(BIND)\obj\x86$(DETOURS_CONFIG) \
        $(BIND)\obj\x64$(DETOURS_CONFIG) \
        $(BIND)\obj\arm$(DETOURS_CONFIG) \
        $(BIND)\obj\arm64$(DETOURS_CONFIG) \

##############################################################################
