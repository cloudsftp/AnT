##
## Process this file with automake to produce Makefile.in
##

# AUTOMAKE_OPTIONS = 1.7.1

## subdirectories
SUBDIRS = doc src 
EXTRA_DIST = AnT.pc.in README.in
includedir = $(ANT_INCLUDEPATH)/engine
include_HEADERS = config.h
ACLOCAL_AMFLAGS = -I m4

docs:
	@cd doc && $(MAKE) $@

tarball:
	@if uname -s | grep 'CYGWIN' > /dev/null || uname -s | grep 'MINGW' > /dev/null ; then \
	echo "Configure and install temporary the package for build a Setup file ..." ; \
	./autogen.sh --prefix=./build/windows ; \
	make install ; \
	echo "Building tarball for Windows ..." ; \
	cd ./build/windows ; \
	./build.sh ; \
	mv -f AnT_Setup*.exe ../../AnT_Setup_R${ANT_SUB}.${ANT_REV_NUMBER}.${ANT_PATCH}.exe ; \
	mv -f AnT_Setup*.exe.md5 ../../AnT_Setup_R${ANT_SUB}.${ANT_REV_NUMBER}.${ANT_PATCH}.exe.md5 ; \
	else \
	echo "Building tarball: @ANT_NAME@-@ANT_MAJOR@.@ANT_MINOR@-R@ANT_SUB@.@ANT_REV_NUMBER@.@ANT_PATCH@" ; \
	$(MAKE) dist ; \
	mv -f @ANT_NAME@-@ANT_MAJOR@.@ANT_MINOR@-R@ANT_SUB@@ANT_REV@_src.tar.gz \
	@ANT_NAME@-@ANT_MAJOR@.@ANT_MINOR@-R@ANT_SUB@.@ANT_REV_NUMBER@.@ANT_PATCH@_src.tar.gz ; \
	md5sum @ANT_NAME@-@ANT_MAJOR@.@ANT_MINOR@-R@ANT_SUB@.@ANT_REV_NUMBER@.@ANT_PATCH@_src.tar.gz \
	> @ANT_NAME@-@ANT_MAJOR@.@ANT_MINOR@-R@ANT_SUB@.@ANT_REV_NUMBER@.@ANT_PATCH@_src.tar.gz.md5 ; \
	fi

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = AnT.pc

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -rf autom4te.cache fftw.pc
	-rm -f Makefile.in configure acinclude.m4 aclocal.m4
	-rm -f config.h.in config.h.in~ config.log config.guess config.sub
	-rm -f config.status stamp-h.in ltconfig ltmain.sh install-sh missing
	-rm -f depcomp mkinstalldirs libtool-nofpic COPYING INSTALL README.txt

