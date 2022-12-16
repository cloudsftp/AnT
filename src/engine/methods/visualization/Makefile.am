##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_HEADERS = VisualizationConnector.hpp ParseVisualizationSection.hpp

noinst_LTLIBRARIES = libvisualization.la
libvisualization_la_SOURCES = VisualizationConnector.cpp \
	VisualizationDefaults.cpp ParseVisualizationSection.cpp


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
