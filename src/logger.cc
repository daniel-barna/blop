#include "logger.h"
using namespace std;


namespace blop {

unsigned int logger::level_ = 0;
std::ofstream *logger::file_ = 0;
bool logger::indented_ = false;

logger::logger(const std::string &name, bool one_line, bool silent) : name_(name), one_line_(one_line), silent_(silent)
{
    my_level_ = ++level_;


    ostringstream sss;
    for(unsigned int i=0; i<my_level_-1; ++i) 
    {
        if(!silent_)
        {
            std::cerr<<"   ";
            if(file_) (*file_)<<"   ";
        }
        else sss<<"   ";
    }
    if(!one_line_)
    {
        if(!silent_)
        {
            std::cerr<<">> ";
            std::cerr<<name_<<" started"<<std::endl;
            if(file_)
            {
                (*file_)<<">> ";
                (*file_)<<name_<<" started"<<std::endl;
            }
        }
        else
        {
            sss<<">> ";
            sss<<name_<<" started";
        }
    }
    else
    {
        if(!silent)
        {
            std::cerr<<name_<<"... ";
            if(file_) (*file_)<<name_<<"... ";
        }
        else sss<<"... ";
    }

    header_ = sss.str();

    start_ = clock::now();
}

void logger::newline()
{
    cerr<<endl;
    if(file_) (*file_)<<endl;
    indented_ = false;
}

logger::~logger()
{
    auto stop = clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start_);

    if(!silent_ || had_messages_)
    {
        if(!one_line_)
        {
            for(unsigned int i=0; i<my_level_-1; ++i)
            {
                std::cerr<<"   ";
                if(file_) (*file_)<<"   ";
            }
            std::cerr<<"<< ";
            std::cerr<<name_<<" finished ("<<duration<<")"<<std::endl;
            if(file_)
            {
                (*file_)<<"<< ";
                (*file_)<<name_<<" finished ("<<duration<<")"<<std::endl;
            }
        }
        else 
        {
            std::cerr<<"done ("<<duration<<")"<<endl;
            if(file_) (*file_)<<"done ("<<duration<<")"<<endl;
        }
    }
    --level_;
}

void logger::open_file(const std::filesystem::path &filename)
{
    if(file_) delete file_;
    indent();
    cerr<<"SAVING LOGS TO: "<<filename<<endl;
    file_ = new std::ofstream(filename);
    if(!(*file_))
    {
        indent();
        cerr<<"FAILED TO OPEN LOG FILE"<<endl;
    }
}
void logger::close_file()
{
    delete file_;
    file_ = 0;
}
std::ostream &logger::indent()
{
    for(unsigned int i=0; i<level_; ++i)
    {
        std::cerr<<"   ";
        if(file_) (*file_)<<"   ";
    }
    return std::cerr;
}

}
