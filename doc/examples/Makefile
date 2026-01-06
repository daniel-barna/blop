include ../../config.mk

SCRIPTS := $(wildcard example*.C)
TEXS    := $(wildcard example*.tex)
ifeq ($(BLOP_ENABLE_CINT),yes)
  FIGS    := $(SCRIPTS:.C=.beps)
  PSS     := $(TEXS:.tex=.ps)
  TESTFILES := ../../test.dir/*.C ../../test.dir/*.tex ../../test.dir/data ../../test.dir/*.ps
else
  FIGS    := 
  PSS     := 
  TESTFILES := ../../test.dir/*.C ../../test.dir/*.tex ../../test.dir/data
endif

all : $(PSS) test


test ::
	cd ../..; $(MAKE) test
	if test ! -d test ; then mkdir test; fi
	cp $(TESTFILES) test/

%.ps : %.beps %.dvi
	dvips -o $@ -t a4 $*.dvi

%.dvi : %.tex
	latex $^

%.beps : %.C data ../../src/blop.exe
	../../src/blop.exe ../../src/init.C $*.C


install : all
	@echo
	@echo ===\> Installing subdirectory \'examples\'
	   if [ ! -d ${DESTDIR}${DOCDIR}/examples ] ; then \
	      mkdir ${DESTDIR}${DOCDIR}/examples; \
	   fi
	   echo            ${DOCDIR}/examples >> ${FILELIST}; 
	cp -r test index.html $(SCRIPTS) $(TEXS) $(PSS) ${DESTDIR}${DOCDIR}/examples/
	for f in test test/* index.html $(SCRIPTS) $(TEXS) $(PSS); do \
	   echo ${DOCDIR}/examples/$$f >> ${FILELIST}; \
	done

clean:
	rm -f $(FIGS) *.aux *.log *.dvi *~

distclean : clean
	rm -rf $(PSS) test
