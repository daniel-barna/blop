# Do NOT call this Makefile from this directory.
# call 'make' in the top-level directory!!!

.PHONY : all install uninstall distclean clean

all : 

include ../config.mk

install: ${DESTDIR}${STYDIR}/blopeps.sty

uninstall:
	rm -f ${DESTDIR}${STYDIR}/blopeps.sty

${DESTDIR}${STYDIR}/blopeps.sty : blopeps.sty
	if [ ! -d ${DESTDIR}${STYDIR} ] ; then \
	  mkdir -p ${DESTDIR}${STYDIR}; \
	fi
	@echo ${STYDIR} >> ${FILELIST}; 
	cp $^ $@
	@echo ${STYDIR}/blopeps.sty >> ${FILELIST}
	if ! echo ${DESTDIR}${STYDIR} | grep `pwd` >/dev/null 2>&1; then texhash; fi

distclean : clean

clean:
	rm -f *~
