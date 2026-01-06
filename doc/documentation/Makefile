.PHONY : all clean install distclean

include ../../config.mk

HEADERS := $(wildcard ../../src/*.h)
SOURCES := $(wildcard ../../src/*.cc)
SRCHTMLS := $(wildcard *.srchtml)
HTMLS    := $(SRCHTMLS:.srchtml=.html)

TMP := $(subst ../../src/,,$(HEADERS))
TMP := $(subst G__cpp_blop.h,,$(TMP))
SOURCEDOCS := $(TMP:.h=.h.html)

TMP := $(subst ../../src/,,$(SOURCES))
SOURCEDOCS += $(TMP:.cc=.cc.html)



alldoc : source.html $(HTMLS) 
	cd figs; $(MAKE) -r

%.html : %.srchtml ../srchtml2html.pl
	perl ../srchtml2html.pl $< .. > $@

source.html : $(SOURCEDOCS)
	@echo '<html><head><link href="../default.css" type="text/css" rel="stylesheet"></head><body>' > $@
	@echo 'Here are the source files, HTML-ized, and somewhat unorganized:' >> $@
	@echo '<ul>' >> $@
	@for f in $(SOURCEDOCS); do echo '<li><a href="'$$f'">'$$f'</a>' >> $@; done 
	@echo '</ul>' >> $@
	@echo '</body></html>' >> $@

%.html : ../../src/% c2html
	perl ./c2html $< > $@

distclean : clean
	rm -f *.html *~
	cd figs; $(MAKE) -r distclean

clean :
	rm -f *~ 
	cd figs; $(MAKE) -r clean

install : all
	@echo
	@echo ===\> Installing subdirectory \'documentation\'
	   if [ ! -d ${DESTDIR}${DOCDIR}/documentation ] ; then \
	      mkdir ${DESTDIR}${DOCDIR}/documentation; \
	   fi
	   @echo            ${DOCDIR}/documentation >> ${FILELIST}; 
	cp       *.html ${DESTDIR}${DOCDIR}/documentation/
	for f in *.html; do \
	   echo ${DOCDIR}/documentation/$$f >> ${FILELIST}; \
	done
	cd figs; $(MAKE) -r install
