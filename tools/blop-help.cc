#include "../src/help.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
using namespace std;
using namespace blop;

int main(int argc, char *argv[])
{
    if(argc<2)
    {
	cerr<<"Usage: blop-help <topic>"<<endl;
	return 1;
    }
    char hostname[100];
    gethostname(hostname,99);
    string user = getenv("USER");
    string tmpfilename = "/tmp/blop-help-" + string(hostname) + "-" + user + ".html";
    help(argv[1],tmpfilename);
    return 0;
}
