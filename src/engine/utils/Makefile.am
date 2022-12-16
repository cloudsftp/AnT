##
## Process this file with automake to produce Makefile.in
##

if ANT_HAS_WIN_ENV
MAYBE_WIN_ENV = win_rc
endif
SUBDIRS = conditions datareader dimensionhandler enumerators \
	machines noise progress timer $(MAYBE_WIN_ENV)

INCLUDES = -I$(top_srcdir)/src/engine

includedir = $(ANT_INCLUDEPATH)/engine/utils

include_HEADERS = Averager.hpp GlobalConstants.hpp Resettable.hpp 

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
