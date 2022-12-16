##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libsimulators.la
libsimulators_la_SOURCES = \
	AbstractSimulator.cpp AveragedMapSimulator.cpp \
	CML_Simulator.cpp CODEL_Simulator.cpp \
	DDE_Simulator.cpp ExternalDataSimulator.cpp \
	FDE_Simulator.cpp HybridDDE_Simulator.cpp HybridMapSimulator.cpp \
	HybridODE_Simulator.cpp MapSimulator.cpp ODE_Simulator.cpp \
	PoincareMapSimulator.cpp RecurrentMapSimulator.cpp \
	SimulatorFactory.cpp StochasticalDDE_Simulator.cpp \
	StochasticalMapSimulator.cpp StochasticalODE_Simulator.cpp \
	PDE_1d_Simulator.cpp

includedir = $(ANT_INCLUDEPATH)/engine/simulators

include_HEADERS = AbstractSimulator.hpp 

noinst_HEADERS = AveragedMapSimulator.hpp CML_Simulator.hpp \
	CODEL_Simulator.hpp DDE_Simulator.hpp \
	ExternalDataSimulator.hpp FDE_Simulator.hpp HybridDDE_Simulator.hpp \
	HybridMapSimulator.hpp HybridODE_Simulator.hpp \
	MapSimulator.hpp ODE_Simulator.hpp \
	PoincareMapSimulator.hpp RecurrentMapSimulator.hpp \
	SimulatorFactory.hpp StochasticalDDE_Simulator.hpp \
	StochasticalMapSimulator.hpp StochasticalODE_Simulator.hpp \
	PDE_1d_Simulator.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
