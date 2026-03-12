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
	static void print(const var &filename,const var &opts = var(""));
        static void print(const char *filename, const var &opts = var("")) { print(var(filename), opts); }  
        static void print(const std::string &filename, const var &opts = var("")) { print(var(filename),opts); }
        static void print(const std::filesystem::path &filename, const var &opts = var("")) {print(var(filename.c_str()), opts);}

	png(const var &filename, const var &opts = var(""));
	~png();
	void picture_end();

        static std::string filename_extension()  { return ".png"; }
    };

}


#endif
