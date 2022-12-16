##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libmachines.la
libmachines_la_SOURCES = IterMachine.cpp ScanMachine.cpp StateMachine.cpp

includedir = $(ANT_INCLUDEPATH)/engine/utils/machines

include_HEADERS = IterMachine.hpp ScanMachine.hpp StateMachine.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
