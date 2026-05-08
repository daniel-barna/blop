#include "blop_bookkeeper.h"
#include "warning.h"
#include <stdlib.h>
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

namespace blop
{
    using namespace std;

    blop_initializer::blop_initializer()
    {
	call_INIT();
    }

    // blop_initializer::call_INIT() is defined in blop_cint.cc or blop_nocint.cc

    std::string blop_bookkeeper::tmpdir_;
    int blop_bookkeeper::tmp_count_ = 1;
    std::vector<int> blop_bookkeeper::wait_pid_;

    blop_bookkeeper::~blop_bookkeeper()
    {
	if(!wait_pid_.empty())
	{
	    cerr<<"[blop] Waiting for child processes to terminate...";
	}
	for(unsigned int i=0; i<wait_pid_.size(); ++i)
	{
	    int status =0;
	    waitpid(wait_pid_[i],&status,0);
	}
	if(tmpdir_ != "")
	{
	    char s[1000];
	    sprintf(s,"rm -rf %s",tmpdir_.c_str());
	    system(s);
	}
    }

    void blop_bookkeeper::register_pid(int pid)
    {
	wait_pid_.push_back(pid);
    }

    void blop_bookkeeper::wait(int pid)
    {
	int status =0;
	waitpid(pid,&status,0);
	for(unsigned int i=0; i<wait_pid_.size(); ++i)
	{
	    if(wait_pid_[i] == pid)
	    {
		wait_pid_.erase(wait_pid_.begin()+1);
		return;
	    }
	}
    }

    std::string blop_bookkeeper::get_tmpdir_()
    {
	if(tmpdir_ == "")
	{
	    char s[1000];
	    if(tmpdir_ == "")
	    {
		sprintf(s,"/tmp/BLOP-%i-XXXXXX",getpid());
		if(mkdtemp(s) == 0)
		{
		    warning::print("Could not create temprary directory: " & var(s));
		    return "";
		}
		tmpdir_ = s;
	    }
	}
	return tmpdir_;
    }

    std::string blop_bookkeeper::tmpfile(var temp)
    {
	var::size_type slash = temp.rfind("/");
	if(slash != var::npos) temp.replace(0,slash+1,"");
	char s[1000];
	if(temp.find("XXXXXX") == var::npos) temp = "XXXXXX-" & temp;
	sprintf(s,"%06i",tmp_count_++);
	temp.replace("XXXXXX",s);
	string result = (get_tmpdir_() & "/" & temp).str();
	struct stat buf;
	if(stat(result.c_str(),&buf) == 0) warning::print("Oops, this temporary file already exists. Please report this problem!");
	return result;
    }

    std::string blop_bookkeeper::tmpdir(var temp)
    {
	string result = tmpfile(temp);
	if(mkdir(result.c_str(),S_IRWXU) != 0) warning::print("Failed to create temporary directory");
	return result;
    }

    blop_bookkeeper BLOP;
}

