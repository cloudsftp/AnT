##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libspatial.la
libspatial_la_SOURCES = SpatialEvaluator.cpp

#noinst_HEADERS = SpatialEvaluator.hpp

include_HEADERS = SpatialEvaluator.hpp
includedir = $(ANT_INCLUDEPATH)/engine/methods/spatial

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
