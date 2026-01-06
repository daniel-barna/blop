#include "pdf.h"
#include "blop_bookkeeper.h"
#include "bloputils.h"
#include "canvas.h"


namespace blop
{
    void pdf::print(const var &filename,const var &opts)
    {
	pdf t(filename,opts);
	canvas::current().print(&t);
    }

    pdf::pdf(const var &filename,const var &opts) :
	blopeps(blop_bookkeeper::tmpfile("PDF_XXXXXX.beps")),  opts_(opts), filename_(filename)
    {
    }

    pdf::~pdf()
    {
    }

    void pdf::picture_end()
    {
	base::picture_end();
	string cmd = "beps2pdf " + opts_ + " '" + base::filename_ + "' '" + filename_ + "' 2>&1"; 
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
