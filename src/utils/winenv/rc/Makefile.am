##
## Process this file with automake to produce Makefile.in
##

EXTRA_DIST = build-dll compile-resource lt-compile-resource

noinst_PROGRAMS = foo

# Compile a dummy object file into the foo.lo libtool object so that
# the ugly hack in lt-compile-resource knows what kind of libtool
# object to produce. (It checks whether there are any .libs/*.o
# files.)

foo.c:
	echo 'int main (int argc, char **argv) { return 0; }' >foo.c


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
