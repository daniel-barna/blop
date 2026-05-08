#include "eps.h"
#include "blop_bookkeeper.h"
#include "bloputils.h"
#include "canvas.h"

namespace blop
{
    void eps::print(const var &filename,const var &opts)
    {
	eps t(filename,opts);
	canvas::current().print(&t);
    }

    eps::eps(const var &filename,const var &opts) :
	blopeps(blop_bookkeeper::tmpfile("EPS_XXXXXX.beps")),  opts_(opts), filename_(filename)
    {
    }

    eps::~eps()
    {
    }

    void eps::picture_end()
    {
	base::picture_end();
	string cmd = "beps2eps " + opts_ + " '" + base::filename_ + "' '" + filename_ + "' 2>&1"; 
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
