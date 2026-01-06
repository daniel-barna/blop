#ifndef __BLOP_FIT_H__
#define __BLOP_FIT_H__

#include "plottable.h"
#include "dgraph.h"
#include "fitopt.h"
#include "matrix.h"

// chi2 statistic for normally distributed measurement-point
class gauss_chi2  
{
 public:
    static double der0(const std::vector<blop::var> &x,
		       const std::vector<blop::var> &sigma_x,
		       const std::vector<blop::var> &y,
		       const std::vector<blop::var> &sigma_y,
		       const std::vector<blop::var> &funcvalue);
    static double der1(const std::vector<blop::var> &x,
		       const std::vector<blop::var> & , //sigma_x
		       const std::vector<blop::var> &y,
		       const std::vector<blop::var> &sigma_y,
		       const std::vector<blop::var> &funcvalue,
		       const std::vector<blop::var> &g1);
    static double der2(const std::vector<blop::var> &x,
		       const std::vector<blop::var> & , //sigma_x
		       const std::vector<blop::var> &y,
		       const std::vector<blop::var> &sigma_y,
		       const std::vector<blop::var> &funcvalue,
		       const std::vector<blop::var> &g1,
		       const std::vector<blop::var> &g2);
};

// chi2 statistic for poisson-distributed measurement-points 
// see Nucl. Instr. and Meth. in Phys. Res. 221 (1984) 437-442
class poisson_chi2
{
 public:
    static double der0(const std::vector<blop::var> &x,
		       const std::vector<blop::var> &sigma_x,
		       const std::vector<blop::var> &y,
		       const std::vector<blop::var> &sigma_y,
		       const std::vector<blop::var> &funcvalue);
    static double der1(const std::vector<blop::var> &x,
		       const std::vector<blop::var> & , //sigma_x
		       const std::vector<blop::var> &y,
		       const std::vector<blop::var> &sigma_y,
		       const std::vector<blop::var> &funcvalue,
		       const std::vector<blop::var> &g1);
    static double der2(const std::vector<blop::var> &x,
		       const std::vector<blop::var> & , //sigma_x
		       const std::vector<blop::var> &y,
		       const std::vector<blop::var> &sigma_y,
		       const std::vector<blop::var> &funcvalue,
		       const std::vector<blop::var> &g1,
		       const std::vector<blop::var> &g2);
};

// chi2 statistic for multinomially-distributed measurement-points 
// see Nucl. Instr. and Meth. in Phys. Res. 221 (1984) 437-442
class multinomial_chi2
{
 public:
    static double der0(const std::vector<blop::var> &x,
		       const std::vector<blop::var> &sigma_x,
		       const std::vector<blop::var> &y,
		       const std::vector<blop::var> &sigma_y,
		       const std::vector<blop::var> &funcvalue);
    static double der1(const std::vector<blop::var> &x,
		       const std::vector<blop::var> & , //sigma_x
		       const std::vector<blop::var> &y,
		       const std::vector<blop::var> &sigma_y,
		       const std::vector<blop::var> &funcvalue,
		       const std::vector<blop::var> &g1);
    static double der2(const std::vector<blop::var> &x,
		       const std::vector<blop::var> & , //sigma_x
		       const std::vector<blop::var> &y,
		       const std::vector<blop::var> &sigma_y,
		       const std::vector<blop::var> &funcvalue,
		       const std::vector<blop::var> &g1,
		       const std::vector<blop::var> &g2);
};


namespace blop
{


    class fitresult
	{
	public:
	    double chi2;

            // Covariance-matrix, 1-based indexes (i.e. the first parameter's error is
            // sqrt(covar(1,1))
	    matrix<var> covar;


	    int nparams;   // number of fitted parameters
	    int N;         // number of degrees of freedom (ndata - nfreeparams)
	    int nsteps;
	};



    void fit_wrapper(const plottable &g, function &func, const fitopt &opt,
		     void *, void *, void *,fitresult &);

    typedef double (*deriv0_func)(const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &);
    typedef double (*deriv1_func)(const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &);
    typedef double (*deriv2_func)(const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &,
				  const std::vector<blop::var> &);

    void fit_wrapper(const plottable &g, function &func, const fitopt &opt,
		     deriv0_func, deriv1_func, deriv2_func, fitresult &res);

    template <class T>
    fitresult fit(const plottable &g, function &func, const fitopt &opt = fitopt())
    {
	fitresult result;
	fit_wrapper(g, func, opt, T::der0, T::der1, T::der2, result);
	return result;
    }

    fitresult fit(const plottable &g, function &func, const fitopt &opt = fitopt()); // moved to fit.cc

    template <class T>
    fitresult fit(const var &filename, function &func, const fitopt &opt = fitopt())
    {
	dgraph g;
	g.read(filename);
	fitresult result;
	fit_wrapper(g, func, opt, T::der0, T::der1, T::der2, result);
	return result;
    }

    fitresult fit(const var &filename, function &func, const fitopt &opt = fitopt()); // moved to fit.cc

    template <class T>
    fitresult fit(const char *filename, function &func, const fitopt &opt = fitopt())
    {
	return fit<T>(var(filename),func,opt);
    }

    fitresult fit(const char *filename, function &func, const fitopt &opt = fitopt()); // moved to fit.cc

    template <class T>
    fitresult fit(const std::string &filename, function &func, const fitopt &opt = fitopt())
    {
	return fit<T>(var(filename),func,opt);
    }

    fitresult fit(const std::string &filename, function &func, const fitopt &opt = fitopt()); // moved to fit.cc


    template <class T>
    inline fitresult fit(const std::vector<double> &x, const std::vector<double> &y,
			 function &func, const fitopt &opt = fitopt())
    {
	dgraph g(x,y);
	fitresult result;
	fit_wrapper(g, func, opt, T::der0, T::der1, T::der2, result);
	return result;
    }

    fitresult fit(const std::vector<double> &x, const std::vector<double> &y,
		  function &func, const fitopt &opt = fitopt()); 

    template <class T>
    inline fitresult fit(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z,
			 function &func, const fitopt &opt = fitopt())
    {
	dgraph g(x,y,z);
	fitresult result;
	fit_wrapper(g, func, opt, T::der0, T::der1, T::der2, result);
	return result;
    }

    fitresult fit(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &yerror,
		  function &func, const fitopt &opt = fitopt()); 

}

/*
#ifdef __MAKECINT__
#pragma link C++ function blop::fit<gauss_chi2>(const blop::plottable &, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<poisson_chi2>(const blop::plottable &, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<multinomial_chi2>(const blop::plottable &, blop::function &, const blop::fitopt &);

#pragma link C++ function blop::fit<gauss_chi2>(const blop::dgraph &, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<poisson_chi2>(const blop::dgraph &, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<multinomial_chi2>(const blop::dgraph &, blop::function &, const blop::fitopt &);


#pragma link C++ function blop::fit<gauss_chi2>(const blop::var &, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<poisson_chi2>(const blop::var &, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<multinomial_chi2>(const blop::var &, blop::function &, const blop::fitopt &);

#pragma link C++ function blop::fit<gauss_chi2>(const char *, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<poisson_chi2>(const char *, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<multinomial_chi2>(const char *, blop::function &, const blop::fitopt &);

#pragma link C++ function blop::fit<gauss_chi2>(const std::string &, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<poisson_chi2>(const std::string &, blop::function &, const blop::fitopt &);
#pragma link C++ function blop::fit<multinomial_chi2>(const std::string &, blop::function &, const blop::fitopt &);
#endif
*/

#endif
