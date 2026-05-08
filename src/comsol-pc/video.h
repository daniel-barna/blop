#ifndef __BLOP_VIDEO_H__
#define __BLOP_VIDEO_H__

#include "var.h"
#include <fstream>
#include <string>
#include <vector>

namespace blop
{

    class video 
    {
    private:
	int frameno_;
	std::string tmpdir_;
	std::string filename_;
	std::string options_;
	std::string pictype_;
	double fps_;
	int autorefresh_;
	std::vector<int> pids_;
	void wait_all_();

        string converter_;

    public:
	video(const var &filename, const var &options="-w 10cm -h 10cm");
	~video();

	void print();
	void flush();

	video &fps(double f) { fps_ = f; return *this; }
	video &pictype(const var &s);
	video &autorefresh(int);        // refresh (i.e. generate the video output) after every this-many frames
    };

}

#endif
