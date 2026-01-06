# This Makefile system (this Makefile, and the others in the subdirs)
# use the DESTDIR variable,  to redirect  the installation  location.
# The  location  specified by   DESTDIR   is meant to  be a temporary 
# location, typically used by  packaging softwares.  Everything  will
# be installed under subdirectories under DESTDIR, as if DESTDIR were
# the root directory (/), for example the executables  will be placed
# into ${DESTDIR}/usr/local/bin/, etc
# However, this  software needs  some hardcoded  paths in its scripts.
# The final  installation  directories  are meant to be those  without 
# the ${DESTDIR} prefix, and these will be hardcoded into the scripts.
# So never  use  for example  the  'make install DESTDIR=/opt/blop'  
# command to install this software to under /opt/blop, this will result
# in unusable scripts. Specify the installation prefix using
# ./configure --prefix=/opt/blop
# for example. 'make install DESTDIR=/some/directory'  sould  only be
# used when building packages (for example a debian package)

BLOP_BUILD_DIR := $(shell pwd)
export BLOP_BUILD_DIR

.PHONY : clean distclean install install-nodoc uninstall test doc rpm scripts compile man tools

include config.mk

ifeq ($(USE_CINTSYSDIR),1)
ifeq ($(LD_LIBRARY_PATH),)
  LD_LIBRARY_PATH = $(CINTSYSDIR)
else
  LD_LIBRARY_PATH += :$(CINTSYSDIR)
endif
export LD_LIBRARY_PATH
endif

all : compile tools scripts man

compile:
	cd src; $(MAKE) 

scripts:
	cd scripts; $(MAKE)

man:
	cd man; $(MAKE)

tools : 
	cd tools; $(MAKE)

clean:
	cd src; INCLUDEDEPS=false $(MAKE) clean
	cd tools; $(MAKE) clean
	cd test.dir; $(MAKE) clean
	cd latex; $(MAKE) clean
	cd doc; $(MAKE) clean
	cd scripts; $(MAKE) clean
	cd man; $(MAKE) clean
	find . -name \*~  -exec rm {} \;
	find . -name \#\*  -exec rm {} \;
	rm -rf *.spec RPMS


distclean:
	cd src; INCLUDEDEPS=false $(MAKE) distclean
	cd tools; $(MAKE) distclean
	cd test.dir; $(MAKE) distclean
	cd latex; $(MAKE) distclean
	cd doc; $(MAKE) distclean
	cd scripts; $(MAKE) distclean
	cd man; $(MAKE) distclean
	find . -name \*~  -exec rm {} \;
	find . -name \#\*  -exec rm {} \;
	rm -rf *.spec RPMS
	rm -rf RPM config.mk autom4te.cache config.status config.cache config.log config.guess config.sub configure.ac.tmp build-stamp

test:
	cd src; $(MAKE)
	cd test.dir; $(MAKE)

doc:
	cd doc; $(MAKE)

install:
	@if which rpm >/dev/null 2>&1 && rpm -q blop-plot >/dev/null 2>&1 ; then \
	 echo "Blop is installed via the rpm package 'blop-plot'"; \
	 echo "I will not install now, since it would interfere with the package"; \
	 false; \
	 fi
	@if [ "$$DEB_PKG_BUILDING" != 1 ] && which dpkg >/dev/null 2>&1 && dpkg -s blop-plot 2>&1 | grep 'Status: install ok installed' >/dev/null 2>&1 ; then \
	 echo "Blop is installed via the deb package 'blop-plot'"; \
	 echo "I will not install now, since it would interfere with the package"; \
	 false; \
	 fi
	@if which blop-config >/dev/null 2>&1 ; then\
	   if [ "$(DESTDIR)" = "" ] ; then \
	     if [ "`blop-config --docdir`" != ${DOCDIR} ] ; then \
	      echo Old installation exists, and docdir does not agree; \
	      false; \
             fi; \
	     if [ "`blop-config --stydir`" != ${STYDIR} ] ; then \
	      echo Old installation exists, and stydir does not agree; \
	      false; \
             fi; \
	     if [ "`blop-config --datadir`" != ${DATADIR} ] ; then \
	      echo Old installation exists, and datadir does not agree; \
	      false; \
             fi; \
	     if [ "`blop-config --bindir`" != ${BINDIR} ] ; then \
	      echo Old installation exists, and bindir does not agree; \
	      false; \
             fi; \
	     if [ "`blop-config --libdir`" != ${LIBDIR} ] ; then \
	      echo Old installation exists, and libdir does not agree; \
	      false; \
             fi; \
	     if [ "`blop-config --incdir`" != ${INCDIR} ] ; then \
	      echo Old installation exists, and incdir does not agree; \
	      false; \
             fi; \
	   fi; \
	 fi
	@rm -f $(FILELIST)
	@if [ ! -d ${DESTDIR}${DATADIR} ] ; then mkdir -p ${DESTDIR}${DATADIR}; fi
ifeq ($(DESTDIR),)
	echo  $(FILELIST)                         >> $(FILELIST)
else
	echo  $(FILELIST) | sed 's|${DESTDIR}||g' >> $(FILELIST)
endif
	cd tools; $(MAKE) install
	cd latex; $(MAKE) install
	cd src; $(MAKE) install
	cd scripts; $(MAKE) install
	cd doc; $(MAKE) install
	cd man; $(MAKE) install
	@cp changelog $(DESTDIR)$(DOCDIR)/
	@echo $(DOCDIR)/changelog >> $(FILELIST)
	@echo
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@echo "%%%                                                         %"
	@echo "%%% It is NOT an error message, although it looks like that %"
	@echo "%%%                                                         %"
	@echo "%%% Installation ready                                      %"
	@echo "%%% You can uninstall blop by the command: blop --uninstall %"
	@echo "%%% (Possibly as root)                                      %"
	@echo "%%% Good Luck!!!                                            %"
	@echo "%%%                                                         %"
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@echo

install-nodoc:
	cd tools; $(MAKE) install
	cd latex; $(MAKE) install
	cd src; $(MAKE) install
	cd scripts; $(MAKE) install

#uninstall:
#	cd src; $(MAKE) uninstall
#	cd scripts; $(MAKE) uninstall
#	cd latex; $(MAKE) uninstall
#	@rm -rf $(DESTDIR)$(DOCDIR)/changelog


ifeq ($(MAKE_DEB),yes)
include deb.mk
endif

ifeq ($(MAKE_RPM),yes)
include rpm.mk
endif
