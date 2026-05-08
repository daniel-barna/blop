#ifndef __EXC_H__
#define __EXC_H__
#ifndef __MAKECINT__

#include <string>

namespace blop
{
    class errormsg
	{
	public:
	    string text;
	    int nest_level;
	    errormsg(const string &s) : text(s),nest_level(0)
		{
		    cerr<<"ERROR: "<<s<<endl;
		}
	};


#define err(a) \
{ \
  char s[1000]; \
  sprintf(s,"%s  (FILE: %s  LINE: %i)",a,__FILE__,__LINE__); \
  throw errormsg(s); \
}


#ifdef DEBUG
#define TRY try
#define CATCH(a) \
catch(errormsg &m) \
{\
  if(m.nest_level++ == 0) cerr<<"Error occured in "<<a<<" :   "<<m.text<<endl;\
  else cerr<<"   ... called from "<<a<<endl; \
  throw;\
}
#else
#define TRY 
#define CATCH(a)
#endif    
    
}



#endif
#endif
