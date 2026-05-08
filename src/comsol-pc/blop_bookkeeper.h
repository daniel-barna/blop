#ifndef __BLOP_BOOKKEEPER_H__
#define __BLOP_BOOKKEEPER_H__
#include <string>
#include <vector>
#include "var.h"

namespace blop
{
    class blop_initializer
    {
    private:
	void call_INIT();
    public:
	blop_initializer();
    };

    class blop_bookkeeper
    {
    private:
	static std::string tmpdir_;
	static int  tmp_count_;
	static std::string get_tmpdir_();
	static std::vector<int> wait_pid_;
    public:

	// create and return a unique temporary filename. file will be automatically removed on exit
	// 'tmp' can contain the string XXXXXX, which will be replaced by a unique identifier. 
	static std::string tmpfile(var tmp);  

	// create a unique temporary directory and return its name. 'tmp' can contain XXXXXX
	static std::string tmpdir(var tmp);   

	// register a process-id to wait for before quitting.
	static void register_pid(int pid); 
	static void wait(int pid);

	~blop_bookkeeper();
    };

    extern blop_bookkeeper BLOP;

}

#endif
