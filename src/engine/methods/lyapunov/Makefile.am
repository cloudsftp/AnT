##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = liblyapunov.la
liblyapunov_la_SOURCES = LyapunovExponentsCalculator.cpp

noinst_HEADERS = LyapunovExponentsCalculator.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
