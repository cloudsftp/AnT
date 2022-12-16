##
## Process this file with automake to produce Makefile.in
##

## subdirectories
SUBDIRS = cas data iterators methods network proxies simulators utils

if ANT_HAS_MINGW_ENV
ANT_LO = o
ANT_LA = a
LIBS_DIR = .libs/
else
ANT_LO = lo
ANT_LA = la
LIBS_DIR = 
endif

if ANT_HAS_WIN_ENV
if ANT_HAS_MINGW_ENV
ANT_WIN_LINK_FLAGS =
else
ANT_WIN_LINK_FLAGS = -ladvapi32
endif
ANT_WIN_GET_REG = ../utils/winenv/$(LIBS_DIR)libwinenv.$(ANT_LA)
ANT_WIN_RES = ./utils/win_rc/$(LIBS_DIR)libwin32res-AnT.$(ANT_LO)
ANT_REGEX = ../utils/regex/$(LIBS_DIR)libregex.$(ANT_LA)
endif

if HAVE_LIBGLUT
if ANT_HAS_MINGW_ENV
VIS_LIBS =
else
VIS_LIBS = $(GL_LIBS)
endif
ANT_VIS = ../antvis/glui/$(LIBS_DIR)libglui.$(ANT_LA) ../antvis/$(LIBS_DIR)libagl.$(ANT_LA)
endif
if HAVE_LIBSOCKET
ANT_NETWORK = ./network/$(LIBS_DIR)libnetwork.$(ANT_LA) ../utils/socket/$(LIBS_DIR)libantsocket.$(ANT_LA)
endif

INCLUDES = -I$(top_srcdir)/src/engine -I$(top_srcdir)/src/utils 

include_HEADERS = AnT.hpp AnT-init.hpp MethodsPlugin.hpp \
	SpatialDiffOperators.hpp SymbolFromShared.hpp
includedir = $(ANT_INCLUDEPATH)/engine

lib_LTLIBRARIES = libAnT.la
libAnT_la_LDFLAGS = -avoid-version
libAnT_la_SOURCES = AnT-init.cpp MethodsPlugin.cpp \
	SpatialDiffOperators.cpp SymbolFromShared.cpp

libAnT_la_LIBADD = \
	../utils/matheval/$(LIBS_DIR)libmatheval.$(ANT_LA) \
	../utils/config/$(LIBS_DIR)libconfig.$(ANT_LA) \
	../utils/strconv/$(LIBS_DIR)libstrconv.$(ANT_LA) \
	./simulators/$(LIBS_DIR)libsimulators.$(ANT_LA) \
	./proxies/$(LIBS_DIR)libproxies.$(ANT_LA) \
	./data/$(LIBS_DIR)libdata.$(ANT_LA) \
	./iterators/$(LIBS_DIR)libiterators.$(ANT_LA) \
	./cas/$(LIBS_DIR)libcas.$(ANT_LA) \
	./methods/$(LIBS_DIR)libmethods.$(ANT_LA) \
	./methods/bandcounter/$(LIBS_DIR)libbandcounter.$(ANT_LA) \
	./methods/fourier/$(LIBS_DIR)libfourier.$(ANT_LA) \
	./methods/singlvalues/$(LIBS_DIR)libsinglvalues.$(ANT_LA)\
	./methods/general/$(LIBS_DIR)libgeneral.$(ANT_LA) \
	./methods/dimensions/$(LIBS_DIR)libdimensions.$(ANT_LA) \
	./methods/conditions/$(LIBS_DIR)libconditions.$(ANT_LA) \
	./methods/lyapunov/$(LIBS_DIR)liblyapunov.$(ANT_LA) \
	./methods/output/$(LIBS_DIR)liboutput.$(ANT_LA) \
	./methods/period/$(LIBS_DIR)libperiod.$(ANT_LA) \
	./methods/region/$(LIBS_DIR)libregion.$(ANT_LA) \
	./methods/spatial/$(LIBS_DIR)libspatial.$(ANT_LA) \
	./methods/symbolic/$(LIBS_DIR)libsymbolic.$(ANT_LA) \
	./methods/symimages/$(LIBS_DIR)libsymimages.$(ANT_LA) \
	./methods/sipa/$(LIBS_DIR)libsipa.$(ANT_LA) \
	../utils/arrays/$(LIBS_DIR)libarrays.$(ANT_LA) \
	./utils/conditions/$(LIBS_DIR)libcond.$(ANT_LA) \
	./utils/datareader/$(LIBS_DIR)libdatareader.$(ANT_LA) \
	../utils/debug/$(LIBS_DIR)libdebug.$(ANT_LA) \
	./utils/dimensionhandler/$(LIBS_DIR)libdimhandl.$(ANT_LA) \
	./utils/machines/$(LIBS_DIR)libmachines.$(ANT_LA) \
	./utils/noise/$(LIBS_DIR)libnoise.$(ANT_LA) ./utils/progress/$(LIBS_DIR)libprogress.$(ANT_LA) \
	./utils/timer/$(LIBS_DIR)libtimer.$(ANT_LA) \
	$(ANT_REGEX) $(ANT_NETWORK) \
	./methods/visualization/$(LIBS_DIR)libvisualization.$(ANT_LA) $(ANT_VIS) $(VIS_LIBS) \
	$(ANT_WIN_GET_REG) $(ANT_WIN_LINK_FLAGS)


if ANT_HAS_MINGW_ENV
libAnT.la: $(libAnT_la_OBJECTS) $(libAnT_la_DEPENDENCIES)
	$(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -shared -Wl,--whole-archive .libs/*.o $(libAnT_la_LIBADD) -Wl,--no-whole-archive -ladvapi32 $(GL_LIBS) $(LIBS) -Wl,--export-dynamic -o .libs/libAnT.dll -Wl,--out-implib,.libs/libAnT.dll.a

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@test -f ./.libs/libAnT.dll && $(libLTLIBRARIES_INSTALL) $(INSTALL_STRIP_FLAG) ./.libs/libAnT.dll $(DESTDIR)$(bindir)/libAnT.dll
	@test -f ./.libs/libAnT.dll.a && $(libLTLIBRARIES_INSTALL) $(INSTALL_STRIP_FLAG) ./.libs/libAnT.dll.a $(DESTDIR)$(libdir)/libAnT.dll.a
#else
#libAnT.la: $(libAnT_la_OBJECTS) $(libAnT_la_DEPENDENCIES)
#	$(CXXLINK) -rpath $(libdir) $(libAnT_la_LDFLAGS) $(libAnT_la_OBJECTS) $(libAnT_la_LIBADD) $(LIBS)

#install-libLTLIBRARIES: $(lib_LTLIBRARIES)
#	@$(NORMAL_INSTALL)
#	$(mkinstalldirs) $(DESTDIR)$(libdir)
#	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
#	  if test -f $$p; then \
#	    echo "$(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p"; \
#	    $(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p; \
#	  else :; fi; \
#	done
endif


## AnT target
bin_PROGRAMS = AnT
AnT_SOURCES = AnT.cpp


#AnT_LDFLAGS = -shared --allow-shlib-undefined --enable-auto-import --no-undefined
#AnT_LDFLAGS = -e _mainCRTStartup # is for win32res
#AnT_LINK = $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@


if ANT_HAS_MINGW_ENV
AnT$(EXEEXT): $(AnT_OBJECTS) $(AnT_DEPENDENCIES)
	@rm -f AnT$(EXEEXT)
	$(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o AnT$(EXEEXT) $(AnT_LDFLAGS) -e _mainCRTStartup $(AnT_OBJECTS) -L./.libs -lAnT $(ANT_WIN_RES) $(ANT_WIN_GET_REG) $(LIBS)
else
AnT_LDADD = ./$(LIBS_DIR)libAnT.$(ANT_LA) 
#AnT$(EXEEXT): $(AnT_OBJECTS) $(AnT_DEPENDENCIES)
#	@rm -f AnT$(EXEEXT)
#	$(CXXLINK) $(AnT_LDFLAGS) $(AnT_OBJECTS) $(AnT_LDADD) $(QHULL_LIBS) $(LIBS)
endif


install-exec-hook:
if ANT_HAS_MINGW_ENV
else
if ANT_HAS_WIN_ENV
	-@test -f $(DESTDIR)$(bindir)/cygAnT.dll && rm -f $(DESTDIR)$(bindir)/libAnT.dll
	@test -f $(DESTDIR)$(bindir)/cygAnT.dll && ln -s $(DESTDIR)$(bindir)/cygAnT.dll $(DESTDIR)$(bindir)/libAnT.dll
endif
endif

uninstall-hook:
if ANT_HAS_MINGW_ENV
else
if ANT_HAS_WIN_ENV
	rm -f $(DESTDIR)$(bindir)/libAnT.dll
endif
endif


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
