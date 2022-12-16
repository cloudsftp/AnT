##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libtimer.la
libtimer_la_SOURCES = Timer.cpp TimerUpdater.cpp

includedir = $(ANT_INCLUDEPATH)/engine/utils/timer

include_HEADERS = Timer.hpp TimerUpdater.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
