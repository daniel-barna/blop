#ifndef __BLOP_DISCRETIZER_H__
#define __BLOP_DISCRETIZER_H__

#include <vector>
#include "dgraph.h"

namespace blop
{
    class discretizer
    {
    private:
        std::vector<double> sums_, minima_, maxima_, values_;
        std::vector<int> n_;
        double diff_;
    public:
        discretizer(double diff) : diff_(diff) {}
        discretizer(double diff, const dgraph &g, const blop::function &f);
        discretizer(double diff, const std::vector<double> &v);

        void proc(double a);

        const std::vector<double> &sums() const { return sums_; }
        const std::vector<double> &minima() const { return minima_; }
        const std::vector<double> &maxima() const { return maxima_; }
        const std::vector<double> &values() const { return values_; }
        const std::vector<int>    &n()   const { return n_;   }

        // Return the discretized value. 
        double value(double v);

        void reset() 
        {
            values_.clear();
            sums_.clear();
            minima_.clear();
            maxima_.clear();
            n_.clear();
        }
    };
}

#endif
