BLOP_ENABLE_CINT = no
BLOP_VERSION = 1.1.60
CINT_VERSION = 
BLOP_ARCH    = linux
BLOP_CONFIG_OPTIONS =  '--disable-cint' '--prefix=/usr/local'

USE_CINTSYSDIR = @USE_CINTSYSDIR@
CINT_INCLUDE_PREFIX = @CINT_INCLUDE_PREFIX@

prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
BINDIR = ${exec_prefix}/bin
INCDIR = ${prefix}/include
LIBDIR = ${exec_prefix}/lib
MANDIR = ${datarootdir}/man
STYDIR = /usr/share/texlive/texmf-dist/tex/latex/blop-plot
DATADIR = ${datarootdir}/blop-plot
DOCDIR = ${DATADIR}/doc
PERL = /usr/bin/perl
ECHO = /bin/echo
CXXFLAGS =  -std=c++20 `gsl-config --cflags` `gts-config --cflags`  
ifeq ($(CXX),g++)
  CXXFLAGS += -Wall -Wno-long-long -O3
endif
#CPPFLAGS = -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/sysprof-6 -pthread -I/usr/include
CXX = g++
MAKEDEPEND = ${CXX} -M ${CXXFLAGS} 
CXXSTD = -std=c++20

X_CFLAGS = 
X_LIBS = 
LIBS = -lX11  `gsl-config --libs` `gts-config --libs`

HAVE_GTS_H = 1
HAVE_GSL = 1

MAKE_DEB = 
MAKE_RPM = 

NCURSES_OR_TERMCAP = termcap

FILELIST = $(DESTDIR)$(DATADIR)/installed-files

# make sed use extended regexps.
ifeq ($(BLOP_ARCH),darwin)
  SED = sed -E
  AR = ar rsv 
else
  SED = sed -r
  AR = ar -r 
endif
