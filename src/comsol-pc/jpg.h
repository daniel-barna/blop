#ifndef __BLOP_JPG_H__
#define __BLOP_JPG_H__

#include "blopeps.h"
#include "var.h"

namespace blop
{
    class jpg : public blopeps
    {
    private:
	std::string filename_,opts_;
    public:
	static void    print(const var &filename,const var &opts = var(""));
	jpg(const var &filename, const var &opts = var(""));
	~jpg();
	void picture_end();

        static std::string filename_extension()  { return ".jpg"; }
    };

}

#endif
