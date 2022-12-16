##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

SUBDIRS = bandcounter conditions dimensions fourier general lyapunov \
	output period region singlvalues sipa spatial symbolic \
	symimages visualization

noinst_LTLIBRARIES = libmethods.la
libmethods_la_SOURCES = MethodsData.cpp 

includedir = $(ANT_INCLUDEPATH)/engine/methods
include_HEADERS = InvestigationMethod.hpp MethodsData.hpp 

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
