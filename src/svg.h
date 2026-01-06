#ifndef __BLOP_SVG_H__
#define __BLOP_SVG_H__

#include "pdf.h"
#include "var.h"

namespace blop
{
    /* A terminal class to produce SVG files */

    class svg : public pdf
    {
    private:
	typedef pdf base;

    protected:
	std::string filename_;

    public:

	// ==============================================================
	// the static 'print' function to print the current canvas into
	// a file (provided as the first argument). This terminal actually
	// first creates a 'blopeps' file, which is then transformed into
	// a pdf file using the script 'beps2pdf', then into svg using inkscape. The second argument
	// of this static 'print' function specifies the command line args
	// for this 'beps2pdf' script. Say 'beps2pdf --help' to get a list
	// of available options (and their meaning :-)

	static void    print(const var &filename, const var &opts = var(""));


	// ---------  constructor, destructor  --------------------------

	svg(const var &filename, const var &opts = var(""));
	~svg();


	// this function is not to be called by the user
	void picture_end();

        static std::string filename_extension()  { return ".svg"; }
    };

}


#endif
