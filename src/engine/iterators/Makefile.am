##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libiterators.la
libiterators_la_SOURCES = ButcherArrays.cpp DDE_Integrator.cpp \
	FDE_Integrator.cpp HybridMapIterator.cpp HybridODE_Integrator.cpp \
	HybridPartIterator.cpp Iterator.cpp MapIterator.cpp \
	ODE_Integrator.cpp StochasticalDDE_Integrator.cpp \
	StochasticalODE_Integrator.cpp

includedir = $(ANT_INCLUDEPATH)/engine/iterators
include_HEADERS = Iterator.hpp
noinst_HEADERS = ButcherArrays.hpp DDE_Integrator.hpp FDE_Integrator.hpp \
	HybridDDE_Integrator.hpp HybridMapIterator.hpp \
	HybridODE_Integrator.hpp HybridPartIterator.hpp \
	MapIterator.hpp ODE_Integrator.hpp \
	StochasticalDDE_Integrator.hpp StochasticalODE_Integrator.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
