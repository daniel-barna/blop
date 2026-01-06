#ifndef __BLOP_MPPDF_H__
#define __BLOP_MPPDF_H__

#include "mpps.h"
#include "blop_bookkeeper.h"

namespace blop
{
    class mppdf : public mpps
    {
    private:
        string filename_;
    public:
        mppdf() {}
        mppdf(const var &filename);
        ~mppdf();

        virtual void open(const var &filename);
        virtual bool flush();
    };
}


#endif
