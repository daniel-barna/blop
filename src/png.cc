#include "png.h"
#include "blop_bookkeeper.h"
#include "bloputils.h"
#include "canvas.h"

namespace blop
{
    void png::print(const var &filename,const var &opts)
    {
	png t(filename,opts);
	canvas::current().print(&t);
    }

    png::png(const var &filename,const var &opts) :
	blopeps(blop_bookkeeper::tmpfile(filename.str() + "_XXXXXX.beps")), filename_(filename), opts_(opts)
    {
    }

    png::~png()
    {
    }

    void png::picture_end()
    {
	blopeps::picture_end();
	string cmd = "beps2png " + opts_ + " '" + blopeps::filename_ + "' '" + filename_ + "' 2>&1"; 
	int rc;
	var message = readcmd(cmd, &rc);
	if(rc != 0)
	{
	    cerr<<"The following command failed:"<<endl;
	    cerr<<cmd<<endl<<endl;
	    cerr<<"Its output is the following:"<<endl;
	    cerr<<message.str()<<endl;
	}

    }
}
