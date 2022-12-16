##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libnoise.la
libnoise_la_SOURCES = NoiseGenerator.cpp

includedir = $(ANT_INCLUDEPATH)/engine/utils/noise

include_HEADERS = NoiseGenerator.hpp 

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
