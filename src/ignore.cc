#include "ignore.h"
#include "constants.h"

namespace blop
{
    std::vector<std::string> &ignore::list_()
    {
	static std::vector<std::string> the_list;
	if(the_list.empty())
	{
	    the_list.push_back("inf");
	    the_list.push_back("-inf");
	    the_list.push_back("nan");
	}
	return the_list;
    }

    void ignore::add(const var &v)
    {
	list_().push_back(v.str());
    }

    void ignore::clear()
    {
	list_().clear();
    }

    bool ignore::it(const var &v)
    {
	if(v.dbl() == unset) return true;
	if(!finite(v.dbl())) return true;
	for(unsigned int i=0; i<list_().size(); ++i)
	{
	    if(list_()[i] == v.str()) return true;
	}
	return false;
    }

}
