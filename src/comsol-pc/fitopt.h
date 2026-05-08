#ifndef __BLOP_FITOPT_H__
#define __BLOP_FITOPT_H__

#include "var.h"
#include "function.h"
#include "constants.h"
#include <vector>

namespace blop
{
    /*
      fitopt is a class to hold many (all) options/specifications of a fitting procedure
      (such as which column of the input data should be interpreted as x/y values or errors,
      what is the convergence criteria, verbosity, fixed parameters, max number of steps,
      etc
    */

    class fitopt
    {
    public:
	typedef int (*cfuncptr)(double,double,
				const std::vector<blop::var> &,
				const std::vector<blop::var> &,
				int);
    private:
	function x_, y_, sigma_x_, sigma_y_;
	static function default_x_, default_y_, default_sigma_x_, default_sigma_y_;
	std::vector<int> fixed_pars_;
	int verbose_;
	static int default_verbose_;
	int maxsteps_;
	static int default_maxsteps_;

        // if 0 (default), take the whole dataset. If >0, take that block of the dataset
        // (blocks are continuous subsets, separated by 'unset' values)
        int block_; 

	void *convergence_void_;
	cfuncptr convergence_;
	static void *default_convergence_void_;
	static cfuncptr default_convergence_;

	function x_condition_, y_condition_, condition_;
	static function default_x_condition_, default_y_condition_, default_condition_;

    public:

	fitopt();

	const function &x() const { return x_; }
	const function &y() const { return y_; }
	const function &sigma_x() const { return sigma_x_; }
	const function &sigma_y() const { return sigma_y_; }

	static void default_x(const function &f);
	static void default_x(const function &f1, const function &f2);
	static void default_x(const function &f1, const function &f2, const function &f3);

	static void default_y(const function &f);
	static void default_y(const function &f1, const function &f2);
	static void default_y(const function &f1, const function &f2, const function &f3);

	static void default_sigma_x(const function &f);
	static void default_sigma_x(const function &f1, const function &f2);
	static void default_sigma_x(const function &f1, const function &f2, const function &f3);

	static void default_sigma_y(const function &f);
	static void default_sigma_y(const function &f1, const function &f2);
	static void default_sigma_y(const function &f1, const function &f2, const function &f3);

	static void default_convergence(void *f) { default_convergence_void_ = f; }
	static void default_convergence(cfuncptr f) { default_convergence_ = f; }


	// ------------  Specify colums for x,y,sigmax,sigmay -------------------------

	fitopt &x(const function &f);
	fitopt &x(const function &f1, const function &f2);
	fitopt &x(const function &f1, const function &f2, const function &f3);
	fitopt &x(const function &f1, const function &f2, const function &f3, const function &f4);
	fitopt &x(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5);
	fitopt &x(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6);

	fitopt &y(const function &f);
	fitopt &y(const function &f1, const function &f2);
	fitopt &y(const function &f1, const function &f2, const function &f3);
	fitopt &y(const function &f1, const function &f2, const function &f3, const function &f4);
	fitopt &y(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5);
	fitopt &y(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6);

	fitopt &sigma_x(const function &f);
	fitopt &sigma_x(const function &f1, const function &f2);
	fitopt &sigma_x(const function &f1, const function &f2, const function &f3);
	fitopt &sigma_x(const function &f1, const function &f2, const function &f3, const function &f4);
	fitopt &sigma_x(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5);
	fitopt &sigma_x(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6);

	fitopt &sigma_y(const function &f);
	fitopt &sigma_y(const function &f1, const function &f2);
	fitopt &sigma_y(const function &f1, const function &f2, const function &f3);
	fitopt &sigma_y(const function &f1, const function &f2, const function &f3, const function &f4);
	fitopt &sigma_y(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5);
	fitopt &sigma_y(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6);


	// --------------- Specify data block --------------------------------------

        fitopt &block(int b);
        int     block() const { return block_; }

	// --------------- Specify convergence criteria ----------------------------

	fitopt &convergence(void *f) { convergence_void_ = f; return *this; }
	fitopt &convergence(cfuncptr f) { convergence_ = f; return *this; }
	cfuncptr convergence() const { return convergence_; }
	void     *convergence_void() const { return convergence_void_; }


	// ----------------  fix a parameter ---------------------------------------

	fitopt &fix(int i, bool fixit = true);
	bool    fixed(int i); 
	const std::vector<int> &fixed() const { return fixed_pars_; }


	// ---------------- Set verbosity ------------------------------------------

	static void default_verbose(int i) { default_verbose_ = i; }
	fitopt &verbose(int i) { verbose_ = i; return *this; }
	int     verbose() const { return verbose_; }


	// ---------------  Specify max number of iteration steps ------------------

	static void default_maxsteps(int i) { default_maxsteps_ = i; }
	fitopt &maxsteps(int i) { maxsteps_ = i; return *this; }
	int     maxsteps() const { return maxsteps_; }


	// -------------------  Specify conditions --------------------------------
	// Specify conditions, which data points from the input should
	// be used for the fitting
	// the functions 'x_condition' and 'y_condition' are evaluated on each
	// data point pairs, and if they both return non-0, the point is used for the fit

	static void default_x_condition(const function &f) { default_x_condition_ = f; }
	static void default_y_condition(const function &f) { default_y_condition_ = f; }
	static void default_condition  (const function &f) { default_condition_ = f; }
	fitopt &x_condition(const function &f) { x_condition_ = f; return *this; }
	fitopt &y_condition(const function &f) { y_condition_ = f; return *this; }
	fitopt &condition  (const function &f) { condition_ = f; return *this; }
	const function &x_condition() const { return x_condition_; }
	const function &y_condition() const { return y_condition_; }
	const function condition()    const { return condition_; }

	// ----------  Specify fitting x-range ------------------------------------
	// Only (x,y) value pairs where x falls into the given range will be used
	// in the fitting. It is implemented by specifying a condition (see above),
	// so do not try to specify a fitting range and another condition simultaneously.

	fitopt &xrange(double x1, double x2)
	    {
		if(x1 == unset && x2 == unset) x_condition_ = unset;
		if(x1 != unset && x2 != unset) x_condition_ = (x1<_1) && (_1<x2);
		if(x1 == unset && x2 != unset) x_condition_ = (_1<x2);
		if(x1 != unset && x2 == unset) x_condition_ = (x1<_1);
		return *this;
	    }
	fitopt &xmin(double x)
	    {
		if(x != unset) x_condition_ = (x<_1);
		else x_condition_ = unset;
		return *this;
	    }
	fitopt &xmax(double x)
	    {
		if(x != unset) x_condition_ = (_1<x);
		else x_condition_ = unset;
		return *this;
	    }

	// ----------  Specify fitting y-range ------------------------------------
	// Only (x,y) value pairs where y falls into the given range will be used
	// in the fitting. It is implemented by specifying a condition (see above),
	// so do not try to specify a fitting range and another condition simultaneously.

	fitopt &yrange(double x1, double x2)
	    {
		if(x1 == unset && x2 == unset) y_condition_ = unset;
		if(x1 != unset && x2 != unset) y_condition_ = (x1<_1) && (_1<x2);
		if(x1 == unset && x2 != unset) y_condition_ = (_1<x2);
		if(x1 != unset && x2 == unset) y_condition_ = (x1<_1);
		return *this;
	    }
	fitopt &ymin(double x)
	    {
		if(x != unset) y_condition_ = (x<_1);
		else y_condition_ = unset;
		return *this;
	    }
	fitopt &ymax(double x)
	    {
		if(x != unset) y_condition_ = (_1<x);
		else y_condition_ = unset;
		return *this;
	    }


    };

}

#endif

