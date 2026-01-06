#include "printer.h"
#include "blop_bookkeeper.h"
#include "bloputils.h"
#include "canvas.h"


namespace blop
{
    std::string printer::default_cmd_ = "lpr %f";
    var printer::default_command() {return default_cmd_;}
    void printer::default_command(const var &o) {default_cmd_ = o.str(); }

    void printer::print(const var &cmd, const var &epsopts)
    {
	printer t(cmd, epsopts);
	canvas::current().print(&t);
    }

    printer::printer(const var &cmd, const var &epsopts) :
	eps(blop_bookkeeper::tmpfile("PRINT_XXXXXX.eps"),epsopts), cmd_(cmd)
    {
    }

    printer::~printer()
    {
    }

    void printer::picture_end()
    {
	base::picture_end();

	var cmd = cmd_;
	if(cmd.find("%f") == var::npos) cmd &= " %f";
	cmd.replace("%f",base::filename_);
	int rc;
	var message = readcmd(cmd, &rc);
	if(rc != 0)
	{
	    cerr<<"The following command failed:"<<endl;
	    cerr<<cmd.str()<<endl<<endl;
	    cerr<<"Its output is the following:"<<endl;
	    cerr<<message.str()<<endl;
	}
    }
}

