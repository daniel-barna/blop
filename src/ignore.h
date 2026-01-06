#ifndef __BLOP_IGNORE_H__
#define __BLOP_IGNORE_H__

#include <vector>
#include <string>
#include "var.h"

namespace blop
{

    class ignore
    {
    private:
	static std::vector<std::string> &list_();

    public:
	static void add(const var &v);
	static void clear();
	static bool it(const var &v);
    };
    
}


#endif

