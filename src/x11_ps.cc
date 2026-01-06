#include "x11_ps.h"
#include "blop_bookkeeper.h"
#include "bloputils.h"
#include "canvas.h"
#include "config.h"
#include <signal.h>
#include <cstring>

using namespace std;

namespace blop
{
    string &x11_ps::default_viewer_()
    {
	static bool called = false;
	static string viewer;
	if(!called)
	{
	    if(!strcmp("BLOP_ARCH","darwin")) viewer = "open";
	    else
	    {
		string user = getenv("USER");
		if(system("which gv >/dev/null 2>&1") == 0) viewer = "gv";
		else if(system(("pgrep -u " + user + " kwin >/dev/null").c_str()) == 0)
		{
		    if(system("which okular >/dev/null 2>&1") == 0) viewer = "okular";
		    else if(system("which kde-open >/dev/null 2>&1") == 0) viewer = "kde-open";
		    else
		    {
			warning::print("No postscript viewer found");
			viewer = "no_postscript_viewer_found";
		    }
		}
		else if(system("which evince >/dev/null 2>&1") == 0)
		{
		    warning::print("Using evince as default postscript viewer. "
				   "It does NOT update automatically, so it will be pretty useless...");
		    viewer = "evince";
		}
		else
		{
		    warning::print("No postscript viewer found");
		    viewer = "no_postscript_viewer_found";
		}
	    }
	}
	return viewer;
    }

    string &x11_ps::default_options_()
    {
	static bool called = false;
	static string option;
	if(!called)
	{
	    if(default_viewer_() == "gv")
	    {
		string gv_options = readcmd("gv -h");
		if(gv_options.find("-resize")  != string::npos) option += " -resize ";
		
		if(gv_options.find("-media=")  != string::npos) option += " -media=BBox ";
		else if(gv_options.find("-media ")  != string::npos) option += " -media BBox ";
		
		if(gv_options.find("-spartan") != string::npos) option += " -spartan ";
		if(gv_options.find("-watch")   != string::npos) option += " -watch ";
	    }
	    if(default_viewer_() == "open")
	    {

	    }
	    called = true;
	}
	return option;
    }

    void x11_ps::default_viewer(const var &v)
    {
	default_viewer_() = v.str();
    }

    void x11_ps::default_options(const var &o)
    {
	default_options_() = o.str();
    }

    x11_ps &x11_ps::width(const var &s)
    {
	width_ = s.str();
	return *this;
    }

    x11_ps &x11_ps::height(const var &s)
    {
	height_ = s.str();
	return *this;
    }

    x11_ps &x11_ps::def()
    {
	static x11_ps g;
	return g;
    }

    void x11_ps::print()
    {
	canvas::current().print(&def());
    }

    x11_ps::x11_ps()
	: blopeps(blop_bookkeeper::tmpfile("x11_ps_XXXXXX.beps"))
    {
	filename_eps_ = "";
        width_ = "0.9\\linewidth";
        height_ = "12cm";
	options_ = default_options_();
    }

    x11_ps::~x11_ps()
    {
	viewer_.kill();
    }

    void x11_ps::picture_end()
    {
	blopeps::picture_end();
	if(filename_eps_ == "")
	    filename_eps_=blop_bookkeeper::tmpfile("x11_ps_XXXXXX.eps");
	system("beps2eps -q -w '" + width_
	       + "' -h '" + height_ + "' " + filename_
	       + " " + filename_eps_);
	if(!viewer_.is_open())
	{
	    std::string cmd = default_viewer_() + " " + options_ + " " + filename_eps_;
	    viewer_.open(cmd);
	}
	else
	{
	    if(viewer_.cmd() == "gv") viewer_.kill(SIGHUP);
	}
    }
}
