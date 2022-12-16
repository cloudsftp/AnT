##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libconditions.la
libconditions_la_SOURCES = ConditionsChecker.cpp

# noinst_HEADERS = ConditionsChecker.hpp

include_HEADERS = ConditionsChecker.hpp
includedir = $(ANT_INCLUDEPATH)/engine/methods/conditions


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
