##
## Process this file with automake to produce Makefile.in
##

EXTRA_DIST = AnT_footer.html AnT_header.html doxy.cfg.in

docs:
	@if test x"$$DOXYGEN" != "xno"; then \
	  echo -n "  + Creating docs: "; \
	  $(DOXYGEN) doxy.cfg; \
	  $(DOXYGEN) doxy-u.cfg; \
	  $(DOXYGEN) doxy-v.cfg; \
	  echo "done.";\
	fi

clean:
	rm -rf ../html

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
