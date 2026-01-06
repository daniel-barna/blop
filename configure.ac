# You do not need to use/have configure.ac
# It is only used by the developer, and is included in the CVS repository
# only to keep it from being lost accidentally

# CXXFLAGS variable should contain the flags for the actual compilation tests,
# since this variable is used by configure. For some reason, CXXFLAGS
# can not contain `gts-config --cflags` type things, so we always have to
# expand this.
# However, the final cxxflags output by configure should contain the non-expanded
# versions (i.e. `gts-config --cflags` and similar, instead of the expanded value)
# so this will be accumulated in CXXFLAGS_FINAL, and after all the compilation
# checks have been done, it will be copied to CXXFLAGS

AC_INIT
# On ubuntu, install autoconf-archive
#m4_include([/usr/share/aclocal/ax_cxx_compile_stdcxx_11.m4])
#m4_include([/usr/share/aclocal/ax_cxx_compile_stdcxx_14.m4])
#m4_include([/usr/share/aclocal/ax_cxx_compile_stdcxx_17.m4])
#m4_include([/usr/share/aclocal/ax_cxx_compile_stdcxx.m4])
m4_include([/usr/share/aclocal/ax_check_compile_flag.m4])

[ 
rm -f scripts/blop-config-vars 
CWD=`pwd`
MISSING_SW=""
INSTALL_PKGS_CMD=""
SUGGESTED_PKG_LIST=""
DISTRO="`lsb_release -i |  sed 's/.*:[ \t]*//g'`"
RELEASE="`lsb_release -r |  sed 's/.*:[ \t]*//g'`"
echo "**"
echo "** Distro : $DISTRO"
echo "** Release: $RELEASE"
echo "**"
echo
if [ "$DISTRO" = "Ubuntu" -o "$DISTRO" = "Debian" ] ; then
  INSTALL_PKGS_CMD="sudo apt-get install "
elif [ "$DISTRO" = "ScientificCERNSLC" ] ; then
  INSTALL_PKGS_CMD="sudo yum install "
elif [ "$DISTRO" = "SUSE LINUX" ] ; then
  INSTALL_PKGS_CMD="sudo yast -i "
else
  INSTALL_PKGS_CMD="sudo <your_package_manager_cmd_to_install_packages> "
fi
]


#[ 
# CXXFLAGS="" 
# CFLAGS=""
# LDFLAGS=""
# LIBS=""
# X_LIBS=""
#]




BLOP_CONFIG_OPTIONS=$ac_configure_args
AC_SUBST([BLOP_CONFIG_OPTIONS])

AC_ARG_ENABLE(cint,[[
  --disable-cint          Disable CINT, the C/C++ interpreter. No interpreted
                          execution will be possible, only blop library will
                          be created to link against, and use blop's code
                          in compiled programs. 
                          Blop scripts (which are C++ programs) can still be
                          quickly executed by   blop -c script.C,
                          which will compile the given script and run it.]],[ ],
[[
  # if no --disable-cint or --enable-cint was given, default is yes.
  enable_cint=yes
]])

# 
AS_IF([test "$enable_cint" = "yes"],[
  AC_CHECK_PROG(CINT_CONFIG, cint-config, `which cint-config`, [])
  AS_IF([test "$CINT_CONFIG" != ""], 
  [],[
     enable_cint=no;
     MISSING_SW="$MISSING_SW\nCINT           - The C/C++ Interpreter
                 Blop can only be used in a compiled environment, 
                 no interpreter will be available.
                 If you do want to use blop with CINT, install 
                 CINT first, then come back here and rerun 
                 ./configure && make
                 If it's ok for you to go without CINT, just run
                 make, make doc, sudo make install\n"
  ])
])

[BLOP_ENABLE_CINT=$enable_cint]
AS_IF([test "$BLOP_ENABLE_CINT" = "yes"],[AC_DEFINE([BLOP_ENABLE_CINT],[yes])],[])
AC_SUBST([BLOP_ENABLE_CINT])


AC_ARG_ENABLE(debug,[[
  --enable-debug[=arg]    Compile with debugging enabled [disabled]
                          The extra arg can be for example 'gdb'
                          to produce debugging info for gdb. See
                          'man gcc' to find other possibilities.]],
[[
  if [ "$enable_debug" != "no" ] ; then
    if [ "$enable_debug" = "yes" ] ; then
      enable_debug=""
    fi
    CXXFLAGS_FINAL="$CXXFLAGS -DDEBUG -g$enable_debug"
  fi
]])



# ------------------- check for operating system -----------------------------

AC_MSG_CHECKING([[architecture]])
[BLOP_ARCH=`uname -s | tr '[A-Z]' '[a-z]'`]
AC_SUBST([BLOP_ARCH])
AC_MSG_RESULT([$BLOP_ARCH])

# ------------------- check for standard header files ------------------------
AC_PROG_CXX
AC_LANG([C++])
#AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
#[ CXXFLAGS_FINAL="$CXXFLAGS" ]

[CXXSTD=""] 
AX_CHECK_COMPILE_FLAG([-std=c++20], [CXXSTD="-std=c++20"],
  AX_CHECK_COMPILE_FLAG([-std=c++18], [CXXSTD="-std=c++18"],
    AX_CHECK_COMPILE_FLAG([-std=c++17], [CXXSTD="-std=c++17"],
      AX_CHECK_COMPILE_FLAG([-std=c++14], [CXXSTD="-std=c++14"],
        AX_CHECK_COMPILE_FLAG([-std=c++11], [CXXSTD="-std=c++11"])
      )
    )
  )
)
[ CXXFLAGS_FINAL="$CXXFLAGS_FINAL $CXXSTD" ] 
AC_SUBST([CXXSTD])

[ STDCPPHEADERS="ok" ]
AC_CHECK_HEADERS([iostream],[],[STDCPPHEADERS="error"])
AC_CHECK_HEADERS([streambuf],[],[STDCPPHEADERS="error"])
AC_CHECK_HEADERS([fstream],[],[STDCPPHEADERS="error"])
AC_CHECK_HEADERS([cmath],[],[STDCPPHEADERS="error"])
AC_CHECK_HEADERS([cstdlib],[],[STDCPPHEADERS="error"])
AC_CHECK_HEADERS([cstdio],[],[STDCPPHEADERS="error"])
AS_IF([test "$STDCPPHEADERS" = "ok"],[],
   AC_MSG_ERROR([
 **   Some of the standard c++ header files do not exist
 **   on your system. Your compiler is probably too old
]))

#AC_CHECK_HEADERS([X11/Xlib.h],[],AC_MSG_ERROR([[missing. Install the corresponding package (libx11-dev on Ubuntu)]]))
#AC_CHECK_LIB(X11, XOpenDisplay, [x_libs="-lX11 $x_libs"],,$full_x_libs)
AC_CHECK_LIB(X11, XOpenDisplay,,,)
AC_SUBST([X_LIBS])
AC_SUBST([X_CFLAGS])

AC_CHECK_HEADERS([regex.h],[],
  [
      MISSING_SW="$MISSING_SW\nregex.h        - Regular expression search will be disabled
                 If you do not have the regex library, you will miss the regular
                 expression searching/replacing feature. If you want to have
                 this feature, install the package containing this library and header
                 file, and rerun configure\n"
      if [ "$DISTRO"="Ubuntu" -o "$DISTRO"="Debian" ] ; then
        SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST libc6-dev"
      else
        SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST libc6-dev"
      fi
  ]
  )

[NCURSES_OR_TERMCAP=""]
AC_SUBST([NCURSES_OR_TERMCAP])
AC_CHECK_LIB([termcap],[tgetent],[NCURSES_OR_TERMCAP=termcap],[])
AS_IF([test "$NCURSES_OR_TERMCAP" = ""],AC_CHECK_LIB([ncurses],[tgetent],[NCURSES_OR_TERMCAP=ncurses],[]),[])
AS_IF([test "$NCURSES_OR_TERMCAP" = ""],AC_MSG_ERROR([Neither termcap nor ncurses is installed or usabled]),[])

# -----------------  get current blop version from changelog -----------------

AC_MSG_CHECKING([[Current blop version]])
[BLOP_VERSION=`grep -m 1 -E '^\[[0123456789]+\.[0123456789]+(\.[0123456789]+)?\]' changelog | sed 's/\[//; s/\]//'`]
AC_SUBST([BLOP_VERSION])
AC_MSG_RESULT([$BLOP_VERSION])


# ----------------- check programs -------------------------------------------


AS_IF([test -x /bin/echo],[ECHO=/bin/echo],
[
 AC_CHECK_PROG(ECHO, echo, `which echo`, [])
 AS_IF([test "$ECHO" != ""], [], AC_MSG_ERROR([[echo does not exist]]))
])
AC_SUBST([ECHO])

AC_CHECK_PROG(PS2EPS, ps2eps, `which ps2eps`, [])
AS_IF([test "$PS2EPS" != ""], [], 
      [
         MISSING_SW="$MISSING_SW\nps2eps          - EPS output will fail\n"
         if [ "$DISTRO" = "Ubuntu" -o "$DISTRO" = "Debian" ] ; then
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST ps2eps"
         elif [ "$DISTRO" = "ScientificCERNSLC" -o "$DISTRO" = "Redhat" ] ; then
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST ps2eps"
         else
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST ps2eps"
         fi
      ]
)
#AC_SUBST([PS2EPS])

AC_CHECK_PROG(PERL, perl, `which perl`, [])
AS_IF([test "$PERL" != ""], [], 
      [
         MISSING_SW="$MISSING_SW\nperl            - 'make doc' (documentation) will fail\n"
         if [ "$DISTRO" = "Ubuntu" -o "$DISTRO" = "Debian" ] ; then
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST perl"
         else
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST perl"
         fi
      ]
)
#AC_SUBST([PERL])

AC_CHECK_PROG(AWK,  awk,  `which awk`)
AS_IF([test "$AWK" != ""], [], 
      [
         MISSING_SW="$MISSING_SW\nawk             - Many things will fail, including blop main script\n"
         if [ "$DISTRO" = "Ubuntu" -o "$DISTRO" = "Debian" ] ; then
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST gawk"
         else
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST gawk"
         fi
      ]
)

AC_CHECK_PROG(PDFTK,  pdftk,  `which pdftk`)
AS_IF([test "$PDFTK" != ""], [], 
      [
         MISSING_SW="$MISSING_SW\npdftk           - Multipage pdf output versatile_pdf will not work\n"
         if [ "$DISTRO" = "Ubuntu" -o "$DISTRO" = "Debian" ] ; then
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST pdftk"
         else
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST pdftk"
         fi
      ]
)


AC_CHECK_PROG(PNMCROP, pnmcrop, `which pnmcrop`)
AS_IF([test "$PNMCROP" != ""], [], 
      [
         MISSING_SW="$MISSING_SW\nnetpbm(pnmcrop)  - This is needed! Install the netpbm package\n"
         if [ "$DISTRO" = "Ubuntu" -o "$DISTRO" = "Debian" ] ; then
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST netpbm"
         else
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST netpbm"
         fi
      ]
)

AC_CHECK_PROG(CONVERT, convert, `which convert`)
AS_IF([test "$CONVERT" != ""], [], 
      [
         MISSING_SW="$MISSING_SW\nimagemagick(convert) - jpg and png output formats will fail
                  Install the imagemagick package\n"]
         if [ "$DISTRO" = "Ubuntu" -o "$DISTRO" = "Debian" ] ; then
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST imagemagick"
         elif [ "$DISTRO" = "ScientificCERNSLC" -o "$DISTRO" = "Redhat" ] ; then
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST ImageMagick"
         else
           SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST imagemagick"
         fi
)


#AC_CHECK_PROG(GV, gv, `which gv`)
#AS_IF([test "$GV" != ""], [], 
#
#      [MISSING_SW="$MISSING_SW\ngv (ghostview)  - will not show plots in interactive sessions\n"]
#)


# --------  check CINT installation and version  -----------------------------

AS_IF([test "$enable_cint" = "yes"],[
  AC_CHECK_PROG(CINT_CONFIG, cint-config, `which cint-config`, [])
  AS_IF([test "$CINT_CONFIG" != ""], 
  [
    # new cint version
    AC_MSG_CHECKING([[Cint version]])
    CINT_VERSION=`$CINT_CONFIG --version`
    AC_MSG_RESULT([$CINT_VERSION])
    CINT_CONFIG_CFLAGS=`cint-config --cflags`
    CXXFLAGS_FINAL="$CXXFLAGS_FINAL "'`cint-config --cflags`'
  ], 
  [
    # no (or too old) cint
    AC_MSG_ERROR([CINT is not found (or is too old, without the cint-config script)])
    
  ])
  AC_SUBST([CINT_VERSION])


# -------------  check cint includefiles location ----------------------------

  AC_MSG_CHECKING([[Cint include files, their prefix]])
  AS_IF([test -f `cint-config --incdir`/Api.h],
  [AC_MSG_RESULT([`cint-config --incdir`])],
  [
    AS_IF([test -f `cint-config --incdir`/cint/Api.h],
    [
      AC_DEFINE([CINT_INCLUDE_PREFIX],[cint])
      AC_MSG_RESULT([`cint-config --incdir`/cint])
    ],
    [
      AC_MSG_ERROR([[Api.h not found in `cint-config --incdir`, `cint-config --incdir`/cint]])
    ])
  ])
],[])

# ----------- check LaTeX style file locations -------------------------------
[
  if [ "$DISTRO" = "Ubuntu" -o "$DISTRO" = "Debian" ] ; then
   LATEX_PACKAGES="texlive-latex-base texlive-latex-recommended texlive-latex-extra texlive-pstricks texlive-fonts-recommended"
  else
   LATEX_PACKAGES="texlive-latex-base texlive-latex-recommended texlive-latex-extra texlive-pstricks texlive-fonts-recommended"
  fi
]
AC_CHECK_PROG(LATEX, latex, `which latex`)
AS_IF([test "$LATEX" != ""], 
      [
AC_MSG_CHECKING([[LaTeX style files]])
AS_IF([test "${stydir+set}" = set],
      AC_MSG_RESULT([$stydir (user specififed)]),
[

stylefiles_warning="
keyval.sty or pstricks.sty    -    all but the  blopeps output 
		  formats will fail. These stylefiles do not exist, 
		  or LaTeX has died for some other reason. Install
                  these files (on Ubuntu they are in the 
                  texlive-latex-recommended package). If  you  are  
                  sure  everything is ok,  specify  the  following 
                  argument to configure to skip this automatic test:
                  stydir=/location/of/latex/stylefiles 
                  If  you  have  recently  installed TeX or  these 
		  style  files,  you may need to launch 'texhash' 
		  as root\n"

mkdir -p configure.tmp
cd configure.tmp
cat >test.tex <<_EOF 
\documentclass{article}
\usepackage{keyval}
\usepackage{pstricks}
\usepackage{attachfile}
\begin{document}
\end{document}
_EOF
echo Q | latex test.tex > latex.out 2>&1

AS_IF([test $? != 0],
      [
         MISSING_SW="$MISSING_SW\n$stylefiles_warning"
         SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST $LATEX_PACKAGES"
      ],

      [
        stydir=`grep /keyval\.sty latex.out | sed 's/(//g; s/)//g'`
        AS_IF([test "$stydir" = "" ], 
              [
               [
                 MISSING_SW="$MISSING_SW\n$stylefiles_warning"
                 SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST $LATEX_PACKAGES"
               ]
              ],
              [
               stydir=`echo $stydir | sed 's,/[[^/]]*$,,' | sed 's,/[[^/]]*$,,'`
               AS_IF([test "$stydir" = "" ], 
                     [
                       [
                         MISSING_SW="$MISSING_SW\n$stylefiles_warning"
                         SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST $LATEX_PACKAGES"
                       ]
                     ],
                     [
                         AS_IF([test -d "$stydir"], [], 
                               [
                                 [
                                    MISSING_SW="$MISSING_SW\nkeyval.sty - all but the blopeps output formats will fail\n"
                                    SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST $LATEX_PACKAGES"
                                 ]
                               ])
                     ]
                     )
              ]
            )
      ]
)

cd ..
rm -rf configure.tmp

AC_MSG_RESULT([[$stydir]])
AC_SUBST([stydir])
])

      ], 
      [
        MISSING_SW="$MISSING_SW\nlatex           - all but the blopeps output formats will fail\n"
        SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST $LATEX_PACKAGES"
      ]
)




# ----------- check gsl presence ---------------------------------------------

AC_CHECK_PROG(GSL_CONFIG, gsl-config, `which gsl-config`, [])
AS_IF([test "$GSL_CONFIG" != ""], 
[
  AC_DEFINE([HAVE_GSL],[1])
  AC_SUBST([HAVE_GSL],[1])
  CXXFLAGS=`gsl-config --cflags`
  CXXFLAGS_FINAL="$CXXFLAGS_FINAL "'`gsl-config --cflags`'
  AC_CHECK_DECL([gsl_fft_forward],[AC_DEFINE([HAVE_GSL_FFT_FORWARD],[1])],,
                [#include <gsl/gsl_fft.h>])
  LIBS="$LIBS "'`gsl-config --libs`';
],
      [
        MISSING_SW="$MISSING_SW\ngsl (Gnu Scientific Library)  - FFT will be limited
                  To Fourier-transform data, the fft routines from
                  gsl are used. If you do not install this library  
                  (libgsl,libgsl-dev or similar)then the algorithm  
                  of  Numerical Recipes  will be  used, which only 
                  works for data  of  length  2^n. If you  want to  
                  have fft of arbitrary-length data, install these 
                  packages  and rerun configure\n"
        if [ "$DISTRO"="Ubuntu" -o "$DISTRO"="Debian" ] ; then
          SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST libgsl0-dev"
        elif [ "$DISTRO" = "ScientificCERNSLC" -o "$DISTRO" = "Redhat" ] ; then
          SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST gsl-devel"
        elif [ "$DISTRO"="SUSE LINUX" ] ; then
          SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST gsl"
        else
          SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST libgsl0-dev"
        fi

      ]
)


# ----------- check gts presence ---------------------------------------------

AC_CHECK_PROG(GTS_CONFIG, gts-config, `which gts-config`, [])
AS_IF([test "$GTS_CONFIG" != ""], 
[
  CXXFLAGS=`gts-config --cflags`;
  CPPFLAGS=`gts-config --cflags`;
  CXXFLAGS_FINAL="$CXXFLAGS_FINAL "'`gts-config --cflags`';
  AC_CHECK_HEADER([glib.h],[],[AC_MSG_WARN("glib.h does not exist. Install the glib-devel (or similar) package")])
  AC_CHECK_HEADER([gts.h],[],[AC_MSG_WARN("gts.h or glib.h does not exist. Install both the gts-devel and glib-devel (or similar) packages")])
  LIBS="$LIBS "'`gts-config --libs`';
  AC_DEFINE([HAVE_GTS_H],[1]) 
  AC_SUBST([HAVE_GTS_H],[1])
],
      [
        MISSING_SW="$MISSING_SW\ngts            - Isolines drawstyle will not be available
                 The isolines drawstyle uses the gts library.  You
		 can compile without this feature,  but this draw-
		 style will then be  disabled.  Install the libgts 
		 and libgts-dev(el) packages,  and rerun configure"
        if [ "$DISTRO"="Ubuntu" -o "$DISTRO"="Debian" ] ; then
          SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST libgts-dev"
        elif [ "$DISTRO" = "ScientificCERNSLC" -o "$DISTRO" = "Redhat" ] ; then
          SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST gts-devel"
        elif [ "$DISTRO" = "SUSE LINUX" ] ; then
          SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST libgts"
        else
          SUGGESTED_PKG_LIST="$SUGGESTED_PKG_LIST libgts-dev"
        fi
      ]
)

# finally copy the CXXFLAGS_FINAL into CXXFLAGS (no compilation checks will be done
# any more here, so it's ok
CXXFLAGS=$CXXFLAGS_FINAL

# ------------------  Checking X11 headers/libraries locations ---------------
#AC_PATH_XTRA

# ------------------  Check package building capabilities  -------------------
[MAKE_DEB=""]
[MAKE_RPM=""]
AC_SUBST([MAKE_DEB])
AC_SUBST([MAKE_RPM])

AC_CHECK_PROG(DPKG_ARCH, dpkg-architecture, `which dpkg-architecture`, [])
AC_CHECK_PROG(DPKG,      dpkg-buildpackage, `which dpkg-buildpackage`, [])
AC_CHECK_PROG(DH_INSTALL, dh_install, `which dh_install`, [])
AC_CHECK_PROG(FAKEROOT, fakeroot, `which fakeroot`, [])
AS_IF([test "$DPKG" != "" -a "$DH_INSTALL" != "" ],
[
 AC_MSG_CHECKING([[if CINT is installed as a native package]])
 dpkg -s cint 2>&1 | grep 'Status: install ok installed' > /dev/null
 result=$?
 AS_IF([test "$result" = 0], [cint_installed_as_pkg=yes], [cint_installed_as_pkg=no])
 AC_MSG_RESULT([$cint_installed_as_pkg])
])

MAKE_DEB="yes"
AS_IF([test "$DPKG" = ""],[MAKE_DEB=""])
AS_IF([test "$DH_INSTALL" = ""],[MAKE_DEB=""])
AS_IF([test "$FAKEROOT" = ""],[MAKE_DEB=""])
AS_IF([test "$DPKG_ARCH" = ""],[MAKE_DEB=""])
AS_IF([test "$cint_installed_as_pkg" != "yes"],[MAKE_DEB=""])
AS_IF([test "$HAVE_GSL" != 1],[MAKE_DEB=""])
AS_IF([test "$HAVE_GTS_H" != 1],[MAKE_DEB=""])
AS_IF([test "$MISSING_SW" != ""],[MAKE_DEB=""])

AC_CHECK_PROG(RPMBUILD,rpmbuild,`which rpmbuild`,[])
MAKE_RPM="yes"
AS_IF([test "$RPMBUILD" = ""],[MAKE_RPM=""])
AS_IF([test "$HAVE_GSL" != 1],[MAKE_RPM=""])
AS_IF([test "$HAVE_GTS_H" != 1],[MAKE_RPM=""])
AS_IF([test "$MISSING_SW" != ""],[MAKE_RPM=""])



[echo]

AC_SUBST([LIBS])
AC_CONFIG_FILES([config.mk:config.mk.in])
AC_CONFIG_HEADERS([src/config.h:src/config.h.in])
AC_OUTPUT


[
cat <<_EOF


------------------------------------------------------------------

                   DEBIAN PACKAGE CREATION

_EOF
]
AS_IF([test "$MAKE_DEB" = "yes" ],[[
cat <<_EOF
You have the program dpkg on your system, indicating that you run
a debian-like OS.  For Debian stable  we will try  to  maintain a 
debian package on our website http://blopplot.sourceforge.net
For other debian versions, or other debian variants (Ubuntu, ...)
you may build a debian package yourself, to make the installation 
of this software easier for other folks using the same OS as you. 

If you are kind enough to do so, please proceed as follows:
(You need the package 'debhelper' to be installed on your system)

1) make deb      

   this reconfigures the package to conform to the debian 
   standards, compiles the program, and creates a .deb package
   in debian/package-files

2) sudo make debinstall

   This will install blop into the system. If everything
   works fine, you are ready
   a - to use blop
   b - to send the .deb file to your friends to install

_EOF]],
[[
cat <<_EOF
Debian package creation is disabled, because either one of these 
packages is missing:
pdpkg-buildpackage, dh_install, fakeroot
or CINT was  not  installed as a  package,  or other recommended  
packages (gsl, gts)  providing  useful functionality in blop are 
not installed.

_EOF
]])


[
cat <<_EOF
------------------------------------------------------------------

                    RPM PACKAGE CREATION

_EOF
]
AS_IF([test "$MAKE_RPM" = "yes" ],[[
cat <<_EOF
Your package management system seems to be RPM
First compile and install blop:  make; make doc; sudo make install
After this you are  ready to use blop.  However, after  installing 
you can also create an .rpm  package to make it  easier for others 
(running the same linux distro as you) to install it.
Use blop for a while, and if it runs without problems, simply say
in this directory (which you kept unchanged...)
make rpm
and check the RPMS/*/ subdirectories for 
cint-xxxx-1.rpm and blop-xxxx-1.rpm   (xxxx = version)
If they are there,  send me an email: barnad@users.sourceforge.net
to discuss how to send me these two packages. 
Thank you for your help!

_EOF]],
[[
cat <<_EOF
RPM package building disabled, because rpmbuild is  not installed, 
or CINT was not installed as an rpm package, or other recommended  
packages (gsl,gts) providing useful functionality in blop are not 
installed.
  
_EOF
]])


[
cat <<_EOF

------------------------------------------------------------------

         NORMAL COMPILATION/INSTALLATION PROCEDURE
                (without package building)

make
make test    (optional, check the file test.dir/test.ps)
make doc
sudo make install

_EOF
]


AS_IF([test "$DPKG" != "" ],[[
cat <<_EOF
(You may also want to compile/install blop through the creation of
a debian package,  in this case  follow the instructions described 
earlier in the section 'DEBIAN PACKAGE CREATION')

_EOF]])

[
cat <<_EOF
------------------------ Good Luck !! ----------------------------

_EOF

]

AS_IF([test "$MISSING_SW" != ""],


  echo "####################  MISSING FEATURES  ##########################"
  echo "##### (this may very severy limits blop's functionality)  ########"
  echo "##################################################################"
  echo
  echo -e "$MISSING_SW"
  echo
  echo "This is the suggested command to install all the missing software."
  echo "Note that  I can not  guarantee  that this is the correct package "
  echo "list in your  linux  distribution.  The list is correct on Ubuntu"
  echo
  echo $INSTALL_PKGS_CMD $SUGGESTED_PKG_LIST
  echo
  echo "##################################################################"
  echo "### We very strongly recommend to install these packages!  #######"
  echo "##################################################################"


)
