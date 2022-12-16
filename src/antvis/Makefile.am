##
## Process this file with automake to produce Makefile.in
##

INCLUDES = -I$(top_srcdir)/src/engine

if HAVE_LIBGLUT
MAYBE_GLUI = glui
endif
SUBDIRS = $(MAYBE_GLUI)

noinst_HEADERS = \
	agl.h                agl_threads.h \
	algebra3_wrapper.h   animator.h \
	arrow.h               axis2d.h \
	axis3d.h             buffer.h             camera.h \
	camera2d.h           camera3d.h           commonmesh.h \
	coloringmethods.h    curves.h             dataiterator.h \
	datasource.h         antvis_dialogs.h     dimensions.h \
	drawmethods.h        event.h              gl_wrapper.h \
	gui.h                list.h            \
	mesh2d.h             mesh3d.h             object.h \
	scene.h \
	text.h               threads.h            visualization.h \
	visualobject.h       window.h             window2d.h \
	window3d.h

noinst_LTLIBRARIES = libagl.la
libagl_la_SOURCES = \
	agl_threads.cpp        algebra3_wrapper.cpp \
	animator.cpp           arrow.cpp \
	axis2d.cpp             axis3d.cpp \
	buffer.cpp             camera.cpp             camera2d.cpp \
	camera3d.cpp           commonmesh.cpp         coloringmethods.cpp \
	curves.cpp             dataiterator.cpp       datasource.cpp \
	demoobjects.cpp        antvis_dialogs.cpp     dimensions.cpp \
	drawmethods.cpp        event.cpp              gui.cpp \
	list.cpp               mesh2d.cpp \
	mesh3d.cpp             object.cpp \
	scene.cpp \
	text.cpp               threads.cpp            visualization.cpp \
	visualobject.cpp       window.cpp             window2d.cpp \
	window3d.cpp

EXTRA_DIST = samples.cpp


## make AnT-core really clean
maintainer-clean-generic:
	-@echo "This command is intended for maintainers to use;"
	-@echo "it deletes files that may require special tools to rebuild."
	-rm -f Makefile.in
