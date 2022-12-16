##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine -I/usr/local/include

AM_CPPFLAGS = -DFORCE_SOCKET_REUSE=1

noinst_LTLIBRARIES = libnetwork.la
libnetwork_la_SOURCES = ANP.cpp ANPClient.cpp ANPClientStatistics.cpp \
	ANPServer.cpp ScanPointManagement.cpp

includedir = $(ANT_INCLUDEPATH)/engine/network
include_HEADERS = ANP.hpp ANPClient.hpp ANPClientStatistics.hpp ANPServer.hpp \
                  ScanPointManagement.hpp Time.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
