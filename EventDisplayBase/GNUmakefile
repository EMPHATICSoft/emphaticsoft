#
# $Id: GNUmakefile,v 1.11 2012-09-20 21:38:32 greenc Exp $
#
# messier@indiana.edu
#

ROOTCINT    := YES
PACKAGE     := EventDisplayBase
LIB_TYPE    := shared
LIB         := lib$(PACKAGE)
LIBCXXFILES := $(wildcard *.cxx)
JOBFILES    := $(wildcard *.fcl)

ROOTLIBS  = $(shell root-config --evelibs)

override LDFLAGS += $(shell root-config --evelibs) 

override LOADLIBES += -L$(ART_LIB) -lart_Framework_IO_Root_RootInput_source $(ROOTLIBS)

LIBLINK     := -L$(SRT_PRIVATE_CONTEXT)/lib/$(SRT_SUBDIR) -L$(SRT_PUBLIC_CONTEXT)/lib/$(SRT_SUBDIR) -l$(PACKAGE)

# SUBDIRS     := test

########################################################################
include SoftRelTools/standard.mk
include SoftRelTools/arch_spec_art.mk
include SoftRelTools/arch_spec_root.mk

override LIBLIBS += $(LOADLIBES) 
