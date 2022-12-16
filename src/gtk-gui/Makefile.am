##
## Process this file with automake to produce Makefile.in
##

## subdirectories
if ANT_HAS_WIN_ENV
MAYBE_WIN_ENV = win_rc
if ANT_HAS_MINGW_ENV
ANT_WIN_RES = ./win_rc/.libs/libwin32res-AnT-gui.o
ANT_WIN_MINGW_LINK_FLAGS = -L/mingw/lib -ladvapi32 -lwsock32
else
# Cygwin environment assumed
ANT_WIN_RES = ./win_rc/libwin32res-AnT-gui.lo
ANT_WIN_MINGW_LINK_FLAGS = 
AnT_gui_LDFLAGS = -Wl,--enable-runtime-pseudo-reloc
endif
AnT_gui_CXXFLAGS = -mno-cygwin -mms-bitfields -DNDEBUG
endif
SUBDIRS = $(MAYBE_WIN_ENV) logos

EXTRA_DIST = logo.xpm AnT-run.cfg
datadir = $(prefix)/share/AnT/gui
data_DATA = logo.xpm AnT-run.cfg

INCLUDES = $(GTK_INCLUDES) -I$(top_srcdir)/src/gtk-gui -I$(top_srcdir)/src/engine
noinst_HEADERS = AnT-run.hpp Callbacks.hpp CommonDefs.hpp \
	Configurators.hpp CustomConfigurators.hpp GuiController.hpp \
	GeneralWidgets.hpp MainWindow.hpp Settings.hpp SemanticCheck.hpp \
	FrameManager.hpp  

LIBS=
GL_LIBS=

bin_PROGRAMS = AnT-gui
AnT_gui_SOURCES = AnT-gui.cpp AnT-run.cpp Callbacks.cpp \
	Configurators.cpp  CustomConfigurators.cpp GuiController.cpp \
	GeneralWidgets.cpp MainWindow.cpp SemanticCheck.cpp \
	FrameManager.cpp


if ANT_HAS_MINGW_ENV
AnT-gui$(EXEEXT): $(AnT_gui_OBJECTS) $(AnT_gui_DEPENDENCIES) 
	@rm -f AnT-gui$(EXEEXT)
	$(CXXLD) -o AnT-gui$(EXEEXT) $(AnT_gui_LDFLAGS) -e _mainCRTStartup $(AnT_gui_OBJECTS) $(GTK_LIBS) -L../engine/.libs -lAnT $(ANT_WIN_RES) $(ANT_WIN_MINGW_LINK_FLAGS) $(LIBS)
else
AnT_gui_LDADD = ../engine/libAnT.la $(GTK_LIBS) $(ANT_WIN_RES) $(ANT_WIN_MINGW_LINK_FLAGS)
#AnT_gui_LINK = $(LIBTOOL) --mode=link $(CXXLD) -o $@
endif


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
