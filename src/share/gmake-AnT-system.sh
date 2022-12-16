#!/bin/sh

#
# The weird, but consistent use of double quotes (") in the context of
# setting (ANT_INSTALLATION_PREFIX; see below) or accessing
# (ANT_INSTALLATION_PREFIX, ANT_TOPDIR) path variables is needed for
# handling blanks (or some other special characters) on MSYS (MinGW).
# This must be especially considered, because AnT is installed per
# default into "Common Files" ("Gemeinsame Dateien") on MS Windows.
#


if [ "x${ANT_TOPDIR}" = "x" ] ; then
  ANT_CMD=AnT

  if [ "x`which ${ANT_CMD}`" = "x" ] ; then
    echo "AnT not found!"
    exit 1
  fi
else
  ANT_CMD="${ANT_TOPDIR}"/bin/AnT
fi

if ( make -v 2>&1 | grep "GNU" > /dev/null ) ; then
 { pushd "`\"${ANT_CMD}\" --installation-prefix`" \
  && ANT_INSTALLATION_PREFIX=`pwd` \
  && popd ; } > /dev/null
 #export ANT_INSTALLATION_PREFIX
 #echo ANT_INSTALLATION_PREFIX="${ANT_INSTALLATION_PREFIX}"

 make -f "${ANT_INSTALLATION_PREFIX}"/share/AnT/share/Makefile.gnu $@
else
 echo " *** Error: `basename $0` needs GNU Make in order to work properly!"
 exit 1
fi

