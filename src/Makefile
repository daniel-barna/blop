# Do NOT run this Makefile from this directory. Run the
# top-level Makefile!

.PHONY : compile clean distclean install uninstall
all : compile
include ../config.mk

SOURCES := $(wildcard *.cc)
OBJECTS := $(SOURCES:.cc=.o)
ifneq ($(BLOP_ENABLE_CINT),yes)
  OBJECTS := $(subst main.o,,$(OBJECTS))
  OBJECTS := $(subst blop_cint.o,,$(OBJECTS))
endif
OBJECTS_NOMAIN := $(subst main.o,,$(OBJECTS))
OBJECTS_NOMAIN := $(subst blop_cint.o,,$(OBJECTS_NOMAIN))
OBJECTS_NOMAIN := $(subst blop_nocint.o,,$(OBJECTS_NOMAIN))
HEADERS := $(wildcard *.h *.hh)
HEADERS := $(subst G__cpp_blop.h,,$(HEADERS))	
HEADERS_CINT := $(wildcard *.h)
HEADERS_CINT := $(subst G__cpp_blop.h,,$(HEADERS_CINT))	

LIBBLOP      := libblop.a    # the library
ifeq ($(BLOP_ENABLE_CINT),yes)
  LIBBLOP_CINT := libblop-cint.a
  LIBBLOP_MAIN := libblopmain.a
  EXE          := blop.exe     # the compiled executable: the interpreter
endif
INIS     := init.C init2.C call_user_init.C

compile : $(LIBBLOP) $(EXE) 
$(EXE)  : $(LIBBLOP) $(LIBBLOP_MAIN) $(LIBBLOP_CINT) Makefile.cint
	@$(ECHO)
	@$(ECHO) ... Compiling the interpreter, this might take a while
	@$(ECHO)
	$(MAKE) -f Makefile.cint

lib : $(LIBBLOP) $(LIBBLOP_CINT)
$(LIBBLOP)      : $(OBJECTS_NOMAIN) blop_nocint.o
	$(AR) $@ $^

$(LIBBLOP_CINT) : $(OBJECTS_NOMAIN) blop_cint.o
	$(AR) $@ $^

$(LIBBLOP_MAIN) : main.o
	$(AR) $@ $^

makecint := `cint-config --bindir`/makecint
#makecint := $(shell if which makecint.cint >/dev/null 2>&1; then echo makecint.cint; else echo makecint; fi) 

ifeq ($(HAVE_GTS_H),1)
  MAKECINT_CC_OPTS += `gts-config --cflags`
endif

mf : Makefile.cint
Makefile.cint :
ifeq ($(USE_CINTSYSDIR),1)
	$(makecint)  -m -mk Makefile.cint.tmp -o $(EXE) -H $(HEADERS_CINT) -l $(LIBBLOP_MAIN) -l $(LIBBLOP_CINT) ${X_LIBS} ${LIBS} -lreadline -l$(NCURSES_OR_TERMCAP) -I$(CINTSYSDIR) -I$(CINTSYSDIR)/src -cc $(MAKECINT_CC_OPTS)
	${SED} -e 's/^[ ]*CINTSYSDIRW[ ]*:?=.*$$/CINTSYSDIRW=\$$(CINTSYSDIR)/g' -e 's/^[ ]*CINTSYSDIRU[ ]*:?=.*$$/CINTSYSDIRU=\$$(CINTSYSDIR)/g' Makefile.cint.tmp | awk '{if($$1=="CXXFLAGS") printf "%s %s\n",$$0,"${CXXSTD}"; else print;}' > $@
	rm -f Makefile.cint.tmp
else
	$(makecint)  -m -mk Makefile.cint.tmp -o $(EXE) -H $(HEADERS_CINT) -l $(LIBBLOP_MAIN) -l $(LIBBLOP_CINT) ${X_LIBS} ${LIBS} -lreadline -l$(NCURSES_OR_TERMCAP)  -cc $(MAKECINT_CC_OPTS)
	${SED} -e 's/\$$\(shell which cint\)/$$(shell if which cint.cint >\/dev\/null 2>\&1; then echo `which cint.cint`; else echo `which cint`; fi)/g' Makefile.cint.tmp | awk '{if($$1=="CXXFLAGS") printf "%s %s\n",$$0,"${CXXSTD}"; else print;}'  > $@
	rm -f Makefile.cint.tmp
endif
	@$(ECHO) $(EXE) : $(LIBBLOP) $(LIBBLOP_CINT) $(LIBBLOP_MAIN) >> $@
	@$(ECHO) include $(OBJECTS:.o=.d) >> $@
	@$(ECHO) %.d : %.C >> $@
	$(ECHO) '	'$(MAKEDEPEND) '$$^ > $$@' >> $@

distclean : clean
	$(MAKE) clean
	rm -f config.h

clean :
	rm -f G__* *.o *~ *.d Makefile.cint *.a $(EXE)

ifneq ($(INCLUDEDEPS),false)
include $(OBJECTS:.o=.d)
endif

%.o : %.cc
	$(CXX) $(CXXFLAGS) -O -c -o $@ $<
%.d : %.cc
	$(MAKEDEPEND) $^ > $@

install: compile $(INIS)
	@$(ECHO)
	@$(ECHO) ===\> Copy initfiles to ${DATADIR}
	   if [ ! -d ${DESTDIR}${DATADIR} ] ; then \
	     mkdir -p ${DESTDIR}${DATADIR}; \
	   fi
	   @echo ${DATADIR} >> ${FILELIST};
	   cp ${INIS} ${DESTDIR}${DATADIR}/
	   for f in ${INIS} ; do echo ${DATADIR}/$$f >> ${FILELIST}; done
ifeq ($(BLOP_ENABLE_CINT),yes)
	@$(ECHO) ===\> Copy executables to ${BINDIR}
	   if [ ! -d ${DESTDIR}${BINDIR} ] ; then \
	     mkdir -p ${DESTDIR}${BINDIR}; \
	   fi
	   cp ${EXE} ${DESTDIR}${BINDIR}/
	   echo ${BINDIR}/${EXE} >> ${FILELIST}
endif
	@$(ECHO) ===\> Copy libraries to ${LIBDIR}
	   @if [ ! -d ${DESTDIR}${LIBDIR} ] ; then \
	      mkdir -p ${DESTDIR}${LIBDIR}; \
	   fi
	   cp ${LIBBLOP} ${DESTDIR}${LIBDIR}/
	   echo ${LIBDIR}/${LIBBLOP} >> ${FILELIST}
ifeq ($(BLOP_ENABLE_CINT),yes)
	   cp ${LIBBLOP_CINT} ${DESTDIR}${LIBDIR}/
	   echo ${LIBDIR}/${LIBBLOP_CINT} >> ${FILELIST}
	   cp G__setup.o    ${DESTDIR}${LIBDIR}/blop_G__setup.o
	   cp G__cpp_blop.o ${DESTDIR}${LIBDIR}/blop_G__cpp_blop.o
	   echo ${LIBDIR}/blop_G__setup.o >> ${FILELIST}
	   echo ${LIBDIR}/blop_G__cpp_blop.o >> ${FILELIST}
endif
	@$(ECHO) ===\> Copy header files to ${INCDIR}
	   if [ ! -d ${DESTDIR}${INCDIR} ] ; then \
	      mkdir -p ${DESTDIR}${INCDIR}; \
	   fi
	   if [ ! -d ${DESTDIR}${INCDIR}/blop-plot ] ; then \
	      mkdir ${DESTDIR}${INCDIR}/blop-plot; \
	   fi
	   @echo            ${INCDIR}/blop-plot >> ${FILELIST};
	   cp ${HEADERS} ${DESTDIR}${INCDIR}/blop-plot/
	   for f in ${HEADERS}; do echo ${INCDIR}/blop-plot/$$f >> ${FILELIST}; done
	   $(MAKE) ${DESTDIR}${INCDIR}/blop.h
	   echo ${INCDIR}/blop.h >> ${FILELIST}
	@$(ECHO)
	@$(ECHO) ..... DONE ....
	@$(ECHO)

${DESTDIR}${INCDIR}/blop.h : Makefile
	   @$(ECHO) '#ifndef __BLOP_H__' >  $@
	   @$(ECHO) '#define __BLOP_H__' >> $@
	   @$(ECHO) '#include "blop-plot/config.h"' >> $@
	   @$(ECHO) '#include "blop-plot/array.h"' >> $@
	   @$(ECHO) '#include "blop-plot/multiloop.h"' >> $@
	   @$(ECHO) '#include "blop-plot/length.h"' >> $@
	   @$(ECHO) '#include "blop-plot/box.h"' >> $@
	   @$(ECHO) '#include "blop-plot/graph_drawer.h"' >> $@
	   @$(ECHO) '#include "blop-plot/arc.h"' >> $@
	   @$(ECHO) '#include "blop-plot/point_drawer.h"' >> $@
	   @$(ECHO) '#include "blop-plot/bloputils.h"' >> $@
	   @$(ECHO) '#include "blop-plot/color.h"' >> $@
	   @$(ECHO) '#include "blop-plot/dgraph.h"' >> $@
	   @$(ECHO) '#include "blop-plot/fgraph.h"' >> $@
	   @$(ECHO) '#include "blop-plot/hist.h"' >> $@
	   @$(ECHO) '#include "blop-plot/mcontainer.h"' >> $@
	   @$(ECHO) '#include "blop-plot/mpad.h"' >> $@
	   @$(ECHO) '#include "blop-plot/epad.h"' >> $@
	   @$(ECHO) '#include "blop-plot/mframe.h"' >> $@
	   @$(ECHO) '#include "blop-plot/function.h"' >> $@
	   @$(ECHO) '#include "blop-plot/plot.h"' >> $@
	   @$(ECHO) '#include "blop-plot/fit.h"' >> $@
	   @$(ECHO) '#include "blop-plot/pstream.h"' >> $@
	   @$(ECHO) '#include "blop-plot/set.h"' >> $@
	   @$(ECHO) '#include "blop-plot/get.h"' >> $@
	   @$(ECHO) '#include "blop-plot/warning.h"' >> $@
	   @$(ECHO) '#include "blop-plot/blopeps.h"' >> $@
	   @$(ECHO) '#include "blop-plot/mpps.h"' >> $@
	   @$(ECHO) '#include "blop-plot/mppdf.h"' >> $@
	   @$(ECHO) '#include "blop-plot/pdf.h"' >> $@
	   @$(ECHO) '#include "blop-plot/svg.h"' >> $@
	   @$(ECHO) '#include "blop-plot/eps.h"' >> $@
	   @$(ECHO) '#include "blop-plot/png.h"' >> $@
	   @$(ECHO) '#include "blop-plot/jpg.h"' >> $@
	   @$(ECHO) '#include "blop-plot/bmp.h"' >> $@
	   @$(ECHO) '#include "blop-plot/x11_ps.h"' >> $@
	   @$(ECHO) '#include "blop-plot/ignore.h"' >> $@
	   @$(ECHO) '#include "blop-plot/line.h"' >> $@
	   @$(ECHO) '#include "blop-plot/point.h"' >> $@
	   @$(ECHO) '#include "blop-plot/printer.h"' >> $@
	   @$(ECHO) '#include "blop-plot/fft.h"' >> $@
	   @$(ECHO) '#include "blop-plot/interpolate.hh"' >> $@
	   @$(ECHO) '#include "blop-plot/arrowhead.h"' >> $@
	   @$(ECHO) '#include "blop-plot/sym.h"' >> $@
	   @$(ECHO) '#include "blop-plot/color_legend.h"' >> $@
	   @$(ECHO) '#include "blop-plot/units.h"' >> $@
	   @$(ECHO) '#include "blop-plot/video.h"' >> $@
	   @$(ECHO) '#include "blop-plot/blop_gts.h"' >> $@
	   @$(ECHO) '#include "blop-plot/vec3.h"' >> $@
	   @$(ECHO) '#include "blop-plot/geometry.h"' >> $@
	   @$(ECHO) '#include "blop-plot/physics.h"' >> $@
	   @$(ECHO) '#include "blop-plot/text.h"' >> $@
	   @$(ECHO) '#include "blop-plot/multipage_terminal.h"' >> $@
	   @$(ECHO) '#include "blop-plot/versatile_pdf.h"' >> $@
	   @$(ECHO) '#include "blop-plot/matrix.h"' >> $@
	   @$(ECHO) '#include "blop-plot/kinematics.h"' >> $@
	   @$(ECHO) '#include "blop-plot/discretizer.h"' >> $@
	   @$(ECHO) '#ifdef USE_NAMESPACE_BLOP' >> $@
	   @$(ECHO) 'using namespace blop;'     >> $@
	   @$(ECHO) '#endif' >> $@
	   @$(ECHO) '#ifdef BLOP_USE_CINT' >> $@
	   @$(ECHO) '#ifdef CINT_INCLUDE_PREFIX' >> $@
	   @$(ECHO) '#include "cint/G__ci.h"' >> $@
	   @$(ECHO) '#else' >> $@
	   @$(ECHO) '#include "G__ci.h"' >> $@
	   @$(ECHO) '#endif' >> $@
	   @$(ECHO) '#endif' >> $@
	   @$(ECHO) '#endif' >> $@

uninstall::
	rm -rf ${DESTDIR}${DATADIR}
	rm -rf ${DESTDIR}${BINDIR}/${EXE}
	rm -f ${DESTDIR}${LIBDIR}/${LIBBLOP} ${DESTDIR}${LIBDIR}/${LIBBLOP_CINT} \
	      ${DESTDIR}${LIBDIR}/blop_G__setup.o ${DESTDIR}${LIBDIR}/blop_G__cpp_blop.o
	rm -rf ${DESTDIR}${INCDIR}/blop-plot ${DESTDIR}${INCDIR}/blop.h

