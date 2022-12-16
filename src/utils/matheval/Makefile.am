##
## Process this file with automake to produce Makefile.in
##

EXTRA_DIST = MathEvalRegistry.m4
includedir = $(ANT_INCLUDEPATH)/utils/matheval

# AM_CPPFLAGS=-DNDEBUG

INCLUDES = -I$(top_srcdir)/src/engine
include_HEADERS = ParserFunctions.hpp
noinst_HEADERS = MathEval.hpp MathEvalParser.hpp

MathEvalRegistry.cpp: MathEvalRegistry.m4
	@m4 MathEvalRegistry.m4 > MathEvalRegistry.cpp

noinst_LTLIBRARIES = libmatheval.la
libmatheval_la_SOURCES = MathEval.cpp MathEvalParser.cpp MathEvalRegistry.cpp \
	ParserFunctions.cpp


#EXTRA_PROGRAMS = matheval
check_PROGRAMS = matheval
matheval_SOURCES = MathEvalMain.cpp
matheval_LDADD = libmatheval.la ../config/libconfig.la ../debug/libdebug.la
#matheval_LDADD = libmatheval.la
#matheval_LDFLAGS = -L../../engine/.libs -L../../engine -lAnT


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
	-rm -f MathEvalRegistry.cpp

