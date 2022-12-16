##
## Process this file with automake to produce Makefile.in
##

if HAVE_LIBGLUT
MAYBE_IMAGES = images
endif
SUBDIRS = $(MAYBE_IMAGES)

noinst_HEADERS = algebra3.h arcball.h glui.h quaternion.h stdinc.h

noinst_LTLIBRARIES = libglui.la
libglui_la_SOURCES = algebra3.cpp arcball.cpp glui.cpp glui_add_controls.cpp \
                     glui_bitmap_img_data.cpp glui_bitmaps.cpp \
                     glui_button.cpp glui_checkbox.cpp glui_column.cpp \
                     glui_control.cpp glui_edittext.cpp glui_listbox.cpp \
                     glui_mouse_iaction.cpp glui_node.cpp glui_panel.cpp \
                     glui_radio.cpp glui_rollout.cpp glui_rotation.cpp \
                     glui_separator.cpp glui_spinner.cpp \
                     glui_statictext.cpp glui_translation.cpp \
                     quaternion.cpp

EXTRA_DIST = ppm2array.cpp

## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
