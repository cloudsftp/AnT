##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libgeneral.la
libgeneral_la_SOURCES = GeneralEvaluator.cpp MinMaxValuesCalculator.cpp \
	PGM_Saver.cpp StatisticsCalculator.cpp TrajectorySaver.cpp \
	VelocityCalculator.cpp WaveNumbers.cpp

noinst_HEADERS = GeneralEvaluator.hpp MinMaxValuesCalculator.hpp \
	PGM_Saver.hpp StatisticsCalculator.hpp \
	TrajectorySaver.hpp VelocityCalculator.hpp \
	WaveNumbers.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
