##
## Process this file with automake to produce Makefile.in
##
INCLUDES = -I. -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libstrconv.la
libstrconv_la_SOURCES = StringConverter.cpp

include_HEADERS = StringConverter.hpp StringStream.hpp
includedir = $(ANT_INCLUDEPATH)/utils/strconv

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
