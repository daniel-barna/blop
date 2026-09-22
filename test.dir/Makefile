# Do not call make in this directory
# call it in the top-level directory!

include ../config.mk

# ifeq ($(BLOP_ENABLE_CINT),yes)
#   TEST := test.ps
# else
#   TEST :=
# endif

TEST = test.pdf

all : $(TEST)

$(TEST) : test.ps
	ps2pdf -dALLOWPSTRANSPARENCY $<

test.ps : test.dvi
	dvips -o $@ -t a4 $^


test.dvi : test.tex picture.beps ../latex/blopeps.sty
	latex test.tex

picture.beps : test.C data 
	$(CXX) $(CXXFLAGS) -I/usr/include/glib-2.0  -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ -I../src main.C -L../src -lblop  `gts-config --libs` `gsl-config --libs` -o test.exe && ./test.exe


distclean : clean

clean:
	rm -f *~ *.dvi *.ps *.tmp *.beps *.eps test.aux test.log
