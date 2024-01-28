##############################################################################
##
##  Makefile for Detours.
##
##  Microsoft Research Detours Package
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##

ROOT = .
!include "$(ROOT)\system.mak"

all:
    cd "$(MAKEDIR)\src"
    @$(MAKE) /NOLOGO /$(MAKEFLAGS)

clean:
    cd "$(MAKEDIR)\src"
    @$(MAKE) /NOLOGO /$(MAKEFLAGS) clean

realclean: clean
    cd "$(MAKEDIR)\src"
    @$(MAKE) /NOLOGO /$(MAKEFLAGS) realclean

    -rmdir /q /s $(BINDS) 2> nul
    -rmdir /q /s dist 2> nul

    -del /q /f /s *~ 2>nul

################################################################# End of File.
