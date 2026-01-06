#ifndef __BLOP_BMP_H__
#define __BLOP_BMP_H__

#include "blopeps.h"
#include "var.h"

namespace blop
{
    class bmp : public blopeps
    {
    private:
	std::string filename_,opts_;
    public:
	static void    print(const var &filename,const var &opts = var(""));
	bmp(const var &filename, const var &opts = var(""));
	~bmp();
	void picture_end();
        static std::string filename_extension()  { return ".bmp"; }
    };

}

#endif
