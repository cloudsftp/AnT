##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

noinst_LTLIBRARIES = libsymimages.la

libsymimages_la_SOURCES = linearextensionscanpointcalculator.cpp \
	symbolicimageabstractcellcreator.cpp symbolicimagegridboxsizedefinition.cpp \
	symbolicimageabstractboxsizedefinition.cpp symbolicimagemorsespectrumlayer.cpp \
	linearextendedgridboxindexvaluemapper.cpp abstractlinearextensionconverter.cpp \
	symbolicimagelinearextendedindexboxmanager.cpp recurrentsetorderlocator.cpp \
	symbolicimagerecurrentcellset.cpp symbolicimageindexboxmanager.cpp \
	symbolicimageabstractscanpointcalculator.cpp symbolicimageabstractindexvaluemapper.cpp \
	symbolicimageperiodfinder.cpp symbolicimageoutputformat.cpp \
	stronglyconnectedcomponentsfinder.cpp greedyperiodiccellfinder.cpp \
	symbolicimageoutputwriter.cpp symbolicimagedefaultcell.cpp \
	gridboxscanpointcalculator.cpp gridboxindexvaluemapper.cpp \
	symbolicimageabstractcell.cpp symbolicimageabstractbox.cpp \
	symbolicimagecalculator.cpp symbolicimageabstractlayer.cpp \
	symbolicimageabstractboxmanager.cpp symbolicimagegridboxmanager.cpp \
	symbolicimagedefaultlayer.cpp datafileperiodiccellfinder.cpp 

noinst_HEADERS = linearextensionscanpointcalculator.hpp \
	symbolicimageabstractcellcreator.hpp symbolicimagegridboxsizedefinition.hpp \
	symbolicimageabstractboxsizedefinition.hpp symbolicimagemorsespectrumlayer.hpp \
	linearextendedgridboxindexvaluemapper.hpp abstractlinearextensionconverter.hpp \
	symbolicimagelinearextendedindexboxmanager.hpp recurrentsetorderlocator.hpp \
	symbolicimagerecurrentcellset.hpp symbolicimageindexboxmanager.hpp \
	symbolicimageabstractscanpointcalculator.hpp symbolicimageabstractindexvaluemapper.hpp \
	symbolicimageperiodfinder.hpp symbolicimageoutputformat.hpp \
	stronglyconnectedcomponentsfinder.hpp greedyperiodiccellfinder.hpp \
	symbolicimageoutputwriter.hpp symbolicimagedefaultcell.hpp \
	gridboxscanpointcalculator.hpp gridboxindexvaluemapper.hpp \
	symbolicimageabstractbox.hpp symbolicimageabstractcell.hpp \
	symbolicimagecalculator.hpp symbolicimageabstractlayer.hpp  \
	symbolicimageabstractboxmanager.hpp symbolicimagegridboxmanager.hpp \
	symbolicimagedefaultlayer.hpp parametermodificationparser.hpp \
  datafileperiodiccellfinder.hpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
