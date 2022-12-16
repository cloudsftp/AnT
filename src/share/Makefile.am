##
## Process this file with automake to produce Makefile.in
##

## subdirectories
# SUBDIRS = 

INCLUDES = -I$(top_srcdir)/src/engine
include_HEADERS = MultivariatePolynomGenerator.hpp
includedir = $(ANT_INCLUDEPATH)/share

EXTRA_DIST = MultivariatePolynomGeneratorExample.ant MultivariatePolynomGeneratorExample.cpp \
	Makefile.def Makefile.gnu build-AnT-system.sh make-AnT-system.sh gmake-AnT-system.sh
datadir = $(prefix)/share/AnT/share
data_DATA = MultivariatePolynomGeneratorExample.ant MultivariatePolynomGeneratorExample.cpp \
	Makefile.def Makefile.gnu

bin_SCRIPTS = build-AnT-system.sh make-AnT-system.sh gmake-AnT-system.sh

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
