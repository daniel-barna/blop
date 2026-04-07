#ifndef __BLOP_LOGGER_H__
#define __BLOP_LOGGER_H__

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

namespace blop{

class logger 
{
private:
    std::string name_;
    unsigned int my_level_ = 0;
    static unsigned int level_;
    bool one_line_ = false;

    bool silent_ = false;
    std::string header_;
    bool had_messages_ = false;

    static bool indented_;
    static std::ofstream *file_;

    typedef std::chrono::steady_clock clock;
    std::chrono::time_point<clock> start_;

public:
    logger(const std::string &name, bool one_line=false, bool silent=false);
    ~logger();

    static void newline();

    static void open_file(const std::filesystem::path &filename);
    static void close_file();
    static std::ostream &indent();

    template <typename T>
    logger &operator<<(const T &t)
    {
        if(silent_ && !had_messages_)
        {
            std::cerr<<header_<<std::endl;
            if(file_) (*file_)<<header_<<std::endl;
        }
        had_messages_ = true;

        if(!indented_)
        {
            indent();
            indented_ = true;
        }
        std::cerr<<t;
        if(file_) (*file_)<<t;
        return *this;
    }

    logger &operator<<(std::ostream& (*manip)(std::ostream&)) 
    {
        if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) 
        {
            indented_ = false;
        } 
        std::cerr<<manip;
        if(file_) (*file_)<<manip;
        return *this;
    }    

    static void print() {}
    
    // Recursive case
    template <typename T, typename... Args>
    static void print(T first, Args... rest) 
    {
        std::cerr<<first;
        if(file_) (*file_)<<first;
        print(rest...);
    }    

    // Utility class to redirect the logger's output to a given file (as well), and automatically close this file
    // when its scope exists. 
    class file
    {
    public:
        file(const std::filesystem::path &filename)
            {
                logger::open_file(filename);
            }
        ~file()
            {
                logger::close_file();
            }
    };

};

}


#endif
