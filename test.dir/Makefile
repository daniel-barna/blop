# Do not call make in this directory
# call it in the top-level directory!

include ../config.mk

# ifeq ($(BLOP_ENABLE_CINT),yes)
#   TEST := test.ps
# else
#   TEST :=
# endif

TEST = test.ps

all : $(TEST)

$(TEST) : test.dvi
	dvips -o $@ -t a4 $^


test.dvi : test.tex picture.beps ../latex/blopeps.sty
	latex test.tex

picture.beps : test.C data ../src/blop.exe
	INPUTMODE=C++ ../src/blop.exe ../src/init.C test.C

distclean : clean

clean:
	rm -f *~ *.dvi *.ps *.tmp *.beps *.eps test.aux test.log
