#ifndef __BLOP_PNG_H__
#define __BLOP_PNG_H__

#include "blopeps.h"
#include "var.h"

namespace blop
{

    class png : public blopeps
    {
    private:
	std::string filename_,opts_;
    public:
	static void    print(const var &filename,const var &opts = var(""));
	png(const var &filename, const var &opts = var(""));
	~png();
	void picture_end();

        static std::string filename_extension()  { return ".png"; }
    };

}


#endif
