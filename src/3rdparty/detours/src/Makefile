##############################################################################
##
##  Makefile for Detours.
##
##  Microsoft Research Detours Package, Version 4.0.1
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##

#DETOURS_DEBUG="debug"

DETOURS_NAME=detours-$(DETOURS_TARGET_PROCESSOR)$(DETOURS_CONFIG)
ROOT = ..
!include "$(ROOT)\system.mak"

#######################/#######################################################
##
CFLAGS=/nologo /W4 /WX /we4777 /we4800 /Zi /MT /Gy /Gm- /Zl /Od /DDETOUR_DEBUG=$(DETOURS_DEBUG)

CFLAGS=$(CFLAGS) /DWIN32_LEAN_AND_MEAN /D_WIN32_WINNT=0x0601 $(DETOURS_DEFINITION)

!if defined(DETOURS_ANALYZE)
CFLAGS=$(CFLAGS) /analyze
!endif

OBJS = \
    $(OBJD)\detours.obj     \
    $(OBJD)\disasm.obj      \

##############################################################################
##
.SUFFIXES: .cpp .h .obj

!ifdef DETOURS_ANALYZE
.cpp{$(OBJD)}.obj:
    $(CC) $(CFLAGS) /Fd$(BIND)\$(DETOURS_NAME).pdb /Fo$(OBJD)\ /c $<
!else
.cpp{$(OBJD)}.obj::
    $(CC) $(CFLAGS) /Fd$(BIND)\$(DETOURS_NAME).pdb /Fo$(OBJD)\ /c $<
!endif

##############################################################################

all: dirs \
    $(BIND)\$(DETOURS_NAME).lib \
    $(BIND)\detours.h   \

##############################################################################

clean:
    -del *~ 2>nul
    -del $(BIND)\$(DETOURS_NAME).pdb $(BIND)\$(DETOURS_NAME).pdb 2>nul
    -rmdir /q /s $(OBJD) 2>nul

realclean: clean
    -rmdir /q /s $(OBJDS) 2>nul

##############################################################################

dirs:
    @if not exist "$(BIND)" mkdir "$(BIND)" && echo.   Created $(BIND)
    @if not exist "$(OBJD)" mkdir "$(OBJD)" && echo.   Created $(OBJD)

$(BIND)\$(DETOURS_NAME).lib : $(OBJS)
    link /lib /MACHINE:$(DETOURS_TARGET_PROCESSOR) /out:$@ /nologo $(OBJS)

$(BIND)\detours.h : detours.h
    copy detours.h $@

$(OBJD)\detours.obj : detours.cpp detours.h
$(OBJD)\disasm.obj : disasm.cpp detours.h


################################################################# End of File.
