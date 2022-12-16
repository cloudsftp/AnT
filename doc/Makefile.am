##
## Process this file with automake to produce Makefile.in
##

## subdirectories
SUBDIRS = doxygen


docs:
	@cd doxygen && $(MAKE) $@


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
