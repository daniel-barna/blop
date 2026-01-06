#ifndef __BLOP_MULTIPAGE_TERMINAL_H__
#define __BLOP_MULTIPAGE_TERMINAL_H__

#include "terminal.h"
#include "pdf.h"
#include "blop_bookkeeper.h"

/*
  multipage_terminal is a base class for terminals which can be
  constructed from several individual pages, and which is refreshed
  (i.e. assembled from its individual pages) regularly during its
  creation. It is useful if a very long procedure is creating the
  pages, and you want to monitor the output during the
  process,i.e. not wait until the whole program terminates.

 */

namespace blop
{
    template <typename T>
    class multipage_terminal
    {
    protected:
        std::string outfilename_;
        bool        needs_flush_;
        std::vector<std::string> page_filenames_;
        std::string print_option_;
    private:
        std::string tmpdir_;
        int pagecounter_;
        int flush_frequency_;
    public:
        multipage_terminal(const var &filename)
            : outfilename_(filename.str()), needs_flush_(false), tmpdir_(blop_bookkeeper::tmpdir("multipage_terminal_XXXXXX")), pagecounter_(0), flush_frequency_(1)
        {}
        virtual ~multipage_terminal() {}

        virtual void assemble_pages(const std::vector<std::string> &filenames) = 0;
        virtual void print_option(const var &opt) { print_option_ = opt.str(); }
        virtual void print(var opt="")
        {
            if(opt.str() == "") opt = print_option_;
            char filename[300];
            sprintf(filename,"%s/page_%i",tmpdir_.c_str(),pagecounter_++);
            if(opt.str() == "") T::print(filename);
            else T::print(filename,opt);
            page_filenames_.push_back(filename);
            if(pagecounter_%flush_frequency_==0)
            {
                assemble_pages(page_filenames_);
                needs_flush_ = false;
            }
            else needs_flush_ = true;
        }
    };

    class multipage_pdf : public multipage_terminal<pdf>
    {
    public:
        multipage_pdf(const var &filename) : multipage_terminal<pdf>(filename) {}
        ~multipage_pdf();
        void assemble_pages(const std::vector<std::string> &filenames);
    };

}

#endif
