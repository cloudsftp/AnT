##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libsipa.la

libsipa_la_SOURCES = sipacalculator.cpp sipabackorbitlayer.cpp sipabackorbitboxmanager.cpp \
  sipaperiodicpointsboxmanager.cpp sipaperiodicpointslayer.cpp

noinst_HEADERS = sipacalculator.hpp sipabackorbitlayer.hpp sipabackorbitboxmanager.hpp \
  sipaperiodicpointsboxmanager.hpp sipaperiodicpointslayer.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
