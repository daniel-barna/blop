include ../../config.mk

all:
	@echo

distclean : clean

clean:
	rm -f *~

install:
	@echo
	@echo ===\> Installing subdirectory \'tutorial\'
	   if [ ! -d ${DESTDIR}${DOCDIR}/tutorial ] ; then \
	      mkdir ${DESTDIR}${DOCDIR}/tutorial; \
	   fi
	   @echo  ${DOCDIR}/tutorial >> ${FILELIST};
	cp *.html ${DESTDIR}${DOCDIR}/tutorial/
	for f in *.html; do \
	   echo ${DOCDIR}/tutorial/$$f >> ${FILELIST}; \
	done
