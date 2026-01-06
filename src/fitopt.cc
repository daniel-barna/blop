#include "fitopt.h"
#include <cmath>

using namespace std;
namespace blop
{
    int default_fit_convergence(double old_chi2, double chi2,
				const std::vector<blop::var> &oldpars,
				const std::vector<blop::var> &newpars,
				int nsteps)
    {
	if(chi2<old_chi2 && ::fabs(old_chi2-chi2)/chi2<1e-4) return 1;
	if(chi2 == 0) return 1;
	return 0;
    }

    function fitopt::default_x_ = ARG(1);
    function fitopt::default_y_ = ARG(2);
    function fitopt::default_sigma_x_;
    function fitopt::default_sigma_y_;
    int       fitopt::default_verbose_ = 1;
    int       fitopt::default_maxsteps_ = 0;

    fitopt::cfuncptr fitopt::default_convergence_ = default_fit_convergence;
    void *fitopt::default_convergence_void_ = 0;

    fitopt::fitopt()
    {
        block_ = 0;
	x_ = default_x_;
	y_ = default_y_;
	sigma_x_ = default_sigma_x_;
	sigma_y_ = default_sigma_y_;
	verbose_ = default_verbose_;
	maxsteps_ = default_maxsteps_;
	convergence_ = default_convergence_; 
	convergence_void_ = default_convergence_void_;
	x_condition_ = default_x_condition_;
	y_condition_ = default_y_condition_;
	condition_   = default_condition_;
    }

    void fitopt::default_x(const function &f)
        { default_x_ = f; }
    void fitopt::default_x(const function &f1, const function &f2)
         { default_x_ = function(f1,f2); }
    void fitopt::default_x(const function &f1, const function &f2, const function &f3)
         { default_x_ = function(f1,f2,f3); }

    void fitopt::default_y(const function &f)
         { default_y_ = f; }
    void fitopt::default_y(const function &f1, const function &f2)
         { default_y_ = function(f1,f2); }
    void fitopt::default_y(const function &f1, const function &f2, const function &f3)
         { default_y_ = function(f1,f2,f3); }
    
    void fitopt::default_sigma_x(const function &f)
        { default_sigma_x_ = f; }
    void fitopt::default_sigma_x(const function &f1, const function &f2)
         { default_sigma_x_ = function(f1,f2); }
    void fitopt::default_sigma_x(const function &f1, const function &f2, const function &f3)
         { default_sigma_x_ = function(f1,f2,f3); }

    void fitopt::default_sigma_y(const function &f)
         { default_sigma_y_ = f; }
    void fitopt::default_sigma_y(const function &f1, const function &f2)
         { default_sigma_y_ = function(f1,f2); }
    void fitopt::default_sigma_y(const function &f1, const function &f2, const function &f3)
         { default_sigma_y_ = function(f1,f2,f3); }
    
    fitopt &fitopt::block(int b) { block_ = b; return *this; }

    fitopt &fitopt::x(const function &f) { x_ = f; return *this; }
    fitopt &fitopt::x(const function &f1, const function &f2) { x_ = function(f1,f2); return *this; }
    fitopt &fitopt::x(const function &f1, const function &f2, const function &f3) { x_ = function(f1,f2,f3); return *this; }
    fitopt &fitopt::x(const function &f1, const function &f2, const function &f3, const function &f4) { x_ = function(f1,f2,f3,f4); return *this; }
    fitopt &fitopt::x(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5) { x_ = function(f1,f2,f3,f4,f5); return *this; }
    fitopt &fitopt::x(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6) { x_ = function(f1,f2,f3,f4,f5,f6); return *this; }

    fitopt &fitopt::y(const function &f) { y_ = f; return *this; }
    fitopt &fitopt::y(const function &f1, const function &f2) { y_ = function(f1,f2); return *this; }
    fitopt &fitopt::y(const function &f1, const function &f2, const function &f3) { y_ = function(f1,f2,f3); return *this; }
    fitopt &fitopt::y(const function &f1, const function &f2, const function &f3, const function &f4) { y_ = function(f1,f2,f3,f4); return *this; }
    fitopt &fitopt::y(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5) { y_ = function(f1,f2,f3,f4,f5); return *this; }
    fitopt &fitopt::y(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6) { y_ = function(f1,f2,f3,f4,f5,f6); return *this; }

    fitopt &fitopt::sigma_x(const function &f) { sigma_x_ = f; return *this; }
    fitopt &fitopt::sigma_x(const function &f1, const function &f2) { sigma_x_ = function(f1,f2); return *this; }
    fitopt &fitopt::sigma_x(const function &f1, const function &f2, const function &f3) { sigma_x_ = function(f1,f2,f3); return *this; }
    fitopt &fitopt::sigma_x(const function &f1, const function &f2, const function &f3, const function &f4) { sigma_x_ = function(f1,f2,f3,f4); return *this; }
    fitopt &fitopt::sigma_x(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5) { sigma_x_ = function(f1,f2,f3,f4,f5); return *this; }
    fitopt &fitopt::sigma_x(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6) { sigma_x_ = function(f1,f2,f3,f4,f5,f6); return *this; }

    fitopt &fitopt::sigma_y(const function &f) { sigma_y_ = f; return *this; }
    fitopt &fitopt::sigma_y(const function &f1, const function &f2) { sigma_y_ = function(f1,f2); return *this; }
    fitopt &fitopt::sigma_y(const function &f1, const function &f2, const function &f3) { sigma_y_ = function(f1,f2,f3); return *this; }
    fitopt &fitopt::sigma_y(const function &f1, const function &f2, const function &f3, const function &f4) { sigma_y_ = function(f1,f2,f3,f4); return *this; }
    fitopt &fitopt::sigma_y(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5) { sigma_y_ = function(f1,f2,f3,f4,f5); return *this; }
    fitopt &fitopt::sigma_y(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6) { sigma_y_ = function(f1,f2,f3,f4,f5,f6); return *this; }

    fitopt &fitopt::fix(int i, bool fixit)
    {
	if(fixit)fixed_pars_.push_back(i);
	else
	{
	    for(int ii=0; ii<(int)fixed_pars_.size(); ++ii)
	    {
		if(fixed_pars_[ii] == i) fixed_pars_.erase(fixed_pars_.begin()+(ii--));
	    }
	}
	return *this;
    }

    bool fitopt::fixed(int pi)
    {
	for(int i=0; i<(int)fixed_pars_.size(); ++i)
	{
	    if(fixed_pars_[i] == pi) return true;
	}
	return false;
    }

    function fitopt::default_x_condition_ = unset;
    function fitopt::default_y_condition_ = unset;
    function fitopt::default_condition_   = unset;

}
