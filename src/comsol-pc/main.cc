#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include "help.h"
#include "frame.h"
#include "exc.hh"
#ifdef CINT_INCLUDE_PREFIX
#include "cint/Api.h"
#else
#include "Api.h"
#endif
#include <vector>
#include <string>
#include <unistd.h>
#include <cstdio>

#include "fit.h"
#include "blop_bookkeeper.h"
#include "config.h"
#include "ignore.h"
#include "pad.h"
#include "x11_ps.h"
#include "warning.h"

using namespace std;
using namespace blop;

int get_full_command(istream &in, string &com)
{
    unsigned int i=0;
    int nest=0;
    int single_quote=0;
    int double_quote=0;
    int semicolumnattheend=0;

    com = "";
    in>>ws;
    getline(in,com);

    // end of file
    if(com == "" && !in) return -1;

    while(i != com.size())
    {
    readagain:
	switch(com[i]) {
	case '"':
	    if(single_quote==0) double_quote ^= 1;
	    break;
	case '\'':
	    if(double_quote==0) single_quote ^= 1;
	    break;
	case '{':
	case '(':
	case '[':
	    if((single_quote==0)&&(double_quote==0)) nest++;
	    break;
	case '}':
	case ')':
	case ']':
	    if((single_quote==0)&&(double_quote==0)) nest--;
	    break;
	case '\\':
	    //++i;
	    //if(0==com[i] || '\n'==com[i]) {
	    if(i+1 == com.size() || com[i+1] == '\n')
	    {
		//--i;
		string line;
		getline(in,line);
		com += "\n";
		com += line;
		//strcpy(com+i,G__input("> "));
		//if(G__return==G__RETURN_IMMEDIATE) return(-1);
	    }
	    break;
	case '/':
	    if((single_quote==0)&&(double_quote==0)) {
		if(i+1 < com.size() && '/'==com[i+1]) {
		    com.erase(i,string::npos);
		    //com[i]=0;
		    //com[i+1]=0;
		}
	    }
	    break;
	}

	if(';'==com[i]) {
	    if((single_quote==0)&&(double_quote==0)&&(nest==0)) semicolumnattheend=1;
	}
	else {
	    if(!isspace(com[i])) semicolumnattheend=0;
	}

	++i;
    }


    if(nest>0 && '{'!=com[0])
    {
	if(com.find("for(") == 0 ||
	   com.find("for ") == 0 ||
	   com.find("while(") == 0 ||
	   com.find("while ") == 0 ||
	   com.find("do ") == 0    ||
	   com.find("do{") == 0    ||
	   com.find("namespace ") == 0 ||
	   com.find("namespace{") == 0)
	{
	    string line;
	    getline(in,line);
	    com += "\n";
	    com += line;
	    //strcpy(com+i,G__input("end with '}', '@':abort > "));
	}
	else
	{
	    string line;
	    getline(in,line);
	    com += "\n";
	    com += line;
	    //strcpy(com+i,G__input("end with ';', '@':abort > "));
	}
/*
	if(G__return==G__RETURN_IMMEDIATE) return(-1);
	if('@'==com[i]) {
	    com[0]=0;
	    return(0);
	}
*/
	goto readagain;
    }

    if(0<nest) return 1;
  
    if(/*G__INPUTCXXMODE==G__rootmode && */
	0==nest &&
	0==semicolumnattheend &&
	'#'!=com[0]           &&
	com.find("for(") != 0 &&
	com.find("for ") != 0 &&
	com.find("while(") != 0 &&
	com.find("while ") != 0 &&
	com.find("do ")    != 0 &&
	com.find("do{")    != 0 &&
	com.find("namespace ") != 0 &&
	com.find("namespace{") != 0)
    {
	string line;
	getline(in,line);
	com += "\n";
	com += line;
/*
	strcpy(com+i,G__input("end with ';', '@':abort > "));
	if(G__return==G__RETURN_IMMEDIATE) return(-1);
	if('@'==com[i]) {
	    com[0]=0;
	    return(0);
	}
*/
	goto readagain;
    }
    if(single_quote || double_quote || nest<0) return 2;
    return 0;
}

//typedef void (*G__DLLINIT)();
//extern std::list<G__DLLINIT>* G__initpermanentsl;


int main(int argc, char *argv[])
TRY
{
    get_config_vars();

    vector<char *> tmpfiles;

    char **argv_tmp = new char* [argc+4];
    int argc_tmp = 0;
    argv_tmp[argc_tmp++] = (char *)"blop";

    const char *HOME = getenv("HOME");
    char user_init_dir[300];
    if(HOME == 0)
    {
	cerr<<"Could not get home directory"<<endl;
    }
    else
    {
	sprintf(user_init_dir,"-I%s/.blop",HOME);
	argv_tmp[argc_tmp++] = user_init_dir;
    }

    bool script_mode = true;
    for(int i=1; i<argc; ++i)
    {
	if(!strcmp(argv[i],"-x") || !strcmp(argv[i],"-s"))
	{
	    if(i+1 >= argc)
	    {
		cerr<<"One more argument expected after the option -x or -s"<<endl;
		return 1;
	    }
	    string tmpfilename = blop_bookkeeper::tmpfile("blop-exec-XXXXXX");
	    tmpfiles.push_back(new char[tmpfilename.size()+2]);
	    sprintf(tmpfiles.back(),"%s",tmpfilename.c_str());
	    ofstream tmpfile(tmpfiles.back());

	    if(!strcmp(argv[i],"-x"))
	    {
		tmpfile<<argv[++i]<<endl;
	    }
	    else if(!strcmp(argv[i],"-s"))
	    {
		char *e = strchr(argv[++i],'=');
		if(e == 0)
		{
		    cerr<<"Syntax error after -s: "<<argv[i]
			<<" does not contain an equal sign"<<endl;
		    return 1;
		}
		tmpfile<<"var ";
		if(*(e+1) == '"')
		{
		    tmpfile<<argv[i]<<";"<<endl;
		}
		else
		{
		    *e = '\0';
		    tmpfile<<argv[i]<<"=\""<<e+1<<"\";"<<endl;
		}
		
	    }
	    argv_tmp[argc_tmp++] = tmpfiles.back();
	}
	else if(!strcmp(argv[i],"-v"))
	{
	    debug::on(true);
	    argv_tmp[argc_tmp++] = argv[i];
	}
	else if(!strcmp(argv[i],"-m"))
	{
	    script_mode = false;
	}
	else
	{
	    argv_tmp[argc_tmp++] = argv[i];
	}
    }
    argv_tmp[argc_tmp] = 0;


    // setting G__quiet to nonzero unfortunately removes the prompt also
    // so leave it as it is.
    // G__quiet = 1;
    int retcode = 0;

    // NOT REDIRECTED
    if(isatty(0))
    {
// 	string version = string("Blop version: ")
// 	    + BLOP_VERSION
// 	    + string(" (http://blopplot.sourceforge.net)");
// 	int version_length = version.size();
// 	int total_length=76;
// 	// number of spaces on left/right
// 	int s1 = 2;
// 	int s2 = 2;
// 	int l1 = (total_length-version_length-s1-s2)/2;
// 	int l2 = total_length-(l1+s1+version_length+s2);
// 	for(int i=0; i<l1; ++i) cout<<"-";
// 	for(int i=0; i<s1; ++i) cout<<" ";
// 	cout<<version;
// 	for(int i=0; i<s2; ++i) cout<<" ";
// 	for(int i=0; i<l2; ++i) cout<<"-";
// 	cout<<endl;

	G__setothermain(0);
	retcode = G__main(argc_tmp,argv_tmp);
    }

    // REDIRECTED
    else
    {
	if(script_mode)
	{
	    string tmpfilename = blop_bookkeeper::tmpfile("STDIN-XXXXXX");
	    tmpfiles.push_back(new char[tmpfilename.size()+2]);
	    sprintf(tmpfiles.back(),"%s",tmpfilename.c_str());
	    ofstream tmpfile(tmpfiles.back());
	    string line;
	    while(getline(cin,line))
	    {
		tmpfile<<line<<endl;
	    }
	    tmpfile.close();
	    argv_tmp[argc_tmp++] = tmpfiles.back();
	    
	    G__setothermain(0);
	    retcode = G__main(argc_tmp,argv_tmp);
	}
	else
	{
	    G__setothermain(2);
	    G__main(argc_tmp, argv_tmp);
	    string cmd;
	    while(true)
	    {
		cmd = "";
		int code = get_full_command(cin,cmd);
		cerr<<endl;
		if(code < 0 || cmd == ".q") break;
		if(code > 0)
		{
		    cerr<<"Error in the input stream:"<<endl;
		    cerr<<cmd<<endl;
		    retcode = 1;
		    break;
		}
		if(cmd != "") G__exec_text(cmd.c_str());
		if(!cin) break;
	    }
	}
    }

    delete [] argv_tmp;

    for(unsigned int i=0; i<tmpfiles.size(); ++i)
    {
	unlink(tmpfiles[i]);
	delete [] tmpfiles[i];
    }

    return retcode;
}
CATCH("main()")
