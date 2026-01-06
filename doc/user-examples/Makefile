include ../../config.mk

all :
	echo No need to compile anything here

install:
	if [ ! -d ${DESTDIR}${DOCDIR}/user-examples ] ; then \
	  mkdir ${DESTDIR}${DOCDIR}/user-examples ; \
	fi
	echo ${DOCDIR}/user-examples >> ${FILELIST};
	for f in *.h *.C *.html *.png ; do \
	  cp $$f ${DESTDIR}${DOCDIR}/user-examples/ ; \
	  echo ${DOCDIR}/user-examples/$$f >> ${FILELIST}; \
	done
