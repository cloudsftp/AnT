##
## Process this file with automake to produce Makefile.in
##

noinst_HEADERS = Inet.hpp MainPtr.hpp SocketBuffer.hpp SocketStreams.hpp

if HAVE_LIBSOCKET
AM_CXXFLAGS = -DFORCE_SOCKET_REUSE=1
endif


noinst_LTLIBRARIES = libantsocket.la
libantsocket_la_SOURCES = Inet.cpp SocketBuffer.cpp SocketStreams.cpp


check_PROGRAMS = ClientDemo ServerDemo
ClientDemo_SOURCES = ClientDemo.cpp
ClientDemo_LDADD = libantsocket.la
ServerDemo_SOURCES = ServerDemo.cpp
ServerDemo_LDADD = libantsocket.la


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in

