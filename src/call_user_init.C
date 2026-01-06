#include <dlfcn.h>
#include <iostream>

class user_init_caller
{
public:
    typedef void (*initfcn)();
    inline user_init_caller()
	{
	    void *handle = dlopen(0,RTLD_LAZY);
	    if(handle == 0)
	    {
		std::cerr<<"This executable doesn't seem to have been compiled with the -rdynamic flag"<<endl;
	    }
	    else
	    {
		initfcn f = (initfcn)dlsym(handle,"BLOP_USER_INIT");
		if(f) f();
	    }
	}
};

user_init_caller user_init_caller_instance;
