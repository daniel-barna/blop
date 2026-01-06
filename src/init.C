using namespace blop;
#include <vector>

//constants from <cmath> or <math.h>

#undef M_E 
#undef M_LOG2E
#undef M_LOG10E
#undef M_LN2
#undef M_LN10
#undef M_PI
#undef M_PI_2
#undef M_PI_4
#undef M_1_PI
#undef M_2_PI
#undef M_2_SQRTPI
#undef M_SQRT2
#undef M_SQRT1_2

const double M_E  =          2.7182818284590452354;   /* e */
const double M_LOG2E =       1.4426950408889634074;   /* log_2 e */
const double M_LOG10E =      0.43429448190325182765;  /* log_10 e */
const double M_LN2 =         0.69314718055994530942;  /* log_e 2 */
const double M_LN10 =        2.30258509299404568402;  /* log_e 10 */
const double M_PI =          3.14159265358979323846;  /* pi */
const double M_PI_2 =        1.57079632679489661923;  /* pi/2 */
const double M_PI_4 =        0.78539816339744830962;  /* pi/4 */
const double M_1_PI =        0.31830988618379067154;  /* 1/pi */
const double M_2_PI =        0.63661977236758134308;  /* 2/pi */
const double M_2_SQRTPI =    1.12837916709551257390;  /* 2/sqrt(pi) */
const double M_SQRT2 =       1.41421356237309504880;  /* sqrt(2) */
const double M_SQRT1_2 =     0.70710678118654752440;  /* 1/sqrt(2) */

int autoprint=1;
template <class T>
int G__ateval(const T &t)
{
    if(autoprint && canvas::current().modified() && !canvas::current().empty())
    {
	blop::x11_ps::print();
    }
    return 1;
}

int G__ateval(var v)
{
    cout<<v.str()<<endl;
    if(autoprint && canvas::current().modified() && !canvas::current().empty())
    {
	blop::x11_ps::print();
    }
    return 1;
}

int G__ateval(double d)
{
    cout<<d<<endl;
    if(autoprint && canvas::current().modified() && !canvas::current().empty())
    {
	blop::x11_ps::print();
    }
    return 1;
}

int G__ateval(int d)
{
    cout<<d<<endl;
    if(autoprint && canvas::current().modified() && !canvas::current().empty())
    {
	blop::x11_ps::print();
    }
    return 1;
}

int G__ateval(const function &f)
{
    if(f.is_constant())
    {
	std::vector<blop::var> &args(f.nargs());
	for(int i=0; i<args.size(); ++i) args[i] = 0;
	var result = f.eval(args);
	cout<<result<<endl;
    }
    return 1;
}

// and some other staff can come here

// --------  fitting --------------------------------------
// These same template functions are also declared/defined
// in fit.h, but those are not available to the interpreter;
// Putting them here (therefore letting the interpreter
// handle them) makes it possible to have any future (possibly
// interpreted) error-policy-class (the T template argument 
// of these functions) also usable
// These declarations also use the (non-c++-standard) feature
// of cint to have a default template argument to such
// functions. That is, one can write (only in interpreted scripts!!)
// fit("filename",somefunc), instead of fit<gauss_chi2>("filename",somefunc)

template <class T>
fitresult fit(const plottable &g, function &func, const fitopt &opt = fitopt())
{
    fitresult result;
    fit_wrapper(g, func, opt, T::der0, T::der1, T::der2, result);
    return result;
}

/*
fitresult fit(const plottable &g, function &func, const fitopt &opt = fitopt())
{
    fitresult result;
    fit_wrapper(g, func, opt, gauss_chi2::der0, gauss_chi2::der1, gauss_chi2::der2, result);
    return result;
}
*/

template <class T>
fitresult fit(const var &filename, function &func, const fitopt &opt = fitopt())
{
    dgraph g;
    g.read(filename);
    fitresult result;
    fit_wrapper(g, func, opt, T::der0, T::der1, T::der2, result);
    return result;
}

/*
fitresult fit(const var &filename, function &func, const fitopt &opt = fitopt())
{
    dgraph g;
    g.read(filename);
    fitresult result;
    fit_wrapper(g, func, opt, gauss_chi2::der0, gauss_chi2::der1, gauss_chi2::der2, result);
    return result;
}
*/

template <class T>
fitresult fit(const char *filename, function &func, const fitopt &opt = fitopt())
{
    return fit<T>(var(filename),func,opt);
}

/*
fitresult fit(const char *filename, function &func, const fitopt &opt = fitopt())
{
    return fit<gauss_chi2>(var(filename),func,opt);
}
*/

template <class T>
fitresult fit(const std::string &filename, function &func, const fitopt &opt = fitopt())
{
    return fit<T>(var(filename),func,opt);
}

/*
fitresult fit(const std::string &filename, function &func, const fitopt &opt = fitopt())
{
    return fit<gauss_chi2>(var(filename),func,opt);
}
*/

template <class T>
inline fitresult fit(const std::vector<double> &x,
		     function &func, const fitopt &opt = fitopt())
{
    dgraph g(x,y);
    fitresult result;
    fit_wrapper(g, func, opt, T::der0, T::der1, T::der2, result);
    return result;
}

/*
inline fitresult fit(const std::vector<double> &x,
		     function &func, const fitopt &opt = fitopt())
{
    return fit<gauss_chi2>(x,y,func,opt);
}
*/

template <class T>
inline fitresult fit(const std::vector<double> &x, const std::vector<double> &y,
		     function &func, const fitopt &opt = fitopt())
{
    dgraph g(x,y);
    fitresult result;
    fit_wrapper(g, func, opt, T::der0, T::der1, T::der2, result);
    return result;
}

/*
inline fitresult fit(const std::vector<double> &x, const std::vector<double> &y,
		     function &func, const fitopt &opt = fitopt())
{
    return fit<gauss_chi2>(x,y,func,opt);
}
*/

#define __BLOP__ 
