#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
using namespace std;

bool help_printed = false;
void help(const char *prg)
{
    help_printed = true;
    cout<<endl;
    cout<<"Usage: "<<prg<<" file1 file2 [file3 ...]"<<endl;
    cout<<"Other options: "<<endl;
    cout<<"  -s|--separator <string>    use 'string' as the separator"<<endl;
    cout<<endl;
    cout<<prg<<" merges the files, i.e. it reads the first lines from each"<<endl;
    cout<<"file given on the command line, appends them to each other separated"<<endl;
    cout<<"by the separator given after the -s or --separator option (default"<<endl;
    cout<<"to a single space character)"<<endl;
}

int main(int argc, char *argv[])
{
    vector<ifstream *> input;
    string separator = " ";
    for(int i=1; i<argc; ++i)
    {
	if(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")) help(argv[0]);
	else if(!strcmp(argv[i],"-s") || !strcmp(argv[i],"--separator"))
	{
	    if(i+1<argc) separator = argv[++i];
	    else
	    {
		cerr<<"Error: separator expected after the -s or --separator argument"<<endl;
		exit(1);
	    }
	}
	else
	{
	    ifstream *f = new ifstream(argv[i]);
	    if(f->good()) input.push_back(f);
	    else
	    {
		cerr<<"Could not open file: "<<argv[i]<<endl;
		delete f;
	    }
	}
    }

    // no imput files were specified, print help and exit
    if(input.empty())
    {
	if(!help_printed) help(argv[0]);
	return 0;
    }

    string line_in, line_out;
    bool eof = false;
    while(!eof)
    {
	line_out = "";
	for(unsigned int i=0; i<input.size(); ++i)
	{
	    getline(*input[i],line_in);
	    if(!*input[i]) 
	    {
		eof = true;
		break;
	    }
	    line_out += line_in + separator;
	}
	cout<<line_out<<endl;
    }
    return 0;
}
