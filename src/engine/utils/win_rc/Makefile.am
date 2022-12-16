##
## Process this file with automake to produce Makefile.in
##

EXTRA_DIST = AnT.rc.in AnT.ico

noinst_DATA = libwin32res-AnT.lo

libwin32res-AnT.lo: AnT.rc AnT.ico
	@echo "Build the win32res in $(PWD)"
	$(top_srcdir)/src/utils/winenv/rc/lt-compile-resource \
	$(top_srcdir)/src/engine/utils/win_rc/AnT.rc \
	$(top_srcdir)/src/engine/utils/win_rc/libwin32res-AnT.lo


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
