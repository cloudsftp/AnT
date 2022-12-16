##
## Process this file with automake to produce Makefile.in
##

## subdirectories
if ANT_HAS_WIN_ENV
MAYBE_WIN_ENV = regex winenv
endif
SUBDIRS = debug arrays strconv socket $(MAYBE_WIN_ENV) config matheval

include_HEADERS = CompilerDetector.hpp

includedir = $(ANT_INCLUDEPATH)/utils

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
