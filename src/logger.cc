#include "logger.h"
using namespace std;


namespace blop {

    unsigned int logger::level_ = 0;
    std::ofstream *logger::file_ = 0;
    std::ofstream *logger::html_file_ = 0;
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
                if(html_file_)
                {
                    (*html_file_)<<"<div class='expandable'>"<<endl;
                    (*html_file_)<<"<div class='header'>"<<name_;
                    (*html_file_)<<R"LIMIT(
    <div class='expandbutton'>[Expand all]</div>
    <div class='collapsebutton'>[Collapse all]</div>)LIMIT";
                    (*html_file_)<<"</div>"<<endl;
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
        if(name_ != "")
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
                    if(html_file_)
                    {
                        (*html_file_)<<"</div>"<<endl;
                        (*html_file_)<<"<script>document.currentScript.parentElement.querySelector(\".header\").insertAdjacentText(\"beforeend\",\" ("<<duration<<")\");</script>"<<endl;
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
        (*html_file_)<<R"(<html>
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
  content: "▶";
  position: absolute;
  left: 0;
  top: 0;
  width: 1em;
  text-align: center;
  transform: translateY(-0.1em);
}

.expandable.open > .header::before {
  content: "▼";
  position: absolute;
  left: 0;
  top: 0;
  width: 1em;
  text-align: center;
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
}

</style>      
      </head>
      <body>)"<<endl;
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
