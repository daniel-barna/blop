#include "mppdf.h"
#include "bloputils.h"

namespace blop
{
    mppdf::mppdf(const var &filename) : mpps(blop_bookkeeper::tmpfile("MPPDF_XXXXXX.ps")), filename_(filename)
    {
    }

    mppdf::~mppdf()
    {
        mppdf::flush();
    }

    void mppdf::open(const var &filename)
    {
        filename_ = filename.str();
        mpps::open(blop_bookkeeper::tmpfile("MPPDF_XXXXXX.ps"));
    }

    bool mppdf::flush()
    {
        if(mpps::flush())
        {
            string cmd = string("ps2pdf ") + mpps::filename() + " \"" + replace(var("\""),var("\\\""),var(filename_)).str() + "\"";
            system(cmd);
            return true;
        }
        return false;
    }
}
