#include "fit.h"
#include <cmath>
#include "cfunc_wrapper.hh"
#include "bloputils.h"
#include "warning.h"
#include "ignore.h"


#include <iomanip>
#include <cstdlib>

#include "levenberg-marquardt.h"

namespace blop
{
    using namespace std;

    typedef matrix<var> dmatrix;

    void fit_kernel(const plottable &g, function &fitfunc, const fitopt &opt,
		    double_func_aref_aref_aref_aref_aref der0,
		    double_func_aref_aref_aref_aref_aref_aref der1,
		    double_func_aref_aref_aref_aref_aref_aref_aref der2,
		    fitresult &fit_result)
    {
	fit_result.chi2    = 0;
	fit_result.nparams = 0;
	fit_result.N       = 0;
	fit_result.nsteps  = 0;

	function get_x = opt.x();
	const int NX    = get_x.components();
	function get_sigma_x = opt.sigma_x();
	if(!get_sigma_x.initialized()) for(int i=0; i<NX; ++i) get_sigma_x.append(function(1.0)); //component(i,1);
	if(get_x.components() != get_sigma_x.components())
	{
	    warning::print("The components of the x() and sigma_x() of fitopt do not agree",
			   "fit_kernel(...)");
	    return;
	}

	function get_y = opt.y();
	const int NY    = get_y.components();
	function get_sigma_y = opt.sigma_y();
	if(!get_sigma_y.initialized()) for(int i=0; i<NY; ++i) get_sigma_y.append(function(1.0)); //component(i,1);
	if(get_y.components() != get_sigma_y.components())
	{
	    warning::print("The components of the y() and sigma_y() of fitopt do not agree",
			   "fit_kernel(...)");
	    return;

	}

        // determine the number of data points
	int NDATA = g.size();

	dmatrix x      (NDATA, NX);
	dmatrix sigma_x(NDATA, NX);
	dmatrix y      (NDATA, NY);
	dmatrix sigma_y(NDATA, NY);

	{
	    const function &x_condition = opt.x_condition();
	    const function &y_condition = opt.y_condition();
	    const function &condition   = opt.condition();

	    int comp = g.columns();
	    vector<var> point(comp);
	    vector<var> x_values(NX);
	    vector<var> sigma_x_values(NX);
	    vector<var> y_values(NY);
	    vector<var> sigma_y_values(NY);

	    int n_good_data = 0;

            int block_index = 0;
            bool previous_unset = false;

	    for(int idata=0; idata<NDATA; ++idata)
	    {
		for(int i=0; i<comp; ++i) point[i].dbl() = (*(g.get(idata)))[i].dbl();

		// skip unset points, corresponding to empty lines in the datafile
		if(point[0].dbl() == unset)
                {
                    previous_unset = true;
                    continue;
                }
                else
                {
                    if(idata==0 || previous_unset) ++block_index;
                    previous_unset = false;
                }

                if(opt.block()>0 && opt.block()!=block_index) continue;

                function::clear_named_params();
		function::named_param("linenumber",idata);
                function::named_param("blocknumber",block_index);
		get_x      .meval_dbl(point,x_values);
		get_sigma_x.meval_dbl(point,sigma_x_values);
		get_y      .meval_dbl(point,y_values);
		get_sigma_y.meval_dbl(point,sigma_y_values);

		// if a global condition is set, apply it for the original data point,
		// and skip this point if the condition evaluates to 0.0
		if(condition.initialized() && condition.eval_dbl(point)==0.0) continue;

		// skip points, which are set globally to be skipped
		{
		    bool skip = false;
		    for(int nx=0; nx<NX; ++nx)
		    {
			if(ignore::it(x_values[nx]      )) { skip = true; break; }
			if(ignore::it(sigma_x_values[nx])) { skip = true; break; }
		    }
		    if(skip) continue;
		    for(int ny=0; ny<NY; ++ny)
		    {
			if(ignore::it(y_values[ny]      )) { skip = true; break; }
			if(ignore::it(sigma_y_values[ny])) { skip = true; break; }
		    }
		    if(skip) continue;
		}

		if( x_values[0].dbl() != unset &&
		    (!x_condition.initialized() || x_condition.eval_dbl(x_values)>0.0) &&
		    (!y_condition.initialized() || y_condition.eval_dbl(y_values)>0.0))
		{
		    for(int ix=0; ix<NX; ++ix)
		    {
			x      (n_good_data,ix).dbl() = x_values[ix].dbl();
			sigma_x(n_good_data,ix).dbl() = sigma_x_values[ix].dbl();
		    }
		    for(int iy=0; iy<NY; ++iy)
		    {
			y      (n_good_data,iy).dbl() = y_values[iy].dbl();
			sigma_y(n_good_data,iy).dbl() = sigma_y_values[iy].dbl();
		    }
		    ++n_good_data;
		}
	    }

	    NDATA = n_good_data;
	    x      .resize(NDATA, NX);
	    sigma_x.resize(NDATA, NX);
	    y      .resize(NDATA, NY);
	    sigma_y.resize(NDATA, NY);
	}

	const int npars = fitfunc.npars();

	std::vector<blop::var> parameters(npars);
	std::vector<bool> parameter_free(npars);
	    //bool   *parameter_free = new bool[npars];
	for(int i=0; i<npars; ++i)
	{
	    parameters[i].dbl() = fitfunc.param(i+1).dbl();
	    parameter_free[i] = true;
	}
	const std::vector<int> &fixed_pars = opt.fixed();
	for(unsigned int i=0; i<fixed_pars.size(); ++i)
	{
	    if(0<fixed_pars[i] && fixed_pars[i]<=npars)
	    {
		parameter_free[fixed_pars[i]-1] = false;
	    }
	}

	fit_result.covar.resize(npars,npars);
	fit_result.covar.index_base(0,0);

	vector<blop::function> fitfunc_derivs(fitfunc.npars());
	for(int i=0; i<fitfunc.npars(); ++i)
	{
	    if(parameter_free[i]) fitfunc_derivs[i] = fitfunc.derivative(-(i+1));
	    else fitfunc_derivs[i] = 0.0;
	}

	fitfunc.print_param_value(true);

	if(opt.convergence())
	{
	    fit_result.nsteps = levenberg_marquardt(x,sigma_x,y,sigma_y,parameters,parameter_free,
						    fit_result.covar,&fit_result.chi2,opt.verbose(),
						    fitfunc,fitfunc_derivs,fitfunc.components(),opt.maxsteps(),0.001,der0,der1,der2,
						    opt.convergence());
	}
	else
	{
	    fit_result.nsteps = levenberg_marquardt(x,sigma_x,y,sigma_y,parameters,parameter_free,
						    fit_result.covar,&fit_result.chi2,opt.verbose(),
						    fitfunc,fitfunc_derivs,fitfunc.components(),opt.maxsteps(),0.001,der0,der1,der2,
						    int_func_double_double_aref_aref_int(opt.convergence_void()));
	}

	for(int i=0; i<npars; ++i) fitfunc.param(i+1, parameters[i].dbl());

	int nfree=0;
	for(int i=0; i<npars; ++i) if(parameter_free[i]) ++nfree;

	fit_result.nparams = nfree;
	fit_result.N       = NDATA-nfree;

	if(opt.verbose() > 0)
	{
	    cerr<<"========================  fit results  =============================="<<endl;
	    cerr<<": number of iteration steps = "<<fit_result.nsteps<<endl;
	    cerr<<": chi2 / n                  = "<<fit_result.chi2<<" / "
		<<fit_result.N<<"  =  "<<fit_result.chi2/fit_result.N<<endl;
	    for(int i=0; i<npars; ++i)
	    {
		const double error = ::sqrt(fit_result.covar(i,i).dbl());
		cerr<<": ";
		string parname = fitfunc.parname(i+1).str();
		if(parname != "")
		{
		    cerr<<parname;
		    for(int j=parname.size(); j<=26; ++j) cerr<<" ";
		}
		else
		{
		    var lab = var("parameter[") & (i+1) & var("]");
		    cerr<<lab;
		    for(int j=lab.size(); j<=26; ++j) cerr<<" ";
		}
		cerr<<" = ";
		cerr<<setw(20)<<parameters[i].dbl()
		    <<"  +/-   "<<error<<endl;
	    }
	    cerr<<": Covariance matrix: "<<endl;
	    for(int i=0; i<npars; ++i)
	    {
		cerr<<":   ";
		for(int j=0; j<=i; ++j)
		{
		    cerr<<setw(15)<<fit_result.covar(i,j).dbl()<<" ";
		}
		cerr<<endl;
	    }
	    cerr<<"====================================================================="<<endl;
	}
	fit_result.covar.index_base(1,1);

//	delete [] parameter_free;

    }

    void fit_wrapper(const plottable &g, function &func, const fitopt &opt,
		     void *der0, void *der1, void *der2, fitresult &result)
    {
	fit_kernel(g, func, opt, der0, der1, der2, result);
    }

    void fit_wrapper(const plottable &g, function &func, const fitopt &opt,
		     deriv0_func der0, deriv1_func der1, deriv2_func der2, fitresult &result)
    {
	fit_kernel(g, func, opt, der0, der1, der2, result);
    }


    template  fitresult fit<gauss_chi2>(const plottable &, function &, const fitopt &opt);
    template  fitresult fit<poisson_chi2>(const plottable &, function &, const fitopt &opt);
    template  fitresult fit<multinomial_chi2>(const plottable &, function &, const fitopt &opt);

    template  fitresult fit<gauss_chi2>(const var &, function &, const fitopt &opt);
    template  fitresult fit<poisson_chi2>(const var &, function &, const fitopt &opt);
    template  fitresult fit<multinomial_chi2>(const var &, function &, const fitopt &opt);

    template  fitresult fit<gauss_chi2>(const std::string &, function &, const fitopt &opt);
    template  fitresult fit<poisson_chi2>(const std::string &, function &, const fitopt &opt);
    template  fitresult fit<multinomial_chi2>(const std::string &, function &, const fitopt &opt);

    template  fitresult fit<gauss_chi2>(const char *, function &, const fitopt &opt);
    template  fitresult fit<poisson_chi2>(const char *, function &, const fitopt &opt);
    template  fitresult fit<multinomial_chi2>(const char *, function &, const fitopt &opt);

    fitresult fit(const plottable &g, function &func, const fitopt &opt)
    {
	fitresult result;
	fit_wrapper(g, func, opt, gauss_chi2::der0, gauss_chi2::der1, gauss_chi2::der2, result);
	return result;
    }

    fitresult fit(const var &filename, function &func, const fitopt &opt)
    {
	dgraph g;
	g.read(filename);
	fitresult result;
	fit_wrapper(g, func, opt, gauss_chi2::der0, gauss_chi2::der1, gauss_chi2::der2, result);
	return result;
    }

    fitresult fit(const char *filename, function &func, const fitopt &opt)
    {
	return fit<gauss_chi2>(var(filename),func,opt);
    }

    fitresult fit(const std::string &filename, function &func, const fitopt &opt)
    {
	return fit<gauss_chi2>(var(filename),func,opt);
    }

    fitresult fit(const std::vector<double> &x, const std::vector<double> &y,
		  function &func, const fitopt &opt)
    {
	return fit<gauss_chi2>(x,y,func,opt);
    }

    fitresult fit(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z,
		  function &func, const fitopt &opt)
    {
	return fit<gauss_chi2>(x,y,z,func,opt);
    }


    
}

inline double sq(double a) { return a*a; }

// -------------  gauss_chi2  ---------------------------------
double gauss_chi2::der0(const std::vector<blop::var> &x,
			const std::vector<blop::var> &, //sigma_x 
			const std::vector<blop::var> &y,
			const std::vector<blop::var> &sigma_y,
			const std::vector<blop::var> &funcvalue)
{
    double result = 0;
    for(unsigned int i=0; i<y.size(); ++i) result += sq(y[i].dbl()-funcvalue[i].dbl())/sq(sigma_y[i].dbl());
    return result;
}

double gauss_chi2::der1(const std::vector<blop::var> &x,
			const std::vector<blop::var> & , //sigma_x
			const std::vector<blop::var> &y,
			const std::vector<blop::var> &sigma_y,
			const std::vector<blop::var> &funcvalue,
			const std::vector<blop::var> &g1)
{
    double result = 0;
    for(unsigned int i=0; i<y.size(); ++i) result += (funcvalue[i].dbl()-y[i].dbl())*g1[i].dbl()/sq(sigma_y[i].dbl());
    return result*2;
}

double gauss_chi2::der2(const std::vector<blop::var> &x,
			const std::vector<blop::var> & , //sigma_x
			const std::vector<blop::var> &y,
			const std::vector<blop::var> &sigma_y,
			const std::vector<blop::var> &funcvalue,
			const std::vector<blop::var> &g1,
			const std::vector<blop::var> &g2)
{
    double result = 0;
    for(unsigned int i=0; i<y.size(); ++i) result += g1[i].dbl()*g2[i].dbl()/sq(sigma_y[i].dbl());
    return result*2;
}


// -------------  poisson_chi2  ---------------------------------
double poisson_chi2::der0(const std::vector<blop::var> &x,
			  const std::vector<blop::var> &, //sigma_x 
			  const std::vector<blop::var> &y,
			  const std::vector<blop::var> &sigma_y,
			  const std::vector<blop::var> &f)
{
    double result = 0;
    for(unsigned int i=0; i<y.size(); ++i)
    {
	result += f[i].dbl() - y[i].dbl();
	if(y[i].dbl() > 0) result += y[i].dbl()*::log(y[i].dbl()/f[i].dbl());
    }
    return result*2;
}

double poisson_chi2::der1(const std::vector<blop::var> &x,
			  const std::vector<blop::var> & , //sigma_x
			  const std::vector<blop::var> &y,
			  const std::vector<blop::var> &sigma_y,
			  const std::vector<blop::var> &funcvalue,
			  const std::vector<blop::var> &g1)
{
    double result = 0;
    for(unsigned int i=0; i<y.size(); ++i) result += (1-y[i].dbl()/funcvalue[i].dbl())*g1[i].dbl();
    return result*2;
}

double poisson_chi2::der2(const std::vector<blop::var> &x,
			  const std::vector<blop::var> & , //sigma_x
			  const std::vector<blop::var> &y,
			  const std::vector<blop::var> &sigma_y,
			  const std::vector<blop::var> &funcvalue,
			  const std::vector<blop::var> &g1,
			  const std::vector<blop::var> &g2)
{
    double result = 0;
    for(unsigned int i=0; i<y.size(); ++i) result += y[i].dbl()/funcvalue[i].dbl()/funcvalue[i].dbl()*g1[i].dbl()*g2[i].dbl();
    return result*2;
}

// -------------  multinominal_chi2  ---------------------------------
double multinomial_chi2::der0(const std::vector<blop::var> &x,
			      const std::vector<blop::var> &, //sigma_x 
			      const std::vector<blop::var> &y,
			      const std::vector<blop::var> &sigma_y,
			      const std::vector<blop::var> &f)
{
    double result = 0;
    for(unsigned int i=0; i<y.size(); ++i) if(y[i].dbl() > 0) result += y[i].dbl()*::log(y[i].dbl()/f[i].dbl());
    return result*2;
}

double multinomial_chi2::der1(const std::vector<blop::var> &x,
			      const std::vector<blop::var> & , //sigma_x
			      const std::vector<blop::var> &y,
			      const std::vector<blop::var> &sigma_y,
			      const std::vector<blop::var> &f,
			      const std::vector<blop::var> &g1)
{
    double result = 0;
    for(unsigned int i=0; i<y.size(); ++i) result += -y[i].dbl()/f[i].dbl()*g1[i].dbl();
    return result*2;
}

double multinomial_chi2::der2(const std::vector<blop::var> &x,
			      const std::vector<blop::var> & , //sigma_x
			      const std::vector<blop::var> &y,
			      const std::vector<blop::var> &sigma_y,
			      const std::vector<blop::var> &f,
			      const std::vector<blop::var> &g1,
			      const std::vector<blop::var> &g2)
{
    double result = 0;
    for(unsigned int i=0; i<y.size(); ++i) result += y[i].dbl()/f[i].dbl()/f[i].dbl()*g1[i].dbl()*g2[i].dbl();
    return result*2;
}
