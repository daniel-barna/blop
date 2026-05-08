#ifndef __BLOP_EPS_H__
#define __BLOP_EPS_H__

#include "blopeps.h"
#include "var.h"

namespace blop
{
    /* A terminal class to produce 'normal' EPS files */

    class eps : public blopeps
    {
    private:
	typedef blopeps base;

	std::string opts_;

    protected:
	std::string filename_;

    public:

	// ---------  Print the current canvas to a given file  --------
	// the static 'print' function to print the current canvas into
	// a file (provided as the first argument). This terminal actually
	// first creates a 'blopeps' file, which is then transformed into
	// an eps file using the script 'beps2eps'. The second argument
	// of this static 'print' function specifies the command line args
	// for this 'beps2eps' script. Say 'beps2eps --help' to get a list
	// of available options (and their meaning :-)

	static void    print(const var &filename, const var &opts = var(""));


	// ---------  Constructor, destructor  -------------------------

	eps(const var &filename, const var &opts = var(""));
	~eps();


	// ---------- this function is not to be called by the user -----
	void picture_end();

        static std::string filename_extension() { return ".eps"; }
    };

}


#endif
