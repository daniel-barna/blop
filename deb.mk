.PHONY : deb debinstall

BLOP_PKG=blop-plot

blop_signature_file=~/.blop-signature
blop_signature=`cat $(blop_signature_file)`
$(blop_signature_file) :
	@$(ECHO) 'We need your name and email address, to store them'; \
	  $(ECHO) 'in the created package. The package also needs to be'; \
	  $(ECHO) 'signed, so make sure that you have a gpg key corresponding'; \
	  $(ECHO) 'to the name/email, that you will specify below'; \
	  $(ECHO) '(gpg --gen-key is the command which generates a key)'; \
	  $(ECHO) ; \
	  $(ECHO) -n 'What is your name: '; \
	  read name; \
	  $(ECHO) -n $$name > $@; \
	  $(ECHO) -n 'What is your email address: '; \
	  read email; \
	  $(ECHO) " <$$email>" >> $@; \
	  $(ECHO) ; \
	  $(ECHO) "Great! The file '$@' will store this info. Edit this file" ;\
	  $(ECHO) "If you want to make any changes" ; \
	  $(ECHO) ; \
	  sleep 5s

arch = $(shell dpkg-architecture -qDEB_HOST_ARCH)

pkgfilename=$(BLOP_PKG)_$(BLOP_VERSION)-1_$(arch).deb 
pkg  = debian/package-files/$(pkgfilename)

CONFIGARGS = --host=$$(DEB_HOST_GNU_TYPE) --build=$$(DEB_BUILD_GNU_TYPE) --prefix=/usr
CONFIGCMD = ./configure $(CONFIGARGS)
#--mandir=\$$$${prefix}/share/man --infodir=\$$$${prefix}/share/info 

# make deb          -- creates the .deb package in the parent directory
deb : $(pkg)

# make debinstall   -- installs the created .deb package file to your system
debinstall : $(pkg)
	dpkg -i $(pkg)

origtgz_deb = $(shell pwd | sed 's|/[^/]*$$||g')/$(BLOP_PKG)_${BLOP_VERSION}.orig.tar.gz

# This is the default value for the -i option of dpkg-source, + some other extensions
# (.png, .o, .exe, .a, 

ignore = '(?:^|/).*~$$|(?:^|/)\.\#.*$$|(?:^|/)\..*\.swp$$|(?:^|/),,.*(?:$$|/.*$$)|(?:^|/)(?:DEADJOE|\.cvsignore|\.arch-inventory|\.bzrignore|\.gitignore)$$|(?:^|/)(?:CVS|RCS|\.deps|\{arch\}|\.arch-ids|\.svn|\.hg|_darcs|\.git|\.shelf|\.bzr(?:\.backup|tags)?)(?:$$|/.*$$)|.*\.png$$|.*\.o$$|.*\.exe$$|.*\.a$$|/config\..*$$|.*\.dvi$$|.*\.gz$$|.*\.deb$$|.*\.rpm$$|.*build-stamp$$|.*\.ps$$'

$(pkg) : $(origtgz_deb) debian/control debian/copyright debian/dirs debian/docs debian/changelog debian/rules debian/compat debian/postinst
	rm -f config.status
	@wd=`pwd | sed 's|^.*/||g'`; \
	if [ "$$wd" != $(BLOP_PKG)-${BLOP_VERSION} ] ; then \
	  cd ..; \
	  if [ -e $(BLOP_PKG)-${BLOP_VERSION} ] ; then \
	    if [ -h $(BLOP_PKG)-${BLOP_VERSION} ] ; then \
	      rm -f $(BLOP_PKG)-${BLOP_VERSION}; \
	    else \
	      echo ; \
	      echo '$(BLOP_PKG)-${BLOP_VERSION} already exists. I need this to build the' ;\
	      echo 'deb package. (This would be a symlink to the current directory)' ;\
	      echo 'Should I remove it? [y/n]'; \
	      read answer; \
	      if [ "$$answer" != "y" ] ; then exit 1; fi; \
	      rm -f $(BLOP_PKG)-${BLOP_VERSION}; \
	    fi; \
	  fi; \
	  ln -s $$wd $(BLOP_PKG)-${BLOP_VERSION}; \
	fi; cd $(BLOP_PKG)-${BLOP_VERSION}; dpkg-buildpackage -rfakeroot -W -i$(ignore)
	@if [ ! -d debian/package-files ] ; then mkdir debian/package-files; fi
	@mv ../$(pkgfilename) $@
	@mv ../$(BLOP_PKG)_$(BLOP_VERSION)-1_$(arch).changes debian/package-files/
	@mv ../$(BLOP_PKG)_$(BLOP_VERSION)-1.dsc             debian/package-files/
	@mv ../$(BLOP_PKG)_$(BLOP_VERSION)-1.diff.gz         debian/package-files/
	@$(ECHO)
	@$(ECHO) Created package file: $(pkg)
	@$(ECHO)


# Create the original source tarball: copy the current directory 
# into a temporary location, then cd to this directory, issue
# 'make distclean' + remove 'debian' and 'RPMS' subdirectories
# then make the tarball of this directory.
$(origtgz_deb) : 
	@$(ECHO) Generating original source tarball: $(origtgz_deb)
	@ pardir=`pwd | sed 's|/[^/]*$$||g'` ; \
	  tmp=`mktemp -d /tmp/blop_deb.XXXXXX`; \
	  cp -r . $${tmp}/$(BLOP_PKG)-${BLOP_VERSION}; \
	  cd $${tmp}/$(BLOP_PKG)-${BLOP_VERSION} ; \
	  make distclean; rm -rf debian RPM; \
	  cd ..; tar cvzf $@ $(BLOP_PKG)-${BLOP_VERSION}; \

debian/compat : deb.mk
	@if [ ! -d debian ] ; then mkdir debian; fi
	@$(ECHO) Creating $@
	@$(ECHO) 5 > $@

# try to cleverly collect the packages needed for the build, and for the usage
build_needed_packages=$(shell unalias -a; dpkg -S `which ps2eps` `which g++` `which gts-config` `which gsl-config` libtermcap.so libtermcap.a keyval.sty pstricks.sty | awk -F: '{a[$$1]=1} END{c=0; for(i in a) { if(c!=0) print ", "; print i; ++c; }}') 
build_needed_packages += , fakeroot , imagemagick , netpbm
cint_package = cint (=$(shell dpkg -p cint | awk '$$1=="Version:" {print $$2}'))
build_needed_packages += , $(cint_package)

needed_packages=$(shell unalias -a; dpkg -S `which gv` `which g++` libgsl.so libgsl.a libgts.so libgts.a libtermcap.so libtermcap.a keyval.sty pstricks.sty |  awk -F: '{a[$$1]=1} END{c=0; for(i in a) { if(c!=0) print ", "; printf "%s",i; ++c; }}')
needed_packages += , $(cint_package) , imagemagick , netpbm

debian/control : $(blop_signature_file) deb.mk
	@if [ ! -d debian ] ; then mkdir debian; fi
	@$(ECHO) Creating $@
	@$(ECHO) 'Source: $(BLOP_PKG)' > $@
	@$(ECHO) 'Section: plot' >> $@
	@$(ECHO) 'Priority: optional' >> $@
	@$(ECHO) "Maintainer: $(blop_signature)" >> $@
	@$(ECHO) 'Build-Depends: debhelper (>= 5), $(build_needed_packages)' >> $@
	@$(ECHO) 'Standards-Version: 3.7.2' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'Package: $(BLOP_PKG)' >> $@
	@$(ECHO) 'Version: $(BLOP_VERSION)' >> $@
	@$(ECHO) 'Architecture: any' >> $@
	@$(ECHO) 'Depends: $${shlibs:Depends}, $(needed_packages)' >> $@
	@$(ECHO) 'Suggests: wmctrl' >> $@
	@$(ECHO) 'Description: Plotting utility in best harmony with LaTeX' >> $@

debian/copyright : $(blop_signature_file) deb.mk
	@if [ ! -d debian ] ; then mkdir debian; fi
	@$(ECHO) Creating $@
	@$(ECHO) "This package was created by $(blop_signature) on" > $@
	@$(ECHO) "`date -R`" >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'It was downloaded from: http://blopplot.sourceforge.net/blop.tar.gz' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'Upstream Author: Daniel Barna <barnad@users.sourceforge.net>' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'Copyright: Daniel Barna' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'License:' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'The Debian packaging is (C) 2006, $(blop_signature) and' >> $@
	@$(ECHO) "is licensed under the GPL, see '/usr/share/common-licenses/GPL'." >> $@

debian/dirs : deb.mk
	@if [ ! -d debian ] ; then mkdir debian; fi
	@$(ECHO) Creating $@
	@$(ECHO) usr/bin > $@
	@$(ECHO) usr/include >> $@
	@$(ECHO) usr/lib >> $@
	@$(ECHO) usr/share/man/man1 >> $@
	@$(ECHO) $(STYDIR) | sed 's|^/||g' >> $@
	@$(ECHO) usr/share/blop/doc >> $@

debian/docs : deb.mk
	@if [ ! -d debian ] ; then mkdir debian; fi
	@$(ECHO) Creating $@
	@$(ECHO) README > $@
	@$(ECHO) TODO   >> $@

debian/changelog : $(blop_signature_file) changelog deb.mk
	@if [ ! -d debian ] ; then mkdir debian; fi
	@$(ECHO) Creating $@
	@$(ECHO) '$(BLOP_PKG) (${BLOP_VERSION}-1) unstable; urgency=low' > $@
	@$(ECHO) '' >> $@
	@$(ECHO) '  * New upstream release' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) " -- $(blop_signature)  `date -R`" >> $@

debian/postinst : deb.mk
	@if [ ! -d debian ] ; then mkdir debian; fi
	@$(ECHO) Creating $@
	@$(ECHO) '#!/bin/sh' > $@
	@$(ECHO) '# postinst script for $(BLOP_PKG)' >> $@
	@$(ECHO) '# see dh_installdeb(1)' >> $@
	@$(ECHO) 'set -e' >> $@
	@$(ECHO) 'case "$$1" in' >> $@
	@$(ECHO) '  configure)' >> $@
	@$(ECHO) '    texhash ;;' >> $@
	@$(ECHO) '  abort-upgrade|abort-remove|abort-deconfigure)' >> $@
	@$(ECHO) '    ;;' >> $@
	@$(ECHO) '  *)' >> $@
	@$(ECHO) '    $(ECHO) "postinst called with unknown argument: $$1" >&2' >> $@
	@$(ECHO) '    exit 1' >> $@
	@$(ECHO) '    ;;' >> $@
	@$(ECHO) 'esac' >> $@
	@$(ECHO) 'exit 0' >> $@
	@chmod +x $@

debian/rules : deb.mk
	@if [ ! -d debian ] ; then mkdir debian; fi
	@$(ECHO) Creating $@
	@$(ECHO) '#!/usr/bin/make -f' > $@
	@$(ECHO) '# -*- makefile -*-' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) '# Uncomment this to turn on verbose mode.' >> $@
	@$(ECHO) '#export DH_VERBOSE=1' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) '# These are used for cross-compiling and for saving the configure script' >> $@
	@$(ECHO) '# from having to guess our platform (since we know it already)' >> $@
	@$(ECHO) 'DEB_HOST_GNU_TYPE   ?= $$(shell dpkg-architecture -qDEB_HOST_GNU_TYPE)' >> $@
	@$(ECHO) 'DEB_BUILD_GNU_TYPE  ?= $$(shell dpkg-architecture -qDEB_BUILD_GNU_TYPE)' >> $@
	@$(ECHO) '#CFLAGS = -Wall' >> $@
	@$(ECHO) 'ifneq (,$$(findstring noopt,$$(DEB_BUILD_OPTIONS)))' >> $@
	@$(ECHO) '        CFLAGS += -O0' >> $@
	@$(ECHO) 'else' >> $@
	@$(ECHO) '        CFLAGS += -O2' >> $@
	@$(ECHO) 'endif' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'config.status: configure' >> $@
#	@$(ECHO) -e '\tdh_testdir' >> $@
	@$(ECHO) -e '\t$(CONFIGCMD)' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'build: build-stamp' >> $@
	@$(ECHO) 'build-stamp:  config.status' >> $@
#	@$(ECHO) -e '\tdh_testdir' >> $@
	@$(ECHO) -e '\t$$(MAKE)' >> $@
	@$(ECHO) -e '\t$$(MAKE) doc' >> $@
	@$(ECHO) -e '\ttouch $$@' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'clean:' >> $@
#	@$(ECHO) -e '\tdh_testdir' >> $@
#	@$(ECHO) -e '\tdh_testroot' >> $@
	@$(ECHO) -e '\trm -f build-stamp' >> $@
#	@$(ECHO) -e '\t-$$(MAKE) distclean' >> $@
	@$(ECHO) 'ifneq "$$(wildcard /usr/share/misc/config.sub)" ""' >> $@
	@$(ECHO) -e '\tcp -f /usr/share/misc/config.sub config.sub' >> $@
	@$(ECHO) 'endif' >> $@
	@$(ECHO) 'ifneq "$$(wildcard /usr/share/misc/config.guess)" ""' >> $@
	@$(ECHO) -e '\tcp -f /usr/share/misc/config.guess config.guess' >> $@
	@$(ECHO) 'endif' >> $@
	@$(ECHO) -e '\tdh_clean' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'install: build' >> $@
	@$(ECHO) -e '\tfor d in `cat debian/dirs`; do mkdir -p debian/$(BLOP_PKG)/$$d; done' >> $@
	@$(ECHO) -e '\t$$(MAKE) install DEB_PKG_BUILDING=1 DESTDIR=$$(CURDIR)/debian/$(BLOP_PKG)' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'binary-indep: build install' >> $@
	@$(ECHO) 'binary-arch: build install' >> $@
	@$(ECHO) -e '\tdh_installchangelogs' >> $@
	@$(ECHO) -e '\tdh_installdocs' >> $@
#	@$(ECHO) -e '\tdh_installman' >> $@
	@$(ECHO) -e '\tdh_link' >> $@
	@$(ECHO) -e '\tdh_strip' >> $@
	@$(ECHO) -e '\tdh_compress' >> $@
	@$(ECHO) -e '\tdh_fixperms' >> $@
	@$(ECHO) '#       dh_perl' >> $@
	@$(ECHO) '#       dh_makeshlibs' >> $@
	@$(ECHO) -e '\tdh_installdeb' >> $@
	@$(ECHO) -e '\tdh_shlibdeps' >> $@
	@$(ECHO) -e '\tdh_gencontrol' >> $@
	@$(ECHO) -e '\tdh_md5sums' >> $@
	@$(ECHO) -e '\tdh_builddeb' >> $@
	@$(ECHO) '' >> $@
	@$(ECHO) 'binary: binary-indep binary-arch' >> $@
	@$(ECHO) '.PHONY: build clean binary-indep binary-arch binary install' >> $@
	@chmod +x $@

