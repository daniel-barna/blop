#include "logger.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;


namespace blop {

    std::vector<std::shared_ptr<logger::file>> &logger::files_()
    {
        static std::vector<std::shared_ptr<logger::file>> f;
        return f;
    }
    std::vector<std::shared_ptr<logger::html>> &logger::htmls_()
    {
        static std::vector<std::shared_ptr<logger::html>> f;
        return f;
    }


    template<typename Rep, typename Period>
    std::string format_duration(std::chrono::duration<Rep, Period> d)
    {
        using namespace std::chrono;

        double seconds = duration<double>(d).count();

        std::ostringstream oss;

        if (seconds < 1.0) oss << std::round(seconds * 1000.0) << " ms";
        else if (seconds < 10) oss << std::fixed << std::setprecision(2) << seconds << " s";
        else if (seconds < 30) oss << std::fixed << std::setprecision(1) << seconds << " s";
        else oss << std::round(seconds) << " s";

        return oss.str();
    }

    logger::option logger::one_line(1);
    logger::option logger::silent(2);

    logger::format_resetter logger::reset;
    logger::format_setter logger::black("\e[30m","<span style='color:black;'>");
    logger::format_setter logger::red("\e[31m","<span style='color:red;'>");
    logger::format_setter logger::orange("\033[38;5;208m","<span style='color:orange;'>");
    logger::format_setter logger::green("\e[32m","<span style='color:green;'>");
    logger::format_setter logger::yellow("\e[33m","<span style='color:yellow;'>");
    logger::format_setter logger::blue("\e[34m","<span style='color:blue;'>");
    logger::format_setter logger::magenta("\e[35m","<span style='color:magenta;'>");
    logger::format_setter logger::cyan("\e[36m","<span style='color:cyan;'>");
    logger::format_setter logger::white("\e[37m","<span style='color:white;'>");

    logger::format_setter logger::black_bg("\e[40m","<span style='background-color:black;'>");
    logger::format_setter logger::red_bg("\e[41m","<span style='background-color:red;'>");
    logger::format_setter logger::green_bg("\e[42m","<span style='background-color:green;'>");
    logger::format_setter logger::yellow_bg("\e[43m","<span style='background-color:yellow;'>");
    logger::format_setter logger::blue_bg("\e[44m","<span style='background-color:blue;'>");
    logger::format_setter logger::magenta_bg("\e[45m","<span style='background-color:magenta;'>");
    logger::format_setter logger::cyan_bg("\e[46m","<span style='background-color:cyan;'>");
    logger::format_setter logger::white_bg("\e[47m","<span style='background-color:white;'>");

    logger::format_setter logger::bold("\e[1m","<span style='font-weight:bold;'>");
//    logger::format_setter logger::italic("\e[3m","<span style='font-style:italic;'>");
    logger::format_setter logger::underline("\e[4m","<span style='text-decoration:underline;'>");

    logger &logger::operator<<(const format_setter &f)
    {
        format_setters_.push_back(f);
        format_setters_.back().n_ = 0;
        cerr<<format_setters_.back().console_set_;
        for(auto f : files_()) f->stream()<<format_setters_.back().console_set_;
        for(auto h : htmls_()) h->stream()<<format_setters_.back().html_set_;
        return *this;
    }
    logger &logger::operator<<(const format_resetter &)
    {
        cerr<<"\e[0m";
        for(auto f : files_()) f->stream()<<"\e[0m";
        for(auto h : htmls_()) for(unsigned int i=0; i<format_setters_.size(); ++i) h->stream()<<"</span>";
        format_setters_.clear();
        return *this;
    }

    unsigned int logger::level_ = 0;
    bool logger::indented_ = false;
    
                                                                          
    void logger::init_()
    {
        my_level_ = ++level_;
        
        // Reset the format of the console
        if(!(flag_&silent.flag)) std::cerr<<"\e[0m";

        for(unsigned int i=0; i<my_level_-1; ++i) 
        {
            if(!(flag_&silent.flag))
            {
                std::cerr<<"   ";
                for(auto f : files_()) f->stream()<<"  ";
            }
        }

        if(!(flag_&one_line.flag))  // If not a one-line message
        {
            if(!(flag_&silent.flag))
            {
                std::cerr<<">> ";
                std::cerr<<name_<<" started"<<std::endl;
                for(auto f : files_())
                {
                    f->stream()<<">> ";
                    f->stream()<<name_<<" started"<<std::endl;
                }
                for(auto h : htmls_())
                {
                    h->stream()<<"<div class='expandable'>"<<endl;
                    h->stream()<<"<div class='header'>"<<name_;
                    h->stream()<<"<div class='duration'></div>";
                    h->stream()<<"<div class='expandbutton'>"<<expand_<<"</div><div class='collapsebutton'>"<<collapse_<<"</div></div>"<<endl;
                    h->stream()<<"<div class='content'>"<<endl;
                }
            }
        }
        else  // one-line message
        {
            if(!(flag_&silent.flag))
            {
                std::cerr<<name_<<"... ";
                for(auto f : files_()) f->stream()<<name_<<"... ";
            }
        }

        start_ = clock::now();

        stack_().push_back(this);
    }

    void logger::newline()
    {
        cerr<<endl;
        for(auto f : files_()) f->stream()<<endl;
        indented_ = false;
    }
    
    logger::~logger()
    {

        if(name_ != "")
        {
            auto stop = clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start_);
            
            if(!(flag_&silent.flag) || had_messages_)
            {
                if(!(flag_&one_line.flag))
                {
                    for(unsigned int i=0; i<my_level_-1; ++i)
                    {
                        std::cerr<<"   ";
                        for(auto f : files_()) f->stream()<<"  ";
                    }

                    cerr<<"\e[0m";

                    for(auto h : htmls_())
                    {
                        while(!format_setters_.empty())
                        {
                            h->stream()<<"</span>";
                            format_setters_.pop_back();
                        }
                    }                        

                    std::cerr<<"<< ";
                    std::cerr<<name_<<" finished ("<<format_duration(duration)<<")"<<std::endl;

                    for(auto f : files_())
                    {
                        f->stream()<<"<< ";
                        f->stream()<<name_<<" finished ("<<format_duration(duration)<<")"<<std::endl;
                    }

                    for(auto h : htmls_())
                    {
                        h->stream()<<"</div>"<<endl;
                        h->stream()<<"<script>document.currentScript.closest(\".expandable\").querySelector(\".duration\").innerHTML = \""<<format_duration(duration)<<"\";</script>"<<endl;
                        h->stream()<<"<div class='footer'>"<<name_<<"</div>";
                        h->stream()<<"</div>"<<endl;
                    }
                }
                else 
                {
                    std::cerr<<"done ("<<format_duration(duration)<<")"<<endl;
                    for(auto f : files_()) f->stream()<<"done ("<<format_duration(duration)<<")"<<endl;
                }
            }
            --level_;
        }
        if(!stack_().empty() && stack_().back() == this)
        {
            stack_().pop_back();
            std::cerr<<"\e[0m";  // Reset the format
        }
        // Reapply the format of the previous log level
        if(!stack_().empty()) stack_().back()->apply_format();
    }

    void logger::apply_format() 
    { 
        for(const auto &f : format_setters_)
        {
            std::cerr<<f.console_set_;
        }
    }

    void logger::html::init_()
    {
        if(file_) 
        {
            logger::indent()<<"SAVING LOGS TO: "<<filename_<<std::endl;
            file_<<R"LIMIT(<html>
        <head>
<style>

.header, .footer, .content {
  padding-left: 2em;
  position: relative;
}


.footer {
  display: flex;
  align-items: flex-end;
  opacity: 0.5;
}

.expandable > .header::before {
  content: "▼";
  //  content: "▶";
  position: absolute;
  left: 0;
  top: 0.2em;
  width: 1em;
  text-align: center;
  transform: rotate(-90deg);
}

.expandable.open > .header::before {
  content: "▼";
  position: absolute;
  left: 0;
  top: 0.15em;
  width: 1em;
  text-align: center;
  transform: rotate(0deg);
}

.footer::before {
  content: "▲ ";
  position: absolute;
  left: 0;
  bottom: 0;
  width: 1em;
  text-align: center;
}

.expandable {
    position: relative;
    color: black;
    background-color: white;
    font-weight: normal;
    text-decoration: none;
}

.expandable.open::before {
    content: "";

    position: absolute;

    /* center of triangle column */
    left: 0.5em;

    /* start below top triangle */
    top: 1.1em;

    /* end above bottom triangle */
    bottom: 1.1em;

    width: 1px;
    background: #888;
}

.expandable > .content, .expandable > .footer {
  display: none;
}

.expandable.open > .content, .expandable.open > .footer {
  display: block;
}

.header, .footer {
  cursor: pointer;
}

.expandbutton, .collapsebutton {
    color: blue;
    display: inline;
    margin: 0 2pt 0 2pt;
}
.expandbutton:hover, .collapsebutton:hover {
    color: red;
}

.duration {
    color: green;
    display: inline;
}
.duration::before {
    content: " (";
}
.duration::after {
    content: ")";
}

body {
    font-family: Consolas;
}

</style>      
      </head>
      <body>)LIMIT"<<endl;

            
        }
        else      logger::indent()<<"FAILED TO OPEN "<<filename_<<std::endl;
        
    }
    

    void logger::close_file()
    {
        for(int i=files_().size()-1; i>=0; --i)
        {
            if(files_()[i]->shared())
            {
                // files marked as 'shared' have been created by std::make_shared, and their lifetime is
                // controlled by this shared_ptr. So simply erasing this shared_ptr will automatically
                // close the file
                files_().erase(files_().begin()+i);
                break;
            }
        }
    }
    void logger::close_html()
    {
        for(int i=htmls_().size()-1; i>=0; --i)
        {
            if(htmls_()[i]->shared())
            {
                // files marked as 'shared' have been created by std::make_shared, and their lifetime is
                // controlled by this shared_ptr. So simply erasing this shared_ptr will automatically
                // close the file
                htmls_().erase(htmls_().begin()+i);
                break;
            }
        }
    }

    logger::html::~html()
    {
        file_<<R"LIMIT(

<script>
  document.addEventListener("DOMContentLoaded", () => {
      document.querySelectorAll(".header").forEach(el=>{
          el.addEventListener("click",event => {
              event.currentTarget.parentElement.classList.toggle("open");
          });
      });
      document.querySelectorAll(".footer").forEach(el=>{
          el.addEventListener("click",event => {
              event.currentTarget.parentElement.classList.toggle("open");
          });
      });
      document.querySelectorAll(".expandbutton").forEach(el=>{
          el.addEventListener("click",event => {
              o =event.currentTarget;
              o.parentElement.parentElement.classList.add("open");
              o.parentElement.parentElement.querySelectorAll(".expandable").forEach(a => {
                  a.classList.add("open");
              });
              event.stopPropagation();
          });
      });
      document.querySelectorAll(".collapsebutton").forEach(el=>{
          el.addEventListener("click",event => {
              o =event.currentTarget;
              o.parentElement.parentElement.classList.remove("open");
              o.parentElement.parentElement.querySelectorAll(".expandable").forEach(a => {
                  a.classList.remove("open");
              });
              event.stopPropagation();
          });
      });
  });
</script>
)LIMIT"<<endl;

        file_<<"      </body>"<<endl;
        file_<<"</html>"<<endl;
        
        if(!shared_) logger::remove_html(this); 
    }

    logger &logger::indent(unsigned int level)
    {
        for(unsigned int i=0; i<level; ++i)
        {
            std::cerr<<"   ";
            for(auto f : files_()) f->stream()<<"  ";
        }
        return top();
    }

    logger &logger::top()
    {
        static logger the_logger;
        return the_logger;
    }

}
