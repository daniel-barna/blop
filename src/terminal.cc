#include "terminal.h"

using namespace std;

namespace blop
{
    bool terminal::id::operator== (const terminal::id &o) const
    {
	if(length_id != 0)
	{
	    if(length_id == o.length_id) return true;
	    return false;
	}
	
	if(relcoord == o.relcoord && direction == o.direction) return true;
	return false;
    }
    

}
