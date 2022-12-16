##
## AnT 4.669 Makefile for user-defined systems
##

ANT_SYSTEM_NAME = 
#
ifeq (,${ANT_SYSTEM_NAME})
ANT_SYSTEM_FILE := $(shell ls *.cpp | xargs -i egrep -l -e 'AnT.h' -e 'connectSystem' '{}')
# this find command is known to work under linux, solaris, mingw and cygwin!

ifeq (${ANT_SYSTEM_FILE},)
define WARNING
$(shell echo)
$(shell echo)
$(shell echo " No system file ($$""{ANT_SYSTEM_NAME}.cpp) found in the current directory!")
$(shell echo)
endef
$(warning ${WARNING})
ANT_SYSTEM_FILE = $(error ANT_SYSTEM_NAME undefined!)
endif

ifneq (,$(word 2, ${ANT_SYSTEM_FILE}))
define WARNING
$(shell echo)
$(shell echo)
$(shell echo " Ambiguous system file detection:")
$(shell echo ${ANT_SYSTEM_FILE})
$(shell echo)
$(shell echo " To disambiguate this just give as argument(s):")
$(shell echo)
$(shell echo "  ANT_SYSTEM_NAME=<file name without .cpp-suffix> [target]")
$(shell echo)
endef
$(warning ${WARNING})
ANT_SYSTEM_NAME = $(error ANT_SYSTEM_NAME undefined!)
else
ANT_SYSTEM_NAME = $(shell basename ${ANT_SYSTEM_FILE} '.cpp')
endif
#
endif

############################################################################

CC = gcc
CXX = g++

#
# The weird, but consistent use of double quotes (") in the context of
# setting (ANT_INSTALLATION_PREFIX; see below) or accessing
# (ANT_INSTALLATION_PREFIX, ANT_TOPDIR) path variables is needed for
# handling blanks (or some other special characters) on MSYS (MinGW).
# This must be especially considered, because AnT is installed per
# default into "Common Files" ("Gemeinsame Dateien") on MS Windows.
#

ANT_SHARED_LIB_EXT := $(shell "${ANT_TOPDIR}"/bin/AnT --shared-lib-ext)
ANT_INSTALLATION_PREFIX := $(shell cd "`\"${ANT_TOPDIR}\"/bin/AnT --installation-prefix`" && pwd)

override CPPFLAGS += -I"${ANT_INSTALLATION_PREFIX}"/include/AnT/engine
override LDFLAGS += -shared -mno-cygwin
ifeq ("x${ANT_SHARED_LIB_EXT}","xdll")
override CXXFLAGS += -mno-cygwin
override LDFLAGS += -L"${ANT_INSTALLATION_PREFIX}"/bin -lAnT
else
override CXXFLAGS += -fPIC
endif


.PHONY: clean


all: ${ANT_SYSTEM_NAME}.${ANT_SHARED_LIB_EXT}
	@echo Available AnT system function plugin: $<


${ANT_SYSTEM_NAME}.o: ${ANT_SYSTEM_NAME}.cpp

${ANT_SYSTEM_NAME}.${ANT_SHARED_LIB_EXT}: ${ANT_SYSTEM_NAME}.o
	${CXX} ${LDFLAGS} -o ${ANT_SYSTEM_NAME}.${ANT_SHARED_LIB_EXT} ${ANT_SYSTEM_NAME}.o


clean:
	-rm -f ${ANT_SYSTEM_NAME}.o
	-rm -f ${ANT_SYSTEM_NAME}.${ANT_SHARED_LIB_EXT}

