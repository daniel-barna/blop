#include "discretizer.h"
#include <algorithm>
#include <iostream>

using namespace std;

namespace blop
{
    discretizer::discretizer(double diff, const dgraph &g, const blop::function &f) : diff_(diff)
    {
        for(unsigned int i=0; i<g.size(); ++i) proc(f.eval(g[i]).dbl());
    }
    discretizer::discretizer(double diff, const std::vector<double> &vec)
    {
        for(auto v : vec) proc(v);
    }


    void discretizer::proc(double val)
    {
        const auto pos = std::lower_bound(maxima_.begin(),maxima_.end(),val-diff_);
        const int  i = pos-maxima_.begin();
        if(pos!=maxima_.end() && minima_[i]-diff_ <= val)
        {
            sums_[i] += val;
            if(val<minima_[i]) minima_[i] = val;
            if(val>maxima_[i]) maxima_[i] = val;
            ++n_[i];
            values_[i] = sums_[i]/n_[i];
        }
        else
        {
            sums_.insert(sums_.begin()+i,val);
            minima_.insert(minima_.begin()+i,val);
            maxima_.insert(maxima_.begin()+i,val);
            n_.insert(n_.begin()+i,1);
            values_.insert(values_.begin()+i,val);
        }
    }

    double discretizer::value(double v)
    {
        if(minima_.empty()) return 0;
        if(v<maxima_.front() || values_.size()==1) return values_.front();
        if(v>minima_.back() ) return values_.back();
        for(unsigned int i=1; i<values_.size(); ++i)
        {
            if(v<(maxima_[i-1]+minima_[i])/2) return values_[i-1];
        }
        return values_.back();
    }
}
