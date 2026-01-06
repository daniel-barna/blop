#include "svg.h"
#include "blop_bookkeeper.h"
#include "bloputils.h"
#include "canvas.h"


namespace blop
{
    void svg::print(const var &filename,const var &opts)
    {
	svg t(filename,opts);
	canvas::current().print(&t);
    }

    svg::svg(const var &filename,const var &opts) :
	pdf(blop_bookkeeper::tmpfile("SVG_XXXXXX.pdf"),opts), filename_(filename)
    {
    }

    svg::~svg()
    {
    }

    void svg::picture_end()
    {
	base::picture_end();
        string cmd;
        if(system("which scour >/dev/null 2>&1") == 0)
        {
            string unoptimized_svg = blop_bookkeeper::tmpfile("SVG_XXXXXX.svg");
            cmd = "(inkscape --pdf-poppler --export-plain-svg '" + base::filename_ + "' -o '" + unoptimized_svg + "' && scour -i '" + unoptimized_svg + "' -o '" + filename_ + "' --remove-descriptive-elements --enable-comment-stripping --enable-id-stripping --shorten-ids) 2>&1"; 
        }
        else
        {
            cmd = "inkscape --pdf-poppler --export-plain-svg '" + base::filename_ + "' -o '" + filename_ + "' 2>&1"; 
            cerr<<"scour is not installed on your system, the svg will not be optimized (resulting in a larger filesize)."<<endl;
        }
        
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
