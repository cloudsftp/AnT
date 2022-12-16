##
## Process this file with automake to produce Makefile.in
##

## subdirectories

if HAVE_LIBGLUT
MAYBE_ANTVIS = antvis
endif
if HAVE_GTK
MAYBE_GUI = gtk-gui
endif
SUBDIRS = utils $(MAYBE_ANTVIS) engine $(MAYBE_GUI) share


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
