.PHONY : install clean distclean

include ../config.mk

blop.1.gz : blop.man
	groff -Tascii -man $^ | gzip -c >  $@

manfile=${MANDIR}/man1/blop.1.gz

install : blop.1.gz
	@if [ ! -d $(DESTDIR)$(MANDIR)/man1 ] ; then mkdir -p $(DESTDIR)$(MANDIR)/man1; fi
	cp $^ $(DESTDIR)$(manfile)

clean :
	rm -f *~

distclean :
	rm -f *~ blop.1.gz
