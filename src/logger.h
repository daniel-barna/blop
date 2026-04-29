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

    // Print the start/end messages on a single line
    bool one_line_ = false;

    // If true, do not print the entry header line (">> XXX started") until some messages
    // are printed. If no messages are printed to this logger during its lifetime, it does not
    // produce any output
    bool silent_ = false;

    // The header (start) line is saved into this variable (rather than being printed to the output directly)
    // in case the silent flag is true. It is then only printed to the output streams at the time of the first
    // message is printed to this logger by the user
    std::string header_;

    // A flag indicating whether there have already been messages printed to the streams.
    bool had_messages_ = false;

    static bool indented_;
    static std::ofstream *file_;

    typedef std::chrono::steady_clock clock;
    std::chrono::time_point<clock> start_;

    // A private constructor to be used by the global topmost logger instance only
    logger() : name_(""), one_line_(false), silent_(false), my_level_(0) {}

public:
    logger(const std::string &name, bool one_line=false, bool silent=false);
    ~logger();

    static void newline();

    static void open_file(const std::filesystem::path &filename);
    static void close_file();

    // Print indentation to the streams corresponding to the actual indentation level ,
    // and return a reference to the global topmost logger instance so that subsequent << operations appear indented
    // This can be used to emit messages at the actual indentation level without having a local logger instance
    // logger::indent()<<"This text appears indented at the actual level"<<endl;
    static logger &indent(unsigned int level=level_);

    // Return reference to the global topmost logger instance. Following operations by << appear unindented:
    // logger::top()<<"Unindented text starting at the beginning of the line"<<endl;
    static logger &top(); 

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
            indent(my_level_);
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
