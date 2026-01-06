#include "multipage_terminal.h"
#include "bloputils.h"

namespace blop
{

    void multipage_pdf::assemble_pages(const std::vector<std::string> &filenames)
    {
        std::string cmd = "pdftk ";
        for(unsigned int i=0; i<filenames.size(); ++i) cmd += filenames[i] + " ";
        cmd += "cat output " + outfilename_;
        system(cmd);
    }

    multipage_pdf::~multipage_pdf()
    {
        if(needs_flush_) assemble_pages(page_filenames_);
    }
}
