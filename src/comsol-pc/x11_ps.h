#ifndef __BLOP_X11_PS_H__
#define __BLOP_X11_PS_H__

#include "blopeps.h"
#include "pstream.h"

namespace blop
{

    class x11_ps : public blopeps
    {
    private:
	opstream viewer_;
	std::string filename_eps_;
	std::string width_,height_;
	static std::string &default_viewer_();
	static std::string &default_options_();
	std::string options_;
    public:
	static x11_ps &def();
	static void    print();
	static void    default_options(const var &o);
	static void    default_viewer(const var &v);

	x11_ps();
	~x11_ps();
	void picture_end();

	x11_ps &width(const var &);
	x11_ps &height(const var &);
    };

}


#endif
