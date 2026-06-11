#ifndef __BLOP_LOGGER_H__
#define __BLOP_LOGGER_H__

#include <vector>
#include <utility>
#include <type_traits>
#include <concepts>
#include <cassert>

/*

A utility class to write hierarchical and well organized logfiles in an easy way.

Usage example:

void f(double d, int i)
{
  logger log("f(" + std::to_string(d) + "," + std::to_string(i) + ")");  // Creates an indented block in the logfile
  log<<"First log in this function"<<endl;
}

int main()
{
  logger::file       logger_file("my_log.txt");       // Until the end of the scope of this helper variable (i.e. program termination), output will also be written to the file
  logger::html_file  logger_html_file("my_log.html"); // Until the end of the scoope of this helper variable, output will also be written in a dynamic, structured html file

  logger log("My program");  // creates an indented block until the end of this variable. Send output to this at this level

  // Set the font color to red for one item (i.e. the following "First log" text), and print text. If no number is given in parenthesis,
  // the style change is permanent until the end of lifetime.
  log<<logger:red(1)<<"First log"<<endl;  

  // Note that a larger lifetime limit overrides previous ones in the following sense:
  // log<<logger:red(1)<<logger::underline(3)<<"first "<<"second "<<"third "<<"fourth ";
  // then "first", "second" and "third" will all be printed with red, underlined

  log<<"Second level"<<endl;

  {
    logger log("Some sub-block, indented");
    log<<"Indented first log"<<endl;
  }
  
}

 */

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

namespace blop{

    class logger 
    {
    public:
        class file
        {
        private:
            std::ofstream file_;
            std::filesystem::path filename_;
            void init_()
            {
                if(file_) logger::indent()<<"SAVING LOGS TO: "<<filename_<<std::endl;
                else      logger::indent()<<"FAILED TO OPEN "<<filename_<<std::endl;
            }

            bool shared_ = false;

            file(const std::string &filename, bool shared)           : file_(filename), filename_(filename) { assert(shared); init_();}
            file(const std::filesystem::path &filename, bool shared) : file_(filename), filename_(filename) { assert(shared); init_();}
            file(const char *filename, bool shared)                  : file_(filename), filename_(filename) { assert(shared); init_();}

        public:
            std::ofstream &stream() { return file_; }

            // Constructors. All of them automatically store a shared_ptr in the global logger::files_ array. 
            file(const std::string &filename)           : file_(filename), filename_(filename) { init_(); logger::add_file(std::shared_ptr<logger::file>(this,[](auto*){})); }
            file(const std::filesystem::path &filename) : file_(filename), filename_(filename) { init_(); logger::add_file(std::shared_ptr<logger::file>(this,[](auto*){})); }
            file(const char *filename)                  : file_(filename), filename_(filename) { init_(); logger::add_file(std::shared_ptr<logger::file>(this,[](auto*){})); }

            // Destructor. Only removes the referencing pointer from logger::files_ if it was not created by make_shared (i.e. it is
            // a local variable, which is deleted at the end of its scope, in which case the referencing shared_ptr's deleter function is
            // set to do nothing). If it is created by make_shared, the lifetime control is fully up to the stored shared_ptr, this object
            // is deleted when the shared_ptr is removed, and the destructor must not remove the pointer
            ~file() { if(!shared_) logger::remove_file(this); }

            // Create an unnamed instance of this file (not referenced directly by a stack variable). Set the 'created_by_make_shared_' flag, so that the destructor 
            static std::shared_ptr<logger::file> make_shared(const std::string &filename)           { auto result = std::shared_ptr<logger::file>(new logger::file(filename,true)); result->shared_ = true; return result; }
            static std::shared_ptr<logger::file> make_shared(const std::filesystem::path &filename) { auto result = std::shared_ptr<logger::file>(new logger::file(filename,true)); result->shared_ = true; return result; }
            static std::shared_ptr<logger::file> make_shared(const char *filename)                  { auto result = std::shared_ptr<logger::file>(new logger::file(filename,true)); result->shared_ = true; return result; }

            bool shared() const { return shared_; }
        };

        class html
        {
        private:
            std::ofstream file_;
            std::filesystem::path filename_;
            void init_();

            bool shared_ = false;

            html(const std::string &filename, bool shared)           : file_(filename), filename_(filename) { assert(shared); init_();}
            html(const std::filesystem::path &filename, bool shared) : file_(filename), filename_(filename) { assert(shared); init_();}
            html(const char *filename, bool shared)                  : file_(filename), filename_(filename) { assert(shared); init_();}

        public:
            std::ofstream &stream() { return file_; }

            // Constructors. All of them automatically store a shared_ptr in the global logger::files_ array. 
            html(const std::string &filename)           : file_(filename), filename_(filename) { init_(); logger::add_html(std::shared_ptr<logger::html>(this,[](auto*){})); }
            html(const std::filesystem::path &filename) : file_(filename), filename_(filename) { init_(); logger::add_html(std::shared_ptr<logger::html>(this,[](auto*){})); }
            html(const char *filename)                  : file_(filename), filename_(filename) { init_(); logger::add_html(std::shared_ptr<logger::html>(this,[](auto*){})); }

            // Destructor. Only removes the referencing pointer from logger::files_ if it was not created by make_shared (i.e. it is
            // a local variable, which is deleted at the end of its scope, in which case the referencing shared_ptr's deleter function is
            // set to do nothing). If it is created by make_shared, the lifetime control is fully up to the stored shared_ptr, this object
            // is deleted when the shared_ptr is removed, and the destructor must not remove the pointer
            ~html();

            // Create an unnamed instance of this file (not referenced directly by a stack variable). Set the 'created_by_make_shared_' flag, so that the destructor 
            static std::shared_ptr<logger::html> make_shared(const std::string &filename)           { auto result = std::shared_ptr<logger::html>(new logger::html(filename,true)); result->shared_ = true; return result; }
            static std::shared_ptr<logger::html> make_shared(const std::filesystem::path &filename) { auto result = std::shared_ptr<logger::html>(new logger::html(filename,true)); result->shared_ = true; return result; }
            static std::shared_ptr<logger::html> make_shared(const char *filename)                  { auto result = std::shared_ptr<logger::html>(new logger::html(filename,true)); result->shared_ = true; return result; }

            bool shared() const { return shared_; }
        };

        
    private:

        static std::vector<std::shared_ptr<file>> &files_();
        static std::vector<std::shared_ptr<html>> &htmls_();

        // html character codes for the "expand all" and "collapse all" buttons
        const std::string collapse_ = "&#9195;&#xFE0E;";
        const std::string expand_   = "&#9196;&#xFE0E;";

        void init_();

        static std::vector<logger*> &stack_()
            {
                static std::vector<logger*> the_stack;
                return the_stack;
            }

        std::string name_;
        unsigned int my_level_ = 0;
        static unsigned int level_;

        unsigned int flag_=0;

        // A flag indicating whether there have already been messages printed to the streams.
        bool had_messages_ = false;

        static bool indented_;

        typedef std::chrono::steady_clock clock;
        std::chrono::time_point<clock> start_;

        // A private constructor to be used by the global topmost logger instance only
        // Should this be stored in the logger stack?
        logger() : name_(""), my_level_(0) {}

        template <typename T>
        static void write_escaped(std::ostream &os, const T &t) {os<<t;}

        static void write_escaped(std::ostream &os, std::string_view s)
            {
                for(char c : s)
                {
                    switch(c)
                    {
                    case '&':  os << "&amp;";  break;
                    case '<':  os << "&lt;";   break;
                    case '>':  os << "&gt;";   break;
                    case '"':  os << "&quot;"; break;
                    case '\n': os << "<br>";   break;
                    default:   os << c;        break;
                    }
                }
            }    
        static void write_escaped(std::ostream &os, const std::string &s) { write_escaped(os,std::string_view(s)); }
        static void write_escaped(std::ostream &os, const char *s) { write_escaped(os,std::string_view(s)); }
        static void write_escaped(std::ostream &os, char c) { write_escaped(os,std::string_view(&c,1)); }

        unsigned int n_formats_ = 0;

        void apply_format();

    public:
        // Add a new file to the list of files, into which output is written
        static void add_file(std::shared_ptr<file> file) { files_().push_back(file); }
        static void add_html(std::shared_ptr<html> file) { htmls_().push_back(file); }

        // Remove the given file from the list of output files
        static void remove_file(logger::file *ptr) 
            { 
                for(int i=0; i<files_().size(); ++i)
                {
                    if(files_()[i].get()==ptr)
                    {
                        files_().erase(files_().begin()+i);
                        break;
                    }
                }

            }
        static void remove_html(logger::html *ptr) 
            { 
                for(int i=0; i<htmls_().size(); ++i)
                {
                    if(htmls_()[i].get()==ptr)
                    {
                        htmls_().erase(htmls_().begin()+i);
                        break;
                    }
                }

            }

        class format_setter
        {
        private:
            std::string console_set_, html_set_;
            friend class logger;
            int n_ = 0;
            int max_ = 0;
        public:
            format_setter(const std::string &console_set, const std::string &html_set)
                : console_set_(console_set), html_set_(html_set) {}
            format_setter operator()(int max)
                {
                    format_setter result(*this);
                    result.max_ = max;
                    return result;
                }
        };

        class format_resetter
        {
        };

    private:
        std::vector<format_setter> format_setters_;
    
    public:

        class option
        {
        public:
            unsigned int flag = 0;
            option(unsigned int f) : flag(f) {}
        };
        static option one_line, silent;
    

        static format_setter black;
        static format_setter red;
        static format_setter orange;
        static format_setter green;
        static format_setter yellow;
        static format_setter blue;
        static format_setter magenta;
        static format_setter cyan;
        static format_setter white;

        static format_setter black_bg;
        static format_setter red_bg;
        static format_setter green_bg;
        static format_setter yellow_bg;
        static format_setter blue_bg;
        static format_setter magenta_bg;
        static format_setter cyan_bg;
        static format_setter white_bg;

        static format_setter bold;
//    static format_setter italic;  // doesn't seem to work in WSL at least
        static format_setter underline;

        static format_resetter reset;

        logger &operator<<(const format_setter &f);
        logger &operator<<(const format_resetter &r);

        // A constructor with an arbitrary number and type of arguments (except logger::option), which are concatenated
        // to give the header line of this logger instance
        template<typename... Args>
        requires (sizeof...(Args) > 0 && !(std::same_as<std::decay_t<Args>, blop::logger::option> || ...))
        logger(Args&&... args)
            {
                std::ostringstream oss;
                (oss << ... << std::forward<Args>(args)); // Fold expression over operator<<
                name_ = oss.str();
                init_();
            }        

        // A constructor with an arbitrary number and type of arguments (except logger::option), which are concatenated
        // to give the header line of this logger instance, and a final logger::option argument (logger::silent, logger::one_line or
        // logger::silent|logger::one_line) which controls is behavior
        template<typename... Args>
        requires (sizeof...(Args) > 0 && !(std::same_as<std::decay_t<Args>, blop::logger::option> || ...))
        logger(logger::option opt, Args&&... args) : flag_(opt.flag)
            {
                std::ostringstream oss;
                (oss << ... << std::forward<Args>(args)); // Fold expression over operator<<
                name_ = oss.str();
                init_();
            }        

        
        ~logger();

        static void newline();

        // Open a plain text log file (it will still contain formatting ASCII commands so
        // we recommend viewing it with "less -R"
        // Send subsequent input into that file as well
        // The text file is valid and can be viewed during the process for monitoring.
        static void open_file(const std::filesystem::path &filename) { add_file(file::make_shared(filename)); }

        // Close the last output file created by file::make_shared (i.e. not a local variable on the stack)
        static void close_file();

        // Open a structured, interactive (collapsible/expandable) html output file, send
        // subsequent input into that file as well.
        // The html file is structured, and is not complete (valid) before it is fully created
        // and closed, so you can not open/view it before
        // This function (the starting of the structured html file) will badly interfere with the structured
        // log output if this function is not called before any output. So call it at the beginning of your code,
        // before any log output (this is anyway the most realistic case)
        static void open_html(const std::filesystem::path &filename) { add_html(html::make_shared(filename)); }

        static void close_html();

        // Open both a text and a html output. Provide the name without extension.
        static void open(const std::filesystem::path &basename) { open_file(basename.string() + ".txt"); open_html(basename.string() + ".html"); }
        static void close() { close_file(); close_html(); }

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
                //if(silent_ && !had_messages_)
                // If we are silent (i.e. header is not printed upon creation by the constructor) and we had
                // no messages so far (so the header was really not printed), then print the header
                if((flag_&silent.flag) && !had_messages_)
                {
                    std::cerr<<"\e[0m";  // reset the format
                    // Print the indentation
                    for(unsigned int i=0; i<my_level_-1; ++i)
                    {
                        std::cerr<<"   ";
                        for(auto f : files_()) f->stream()<<"  ";
                    }
                    // print the header
                    if(flag_&one_line.flag)
                    {
                        std::cerr<<name_<<"... ";
                        for(auto f : files_()) f->stream()<<name_<<"... ";
                    }
                    else
                    {
                        std::cerr<<">> "<<name_<<" started"<<std::endl;
                        for(auto f : files_()) f->stream()<<">> "<<name_<<" started"<<std::endl;
                    }
                    for(auto h : htmls_())
                    {
                        h->stream()<<"<div class='expandable'><div class='header'>"<<name_;
                        h->stream()<<"<div class='expandbutton'>"<<expand_<<"</div> <div class='collapsebutton'>"<<collapse_<<"</div></div>";
                        h->stream()<<"<div class='content'>"<<std::endl;
                    }
                }
                had_messages_ = true;

                if(!indented_)
                {
                    indent(my_level_);
                    indented_ = true;
                }
                std::cerr<<t;
                for(auto f : files_()) f->stream()<<t;
                for(auto h : htmls_()) write_escaped(h->stream(),t);

                // Increment the number of calls for all setters
                for(auto &f : format_setters_) ++f.n_;

                // From the top of the stack, check which format has been called the specified amount of times
                // and remove it
                bool removed_format = false;
                while(!format_setters_.empty() && format_setters_.back().max_ > 0 && format_setters_.back().n_ >= format_setters_.back().max_)
                {
                    for(auto h : htmls_()) h->stream()<<"</span>";
                    format_setters_.pop_back();
                    removed_format = true;
                }
                if(removed_format)
                {
                    std::cerr<<"\e[0m";  // Reset
                    for(const auto &f : format_setters_) std::cerr<<f.console_set_;  // reapply the remaining formats
                }
                return *this;
            }

        logger &operator<<(std::ostream& (*manip)(std::ostream&)) 
            {
                if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) 
                {
                    indented_ = false;
                    for(auto h : htmls_()) h->stream()<<"<br>"<<std::endl;
                } 
                std::cerr<<manip;
                for(auto f : files_()) f->stream()<<manip;
                return *this;
            }    

        static void print() {}
    
        // Recursive case
        template <typename T, typename... Args>
        static void print(T first, Args... rest) 
            {
                std::cerr<<first;
                for(auto f : files_()) f->stream()<<first;
                print(rest...);
            }    

        // Utility class to redirect the logger's output to a given file (as well), and automatically close this file
        // when its scope exists. 
        // Use it within a code block as follows:
        // {
        //   logger::file logger_redirector("my-log.txt");
        //   ... your code comes here, with log redirected into the file (besides being shown on the terminal)
        // } // local variable goes out of scope and gets deleted, closing the output files

        /*
        class file
        {
        public:
            file(const std::filesystem::path &filename) {logger::open_file(filename);}
            ~file() {logger::close_file();}
        };
        */
/*
        class html
        {
        public:
            html(const std::filesystem::path &filename) {logger::open_html(filename);}
            ~html() {logger::close_html();}
        };
*/
    };

    // bitwise OR operator to concatenate several options
    inline logger::option operator| (logger::option o1, logger::option o2)
    {
        return o1.flag|o2.flag;
    }

}


#endif
