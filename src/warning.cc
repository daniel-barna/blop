#include "warning.h"
#include <fstream>

using namespace std;

namespace blop
{
    var warning::format_ = "%p %m\n%F";
    var warning::prompt_ = "[blop]";
    var warning::fnformat_ = "%p      [in %f]\n";
    std::ostream *warning::destination_ = &std::cerr;
    bool warning::delete_destination_ = false;
    vector<string> warning::exclude_;

    void warning::exclude(const var &pat)
    {
	exclude_.push_back(pat.str());
    }

    void warning::print(const var &message, const var &funcname)
    {
	for(unsigned int i=0; i<exclude_.size(); ++i)
	{
	    if(funcname.str().find(exclude_[i]) != string::npos) return;
	}

	var m = format_;

	if(funcname.str() == "") m.replace("%F","");
	else m.replace("%F",fnformat_);

	m.replace("%f",funcname);
	m.replace("%p",prompt_);
	m.replace("%m",message);

	(*destination_)<<m;
    }

    void warning::destination(ostream &dest)
    {
	if(delete_destination_) delete destination_;
	destination_ = &dest;
	delete_destination_ = false;
    }

    void warning::destination(const var &filename)
    {
	if(delete_destination_) delete destination_;
	destination_ = new ofstream(filename.c_str());
	delete_destination_ = true;
    }

    // ------------- debug
    var debug::format_ = "%p %m\n%F";
    var debug::prompt_ = "[blop-debug]";
    var debug::fnformat_ = "%p      [in %f]\n";
    std::ostream *debug::destination_ = &std::cerr;
    bool debug::delete_destination_ = false;
    vector<string> debug::exclude_;
    bool debug::on_ = false;

    void debug::exclude(const var &pat)
    {
	exclude_.push_back(pat.str());
    }

    void debug::print(const var &message, const var &funcname)
    {
	if(!on_) return;
	for(unsigned int i=0; i<exclude_.size(); ++i)
	{
	    if(funcname.str().find(exclude_[i]) != string::npos) return;
	}

	var m = format_;

	if(funcname.str() == "") m.replace("%F","");
	else m.replace("%F",fnformat_);

	m.replace("%f",funcname);
	m.replace("%p",prompt_);
	m.replace("%m",message);

	(*destination_)<<m;
    }

    void debug::destination(ostream &dest)
    {
	if(delete_destination_) delete destination_;
	destination_ = &dest;
	delete_destination_ = false;
    }

    void debug::destination(const var &filename)
    {
	if(delete_destination_) delete destination_;
	destination_ = new ofstream(filename.c_str());
	delete_destination_ = true;
    }


}

