#include "jpg.h"
#include "blop_bookkeeper.h"
#include "bloputils.h"
#include "canvas.h"

namespace blop
{
    void jpg::print(const var &filename,const var &opts)
    {
	jpg t(filename,opts);
	canvas::current().print(&t);
    }

    jpg::jpg(const var &filename,const var &opts) :
	blopeps(blop_bookkeeper::tmpfile(filename.str() + "_XXXXXX.beps")), filename_(filename), opts_(opts)
    {
    }

    jpg::~jpg()
    {
    }

    void jpg::picture_end()
    {
	blopeps::picture_end();
	string cmd = "beps2jpg " + opts_ + " '" + blopeps::filename_ + "' '" + filename_ + "' 2>&1"; 
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
