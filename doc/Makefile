# Do NOT call 'make' in this directory!
# Call it in the top-level directory!

.PHONY : all alldoc install clean distclean

all : alldoc

include ../config.mk

alldoc:
	cd tutorial; $(MAKE)
	cd documentation; $(MAKE)
	cd examples; $(MAKE)

install : all
	@echo
	@echo ===\> Creating documentation directory
	   if [ ! -d ${DESTDIR}${DOCDIR} ] ; then \
	      mkdir -p ${DESTDIR}${DOCDIR}; \
	   fi
	   echo ${DOCDIR} >> ${FILELIST};
	@echo
	@echo ===\> Copying files
	   cp -r    *.html *.css help-index *.png tip-of-the-day ${DESTDIR}${DOCDIR}/
	   for f in *.html *.css help-index *.png tip-of-the-day ; do\
	      echo ${DOCDIR}/$$f >> ${FILELIST}; \
	   done
	@echo
	@echo ===\> Installing subdirectories .....
	cd user-examples; $(MAKE) install
	cd documentation; $(MAKE) install
	cd tutorial;      $(MAKE) install
	cd examples;      $(MAKE) install

clean:
	rm -f *~
	cd tutorial; $(MAKE) clean
	cd documentation; $(MAKE) clean
	cd examples; $(MAKE) clean

distclean :
	rm -f *~
	cd tutorial; $(MAKE) distclean
	cd documentation; $(MAKE) distclean
	cd examples; $(MAKE) distclean

