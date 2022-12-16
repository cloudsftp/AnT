##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libproxies.la
libproxies_la_SOURCES = AbstractDDE_Proxy.cpp AbstractFDE_Proxy.cpp \
	AbstractHybridFunctionProxy.cpp \
	AbstractMapProxy.cpp AbstractODE_Proxy.cpp AveragedMapProxy.cpp \
	CML_Proxy.cpp CODEL_Proxy.cpp DDE_Proxy.cpp \
	ExternalDataProxy.cpp FDE_Proxy.cpp HybridDDE_Proxy.cpp \
	HybridMapProxy.cpp HybridODE_Proxy.cpp MapProxy.cpp \
	ODE_Proxy.cpp PoincareMapProxy.cpp \
	RecurrentMapProxy.cpp StochasticalMapProxy.cpp \
	SystemFunctionProxy.cpp PDE_1d_Proxy.cpp

includedir = $(ANT_INCLUDEPATH)/engine/proxies

include_HEADERS = AbstractDDE_Proxy.hpp AbstractFDE_Proxy.hpp \
	AbstractHybridFunctionProxy.hpp \
	AbstractMapProxy.hpp AbstractODE_Proxy.hpp AveragedMapProxy.hpp \
	CML_Proxy.hpp CODEL_Proxy.hpp DDE_Proxy.hpp HybridDDE_Proxy.hpp \
	FDE_Proxy.hpp ExternalDataProxy.hpp HybridMapProxy.hpp \
	HybridODE_Proxy.hpp MapProxy.hpp ODE_Proxy.hpp PoincareMapProxy.hpp \
	RecurrentMapProxy.hpp StochasticalMapProxy.hpp \
	SystemFunctionProxy.hpp PDE_1d_Proxy.hpp


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
