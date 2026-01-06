# Do NOT run this Makefile from this directory. Run the
# top-level Makefile!

.PHONY : compile clean distclean install uninstall
all : compile
include ../config.mk

SOURCES := $(wildcard *.cc)
OBJECTS := $(SOURCES:.cc=.o)
TOOLS   := blop-merge blop-help
LIBBLOP := -L../src -lblop

compile : $(TOOLS) 

distclean : clean

clean :
	rm -f  *.o *~ *.d *.a $(TOOLS)

ifneq ($(INCLUDEDEPS),false)
include $(OBJECTS:.o=.d)
endif

% : %.o
	${CXX} -o $@ $^ ${LIBBLOP} ${LIBS}

%.o : %.cc
	$(CXX) $(CXXFLAGS) -O -c -o $@ $< -I../src
%.d : %.cc
	$(MAKEDEPEND) $^ > $@

install: $(TOOLS)
	@$(ECHO)
	@$(ECHO) ===\> Copy blop tools to ${BINDIR}
	   if [ ! -d ${DESTDIR}${BINDIR} ] ; then \
	     mkdir -p ${DESTDIR}${BINDIR}; \
	   fi
	   cp ${TOOLS} ${DESTDIR}${BINDIR}/
	   for f in ${TOOLS}; do echo ${BINDIR}/$$f >> ${FILELIST}; done
	@$(ECHO)
	@$(ECHO) ..... DONE ....
	@$(ECHO)

uninstall::
	for f in ${TOOLS}; do rm -f ${BINDIR}/$$f; done

