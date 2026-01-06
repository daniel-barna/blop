.PHONY : rpm rpminstall check_configure_options

rpmdirs = RPM RPM/BUILD RPM/SOURCES RPM/SRPMS RPM/RPMS RPM/SPECS
$(rpmdirs) :
	mkdir $@

origtgz_rpm = $(shell pwd)/RPM/SOURCES/blop-plot-${BLOP_VERSION}.tar.gz
$(origtgz_rpm) : $(rpmdirs)
	@echo Generating original source tarball: $(origtgz_rpm)
	@tmp=`mktemp -d /tmp/blop-plot_rpm.XXXXXX`; \
	  cp -r . $${tmp}/blop-plot-${BLOP_VERSION}; \
	  cd $${tmp}/blop-plot-${BLOP_VERSION} ; \
	  make distclean; rm -rf debian RPM rpm.spec; \
	  cd ..; tar cvzf $@ blop-plot-${BLOP_VERSION}; \

build_requires := cint = $(CINT_VERSION)
requires := cint = $(CINT_VERSION)

gcc_pkg := 
ifeq ($(shell rpm -q --whatprovides `which g++ 2>/dev/null` >/dev/null 2>&1; echo $$?),0)
  gcc_pkg := $(strip $(gcc_pkg))
  gcc_pkg := $(shell rpm -q --whatprovides `which g++ 2>/dev/null`)
  gcc_pkg := $(shell echo $(gcc_pkg) | sed 's/-[^-]*-[^-]*$$//g') # strip off version numbers
  build_requires += , $(gcc_pkg)
endif

netpbm_pkg :=
ifeq ($(shell rpm -q --whatprovides `which pnmtopng 2>/dev/null` >/dev/null 2>&1; echo $$?),0)
  netpbm_pkg := $(strip $(netpbm_pkg))
  netpbm_pkg := $(shell rpm -q --whatprovides `which pnmtopng 2>/dev/null`)
  netpbm_pkg := $(shell echo $(netpbm_pkg) | sed 's/-[^-]*-[^-]*$$//g') # strip off version numbers
  requires += , $(netpbm_pkg)
  build_requires += , $(netpbm_pkg)
endif

awk_pkg :=
ifeq ($(shell rpm -q --whatprovides `which awk 2>/dev/null` >/dev/null 2>&1; echo $$?),0)
  awk_pkg := $(strip $(awk_pkg))
  awk_pkg := $(shell rpm -q --whatprovides `which awk 2>/dev/null`)
  awk_pkg := $(shell echo $(awk_pkg) | sed 's/-[^-]*-[^-]*$$//g') # strip off version numbers
  requires += , $(awk_pkg)
  build_requires += , $(awk_pkg)
endif

perl_pkg :=
ifeq ($(shell rpm -q --whatprovides `which perl 2>/dev/null` >/dev/null 2>&1; echo $$?),0)
  perl_pkg := $(strip $(perl_pkg))
  perl_pkg := $(shell rpm -q --whatprovides `which perl 2>/dev/null`)
  perl_pkg := $(shell echo $(perl_pkg) | sed 's/-[^-]*-[^-]*$$//g') # strip off version numbers
  requires += , $(perl_pkg)
  build_requires += , $(perl_pkg)
endif

latex_pkg :=
ifeq ($(shell rpm -q --whatprovides `which latex 2>/dev/null` >/dev/null 2>&1; echo $$?),0)
  latex_pkg := $(strip $(latex_pkg))
  latex_pkg := $(shell rpm -q --whatprovides `which latex 2>/dev/null`)
  latex_pkg := $(shell echo $(latex_pkg) | sed 's/-[^-]*-[^-]*$$//g') # strip off version numbers
  requires += , $(latex_pkg)
  build_requires += , $(latex_pkg)
endif

gv_pkg :=
ifeq ($(shell rpm -q --whatprovides `which gv 2>/dev/null` >/dev/null 2>&1; echo $$?),0)
  gv_pkg := $(strip $(gv_pkg))
  gv_pkg := $(shell rpm -q --whatprovides `which gv 2>/dev/null`)
  gv_pkg := $(shell echo $(gv_pkg) | sed 's/-[^-]*-[^-]*$$//g') # strip off version numbers
  requires += , $(gv_pkg)
  build_requires += , $(gv_pkg)
endif

gs_pkg :=
ifeq ($(shell rpm -q --whatprovides `which gs 2>/dev/null` >/dev/null 2>&1; echo $$?),0)
  gs_pkg := $(strip $(gs_pkg))
  gs_pkg := $(shell rpm -q --whatprovides `which gs 2>/dev/null`)
  gs_pkg := $(shell echo $(gs_pkg) | sed 's/-[^-]*-[^-]*$$//g') # strip off version numbers
  requires += , $(gs_pkg)
  build_requires += , $(gs_pkg)
endif

dvips_pkg :=
ifeq ($(shell rpm -q --whatprovides `which dvips 2>/dev/null` >/dev/null 2>&1; echo $$?),0)
  dvips_pkg := $(strip $(dvips_pkg))
  dvips_pkg := $(shell rpm -q --whatprovides `which dvips 2>/dev/null`)
  dvips_pkg := $(shell echo $(dvips_pkg) | sed 's/-[^-]*-[^-]*$$//g') # strip off version numbers
  requires += , $(dvips_pkg)
  build_requires += , $(dvips_pkg)
endif

imagemagick_pkg :=
ifeq ($(shell rpm -q --whatprovides `which convert 2>/dev/null` >/dev/null 2>&1; echo $$?),0)
  imagemagick_pkg := $(strip $(imagemagick_pkg))
  imagemagick_pkg := $(shell rpm -q --whatprovides `which convert 2>/dev/null`)
  imagemagick_pkg := $(shell echo $(imagemagick_pkg) | sed 's/-[^-]*-[^-]*$$//g') # strip off version numbers
  requires += , $(imagemagick_pkg)
  build_requires += , $(imagemagick_pkg)
endif

blopspec = RPM/SPECS/blop.spec
$(blopspec) : Makefile $(rpmdirs)
	@echo Creating $@
	@echo %define _topdir $(shell pwd)/RPM > $@
	@echo Summary: BLOP - a LaTeX oriented plotter with C++ interface >> $@
	@echo Url: http://blopplot.sourceforge.net >>$@
	@echo Name: blop-plot >> $@
	@echo Version: $(BLOP_VERSION) >> $@
	@echo Release: 1 >> $@
	@echo Requires: $(requires) >> $@
# test for native rpm-based systems: is gcc installed by rpm?
# Require the build_requires things only for such systems,
# on other systems building would fail due to missing
# deps, although they could maybe create the package
ifneq ($(gcc_pkg),)
	@echo BuildRequires: $(build_requires) >> $@
endif
	@echo Source0: %{name}-%{version}.tar.gz >> $@
	@echo License: GPL >> $@
	@echo Group: Productivity/Graphics/Visualization/Graph >> $@
	@echo BuildRoot: %{_builddir}/%{name}-root >> $@
	@echo %description >> $@
	@echo 'A LaTeX-oriented plotter with C++ interface. Designed to create plots, which can be nicely' >>$@
	@echo 'included in latex documents (labels, text, etc are typeset by latex within' >>$@
	@echo "the figure, inheriting the document's font. Figures can be scaled *correctly*" >>$@
	@echo "at the time of inclusion into the document; that is, scaling the figure only " >>$@
	@echo 'scales the graphics, but not the text/fonts). Linewidth and other properties of'  >>$@
	@echo 'the figure can be customized also during the inclusion into the latex document'  >>$@
	@echo %prep >> $@
	@echo %setup -q >> $@
	@echo %build >> $@
	@echo ./configure --prefix=/usr >> $@
	@echo make >> $@
	@echo make doc >> $@
	@echo %install >> $@
	@echo 'rm -rf $$RPM_BUILD_ROOT' >> $@
	@echo 'make DESTDIR=$$RPM_BUILD_ROOT install' >> $@
	@echo %clean >> $@
	@echo 'rm -rf $$RPM_BUILD_ROOT' >> $@
	@echo %post >>$@
	@echo ldconfig >>$@
	@echo texhash >>$@
	@echo %files >> $@
	@echo "%defattr(-,root,root)"  >>$@
	@echo '$(DATADIR)'  >>$@
	@echo '$(BINDIR)' >>$@
	@echo '$(LIBDIR)' >>$@
	@echo '$(INCDIR)' >>$@
	@echo '$(STYDIR)'  >>$@
	@echo '$(MANDIR)'  >>$@

build_arch = $(shell rpmbuild --showrc | awk '$$1=="build" && $$2=="arch" && $$3==":" { print $$4; exit; }')
RPM=RPM/RPMS/$(build_arch)/blop-plot-${BLOP_VERSION}-1.$(build_arch).rpm

rpm : check_configure_options $(RPM)
rpminstall : rpm
	rpm -q blop-plot > /dev/null 2>&1 && rpm -U $(RPM) || rpm -i $(RPM) 

check_configure_options :
ifneq ($(BLOP_CONFIG_OPTIONS),'--prefix=/usr')
	@echo configure was not called with --prefix=/usr option
	@echo Run configure --prefix=/usr, and then run make rpm again
	@false
endif


$(RPM) : $(origtgz_rpm) $(blopspec)
	rpmbuild  -ba RPM/SPECS/blop.spec
	@$(ECHO)
	@$(ECHO) Created package file: $@
	@$(ECHO)
