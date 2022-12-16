dnl --------------------------------------------------------------------
dnl configure.in for AnT-core.
dnl

AC_PREREQ(2.52)

dnl --------------------------------------------------------------------
# Process this file with autoconf to produce a configure script.
AC_INIT([AnT-core])
AC_CONFIG_SRCDIR(src/engine/AnT.hpp)
AC_CONFIG_MACRO_DIR([m4])

dnl --------------------------------------------------------------------
# Version Number and some information
AC_SUBST(ANT_NAME,[AnT-core])
AC_SUBST(ANT_MAJOR,[4])
AC_SUBST(ANT_MINOR,[669])
AC_SUBST(ANT_SUB,[4])
AC_SUBST(ANT_REV,[c])
AC_SUBST(ANT_PATCH,[1])
AC_SUBST(ANT_YEARS,[1999-2011])
AC_SUBST(ANT_HOMEPAGE,[http://www.AnT4669.de])
AC_SUBST(ANT_BUGREPORT,[mail@ant4669.de])
AC_SUBST(ANT_WEB_PREFIX,[http://www.informatik.uni-stuttgart.de/ipvr/bv/projekte/nld/software])
AC_SUBST(ANT_WEB_SUFFIX,[AnT4669])
AC_SUBST(ANT_DESCRIPTION,["Simulation and Analysis Tool for Dynamical Systems"])

AC_DEFINE(ANT_CONFIG_H,                  1,              [For seting the internal flags])
AC_DEFINE(ANT_INI_FILE_EXT,          "ant",              [Defines the extension for initialization files])
AC_DEFINE_UNQUOTED(ANT_MAJOR,        "$ANT_MAJOR",       [AnT major version number])
AC_DEFINE_UNQUOTED(ANT_MINOR,        "$ANT_MINOR",       [AnT minor version number])
AC_DEFINE_UNQUOTED(ANT_SUB,          "$ANT_SUB",         [AnT sub version number])
AC_DEFINE_UNQUOTED(ANT_REV,          "$ANT_REV",         [AnT revision])
AC_DEFINE_UNQUOTED(ANT_PATCH,        "$ANT_PATCH",       [AnT patch number])
AC_DEFINE_UNQUOTED(ANT_YEARS,        "$ANT_YEARS",       [AnT copyright])
AC_DEFINE_UNQUOTED(ANT_HOMEPAGE,     "$ANT_HOMEPAGE",    [AnT homepage])
AC_DEFINE_UNQUOTED(ANT_BUGREPORT,    "$ANT_BUGREPORT",   [Define to the address where bug reports for this package should be sent.])

ANT_REV_NUMBER=`echo "$ANT_REV" | tr "abcdefghi" "123456789"`       # i know, it is a very dirty trick G.W.
AC_SUBST(ANT_REV_NUMBER,[$ANT_REV_NUMBER])
AC_SUBST(ANT_VERSION_INFO,[$ANT_SUB:$ANT_REV_NUMBER:$ANT_PATCH])
AC_DEFINE_UNQUOTED(ANT_REV_NUMBER,   "$ANT_REV_NUMBER",  [AnT revision number])
AC_DEFINE_UNQUOTED(ANT_VERSION_INFO, "$ANT_VERSION_INFO",[Define the current version of AnT package])

AC_COPYRIGHT([This script is used to build the AnT 4.669 package. For more information
about the package and therms of copyright please visit our homepage:

       http://www.AnT4669.de
])

dnl --------------------------------------------------------------------
# initialize stuff for automake
AM_INIT_AUTOMAKE([$ANT_NAME], [$ANT_MAJOR].[$ANT_MINOR]"-R"[$ANT_SUB][$ANT_REV]"_src")

dnl initialize stuff for autoheader
AM_CONFIG_HEADER(config.h)

dnl --------------------------------------------------------------------
# Checks for programs.

AC_PROG_AWK
# Some AWK fail
if echo xfoo | $AWK '/foo|^bar$/ { print }' | grep xfoo >/dev/null; then :;
else
  AC_MSG_ERROR([the regex engine of $AWK is too broken to be used
                you might want to install GNU AWK])
fi

[[ -n "$AR" ]] || AR="gar ar"
AC_PATH_PROGS(AR, $AR,no,${with_xcompile_path}:${PATH})
[[ "x$AR" = "xno" ]] && AC_MSG_ERROR([Couldn't find ar])

# Declare win32 dll and libtool support
AC_CYGWIN
# AC_MINGW32
AC_PATH_PROGS(DLLTOOL, dlltool)
AC_LIBTOOL_WIN32_DLL
AC_LIBTOOL_DLOPEN
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AM_PROG_LD
AC_SUBST(LD)
AM_PROG_NM
AC_SUBST(NM)
AC_PROG_LN_S
AC_PROG_YACC

dnl check for doxygen
AC_CHECK_PROG(DOXYGEN, doxygen, doxygen, no)

dnl honor flags that come through $INCLUDES
CPPFLAGS="$CPPFLAGS $INCLUDES"

AC_PROG_GCC_TRADITIONAL
AC_PROG_CXX
AC_PROG_CC

AC_PROG_INSTALL
AC_LANG([C++])

dnl --------------------------------------------------------------------
# Checks for libraries.
AC_CHECK_LIB(dl, dlopen)
AC_CHECK_LIB(m, sin)
#AC_CHECK_LIB(stdc++,__gxx_personality_v0)

dnl --------------------------------------------------------------------
# host
host_or_hostalias="$host"
if test "$host_or_hostalias" = ""; then
    dnl user has called ./configure with a host parameter unknown to
    dnl config.sub; the canonical "$host" is empty
    dnl
    dnl Try the following switch with user's original host_alias 
    dnl input instead.
    dnl
    host_or_hostalias="$host_alias"
fi

# initialize stuff for prefix
if test "x$prefix" = xNONE; then
   prefix="${ac_default_prefix}"
fi
if test "x$exec_prefix" = xNONE; then
   exec_prefix="${prefix}"
fi

case "$host_or_hostalias" in
  *cygwin* | *pw32* | *os2*)
    ANT_HAS_WIN_ENV="yes"
    AC_DEFINE(ANT_HAS_WIN_ENV,1,[Defined to 0 if U*ix-like environment detected])
    AC_DEFINE_UNQUOTED(ANT_INSTALLATION_PATH,"",[The AnT installation path])
    ;;
  *mingw*)
    ANT_HAS_MINGW_ENV="yes"
    ANT_HAS_WIN_ENV="yes"
    AC_DEFINE(ANT_HAS_WIN_ENV,1)
    AC_DEFINE(ANT_HAS_MINGW_ENV,1,[Defined to 1 if MinGW environment detected])
    AC_DEFINE_UNQUOTED(ANT_INSTALLATION_PATH,"")
   ;;
  *)
    ANT_HAS_WIN_ENV="no"
    AC_DEFINE(ANT_HAS_WIN_ENV,0)
    AC_DEFINE_UNQUOTED(ANT_INSTALLATION_PATH,"${prefix}")
   ;;
esac
AC_SUBST(ANT_HAS_WIN_ENV)
AM_CONDITIONAL(ANT_HAS_WIN_ENV, test x$ANT_HAS_WIN_ENV = "xyes" )
AC_SUBST(ANT_HAS_MINGW_ENV)
AM_CONDITIONAL(ANT_HAS_MINGW_ENV, test x$ANT_HAS_MINGW_ENV = "xyes" )

# include path
ANT_INCLUDEPATH="$includedir/AnT"
AC_SUBST(ANT_INCLUDEPATH)

dnl --------------------------------------------------------------------
# networking support
AC_ARG_ENABLE(network,[  --enable-network        Enable the network support (if the needed libraries are present) [[default=yes]]])
if test x$enable_network != xno; then
  if test x$ANT_HAS_WIN_ENV = "xyes"; then
    HAVE_SOCKET="yes"
    HAVE_GETHOST="yes"
    SOCKET_LIBS="-lwsock32"
  else
    AC_CHECK_LIB([nsl],[gethostbyname], [HAVE_GETHOST="yes" SOCKET_LIBS="-lnsl $SOCKET_LIBS"], [HAVE_GETHOST="no"], $SOCKET_LIBS)
    AC_CHECK_LIB([c],[socketpair], [HAVE_SOCKET="yes" SOCKET_LIBS="-lc $SOCKET_LIBS"],
    [HAVE_SOCKET="no"
    AC_MSG_WARN([Couldn't find socket in c library,
                 will be search in the another library])
    AC_CHECK_LIB([socket],[socketpair], [HAVE_SOCKET="yes" SOCKET_LIBS="-lsocket $SOCKET_LIBS"],
    [HAVE_SOCKET="no" 
    AC_MSG_WARN([Couldn't find socket library])], $SOCKET_LIBS)],$SOCKET_LIBS)

    if test x$HAVE_SOCKET = "xyes" ; then
       AC_MSG_NOTICE([Found libsocket library and necessary routines for network calculating.])
       echo "           Configuring with support for network calculating."
    else
       AC_MSG_WARN([Couldn't find necessary routines for network calculating.])
       echo "           Configuring without network calculating support."
    fi

  fi
else
  HAVE_SOCKET="no"
  AC_MSG_NOTICE([Configuring without network support due the option: --enable-network=no ])
  AC_MSG_ERROR([Configuring without network is not supported yet])
fi
if test x$HAVE_SOCKET = xyes; then
  LIBS="$SOCKET_LIBS $LIBS"
  CONF_WITH_SOCKET="yes"
  AC_DEFINE(ANT_HAS_LIBSOCKET,1,[Defined to 1 if the libsocket library and necessary routines for network calculating are available])
else
  CONF_WITH_SOCKET="no"
  AC_DEFINE(ANT_HAS_LIBSOCKET,0)
fi
AC_SUBST(SOCKET_LIBS)
AM_CONDITIONAL(HAVE_LIBSOCKET, test x$HAVE_SOCKET = "xyes")

# frequency analysis support (fftw)
AC_ARG_WITH(fftw,[  --with-fftw             Enable the fftw support (if the needed libraries are present) [[default=yes]]])
if test x$with_fftw != xno; then
  AC_CHECK_LIB([fftw3],[fftw_plan_dft_1d],[ dnl libfftw3 found
     AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <fftw3.h>]])
     ],[ dnl fftw3.h found
        LIBS="-lfftw3 $LIBS"
        HAVE_FFTW="yes"
        FOUND_FFTW_VERSION=", version 3"
        CONF_WITH_FFTW="yes"
        AC_DEFINE(ANT_HAS_LIBFFTW,3,[Contains the version of the fftw library, 0 if none available])
           AC_MSG_NOTICE([Found libfftw library version 3 and the corresponding header file is available
           Configuring with support for frequency analysis.])
     ],[ dnl fftw3.h not found
        AC_MSG_ERROR([libfftw library version 3 found, but the corresponding header file is not available.
               Please correct the INCLUDES environment variable or
               configure without fftw support (see instructions in the README file).])])
  ],[ dnl libfftw3 not found
     AC_CHECK_LIB([fftw],[fftw_one],[ dnl libfftw found
        AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <fftw.h>]])],[ dnl fftw.h found
           LIBS="-lfftw $LIBS"
           HAVE_FFTW="yes"
           FOUND_FFTW_VERSION=", version 2"
           CONF_WITH_FFTW="yes"
           AC_DEFINE(ANT_HAS_LIBFFTW,2)
           AC_MSG_NOTICE([Found libfftw library version 2 and the corresponding header file is available
           Configuring with support for frequency analysis.])
        ],[ dnl fftw.h not found
           CONF_WITH_FFTW="no"
           AC_MSG_ERROR([libfftw library version 2 found, but the corresponding header file is not available.
                  Please correct the INCLUDES environment variable or
                  configure without fftw support (see instructions in the README file).])])
     ],[ dnl neither libfftw3 nor libfftw found
        HAVE_FFTW="no"
        FOUND_FFTW_VERSION=""
        CONF_WITH_FFTW="no"
        AC_MSG_WARN([Couldn't find libfftw library. Configuring without frequency analysis support.])],
     $LIBS)],
  $LIBS)
else
  CONF_WITH_FFTW="no"
  AC_MSG_NOTICE([Configuring without fftw support due the option: --with-fftw=no ])
  AC_DEFINE(ANT_HAS_LIBFFTW,0)
fi

AM_CONDITIONAL(HAVE_LIBFFTW, test x$CONF_WITH_FFTW = "xyes" )

AC_LANG_PUSH([C])

# singular value decomposition analysis support (clapack)
AC_ARG_WITH(lapack,[  --with-lapack           Enable the lapack (if the needed libraries are present)       [[default=yes]]])
if test x$with_lapack != xno; then
  if test x$ANT_HAS_WIN_ENV = "xyes"; then
        HAVE_LAPACK="no"
  else
     AC_CHECK_LIB([F77],[main],[LIBS="-lF77 $LIBS"],,$LIBS)
     AC_CHECK_LIB([I77],[fread],[LIBS="-lI77 $LIBS"],,$LIBS)
     AC_CHECK_LIB([blas],[lsame_],[LIBS="-lblas $LIBS"],,$LIBS)
     AC_CHECK_LIB([eigsrc],[slarrb_], [LIBS="-leigsrc $LIBS"],,$LIBS)
     AC_CHECK_LIB([linsrc],[slaic1_], [LIBS="-llinsrc $LIBS"],,$LIBS)
     AC_CHECK_LIB([lapack],[slacpy_],[LIBS="-llapack $LIBS"
       AC_CHECK_FUNC([dgesvd_],
         [HAVE_LAPACK="yes"
          AC_MSG_NOTICE([Found needed function in lapack library.])],
         [HAVE_LAPACK="no"
          AC_MSG_WARN([Couldn't find needed function in lapack library.])]
       )],[HAVE_LAPACK="no"],
     $LIBS)
  fi
CFLAGS=$save_CFLAGS
else
  HAVE_LAPACK="no"
  AC_MSG_NOTICE([Configuring without lapack support due the option: --with-lapack=no ])
fi
if test x$HAVE_LAPACK = xyes; then
  CONF_WITH_LAPACK="yes"
  AC_MSG_NOTICE([Configuring with support for singular value decomposition analysis."])
  AC_DEFINE(ANT_HAS_LIBLAPACK,1,[Defined to 1 if the clapack library is available])
else
  CONF_WITH_LAPACK="no"
  AC_MSG_NOTICE([Configuring without singular value decomposition analysis support."])
  AC_DEFINE(ANT_HAS_LIBLAPACK,0)
fi
AM_CONDITIONAL(HAVE_LIBLAPACK, test x$HAVE_LAPACK = "xyes")

# support for convex hulls computation (qhull)
AC_ARG_WITH(qhull,[  --with-qhull            Enable the qhull support (if the needed libraries are present) [[default=yes]]])
if test x$with_qhull != xno; then
  AC_CHECK_LIB([qhull],[qh_clock],[ dnl libqhull found
     AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <qhull/qhull_a.h>]])
       ],[ dnl qhull.h found
           LIBS="-lqhull $LIBS"
           HAVE_QHULL="yes"
           CONF_WITH_QHULL="yes"
           AC_DEFINE(ANT_HAS_LIBQHULL,1,[Contains the version of the qhull library, 0 if none available])
           AC_MSG_NOTICE([Found libqhull library and the corresponding header file is available
           Configuring with support for convex hulls computation.])
          ],[ dnl qhull.h not found
           HAVE_QHULL="no"
           CONF_WITH_QHULL="no"
           AC_DEFINE(ANT_HAS_LIBQHULL,0)
           AC_MSG_WARN([libqhull library found, but the corresponding header file is not available.
                  Please correct the INCLUDES environment variable or
                  configure without qhull support (see instructions in the README file).])])
     ],[ dnl libqhull not found
        HAVE_QHULL="no"
        CONF_WITH_QHULL="no"
        AC_DEFINE(ANT_HAS_LIBQHULL,0)
        AC_MSG_WARN([Couldn't find libqhull library. Configuring without support for convex hulls computation.])],
  $LIBS)
else
  CONF_WITH_QHULL="no"
  AC_MSG_NOTICE([Configuring without qhull support due the option: --with-qhull=no ])
  AC_DEFINE(ANT_HAS_LIBQHULL,0)
fi

AM_CONDITIONAL(HAVE_LIBQHULL, test x$CONF_WITH_QHULL = "xyes" )

AC_LANG_POP([C])

# visualization support (OpenGL)
AC_ARG_ENABLE(visualization,[  --enable-visualization  Enable the visualization (if the needed libraries and headers are present)   [[default=yes]]])
if test x$enable_visualization != xno; then
  if test x$ANT_HAS_WIN_ENV = "xyes"; then
     GL_LIBS="-lopengl32 -lglu32 -lglut32"
     HAVE_GL="yes"
     HAVE_GLU="yes"
     HAVE_GLUT="yes"
  else
     AC_CHECK_LIB(pthread, pthread_create, [GL_LIBS="-lpthread $GL_LIBS"], [AC_MSG_WARN([Couldn't find libpthread])],$GL_LIBS)
     AC_CHECK_LIB(X11, XOpenDisplay, [GL_LIBS="-lX11 $GL_LIBS"], [AC_MSG_WARN([Couldn't find libX11])],$GL_LIBS)
     AC_CHECK_LIB(Xmu, XmuLookupStandardColormap, [GL_LIBS="-lXmu $GL_LIBS"], [AC_MSG_WARN([Couldn't find libXmu])],$GL_LIBS)
     AC_CHECK_LIB(GL, glXQueryExtension, [
                  AC_CHECK_HEADER([GL/gl.h], [
                     AC_MSG_NOTICE([Header file corresponding to libGL found])
                     HAVE_GL="yes" GL_LIBS="-lGL $GL_LIBS"],[
                     AC_MSG_NOTICE([Header file corresponding to libGL not found])
                     HAVE_GL="no"])],
        [HAVE_GL="no"
        AC_MSG_WARN([Couldn't find the necessary library for visualization support libGL])],$GL_LIBS)
     AC_CHECK_LIB(GLU, gluCylinder, [
                  AC_CHECK_HEADER([GL/glu.h], [
                     AC_MSG_NOTICE([Header file corresponding to libGLU found])
                     HAVE_GLU="yes" GL_LIBS="-lGLU $GL_LIBS"],[
                     AC_MSG_NOTICE([Header file corresponding to libGLU not found])
                     HAVE_GLU="no"])],
        [HAVE_GLU="no"
        AC_MSG_WARN([Couldn't find the necessary library for visualization support libGLU])],$GL_LIBS)
     AC_CHECK_LIB(glut, glutInit, [
                  AC_CHECK_HEADER([GL/glut.h], [
                     AC_MSG_NOTICE([Header file corresponding to libglut found])
                     HAVE_GLUT="yes" GL_LIBS="-lglut $GL_LIBS"],[
                     AC_MSG_NOTICE([Header file corresponding to libglut not found])
                     HAVE_GLUT="no"])],
        [HAVE_GLUT="no"
        AC_MSG_WARN([Couldn't find the necessary library for visualization support libglut])],$GL_LIBS)
  fi
else
  CONF_WITH_OpenGL="no"
  AC_MSG_NOTICE([Configuring without OpenGL support due the option: --enable-visualization=no ])
fi
if [ [ x$HAVE_GL = "xyes" ] && [ x$HAVE_GLU = "xyes" ] && [ x$HAVE_GLUT = "xyes" ] ] ; then
  AC_SUBST(GL_LIBS)
  CONF_WITH_OpenGL="yes"
  AC_MSG_NOTICE([Found all necessary libraries and headers for OpenGL support.])
  echo "           Configuring with support for OpenGL."
  AC_DEFINE(ANT_HAS_VIS,1,[Defined to 1 if the necessary libraries for visualization support are available])
else
   CONF_WITH_OpenGL="no"
   AC_MSG_WARN([Couldn't find necessary OpenGL libraries or corresponding headers.])
   echo "           Configuring without OpenGL support."
  AC_DEFINE(ANT_HAS_VIS,0)
fi
AM_CONDITIONAL(HAVE_LIBGLUT, test x$CONF_WITH_OpenGL = "xyes" )

# GUI support (X11/GTK/GDK)
AC_ARG_ENABLE(gui,[  --enable-gui            Enable the gui (if the needed libraries are present)             [[default=yes]]])
if test x$enable_gui != xno ; then
  if test x$ANT_HAS_WIN_ENV = "xyes" ; then
     if test x$GTK_DEV_DIR = "x" ; then
       AC_MSG_ERROR([The variable GTK_DEV_DIR is empty. Pease set this variable
                     to the directory containing the developer version of GTK+ 2.])
     else
       if test `${GTK_DEV_DIR}/bin/pkg-config gtk+-2.0 --modversion` > /dev/null ; then
          GTK_LIBS=`${GTK_DEV_DIR}/bin/pkg-config gtk+-2.0 --libs`
          GTK_INCLUDES=`${GTK_DEV_DIR}/bin/pkg-config gtk+-2.0 --cflags`
          HAVE_GTK="yes"
          ANT_GTK_CONFIG_VERSION_CMD="${GTK_DEV_DIR}/bin/pkg-config gtk+-2.0 --modversion"
       else
          HAVE_GTK="no"
          AC_MSG_ERROR([Couldn't find necessary libraries for GUI support.])
       fi
     fi
  else
      ANT_HAS_WIN_ENV="no"
      # GTK+ 2.0 support
      AC_ARG_ENABLE(GTK_2,[  --enable-gtk_2          Enable the GTK+ 2.0 (if the needed libraries are present)        [[default=yes]]])
      if test x$enable_gtk_2 != xno && pkg-config gtk+-2.0 --modversion > /dev/null ; then
         ANT_GTK_CONFIG_CMD="pkg-config gtk+-2.0"
         ANT_GTK_CONFIG_VERSION_CMD="pkg-config gtk+-2.0 --modversion"
      else
         ANT_GTK_CONFIG_CMD=""
         if gtk-config --version > /dev/null ; then
            ANT_GTK_CONFIG_CMD="gtk-config"
            ANT_GTK_CONFIG_VERSION_CMD="gtk-config --version"
         fi
      fi
      if test "x$ANT_GTK_CONFIG_CMD" != "x" ; then
         GTK_LIBS=`${ANT_GTK_CONFIG_CMD} --libs`
         GTK_INCLUDES=`${ANT_GTK_CONFIG_CMD} --cflags`
         HAVE_GTK="yes"
         AC_MSG_NOTICE([Found necessary libraries for GUI support.])
         echo "           Configuring with support for GUI."
      else
         HAVE_GTK="no"
         AC_MSG_WARN([Couldn't find necessary libraries for GUI support.])
         echo "           Configuring without GUI support."
      fi
  fi
else
  HAVE_GTK="no"
  AC_MSG_NOTICE([Configuring without GUI support due the option: --enable-gui=no ])
fi
if test x$HAVE_GTK = xyes; then
  CONF_WITH_GTK="yes"
  FOUND_GTK_VERSION=", version `${ANT_GTK_CONFIG_VERSION_CMD}`"
  AC_SUBST(GTK_LIBS)
  AC_SUBST(GTK_INCLUDES)
  AC_DEFINE(ANT_HAS_GTK,1,[Defined to 1 if the necessary libraries for GUI support are available])
else
  CONF_WITH_GTK="no"
  FOUND_GTK_VERSION=""
  AC_DEFINE(ANT_HAS_GTK,0)
fi
AM_CONDITIONAL(HAVE_GTK, test x$HAVE_GTK = "xyes" )

dnl--------------------
#Danny test
#LIBS = "/usr/local/lib $LIBS"
#QHULL_LIBS="/usr/local/lib/libqhull.a "
#AC_SUBST(QHULL_LIBS)
#AC_DEFINE(ANT_HAS_LIBQHULL,1,[Defined to 1 if the necessary libraries are available])

dnl --------------------------------------------------------------------
# Checks for library functions.
# AC_FUNC_MALLOC

dnl --------------------------------------------------------------------
# Checks for header files.
AC_HEADER_STDC
#AC_CHECK_HEADER([sys/time.h math.h])

dnl --------------------------------------------------------------------
# Checks for typedefs, structures, and compiler characteristics.
#case "$host_or_hostalias" in
#  *mingw*)
#    CFLAGS="$CFLAGS -Wall"
#    CXXFLAGS="$CXXFLAGS -Wall"
#    ;;
#  *)
CFLAGS="-Wall"
CXXFLAGS="-Wall"
#CXXFLAGS=`echo $CXXFLAGS | sed 's/-g//' | sed 's/-O.//'` # this is needed due problems with optimization
#    ;;
#esac


dnl --------------------------------------------------------------------
# Set flag to export symbols (system-plugins need to use symbols that are within the AnT binary).
AC_MSG_NOTICE([You work on '$host_or_hostalias' system. Set flag to export symbols for this system])
case "$host_or_hostalias" in
  *linux* | *-gnu-*)
    LDFLAGS="-Wl,--export-dynamic $LDFLAGS"
    AC_DEFINE(ANT_SHARED_LIB_EXT,"so",[Convey the suffix of the shared library (default is so)])
    ;;
  *solaris*)
    LDFLAGS="-Wl,-Bdynamic $LDFLAGS"
    AC_DEFINE(ANT_SHARED_LIB_EXT,"so")
    ;;
  *cygwin* | *pw32* | *os2*)
    LDFLAGS="-Wl,--export-dynamic -no-undefined -Wl,--dll-search-prefix='' $LDFLAGS"
    AC_DEFINE(ANT_SHARED_LIB_EXT,"dll")
    ;;
  *mingw*)
    LIBS=`echo $LIBS | sed 's/-lm//'`
    LDFLAGS="-Wl,--export-dynamic  $LDFLAGS"
    AC_DEFINE(ANT_SHARED_LIB_EXT,"dll")
    ;;
esac

#    CFLAGS="-mdll -mno-cygwin -mms-bitfields -DNDEBUG=1 $CFLAGS"
#    CXXFLAGS="-mdll -mno-cygwin -mms-bitfields -DNDEBUG=1 $CXXFLAGS"
#    LDFLAGS="-Wl,--export-dynamic --dll-search-prefix='' -no-undefined -mno-cygwin -mms-bitfields $LDFLAGS"

# -lcomctl32 -mwindows

#      i386-*-freebsd*)
#        LDFLAGS="$LDFLAGS..."
#        ;;

if test x$ANT_LDFLAGS != "x" ; then
AC_MSG_WARN([The variable ANT_LDFLAGS is set by user. Set this flag for linking additionally])
fi
AC_SUBST(ANT_LDFLAGS)

dnl --------------------------------------------------------------------
AC_CONFIG_FILES([
Makefile
AnT.pc
build.sh
README
doc/Makefile
doc/doxygen/Makefile
doc/doxygen/AnT_footer.html
doc/doxygen/doxy.cfg
doc/tex/head.tex
src/Makefile
src/antvis/Makefile
src/antvis/glui/Makefile
src/antvis/glui/images/Makefile
src/gtk-gui/Makefile
src/gtk-gui/win_rc/Makefile
src/gtk-gui/win_rc/AnT-gui.rc
src/gtk-gui/logos/Makefile
src/engine/Makefile
src/engine/cas/Makefile
src/engine/data/Makefile
src/engine/iterators/Makefile
src/engine/methods/Makefile
src/engine/methods/bandcounter/Makefile
src/engine/methods/conditions/Makefile
src/engine/methods/dimensions/Makefile
src/engine/methods/fourier/Makefile
src/engine/methods/general/Makefile
src/engine/methods/lyapunov/Makefile
src/engine/methods/output/Makefile
src/engine/methods/period/Makefile
src/engine/methods/region/Makefile
src/engine/methods/spatial/Makefile
src/engine/methods/symbolic/Makefile
src/engine/methods/symimages/Makefile
src/engine/methods/sipa/Makefile
src/engine/methods/singlvalues/Makefile
src/engine/methods/visualization/Makefile
src/engine/network/Makefile
src/engine/proxies/Makefile
src/engine/simulators/Makefile
src/engine/utils/Makefile
src/engine/utils/conditions/Makefile
src/engine/utils/datareader/Makefile
src/engine/utils/dimensionhandler/Makefile
src/engine/utils/enumerators/Makefile
src/engine/utils/machines/Makefile
src/engine/utils/noise/Makefile
src/engine/utils/progress/Makefile
src/engine/utils/timer/Makefile
src/engine/utils/win_rc/AnT.rc
src/engine/utils/win_rc/Makefile
src/share/Makefile
src/utils/Makefile
src/utils/arrays/Makefile
src/utils/config/Makefile
src/utils/debug/Makefile
src/utils/matheval/Makefile
src/utils/socket/Makefile
src/utils/strconv/Makefile
src/utils/regex/Makefile
src/utils/winenv/Makefile
src/utils/winenv/rc/Makefile
])

AC_OUTPUT
echo "========================================================================"
echo "Current options setting:"
echo " CC               : $CC"
echo " CPPFLAGS         : $CPPFLAGS"
echo " CFLAGS           : $CFLAGS"
echo " CXXFLAGS         : $CXXFLAGS"
echo ""
echo " LD               : $LD"
echo " LDFLAGS          : $LDFLAGS"
echo ""
echo " LIBS             : $LIBS"
echo " GL_LIBS          : $GL_LIBS"
echo " GTK_LIBS         : $GTK_LIBS"
echo ""
echo " INCLUDES         : $INCLUDES"
echo " GTK_INCLUDES     : $GTK_INCLUDES"
echo ""
echo " HAVE_GETHOST     : $HAVE_GETHOST"
echo " HAVE_SOCKET      : $HAVE_SOCKET"
echo " HAVE_FFTW        : $HAVE_FFTW$FOUND_FFTW_VERSION"
echo " HAVE_LAPACK      : $HAVE_LAPACK"
echo " HAVE_QHULL       : $HAVE_QHULL"
echo " HAVE_GL          : $HAVE_GL"
echo " HAVE_GLU         : $HAVE_GLU"
echo " HAVE_GLUT        : $HAVE_GLUT"
echo " HAVE_GTK         : $HAVE_GTK$FOUND_GTK_VERSION"
echo ""
echo " CONF_WITH_SOCKET : $CONF_WITH_SOCKET"
echo " CONF_WITH_FFTW   : $CONF_WITH_FFTW"
echo " CONF_WITH_LAPACK : $CONF_WITH_LAPACK"
echo " CONF_WITH_QHULL  : $CONF_WITH_QHULL"
echo " CONF_WITH_OpenGL : $CONF_WITH_OpenGL"
echo " CONF_WITH_GTK    : $CONF_WITH_GTK"
echo ""
echo " LIBTOOL_VERSION  : `libtool --version | sed -ne "/libtool/p"`"
echo " AUTOCONF_VERSION : `autoconf --version | sed -ne "/autoconf/p"`"
echo " AUTOMAKE_VERSION : `automake --version | sed -ne "/automake/p"`"
echo " GCC_VERSION      : `gcc --version | sed -ne "/gcc/p"`"
echo "------------------------------------------------------------------------"
echo "Remark! Important for all ordinary users of this package!"
echo ""
echo " Please check that:"
echo "     ${exec_prefix}/bin"
echo " is included in the enviroment variable PATH"
echo "========================================================================"
echo ""

