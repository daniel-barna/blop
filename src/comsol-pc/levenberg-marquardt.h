#ifndef __LEVENBERG_MARQUARDT_H__
#define __LEVENBERG_MARQUARDT_H__

#include "matrix.h"

namespace blop
{
    // The Levenberg-Marquardt fitting algorithm is implemented here in a general way,
    // using template functions, so that it can be used for any type of variables,
    // on which a fitfunction is evaluated (i.e. the fitfunction does not need to be
    // of type double (func)(double), can have any type of arguments and any type
    // of return value.
    // The template classes throughout this file are the following:
    // 
    //   x_type          The type of the argument(s) of the fitfunction, normally a double
    //                   in more exotic cases it can be anything
    //   y_type          The type of the return value(es) of the fitfunction
    //   par_type        The type of the parameter(s) of the fitfunction (over which the fit
    //                   finds the optimum)
    //   func_type       The function type. This class must implement the following members:
    //                   - operator()(const std::vector<x_type> &args, const std::vector<par_type> &pars, std::vector<y_type> &results)
    //   der0_type       
    //   der1_type
    //   der2_type       Function-like types, which should implement the () operator (returning double) with the argument lists as below,
    //                   to evaluate the 'error' (chisq) between a set of measured y[i] points and
    //                   the corresponding fitfunc(x[i]) values. In the most common case (assuming normal-distribution
    //                   for the measurement errors on y[i])
    //                   der0(const std::vector<x_type> &x,const std::vector<x_type> &sigmax,
    //                        const std::vector<y_type> &y,const std::vector<y_type> &sigmay,
    //                        const std::vector<y_type> &funcvalues) = sum_i |y[i]-funcvalues[i]|^2/sigmay[i]^2
    //                   der1 and der2 return the 1st and 2nd derivative of this value w.r.t. changes of funcvalues
    //                   given as extra parameters:
    //                   der1(const std::vector<x_type> &x,const std::vector<x_type> &sigmax,
    //                        const std::vector<y_type> &y,const std::vector<y_type> &sigmay,
    //                        const std::vector<y_type> &funcvalues, const std::vector<y_type> &dfda) =
    //                           2*sum_i (funcvalues[i]-y[i])*dfda[i]/sigmay[i]^2
    //                   der2(const std::vector<x_type> &x,const std::vector<x_type> &sigmax,
    //                        const std::vector<y_type> &y,const std::vector<y_type> &sigmay,
    //                        const std::vector<y_type> &funcvalues, const std::vector<y_type> &dfda1, const std::vector<y_type> &dfda2) =
    //                           2*sum_i dfda1[i]*dfda2[i]/sigmay[i]^2
    // 
    //   convfunc_type   A function-like object which is called in each iteration step, and judges if the fit has converged
    //                   It must implement the () operator:
    //                   int operator()(double old_chi2, double chi2,
    //                                  const std::vector<par_type> &oldpars,
    //                                  const std::vector<par_type> &newpars,
    //                                  int nsteps)

    // -----------------------  gauss-jordan elimination -----------------------------
    // the routine solves the equation 'x = A * y' for y (A is a NxN matrix,
    // x and y are vectors). The argument 'vec' is 'x' at input, and
    // it will contain the solution at output. The routine uses only
    // the ndim x ndim (left upper) part of the matrix, and the first
    // ndim elemenets of the vector.
    
    template <class par_type>
    inline bool gaussj(matrix<par_type> &a, double b[], int n, int *indxc, int *indxr, int *ipiv)
    {
	int irow=0, icol=0;

	for(int j=0; j<n; ++j) ipiv[j] = 0;
	for(int i=0; i<n; ++i)
	{
	    double big = 0;
	    for(int j=0; j<n; ++j)
	    {
		if( ipiv[j] != 1)
		{
		    for(int k=0; k<n; ++k)
		    {
			if(ipiv[k] == 0)
			{
			    if(::fabs(a(j,k).dbl()) >= big)
			    {
				big = ::fabs(a(j,k).dbl());
				irow = j;
				icol = k;
			    }
			}
		    }
		}
	    }
	    ++(ipiv[icol]);
	    if(irow != icol)
	    {
		for(int l=0; l<n; ++l) swap(a(irow,l).dbl(),a(icol,l).dbl());
		swap(b[irow],b[icol]);
	    }
	    indxr[i] = irow;
	    indxc[i] = icol;
	    if(a(icol,icol).dbl() == 0.0) return false;

	    double pivinv = 1.0/a(icol,icol).dbl();
	    a(icol,icol).dbl() = 1.0;
	    for(int l=0; l<n; ++l) a(icol,l).dbl() *= pivinv;
	    b[icol] *= pivinv;
	    for(int ll=0; ll<n; ++ll)
	    {
		if(ll != icol)
		{
		    double dum=a(ll,icol).dbl();
		    a(ll,icol).dbl() = 0.0;
		    for(int l=0; l<n; ++l) a(ll,l).dbl() -= a(icol,l).dbl()*dum;
		    b[ll] -= b[icol]*dum;
		}
	    }
	}
	for(int l=n-1; l>=0; --l)
	{
	    if(indxr[l] != indxc[l])
	    {
		for(int k=0; k<n; ++k)
		{
		    swap(a(k,indxr[l]).dbl(),a(k,indxc[l]).dbl());
		}
	    }
	}
	return true;
    }

    /*
    int gauss_jordan(matrix<var> &mat,double vec[],int ndim)
    {
	for(int i=0; i<ndim-1; ++i)
	{
	    for(int j=i+1; j<ndim; ++j)
	    {
		double ratio = mat(j,i).dbl()/mat(i,i).dbl();
		if(!isfinite(ratio)) return 0;
		mat(j,i).dbl() = 0;
		for(int k=i+1; k<ndim; ++k)
		{
		    mat(j,k).dbl() -= ratio * mat(i,k).dbl();
		}
		vec[j] -= ratio * vec[i];
	    }
	}

	for(int i=ndim-1; i>=0; --i)
	{
	    double others = 0;
	    for(int j=i+1; j<ndim; ++j) others += mat(i,j).dbl()*vec[j];
	    vec[i] = (vec[i]-others)/mat(i,i).dbl();
	    //if(!isfinite(vec[i])) return 0;
	}
	return 1;
    }
    */


    // ----------------------------  invert_hessian  --------------------------------------
    // INPUT:
    // alpha               -- hessian matrix of chi2
    // beta                -- gradient of chi2
    // 
    // OUTPUT:
    // covar               -- alpha_lambda inverted, where alpha_lambda is
    //                        the same as 'alpha', except diagonal elements
    //                        multiplied by (1+lambda)
    // beta_temp           -- the solution of beta = alpha_lambda * beta_temp

    template <class par_type>
    inline bool invert_hessian(const matrix<par_type> &alpha, matrix<par_type> &covar,
			       const double beta[], double beta_temp[],
			       int NFREE, double lambda,
			       int *indxc, int *indxr, int *ipiv)
    {
	for(int j=0; j<NFREE; ++j)
	{
	    for(int k=0; k<NFREE; ++k) covar(j,k).dbl() = alpha(j,k).dbl();
	    covar(j,j).dbl() = alpha(j,j).dbl() * (1 + lambda);
	    beta_temp[j] = beta[j];
	}
	return gaussj(covar,beta_temp,NFREE,indxc,indxr,ipiv);
	// return gauss_jordan(covar,beta_temp,NFREE);
    }

    // Calculate the chisq at the given values, using the error-function der0
    template <class x_type, class y_type, class par_type, class func_type, class der0_type>
    inline double  calc_chisq(const matrix<x_type> &x, const matrix<x_type> &sigma_x,
			      const matrix<y_type> &y, const matrix<y_type> &sigma_y,
			      const std::vector<par_type> &parameters,
			      const func_type &fitfunc,
			      matrix<y_type> &y_out,
			      der0_type &der0)
    {
	const int NDATA = x.rows();

	double chisq = 0;
	for(int idata=0; idata<NDATA; ++idata)
	{
	    fitfunc(x.row(idata), parameters, y_out.row(idata)); // the last arg is the output

	    chisq += der0(x.row(idata),
			  sigma_x.row(idata),
			  y.row(idata),
			  sigma_y.row(idata),
			  y_out.row(idata));
	}
	return chisq;
    }

    template <class x_type, class y_type, class par_type, class func_type, class der1_type, class der2_type>
    inline void calc_der(const matrix<x_type> &x, const matrix<x_type> &sigma_x,
			 const matrix<y_type> &y, const matrix<y_type> &sigma_y,
			 std::vector<par_type> parameters,
			 const std::vector<bool> &parameter_free,
			 const int NPARS, const int NFREE,
			 matrix<par_type> &alpha, double beta[],
			 const std::vector<func_type> &fitfunc_derivs,
			 const matrix<y_type> &func_values,
			 matrix<y_type> dfda[],
			 der1_type &der1,
			 der2_type &der2)
    // INPUT VARIABLES
    // x(idata,ix)           is the ix-th component of the idata-th X-point
    // sigma_x(idata,ix)     is its error
    // y(idata,iy)           is the iy-th component of the idata-th Y-point
    // sigma_y(idata,iy)     is its error
    // parameters            the set of parameters (0..NPARS)
    // parameter_free        indicates wheter fit with respect to this parameter
    // fitfunc_derivs[i]     is the (multi-valued) derivative with respect to
    //                       the i+1 th parameter
    //                       (i is 0-based, and parameters are numbered from 1!)
    // func_values(idata,iy) the iy-th component of the function's value at the idata-th point
    //                       (at the current values of the parameters)
	
    // TO BE CALCULATED HERE:
    // alpha(i,j)            0.5 x the 'compactified' hessian matrix at the current values of parameters
    //                       'compactified' means that only derivs with respect to the free parameters
    //                       are included, rows/cols corresponding to fixed parameters are dropped
    //                       i,j=0..NFREE
    // beta[i]               0.5 x the 'compactified' gradient of the chi2 function at the current point
    //                       (with respect to free parameters)
    //                       i=0..NFREE
    // dfda[ipar](idata,iy)  is the derivative of the model function's iy-th component
    //                       at the idata-th X-point, with respect to the ipar-th parameter
    //                       ipar=0..NPARS-1
    {

	const int NDATA = y.rows();

	for(int j=0; j<NFREE; ++j)
	{
	    for(int k=0; k<=j; ++k) alpha(j,k).dbl() = 0;
	    beta[j] = 0;
	}


	for(int idata=0; idata<NDATA; ++idata)
	{
	    for(int i1=-1,i1_all=0; i1_all<NPARS; ++i1_all)
	    {
		if(!parameter_free[i1_all])  continue;
		++i1;

		// itt van a hiba!!!!
		fitfunc_derivs[i1_all](x.row(idata), parameters, dfda[i1_all].row(idata));

		beta[i1] -= 0.5* der1(x.row(idata), sigma_x.row(idata),
				      y.row(idata), sigma_y.row(idata),
				      func_values.row(idata),
				      dfda[i1_all].row(idata));
		
		for(int i2=-1,i2_all=0; i2_all <= i1_all; ++i2_all)
		{
		    if(!parameter_free[i2_all]) continue;
		    ++i2;

                    alpha(i1,i2).dbl() += 0.5 * der2(x.row(idata), sigma_x.row(idata),
						     y.row(idata), sigma_y.row(idata),
						     func_values.row(idata),
						     dfda[i1_all].row(idata),
						     dfda[i2_all].row(idata));
		}
	    }
	}
	for(int j=1; j<NFREE; ++j)
	{
	    for(int k=0; k<j; ++k) alpha(k,j).dbl() = alpha(j,k).dbl();
	}
    }
    
    template <class par_type>
    void expand_covar(matrix<par_type> &covar,const std::vector<bool> &parameter_free,int nfit)
    {
	int npars = covar.rows();

	// if all parameters were free (and fitted), nothing
	// has to be done
	if(nfit == npars) return;

	int i = nfit-1;
	for(int i_out = npars-1; i_out >= 0; --i_out)
	{
	    if(!parameter_free[i_out])
	    {
		for(int j=0; j<=i_out; ++j) covar(i_out,j).dbl() = covar(j,i_out).dbl() = 0;
	    }
	    else
	    {
		int k=0;
		for(int j=0; j<=i_out; ++j)
		{
		    if(parameter_free[j]) covar(i_out,j).dbl() = covar(j,i_out).dbl() = covar(i,k++).dbl();
		    else covar(i_out,j).dbl() = covar(j,i_out).dbl() = 0;
		}
		--i;
	    }
	}
    }

    // The Levenberg-Marquardt fitting algorithm. 
    // It returns the number of iterations. 
    template <class x_type, class y_type, class par_type, class func_type, class der0_type, class der1_type, class der2_type, class convfunc_type>
    int levenberg_marquardt(const matrix<x_type> &x, const matrix<x_type> &sigma_x,
			    const matrix<y_type> &y, const matrix<y_type> &sigma_y,
			    std::vector<par_type> &parameters,
			    const std::vector<bool> &parameter_free,
			    matrix<par_type> &covar, double *chisq_out,int verbose,
			    func_type &fitfunc,
			    const std::vector<func_type> &fitfunc_derivs,
			    int fitfunc_components,
			    int maxstep, double lambda,
			    der0_type &der0,
			    der1_type &der1,
			    der2_type &der2,
			    convfunc_type convergence_criterion)
	
    {
	const int NPARS = parameters.size();

	const int NDATA = x.rows();
	// const int NX    = x.cols();
	const int NY    = y.cols();

	// some cross checks
	if(x.rows() != y.rows() ||
	   x.rows() != sigma_x.rows() ||
	   x.rows() != sigma_y.rows())
	{
	    warning::print("Mismatch in the number of data points!","levenberg_marquardt(...)");
	    return 0;
	}

	if(y.cols() != sigma_y.cols())
	{
	    warning::print("# of components in y and sigma_y do not agree","levenberg_marquardt(...)");
	    return 0;
	}
	if(y.cols() != fitfunc_components)
	{
	    warning::print("# of components in y and fitfunc do not agree","levenberg_marquardt(...)");
	    return 0;
	}
	if(x.cols() != sigma_x.cols())
	{
	    warning::print("# of components in x and sigma_x do not agree","levenberg_marquardt(...)");
	    return 0;
	}

	if(covar.rows() != NPARS || covar.cols() != NPARS)
	{
	    warning::print("Mismatch in the number of parameters and covariance matrix dimension","levenberg_marquardt(...)");
	    return 0;
	}

	matrix<par_type> alpha(NPARS,NPARS);

	double *beta             = new double[NPARS];
	double *beta_temp        = new double[NPARS];
	std::vector<blop::var> trial_parameters(NPARS);
	double *delta_parameters = new double[NPARS];

	matrix<y_type> func_values(NDATA,NY);

	double chisq;

	matrix<y_type>::default_nrows(NDATA);
	matrix<y_type>::default_ncols(NY);
	matrix<y_type> *dfda = new matrix<y_type>[NPARS];

	// number of free parameters
	int NFREE=0;
	for(int i=0; i<NPARS; ++i)
	{
	    if(parameter_free[i])
	    {
		++NFREE;
		if(verbose>1) cerr<<"parameter["<<i<<"] is free"<<endl;
	    }
	    else
	    {
		if(verbose>1) cerr<<"parameter["<<i<<"] is fixed"<<endl;
	    }
	}
	if(verbose>1) cerr<<"Free params = "<<NFREE<<endl;

	// buffers for gaussj
	int *indxc = new int[NFREE];
	int *indxr = new int[NFREE];
	int *ipiv  = new int[NFREE];

	/*
	function *func_derivs = new function[NPARS];
	for(int ipar=0; ipar<NPARS; ++ipar)
	{
	    if(parameter_free[ipar]) func_derivs[ipar] = funcs.derivative(-ipar-1);
	    else for(int iy=0; iy<NY; ++iy) func_derivs[ipar].append(function(0.0));    // component(iy,0);
	}
	*/

	if(verbose>2)
	{
	    cerr<<"..: Fitted function: "<<fitfunc<<endl;
	    for(int ipar=0; ipar<NPARS; ++ipar)
	    {
		cerr<<"..: Derivative wrt par #"<<ipar+1<<": "<<fitfunc_derivs[ipar]<<endl;
	    }
	}

	*chisq_out = calc_chisq(x,sigma_x,y,sigma_y,parameters,fitfunc,func_values,der0);

	calc_der(x,sigma_x,y,sigma_y,parameters,parameter_free,NPARS,NFREE,alpha,beta,
		 fitfunc_derivs,func_values,dfda,der1,der2);

	if(verbose>1)
	{
	    cerr<<"..: Fit initialized"<<endl;
	    cerr<<"    Starting parameters: ";
	    for(int i=0; i<NPARS; ++i) cerr<<parameters[i].dbl()<<"  ";
	    cerr<<endl;
	    cerr<<"    Initial chi2 = "<<*chisq_out<<endl;
	}

	if(maxstep <= 0) maxstep = NFREE * 50;

	int it_numb;

	for(it_numb = 0; it_numb < maxstep; ++it_numb)
	{
	    if(verbose>1) cerr<<"..: iterations step #"<<it_numb<<endl;

	    if(invert_hessian(alpha,covar,beta,beta_temp,NFREE,lambda,indxc,indxr,ipiv))
	    {
		if(verbose>1) cerr<<"    gauss_jordan succeeded"<<endl;
		for(int j=0; j<NFREE; ++j) delta_parameters[j] = beta_temp[j];
	    }
	    else
	    {
		if(verbose>1) cerr<<"..: gauss_jordan failed"<<endl;
		double a=0;
		for(int j=0; j<NFREE; ++j)  a += ::fabs(covar(j,j).dbl())*(1+lambda);
		if(a <= 0) a = lambda;
		for(int j=0; j<NFREE; ++j)  delta_parameters[j] = beta[j]/a;
	    }

	    for(int j=0,l=0; l<NPARS; ++l)
	    {
		if(parameter_free[l]) trial_parameters[l].dbl() = parameters[l].dbl()+delta_parameters[j++];
		else trial_parameters[l].dbl() = parameters[l].dbl();
	    }

	    // calculate the chisq with the trial parameters, and store
	    // the evaluated function values in 'func_values'
	    chisq = calc_chisq(x,sigma_x,y,sigma_y,trial_parameters,fitfunc,func_values,der0);

	    if(verbose>1) 
	    {
		cerr<<"    Lambda       = "<<lambda<<endl;
		cerr<<"    Delta params = ";
		for(int i=0; i<NPARS; ++i) cerr<<delta_parameters[i]<<" ";
		cerr<<endl;
		cerr<<"    Trial params = ";
		for(int i=0; i<NPARS; ++i) cerr<<trial_parameters[i].dbl()<<" ";
		cerr<<endl;
		cerr<<"    chi2         = "<<*chisq_out<<"  -->  "<<chisq<<endl;
	    }

	    int converged = convergence_criterion(*chisq_out, chisq, trial_parameters, parameters, it_numb);

	    if(chisq < *chisq_out)
	    {
		if(verbose>1) cerr<<"    Step accepted"<<endl;
		for(int j=0; j<NPARS; ++j) parameters[j].dbl() = trial_parameters[j].dbl();
		calc_der(x,sigma_x,y,sigma_y,parameters,parameter_free,NPARS,NFREE,
			 alpha,beta,fitfunc_derivs,func_values,dfda,der1,der2);
		lambda *= 0.1;
		*chisq_out = chisq;
	    }
	    else
	    {
		if(verbose>1) cerr<<"    Step rejected"<<endl;
		lambda *= 10;
		chisq = *chisq_out;
	    }
	    if(verbose>1) cerr<<endl;

	    if(converged) break;
	}
	if(it_numb >= maxstep && verbose>0) cerr<<"FIT REACHED MAX ITERATION NUMBER: "<<maxstep<<endl;

	if(!invert_hessian(alpha,covar,beta,beta_temp,NFREE,0,indxc,indxr,ipiv))
	{
	    cerr<<"Failed to invert the hessian matrix of chi2!"<<endl;
	    cerr<<"The reported covariance matrix is meaningless"<<endl;
	}
	expand_covar(covar,parameter_free,NFREE);

	delete [] dfda;
	delete [] delta_parameters;
	delete [] beta;
	delete [] beta_temp;
	delete [] indxr;
	delete [] indxc;
	delete [] ipiv;
	return it_numb;
    }

}

#endif
