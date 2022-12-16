##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libdatareader.la
libdatareader_la_SOURCES = ExternalDataReader.cpp ParameterParser.cpp inputdatareader.cpp

includedir = $(ANT_INCLUDEPATH)/engine/utils/datareader

include_HEADERS = ExternalDataReader.hpp ParameterParser.hpp ExternalDataTypes.hpp inputdatareader.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
