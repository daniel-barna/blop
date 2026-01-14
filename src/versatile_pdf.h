#ifndef __BLOP_VERSATILE_PDF_H__
#define __BLOP_VERSATILE_PDF_H__

#include "terminal.h"
#include "pdf.h"
#include "jpg.h"
#include "blop_bookkeeper.h"
#include "bloputils.h"
#include "var.h"

namespace blop
{
    class versatile_pdf
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
        versatile_pdf(const var &filename)
            : outfilename_(filename.str()), needs_flush_(false), tmpdir_(blop_bookkeeper::tmpdir("versatile_pdf_XXXXXX")), pagecounter_(0), flush_frequency_(1)
        {}
        versatile_pdf(const std::filesystem::path &filename)
            : outfilename_(filename.c_str()), needs_flush_(false), tmpdir_(blop_bookkeeper::tmpdir("versatile_pdf_XXXXXX")), pagecounter_(0), flush_frequency_(1)
        {}

        virtual ~versatile_pdf();

        virtual void assemble_pages(const std::vector<std::string> &filenames);
        virtual void print_option(const var &opt) { print_option_ = opt.str(); }

        virtual void include(const var &filename, const var &convert_options="");

        // Print the current canvas into the pdf file, at the specified page count 'at_page'.
        // For example at_page=1 means insert it as the first page.
        // If this is the default 0, the printout is appended to the pdf file
        template <typename T =pdf>
        void print(var opt="",int at_page=0, bool replace=false)
        {
            if(opt.str() == "") opt = print_option_;
            char filename[300];

            {
                string fmt = string("%s/page_%i") + T::filename_extension();
                sprintf(filename,fmt.c_str(),tmpdir_.c_str(),pagecounter_++);
            }

            if(opt.str() == "") T::print(filename);
            else T::print(filename,opt);

            string filename_to_be_stored;
            if(T::filename_extension() == ".pdf")
            {
                //page_filenames_.push_back(filename);
                filename_to_be_stored = filename;
            }
            else
            {
                string pdf_filename = blop::replace(var(T::filename_extension()), var(pdf::filename_extension()), var(filename)).str();
                string cmd = std::string("convert ") + std::string(filename) + " " + pdf_filename;
                if(system(cmd) == 0)
                {
                    filename_to_be_stored = pdf_filename;
                    //page_filenames_.push_back(pdf_filename);
                }
                else
                {
                    warning::print(string("Conversion failed: ") + cmd,
                                   "versatile_pdf::print(...)");
                                   
                }
            }
            if(at_page<=0) page_filenames_.push_back(filename_to_be_stored);
            else
            {
                if(--at_page >= (int)(page_filenames_.size()))
                {
                    page_filenames_.push_back(filename_to_be_stored);
                }
                else
                {
                    if(replace) page_filenames_[at_page] = filename_to_be_stored;
                    else page_filenames_.insert(page_filenames_.begin()+at_page,filename_to_be_stored);
                }
            }
            if(pagecounter_%flush_frequency_==0)
            {
                assemble_pages(page_filenames_);
                needs_flush_ = false;
            }
            else needs_flush_ = true;
        }

    };


}

#endif
