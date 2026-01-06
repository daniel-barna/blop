scripts := blop-helpviewer beps2eps beps2pdf beps2png beps2jpg beps2bmp blop blop-config bepsview beps-annotate ps2eps-blop blop-terminal

.PHONY : make_executable all

all : blop-config-vars make_executable

make_executable :
	chmod +x $(scripts)

include ../config.mk


blop-config-vars : Makefile ../config.mk
	@$(ECHO) Creating $@
	@rm -f $@
	@$(ECHO) export BLOP_ENABLE_CINT=${BLOP_ENABLE_CINT} > $@
	@$(ECHO) export BLOP_VERSION=${BLOP_VERSION} >> $@
	@$(ECHO) export CINT_VERSION=${CINT_VERSION} >> $@
	@$(ECHO) export BLOP_ARCH=${BLOP_ARCH} >> $@
	@$(ECHO) export BLOP_DATADIR=${DATADIR} >> $@
	@$(ECHO) export BLOP_DOCDIR=${DOCDIR} >> $@
	@$(ECHO) export BLOP_LIBDIR=${LIBDIR} >> $@
	@$(ECHO) export BLOP_BINDIR=${BINDIR} >> $@
	@$(ECHO) export BLOP_INCDIR=${INCDIR} >> $@
	@$(ECHO) export BLOP_STYDIR=${STYDIR} >> $@
ifeq ($(USE_CINTSYSDIR),1)
	@$(ECHO) export BLOP_CINTSYSDIR=${CINTSYSDIR} >> $@
endif
	@$(ECHO) export BLOP_CXX=${CXX} >> $@
	@$(ECHO) export BLOP_CXXSTD=${CXXSTD} >> $@
	@$(ECHO) 'export BLOP_CXXFLAGS="${CXXFLAGS}"' >> $@
	@$(ECHO) export ECHO=$(ECHO) >> $@
	@$(ECHO) export HAVE_GTS_H=${HAVE_GTS_H} >> $@
	@$(ECHO) export HAVE_GSL=${HAVE_GSL} >> $@
	@$(ECHO) "export BLOP_CONFIG_OPTIONS=\"${BLOP_CONFIG_OPTIONS}\"" >> $@

install : 
	@$(ECHO)
	@$(ECHO) ===\> Copy the scripts to the directory: ${DESTDIR}${BINDIR}
	   cp blop-config-vars bloprc.C ${DESTDIR}${DATADIR}/
	   echo ${DATADIR}/blop-config-vars >> ${FILELIST}
	   echo ${DATADIR}/bloprc.C         >> ${FILELIST}
	for f in $(scripts); do \
	  target=${DESTDIR}${BINDIR}/$$f ;\
	  rm -f $$target; \
	  cat $$f | sed "s|@PERL@|$(PERL)|; s|@config@|${DATADIR}/blop-config-vars|;" >$$target;\
	  chmod +x $$target; \
	  echo ${BINDIR}/$$f >> ${FILELIST}; \
	done
	@$(ECHO) 

#$(DESTDIR)$(INSDIR)/%.pl $(DESTDIR)$(BINDIR)/% : %.pl
#	@ $(ECHO) -n "   Installing $(BINDIR)/$* --> $(INSDIR)/$*"
#	@ awk '/# short description:/ {printf "( "; for(i=4;i<=NF;++i) printf "%s ",$$i; printf ")";}' $^
#	@ $(ECHO)
#	@ cp $^ $@
#	@ $(ECHO) "#!/bin/sh"                    >  $(DESTDIR)$(BINDIR)/$*
#	@ $(ECHO) ". $(INSDIR)/blop-config-vars" >> $(DESTDIR)$(BINDIR)/$*
#	@ $(ECHO) 'perl $(INSDIR)/$*.pl $$*'     >> $(DESTDIR)$(BINDIR)/$*
#	@ chmod +x $(DESTDIR)$(BINDIR)/$*
#	@ rm -f $(DESTDIR)$(BINDIR)/$* 
#	@ ln -s $(DESTDIR)$(INSDIR)/$* $(DESTDIR)$(BINDIR)/$*

#$(DESTDIR)$(INSDIR)/% : %
#	@ $(ECHO) -n "   Installing $(BINDIR)/$* --> $(INSDIR)/$*"
#	@ awk '/# short description:/ {printf "( "; for(i=4;i<=NF;++i) printf "%s ",$$i; printf ")";}' $^
#	@ $(ECHO)
#	@ awk '{print} NR==1 && $$1!~/perl/ {print ". $(INSDIR)/blop-config-vars";}' $^ > $(DESTDIR)$(INSDIR)/$*
#	@ chmod +x $(DESTDIR)$(INSDIR)/$*
#	@ rm -f $(DESTDIR)$(BINDIR)/$*
#	@ ln -s $(INSDIR)/$* $(BINDIR)/$*

uninstall:
	for f in $(scripts); do \
	  rm -f ${DESTDIR}${BINDIR}/$$f ;\
	done

clean:
	rm -f *~

distclean :
	rm -f *~ blop-config-vars
