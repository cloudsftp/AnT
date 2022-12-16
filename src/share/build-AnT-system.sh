#!/bin/sh

#
# Please copy this script to the directory containing your cpp-file
# (dynamical system file) and execute it in order to get the needed
# system dll.
#

ANT_SYSTEM_NAME=
USER_FLAGS=
for ARG in $@ ; do
  if test "x${ANT_SYSTEM_NAME}" != "x" ; then
    USER_FLAGS="${USER_FLAGS} ${ARG}"
    continue
  fi

  ANT_SYSTEM_NAME=`echo "${ARG}" | sed 's/ANT_SYSTEM_NAME=//'` # delete this prefix
  if test "x${ARG}" = "x${ANT_SYSTEM_NAME}" ; then # no prefix detected
    ANT_SYSTEM_NAME=
    USER_FLAGS="${USER_FLAGS} ${ARG}"
  fi
done

if test "x${ANT_SYSTEM_NAME}" = "x" ; then # find the system file
  ANT_SYSTEM_FILE=`ls *.cpp | xargs -i egrep -l -e 'AnT.h' -e 'connectSystem' '{}'`
  # this find command is known to work under linux, solaris, mingw and cygwin!

  if test "x${ANT_SYSTEM_FILE}" = "x" ; then
    echo
    echo " No system file found in the current directory!"
    echo
    exit 1
  fi

  for WORD in ${ANT_SYSTEM_FILE} ; do
  continue
  done
  # ${WORD} is now the last word of ${ANT_SYSTEM_FILE}

  if test "${ANT_SYSTEM_FILE}" != "${WORD}" ; then
    echo
    echo " Ambiguous system file detection:"
    echo ${ANT_SYSTEM_FILE}
    echo
    echo " To disambiguate this just call:"
    echo
    echo "  `basename \"$0\"` ANT_SYSTEM_NAME=<file name without .cpp-suffix>"
    echo
    exit 1
  fi

  ANT_SYSTEM_NAME=`basename ${ANT_SYSTEM_FILE} .cpp`;
fi # find the system file


if [ "x${ANT_TOPDIR}" = "x" ] ; then
  ANT_CMD=AnT

  if [ "x`which ${ANT_CMD}`" = "x" ] ; then
    echo "AnT not found!"
    exit 1
  fi
else
  ANT_CMD="${ANT_TOPDIR}"/bin/AnT
fi


ANT_SHARED_LIB_EXT=`"${ANT_CMD}" --shared-lib-ext`

#
# The weird, but consistent use of double quotes (") in the context of
# setting (ANT_INSTALLATION_PREFIX; see below) or accessing
# (ANT_INSTALLATION_PREFIX, ANT_TOPDIR) path variables is needed for
# handling blanks (or some other special characters) on MSYS (MinGW).
# This must be especially considered, because AnT is installed per
# default into "Common Files" ("Gemeinsame Dateien") on MS Windows.
#

#{ pushd `"${ANT_CMD}" --installation-prefix` \
# && ANT_INSTALLATION_PREFIX=`pwd` \
# && popd ; } > /dev/null
ANT_INSTALLATION_PREFIX=`cd "\`\"${ANT_CMD}\" --installation-prefix\`" && pwd`
#echo ANT_INSTALLATION_PREFIX="${ANT_INSTALLATION_PREFIX}"


if [ "x${ANT_SHARED_LIB_EXT}" = "xdll" ] ; then
   ANT_CXXFLAGS="-mno-cygwin -mms-bitfields"
   ANT_LDFLAGS="-shared -mno-cygwin"
   ANT_LIB_DIR="bin"
else \
   ANT_CXXFLAGS="-fPIC"
   ANT_LDFLAGS="-shared"
   ANT_LIB_DIR="lib"
fi

# transfer the other arguments of $@ to the compile command:
echo "Compiling ${ANT_SYSTEM_NAME}.cpp ..."
g++ -c ${USER_FLAGS} ${ANT_CXXFLAGS} ${ANT_SYSTEM_NAME}.cpp \
  -I"${ANT_INSTALLATION_PREFIX}"/include/AnT/engine \
  -o ${ANT_SYSTEM_NAME}.o \
|| exit

echo "Building shared library ${ANT_SYSTEM_NAME}.${ANT_SHARED_LIB_EXT} ..."
g++ ${USER_FLAGS} ${ANT_LDFLAGS} ${ANT_SYSTEM_NAME}.o \
  -L"${ANT_INSTALLATION_PREFIX}"/${ANT_LIB_DIR} -lAnT \
  -o ${ANT_SYSTEM_NAME}.${ANT_SHARED_LIB_EXT} \
|| exit

rm ${ANT_SYSTEM_NAME}.o

echo "Available AnT system function plugin: ${ANT_SYSTEM_NAME}.${ANT_SHARED_LIB_EXT}"

