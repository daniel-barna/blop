#ifndef __BLOP_TIC_H__
#define __BLOP_TIC_H__

#include <string>
#include "var.h"
#include "function.h"
#include "constants.h"

namespace blop
{


    class tic
	{
	private:
	    double value_;
	    std::string label_;
	public:
	    tic(double v=0,const var &s="")
		: value_(v), label_(s.str()) {}

	    tic(const tic &);
	    bool operator== (const tic &) const;
	    bool operator< (const tic &) const {return false;}
	    bool operator> (const tic &) const {return false;}

	    tic &label(const var &s) { label_ = s.str(); return *this;}
	    var label() const {return label_;}
	    
	    tic &value(double v) {value_ = v; return *this;}
	    double value() const {return value_;}
	};



// This function determines an optimal scale parameters for a datarange.
// Note that in the case of logscale the value 'minimum' and 'maximum' 
// has to be greater than zero. The value 'step' has to be greater than 
// zero if fixed. The axis cut-out ranges can be specified arbitrarily 
// provided that the cut-out domains do not overlap.
// The function prepares a tic vector 'tics', with number of tics between 
// 'nmintics' and '2*nmintics'.
void calculate_tics(double &minimum,
		    const bool minfixed, 
		    double &maximum,
		    const bool maxfixed, 
		    double &step,
		    const bool stepfixed,
		    double tic_min,       // specify a restricted range for the tics
		    double tic_max,
		    blop::tic &scale,
		    double unit, 
		    const std::vector< std::pair<double,double> > &cuts, 
		    const bool logscale, 
		    const int normalform_tics,  // 0 --> no normal (with powers of ten) form. 1 --> normal form. 2 --> if range is beyond 0.1..10, use normalform, otherwise no normal form
		    const bool normalform_scale, 
		    std::vector<blop::tic> &tics, 
		    const char *tic_format="%g",
                    const blop::function &tic_format_func=unset,
		    const char *scale_format="%g",
		    const int nmintics=5,
                    bool symmetric_range=false);
    
}
#endif
