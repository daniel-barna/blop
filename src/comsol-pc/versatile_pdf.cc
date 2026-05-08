#include "versatile_pdf.h"
#include "bloputils.h"
#include <unistd.h>

namespace blop
{

    void versatile_pdf::assemble_pages(const std::vector<std::string> &filenames)
    {
        std::string cmd = "pdftk ";
        for(unsigned int i=0; i<filenames.size(); ++i) cmd += filenames[i] + " ";
        cmd += "cat output " + outfilename_;
        if(system(cmd) != 0)
        {
            warning::print("Failed to assemble versatile_pdf: " + outfilename_,
                           "versatile_pdf::assemble_pages(...)");
        }
    }

    versatile_pdf::~versatile_pdf()
    {
        if(needs_flush_) assemble_pages(page_filenames_);
    }

    void versatile_pdf::include(const var &fn, const var &convert_options)
    {
        string filename(fn);
        if(filename[0] != '/')
        {
            char buf[1000];
            char *cwd = getcwd(buf,999);
            if(cwd) filename = string(cwd) + "/" + filename;
        }

        ifstream test(filename.c_str());
        if(!test.good()) 
        {
            warning::print("File does not exist: " & filename,
                           "versatile_pdf::include_pdf(filename)");
            return;
        }

        if(filename.rfind(".pdf") == filename.size()-4 ||
           filename.rfind(".PDF") == filename.size()-4)
        {
            ++pagecounter_;
            page_filenames_.push_back(filename);
        }
        else
        {
            char tmpfilename[200];
            sprintf(tmpfilename,"%s/page_%i.pdf",tmpdir_.c_str(),pagecounter_++);
            string cmd = string("convert ") + convert_options.str() + " '" + filename + "' " + tmpfilename;
            if(system(cmd)==0)
            {
                page_filenames_.push_back(tmpfilename);
            }
            else
            {
                warning::print("Conversion failed: " + cmd,
                               "versatile_pdf::include(\"" + filename + "\")");
            }
        }
        if(pagecounter_%flush_frequency_==0)
        {
            assemble_pages(page_filenames_);
            needs_flush_ = false;
        }
        else needs_flush_ = true;
    }
}
