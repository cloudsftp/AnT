##
## Process this file with automake to produce Makefile.in
##

EXTRA_DIST = AnT-gui.rc.in AnT-gui.ico

noinst_DATA = libwin32res-AnT-gui.lo

libwin32res-AnT-gui.lo: AnT-gui.rc AnT-gui.ico
	@echo -n "Build the win32res in $(PWD)"
	$(top_srcdir)/src/utils/winenv/rc/lt-compile-resource \
	$(PWD)/AnT-gui.rc $(PWD)/libwin32res-AnT-gui.lo


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
