#ifndef __BLOP_PDF_H__
#define __BLOP_PDF_H__

#include "blopeps.h"
#include "var.h"

namespace blop
{
    /* A terminal class to produce pdf files */

    class pdf : public blopeps
    {
    private:
	typedef blopeps base;

	std::string opts_;

    protected:
	std::string filename_;

    public:

	// ==============================================================
	// the static 'print' function to print the current canvas into
	// a file (provided as the first argument). This terminal actually
	// first creates a 'blopeps' file, which is then transformed into
	// a pdf file using the script 'beps2pdf'. The second argument
	// of this static 'print' function specifies the command line args
	// for this 'beps2pdf' script. Say 'beps2pdf --help' to get a list
	// of available options (and their meaning :-)

	static void print(const var &filename, const var &opts = var(""));
        static void print(const char *filename, const var &opts = var("")) { print(var(filename), opts); }  
        static void print(const std::string &filename, const var &opts = var("")) { print(var(filename),opts); }
        static void print(const std::filesystem::path &filename, const var &opts = var("")) {print(var(filename.c_str()), opts);}


	// ---------  constructor, destructor  --------------------------

	pdf(const var &filename, const var &opts = var(""));
	~pdf();


	// this function is not to be called by the user
	void picture_end();

        static std::string filename_extension()  { return ".pdf"; }
    };

}


#endif
