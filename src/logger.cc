#include "logger.h"
using namespace std;


namespace blop {

    logger::option logger::one_line(1);
    logger::option logger::silent(2);

    logger::format_resetter logger::reset;
    logger::format_setter logger::black("\e[30m","<span style='color:black;'>");
    logger::format_setter logger::red("\e[31m","<span style='color:red;'>");
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
        if(file_) (*file_)<<format_setters_.back().console_set_;
        if(html_file_) (*html_file_)<<format_setters_.back().html_set_;
        return *this;
    }
    logger &logger::operator<<(const format_resetter &)
    {
        cerr<<"\e[0m";
        if(file_) (*file_)<<"\e[0m";
        if(html_file_) for(unsigned int i=0; i<format_setters_.size(); ++i) (*html_file_)<<"</span>";
        format_setters_.clear();
        return *this;
    }

    unsigned int logger::level_ = 0;
    std::ofstream *logger::file_ = 0;
    std::ofstream *logger::html_file_ = 0;
    bool logger::indented_ = false;
    
//    logger::logger(const std::string &name, bool one_line, bool silent) : name_(name), one_line_(one_line), silent_(silent)
//    {
//        init_();
//    }
                                                                          
    void logger::init_()
    {
        my_level_ = ++level_;
        
        ostringstream sss;

        // Reset the format of the console
        //if(!silent_) std::cerr<<"\e[0m";
        if(!(flag_&silent.flag)) std::cerr<<"\e[0m";

        for(unsigned int i=0; i<my_level_-1; ++i) 
        {
            //if(!silent_)
            if(!(flag_&silent.flag))
            {
                std::cerr<<"   ";
                if(file_) (*file_)<<"   ";
            }
            else sss<<"   ";
        }
        //if(!one_line_)
        if(!(flag_&one_line.flag))
        {
            //if(!silent_)
            if(!(flag_&silent.flag))
            {
                std::cerr<<">> ";
                std::cerr<<name_<<" started"<<std::endl;
                if(file_)
                {
                    (*file_)<<">> ";
                    (*file_)<<name_<<" started"<<std::endl;
                }
                if(html_file_)
                {
                    (*html_file_)<<"<div class='expandable'>"<<endl;
                    (*html_file_)<<"<div class='header'>"<<name_;
                    (*html_file_)<<"<div class='duration'></div>";
                    (*html_file_)<<"<div class='expandbutton'>&#9196;&#xFE0E;</div><div class='collapsebutton'>&#9195;&#xFE0E;</div></div>"<<endl;
                    (*html_file_)<<"<div class='content'>"<<endl;
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
            //if(!silent_)
            if(!(flag_&silent.flag))
            {
                std::cerr<<name_<<"... ";
                if(file_) (*file_)<<name_<<"... ";
            }
            else sss<<"... ";
        }

        header_ = sss.str();

        start_ = clock::now();

        stack_().push_back(this);
    }

    void logger::newline()
    {
        cerr<<endl;
        if(file_) (*file_)<<endl;
        indented_ = false;
    }
    
    logger::~logger()
    {

        if(name_ != "")
        {
            auto stop = clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start_);
            
            //if(!silent_ || had_messages_)
            if(!(flag_&silent.flag) || had_messages_)
            {
                //if(!one_line_)
                if(!(flag_&one_line.flag))
                {
                    for(unsigned int i=0; i<my_level_-1; ++i)
                    {
                        std::cerr<<"   ";
                        if(file_) (*file_)<<"   ";
                    }

                    cerr<<"\e[0m";
                    if(html_file_)
                    {
                        while(!format_setters_.empty())
                        {
                            (*html_file_)<<"</span>";
                            format_setters_.pop_back();
                        }
                    }                        

                    std::cerr<<"<< ";
                    std::cerr<<name_<<" finished ("<<duration<<")"<<std::endl;
                    if(file_)
                    {
                        (*file_)<<"<< ";
                        (*file_)<<name_<<" finished ("<<duration<<")"<<std::endl;
                    }
                    if(html_file_)
                    {
                        (*html_file_)<<"</div>"<<endl;
                        (*html_file_)<<"<script>document.currentScript.closest(\".expandable\").querySelector(\".duration\").innerHTML = \""<<duration<<"\";</script>"<<endl;
                        (*html_file_)<<"<div class='footer'>"<<name_<<"</div>";
                        (*html_file_)<<"</div>"<<endl;
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

    void logger::open_html_file(const std::filesystem::path &filename)
    {
        if(html_file_) 
        {
            delete html_file_;
            html_file_ = 0;
        }
        indent();
        cerr<<"SAVING LOGS TO STRUCTURED HTML FILE: "<<filename<<endl;
        html_file_ = new std::ofstream(filename);
        if(!(*html_file_))
        {
            indent();
            cerr<<"FAILED TO OPEN HTML LOG FILE"<<endl;
        }
        (*html_file_)<<R"LIMIT(<html>
        <head>
<style>

.header, .footer, .content {
  padding-left: 2em;
  position: relative;
}


.footer {
  display: flex;
  align-items: flex-end;
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

    void logger::open_file(const std::filesystem::path &filename)
    {
        if(file_)
        {
            delete file_;
            file_ = 0;
        }
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
    void logger::close_html_file()
    {
        if(html_file_)
        {
            (*html_file_)<<R"LIMIT(

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

            (*html_file_)<<"      </body>"<<endl;
            (*html_file_)<<"</html>"<<endl;
            delete html_file_;
        }
        html_file_ = 0;
    }

    logger &logger::indent(unsigned int level)
    {
        for(unsigned int i=0; i<level; ++i)
        {
            std::cerr<<"   ";
            if(file_) (*file_)<<"   ";
        }
        return top();
    }

    logger &logger::top()
    {
        static logger the_logger;
        return the_logger;
    }

}
