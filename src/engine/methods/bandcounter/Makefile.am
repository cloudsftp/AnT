##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libbandcounter.la
libbandcounter_la_SOURCES = BandCounter.cpp

#noinst_HEADERS = SpatialEvaluator.hpp

include_HEADERS = BandCounter.hpp
includedir = $(ANT_INCLUDEPATH)/engine/methods/bandcounter

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
