##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libdata.la
libdata_la_SOURCES = CellularState.cpp \
		DynSysData.cpp \
		InitialStates.cpp \
		InitialStatesResetter.cpp \
		OrbitResetter.cpp \
		ParameterResetter.cpp \
		ScanData.cpp \
		ScannableObjects.cpp

includedir = $(ANT_INCLUDEPATH)/engine/data
include_HEADERS = CellularState.hpp \
		DynSysData.hpp \
		InitialStates.hpp \
		InitialStatesResetter.hpp \
		Orbit.hpp \
		OrbitResetter.hpp \
		ParameterResetter.hpp \
		ScanData.hpp \
		ScannableObjects.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
