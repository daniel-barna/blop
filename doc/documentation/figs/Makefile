.PHONY : all allfigs clean distclean install

include ../../../config.mk

CFILES := $(wildcard *.C)
ifeq ($(BLOP_ENABLE_CINT),yes)
PNGS   := $(CFILES:.C=.png)
else
PNGS   := 
endif


all : $(PNGS)

%.png : %.pnm
	pnmcrop -white $^ | pnmtopng -transparent rgb:ff/ff/ff > $@

%.pnm : %.ps
	gs -dBATCH -dNOPAUSE -dTextAlphaBits=4 -r100 -sDEVICE=pnmraw -sOutputFile=$@ $^

%.ps : %.dvi
	dvips -o $@ -t a4 $^

%.dvi :  %.tex %.beps
	echo Q | latex $<

%.beps : %.C  ../../../src/blop.exe
	${BLOP_BUILD_DIR}/scripts/blop --run-during-build $<
#	../../../src/blop.exe ../../../src/init.C $<

clean : 
	rm -f *.aux *.log *.dvi *~ *.beps *.ps *.png

distclean : clean
	rm -f *.png

install :: all
	@echo
	@echo ===\> Installing subdirectory \'figs\'
	   if [ ! -d ${DESTDIR}${DOCDIR}/documentation/figs ] ; then \
	      mkdir ${DESTDIR}${DOCDIR}/documentation/figs; \
	   fi
	   echo            ${DOCDIR}/documentation/figs >> ${FILELIST};
	cp       $(PNGS) *.C ${DESTDIR}${DOCDIR}/documentation/figs/
	for f in $(PNGS) *.C ; do\
	   echo ${DOCDIR}/documentation/figs/$$f >> ${FILELIST}; \
	done
