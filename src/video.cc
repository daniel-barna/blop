#include "video.h"
#include "blop_bookkeeper.h"
#include "png.h"
#include "jpg.h"
#include "warning.h"
#include "bloputils.h"
#include "global.h"

namespace blop
{
    using namespace std;

    video::video(const var &filename, const var  &options) : frameno_(0), filename_(filename), options_(options), pictype_("jpg"), fps_(25), autorefresh_(100)
    {
	tmpdir_ = blop_bookkeeper::tmpdir("video-XXXXXX");
	if(system("which ffmpeg >/dev/null 2>&1") == 0) converter_ = "ffmpeg";
        else if(system("which avconv >/dev/null 2>&1") == 0) converter_ = "avconv";
        else
        {
            warning::print("Neither ffmpeg nor avconv is available on your system. Video output will fail!");
        }
    }

    video::~video()
    {
	flush();
    }

    void video::wait_all_()
    {
	for(unsigned int i=0; i<pids_.size(); ++i) blop_bookkeeper::wait(pids_[i]);
	pids_.clear();
    }

    video &video::pictype(const var &s)
    {
	if(s != "jpg" && s != "png")
	{
	    warning::print("Bad picture type for video. Using jpg.","video::pictype(" & s & var(")"));
	    pictype_ = "jpg";
	}
	else
	{
	    pictype_ = s.str();
	}
	return *this;
    }

    video &video::autorefresh(int f)
    {
	autorefresh_ = f;
	return *this;
    }

    void video::print()
    {
	char s[100];
	sprintf(s,"%s/frame-%06d.beps",tmpdir_.c_str(),frameno_);
	string bepsname = s;
	blopeps::print(bepsname);

	// don't let too many processes run in parallel.... 
	if(pids_.size()>20) wait_all_();

	string cmd;
	if(pictype_ == "png")
	{
	    sprintf(s,"%s/frame-%06d.png",tmpdir_.c_str(),frameno_);
	    cmd = "beps2png " + options_ + " " + bepsname + " " + s + "; rm -f " + bepsname;
	}
	else
	{
	    sprintf(s,"%s/frame-%06d.jpg",tmpdir_.c_str(),frameno_);
	    cmd = "beps2jpg " + options_ + " " + bepsname + " " + s + "; rm -f " + bepsname;
	}
	if(global::debug>0) cout<<"[blop] [DEBUG] Launching in background: "<<cmd<<endl;
	pids_.push_back(system_bg(cmd));
	++frameno_;
	if(autorefresh_>0 && frameno_%autorefresh_==0) flush();
    }

    void video::flush()
    {
	wait_all_();
	std::string cmd = converter_ + " -y -i " + tmpdir_ + (pictype_ == "png" ? "/frame-%06d.png" : "/frame-%06d.jpg")
	    + " -r " + var(fps_).str() + " -sameq " + filename_;
	if(system(cmd) != 0)
	{
	    warning::print("Failed to create video");
	}
    }

}
