##
## Process this file with automake to produce Makefile.in
##
SUBDIRS = rc

INCLUDES = -I. -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libwinenv.la
libwinenv_la_SOURCES = WinRegistryManager.cpp WinFunctions.cpp

noinst_HEADERS = WinRegistryManager.hpp WinFunctions.hpp


check_PROGRAMS = WinEnvTycoon
WinEnvTycoon_SOURCES = WinEnvTycoon.cpp
WinEnvTycoon_CXXFLAGS = -mno-cygwin -mms-bitfields
WinEnvTycoon_LDADD = ./libwinenv.la ../debug/libdebug.la


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in

