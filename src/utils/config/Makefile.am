##
## Process this file with automake to produce Makefile.in
##

datadir = $(prefix)/share/AnT
data_DATA = GlobalKeys.cfg
EXTRA_DIST = GlobalKeys.cfg

includedir = $(ANT_INCLUDEPATH)/utils/config
include_HEADERS = Configuration.hpp DerefRec.hpp Indentation.hpp KeyIndexedTree.hpp Parsing.hpp PreSemanticCheck.hpp

INCLUDES = -I. -I$(top_srcdir)/src/engine 

noinst_LTLIBRARIES = libconfig.la
libconfig_la_SOURCES = Configuration.cpp Indentation.cpp KeyIndexedTree.cpp Parsing.cpp PreSemanticCheck.cpp

check_PROGRAMS = presemchk
presemchk_SOURCES = PreSemanticCheckMain.cpp
if ANT_HAS_WIN_ENV
presemchk_LDADD = libconfig.la ../debug/libdebug.la ../strconv/libstrconv.la ../winenv/libwinenv.la
else
presemchk_LDADD = libconfig.la ../debug/libdebug.la ../strconv/libstrconv.la
endif

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in

