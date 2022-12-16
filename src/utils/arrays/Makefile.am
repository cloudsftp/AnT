##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libarrays.la
libarrays_la_SOURCES = RealVector.cpp

include_HEADERS = Array.hpp CyclicArray.hpp Subarray.hpp Indexable.hpp
noinst_HEADERS = RealVector.hpp

includedir = $(ANT_INCLUDEPATH)/utils/arrays

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
