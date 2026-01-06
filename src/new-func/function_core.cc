#include "function_core.hh"
#include "cfunc_wrapper.hh"
#include "constants.h"
#include <ctime>
#include <algorithm>
#include <stdlib.h>
#ifdef HAVE_GSL
#include <gsl/gsl_sf_bessel.h>
#endif

namespace blop
{
    namespace function_core
    {
	var extra_param::value = "";

	std::vector<blop::var> tmp(20);

	// -----------------  interpolated_func ------------------------------

	interpolated_func::interpolated_func(const interpolated_func &rhs)
	{
	    interpolator_ = (rhs.interpolator_?rhs.interpolator_->clone():0);
	}

	base *interpolated_func::clone() const
	{
	    return new interpolated_func(*this);
	}

	void interpolated_func::eval(const std::vector<blop::var> &args,
				     const std::vector<blop::var> &def_args,
				     const std::vector<blop::var> &params,
				     std::vector<blop::var> &result, int *ind) const
	{
	    result[(*ind)++] = (interpolator_?interpolator_->interpolate(args):0);
	}
	
	void interpolated_func::eval_dbl(const std::vector<blop::var> &args,
					 const std::vector<blop::var> &def_args,
					 const std::vector<blop::var> &params,
					 std::vector<blop::var> &result, int *ind) const
	{
	    result[(*ind)++].dbl() = (interpolator_?interpolator_->interpolate(args):0);
	}

	base *interpolated_func::create_derivative(int i) const
	{
	    interpolated_func *result = new interpolated_func(*this);
	    if(result->interpolator_) result->interpolator_->derivate(i);
	    return result;
	}

	// -------------------  bessel ---------------------------------


	void bessel::setup_bessel_core(int nu, const function &a,
				       bessel::type type, function &result)
	{
	    // this function should be very similar to function::init_unary_
	    result.print_param_value_ = a.print_param_value_;
	    unsigned int n = a.base_.size();
	    result.base_.resize(n);
	    for(unsigned int i=0; i<n; ++i) result.base_[i] = new bessel(a.base_[i],type,nu);
	    result.init_();
	    result.parameters_ = a.parameters_;
	}
	
	void bessel::eval(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &params,
			  std::vector<blop::var> &result,
			  int *ind) const
	{
	    int dummy = 0;
	    operand_->eval(args,def_args,params,tmp,&dummy);
	    const double x = tmp[0].dbl();
	    
	    double res = 0;
	    switch(type_)
	    {
	    case J:
		if(n_==0) res = gsl_sf_bessel_J0(x);
		else if(n_==1) res = gsl_sf_bessel_J1(x);
		else res = gsl_sf_bessel_Jn(n_,x);
		break;
	    case Y:
		if(n_==0) res = gsl_sf_bessel_Y0(x);
		else if(n_==1) res = gsl_sf_bessel_Y1(x);
		else res = gsl_sf_bessel_Yn(n_,x);
		break;
	    case I:
		if(n_==0) res = gsl_sf_bessel_I0(x);
		else if(n_==1) res = gsl_sf_bessel_I1(x);
		else res = gsl_sf_bessel_In(n_,x);
		break;
	    case K:
		if(n_==0) res = gsl_sf_bessel_K0(x);
		else if(n_==1) res = gsl_sf_bessel_K1(x);
		else res = gsl_sf_bessel_Kn(n_,x);
		break;
	    case j:
		if(n_==0) res = gsl_sf_bessel_j0(x);
		else if(n_==1) res = gsl_sf_bessel_j1(x);
		else if(n_==2) res = gsl_sf_bessel_j2(x);
		else res = gsl_sf_bessel_jl(n_,x);
		break;
	    case y:
		if(n_==0) res = gsl_sf_bessel_y0(x);
		else if(n_==1) res = gsl_sf_bessel_y1(x);
		else if(n_==2) res = gsl_sf_bessel_y2(x);
		else res = gsl_sf_bessel_yl(n_,x);
		break;
	    case i_scaled:
		if(n_==0) res = gsl_sf_bessel_i0_scaled(x);
		else if(n_==1) res = gsl_sf_bessel_i1_scaled(x);
		else if(n_==2) res = gsl_sf_bessel_i2_scaled(x);
		else res = gsl_sf_bessel_il_scaled(n_,x);
		break;
	    case k_scaled:
		if(n_==0) res = gsl_sf_bessel_k0_scaled(x);
		else if(n_==1) res = gsl_sf_bessel_k1_scaled(x);
		else if(n_==2) res = gsl_sf_bessel_k2_scaled(x);
		else res = gsl_sf_bessel_kl_scaled(n_,x);
		break;
	    default:
		cerr<<"Only bessel J and Y are implemented at the moment"<<endl;
		break;
	    }
	    result[(*ind)++] = res;
	}		
	void bessel::eval_dbl(const std::vector<blop::var> &args,
			      const std::vector<blop::var> &def_args,
			      const std::vector<blop::var> &params,
			      std::vector<blop::var> &result,
			      int *ind) const
	{
	    int dummy = 0;
	    operand_->eval_dbl(args,def_args,params,tmp,&dummy);
	    const double x = tmp[0].dbl();
	    
	    double res = 0;
	    switch(type_)
	    {
	    case J:
		if(n_==0) res = gsl_sf_bessel_J0(x);
		else if(n_==1) res = gsl_sf_bessel_J1(x);
		else res = gsl_sf_bessel_Jn(n_,x);
		break;
	    case Y:
		if(n_==0) res = gsl_sf_bessel_Y0(x);
		else if(n_==1) res = gsl_sf_bessel_Y1(x);
		else res = gsl_sf_bessel_Yn(n_,x);
		break;
	    default:
		cerr<<"Only bessel J and Y are implemented at the moment"<<endl;
		break;
	    }
	    result[(*ind)++] = res;
	}		
	
	base* bessel::create_my_derivative() const
	{
	    // http://en.wikipedia.org/wiki/Bessel_function#Derivatives_of_J.2CY.2CI.2CH.2CK
	    if(type_ == J)
	    {
		if(n_>=1)
		{
		    Sub *sub = new Sub;
		    sub->left_ = new bessel(operand_->clone(), type_, n_-1);
		    Div *div = new Div;
		    sub->right_ = div;

		    if(n_>1)
		    {
			Mul *mul = new Mul;
			div->left_ = mul;
			mul->left_ = new constant(n_);
			mul->right_ = new bessel(operand_->clone(), type_, n_);
			div->right_ = operand_->clone();
		    }
		    else
		    {
			div->left_ = new bessel(operand_->clone(), type_, n_);
			div->right_ = operand_->clone();
		    }
		    return sub;
		}
		else  // n_ == 0
		{
		    Neg *neg = new Neg;
		    neg->operand_ = new bessel(operand_->clone(), type_, 1);
		    return neg;
		    /*
		      Sub *sub = new Sub;
		      Div *div = new Div;
		      sub->left_ = div;
		      Mul *mul = new Mul;
		      div->left_ = mul;
		      mul->left_ = new constant(n_);
		      mul->right_ = new bessel(operand_->clone(), type_, n_);
		      div->right_ = operand_->clone();
		      sub->right_ = new bessel(operand_->clone(), type_, n_+1);
		      return sub;
		    */
		}
	    }
	    else
	    {
		cerr<<"These bessel derivatives are not yet implemented"<<endl;
	    }
	    return new constant(0.0);
	}
	

	// -----------------  step_func ------------------------------

	step_func::step_func(const vector<double> &x,
			     const vector<double> &y)
	    : x_(x)
	{
	    y_.resize(1);
	    y_[0] = y;
	}

	step_func::step_func(const vector<double> &x,
			     const vector<vector<double> > &y)
	    : x_(x), y_(y)
	{
	}

	base *step_func::clone() const
	{
	    return new step_func(*this);
	}

	void step_func::eval(const std::vector<blop::var> &args,
			     const std::vector<blop::var> &def_args,
			     const std::vector<blop::var> &params,
			     std::vector<blop::var> &result, int *ind) const
	{
	    const double x = args[0].dbl();
	    if(x<x_.front())
	    {
		for(unsigned int i=0; i<y_.size(); ++i) result[(*ind)++] = 0;
		return;
	    }
	    const vector<double>::const_iterator pos = std::lower_bound(x_.begin(), x_.end(), x);
	    const int p=::max((long int)(pos-x_.begin()-1),(long int)0);
	    for(unsigned int i=0; i<y_.size(); ++i)
	    {
		result[(*ind)++] = y_[i][p];
	    }
	}
	
	void step_func::eval_dbl(const std::vector<blop::var> &args,
				 const std::vector<blop::var> &def_args,
				 const std::vector<blop::var> &params,
				 std::vector<blop::var> &result, int *ind) const
	{
	    const double x = args[0].dbl();
	    if(x<x_.front() || x>x_.back())
	    {
		for(unsigned int i=0; i<y_.size(); ++i) result[(*ind)++].dbl() = 0;
		return;
	    }
	    const vector<double>::const_iterator pos = std::lower_bound(x_.begin(), x_.end(), x);
	    const int p=::max((long int)(pos-x_.begin()-1),(long int)0);
	    for(unsigned int i=0; i<y_.size(); ++i)
	    {
		result[(*ind)++].dbl() = y_[i][p];
	    }
	}

	base *step_func::create_derivative(int i) const
	{
	    return new constant(0.0);
	}
		

	// ----------------  random ------------------------------------------

	double random::get_()
	{
	    static bool first_call = true;
	    if(first_call)
	    {
		time_t TIME;
		time(&TIME);
		srand48((unsigned int)(TIME));
		first_call = false;
	    }
	    return drand48();
	}
	    

	// ----------------  char_func ---------------------------------------

	void     char_func::eval_dbl(const std::vector<blop::var> &args,
				     const std::vector<blop::var> &def_args,
				     const std::vector<blop::var> &pars,
				     std::vector<blop::var> &result,
				     int *ind) const
	{
	    const int N = ::max(low_->n_out(),high_->n_out());
	    vector<double> low(N),high(N);

	    int dummy_ind = 0;
	    low_->eval_dbl(args,def_args,pars,tmp,&dummy_ind);
	    for(int i=0; i<dummy_ind; ++i) low[i] = tmp[i].dbl();

	    dummy_ind = 0;
	    high_->eval_dbl(args,def_args,pars,tmp,&dummy_ind);
	    for(int i=0; i<dummy_ind; ++i) high[i] = tmp[i].dbl();

	    int inside = 1;
	    for(int i=0; i<N; ++i)
	    {
		if(args[i].dbl() < low[i] ||
		   (!low_in_ && args[i].dbl() <= low[i]) ||
		   high[i] < args[i].dbl() ||
		   (!high_in_ && high[i] <= args[i].dbl()))
		{
		    inside = 0;
		    break;
		}
	    }
	    result[(*ind)++].dbl() = inside;
	}


	void      char_func::eval(const std::vector<blop::var> &args,
				  const std::vector<blop::var> &def_args,
				  const std::vector<blop::var> &pars,
				  std::vector<blop::var> &result,
				  int *ind) const
	{
	    const int N = ::max(low_->n_out(),high_->n_out());
	    vector<double> low(N),high(N);

	    int dummy_ind = 0;
	    low_->eval(args,def_args,pars,tmp,&dummy_ind);
	    for(int i=0; i<dummy_ind; ++i) low[i] = tmp[i].dbl();

	    dummy_ind = 0;
	    high_->eval(args,def_args,pars,tmp,&dummy_ind);
	    for(int i=0; i<dummy_ind; ++i) high[i] = tmp[i].dbl();


	    int inside = 1;
	    for(int i=0; i<N; ++i)
	    {
		if(args[i].dbl() < low[i] ||
		   (!low_in_ && args[i].dbl() <= low[i]) ||
		   high[i] < args[i].dbl() ||
		   (!high_in_ && high[i] <= args[i].dbl()))
		{
		    inside = 0;
		    break;
		}
	    }
	    result[(*ind)++].dbl() = inside;
	}

	// -----------------  function parameter -----------------------------

	var funcparameter::sprint(const std::vector<blop::var> &pars, bool parvalue) const
	{
	    var result;
	    if(parvalue) // print the actual value of the parameter
	    {
		if(parameter_index_ < 0 || (int)pars.size() <= parameter_index_)
		{
		    var msg = "Function does not have so many [";
		    msg &= var(parameter_index_+1);
		    msg &= "] parameters";
		    warning::print(msg, "function::funcparameter::sprint(...)");
		    result &= "{";
		    result &= var(parameter_index_+1);
		    result &= "}";
		    return result;
		}
		if(pars[parameter_index_].is_dbl()) return pars[parameter_index_];
		result = "'";
		result &= pars[parameter_index_];
		result &= "'";
		return result;
	    }
	    else         // print the parameter symbolically
	    {
		result &= "{";
		result &= var(parameter_index_+1);
		result &= "}";
	    }
	    return result;
	}
	var funcparameter::sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
					const var &, const var &, const var &) const
	{
	    var result;
	    if(parvalue) // print the actual value of the parameter
	    {
		if(parameter_index_ < 0 || (int)pars.size() <= parameter_index_)
		{
		    var msg = "Function does not have so many [";
		    msg &= var(parameter_index_+1);
		    msg &= "] parameters";
		    warning::print(msg, "function::funcparameter::sprint(...)");
		    result &= "\\left[";
		    result &= var(parameter_index_+1);
		    result &= "\\right]";
		    return result;
		}
		if(pars[parameter_index_].is_dbl()) return pars[parameter_index_];
		result = "'";
		result &= pars[parameter_index_];
		result &= "'";
		return result;
	    }
	    else         // print the parameter symbolically
	    {
		result &= "\\left[";
		result &= var(parameter_index_+1);
		result &= "\\right]";
	    }
	    return result;
	}


	// -----------------  argument  --------------------------------------

	bool arg::uses_arg(int i)
	{
	    return ( i-1 == arg_index_);
	}


	// ----------------  component ---------------------------------------

	component::component(const base &b, int i)
	{
	    index_ = i;
	    base_ = b.clone();
	    if((unsigned int)(base_->n_out()) > tmp.size()) tmp.resize(base_->n_out());
	}

	component::~component()
	{
	    delete base_;
	}

	base *component::clone() const
	{
	    return new component(*base_,index_);
	}

	void component::eval(const std::vector<blop::var> &args,
			     const std::vector<blop::var> &def_args,
			     const std::vector<blop::var> &pars,
			     std::vector<blop::var> &result,
			     int *ind) const
	{
	    int dummy = 0;
	    base_->eval(args,def_args,pars,tmp,&dummy);
	    result[(*ind)++] = tmp[index_];
	}

	void component::eval_dbl(const std::vector<blop::var> &args,
				 const std::vector<blop::var> &def_args,
				 const std::vector<blop::var> &pars,
				 std::vector<blop::var> &result,
				 int *ind) const
	{
	    int dummy = 0;
	    base_->eval_dbl(args,def_args,pars,tmp,&dummy);
	    result[(*ind)++].dbl(tmp[index_].dbl());
	}

	var component::sprint(const std::vector<blop::var> &pars, bool parvalue) const
	{
	    var result = var("(") & base_->sprint(pars,parvalue) & var(")[") & var(index_) & var("]");
	    return result;
	}

	base *component::create_derivative(int i) const
	{
	    component *result = new component;
	    result->index_ = index_;
	    result->base_  = base_->create_derivative(i);
	    return result;
	}

	//-----------------  cfunc  ------------------------------------------

	void cfunc::init_arrays_()
	{
	    if(wrapper_ == 0) return;
	    if(wrapper_->n_args() <= 0 || wrapper_->n_out() <= 0)
	    {
		warning::print("This cfunc_wrapper is not designed to work "
			       "together with function");
		delete wrapper_;
		wrapper_ = 0;
		return;
	    }
	    if((unsigned int)(wrapper_->n_out()*2) > tmp.size()) tmp.resize(wrapper_->n_out()*2);
	}

	bool cfunc::equals(const base *o) const
	{
	    const cfunc *c = dynamic_cast<const cfunc *>(o);
	    if(c == 0) return false;
	    if(c == this) return true;
	    return false;
	}

	int cfunc::n_out() const
	{
	    if(wrapper_ == 0)
	    {
		cerr<<"Uninitialized cfunc in cfunc::n_out() "<<endl;
		return 0;
	    }
	    return wrapper_->n_out();
	}

	base* cfunc::create_derivative(int) const
	{
	    return 0;
	}

	var cfunc::sprint(const std::vector<blop::var> &pars, bool parvalue)  const
	{
	    if(wrapper_ == 0) return "UNINITIALIZED_CFUNC";
	    var result = wrapper_->name();
	    result &= var("(");
	    result &= ")";
	    return result;
	}


	bool cfunc::uses_arg(int arg_ind)
	{
	    if(wrapper_ == 0)
	    {
		cerr<<"uninitialized cfunc in cfunc::users_arg(int)"<<endl;
		return 0;
	    }
	    if(wrapper_->n_args() >= arg_ind) return true;
	    return false;
	}


	cfunc::~cfunc()
	{
	    delete wrapper_;
	}

	cfunc::cfunc(cfunc_wrapper_base *w)
	{
	    wrapper_ = w;
	    init_arrays_();
	}

	cfunc::cfunc(const cfunc &o)
	{
	    if(o.wrapper_)	wrapper_ = o.wrapper_->clone();
	    else wrapper_ = 0;
	    init_arrays_();
	}

	cfunc::cfunc()
	{
	    wrapper_ = 0;
	    init_arrays_();
	}

	cfunc::cfunc(void *p)
	{
	    init_(p);
	    init_arrays_();
	}

	cfunc::cfunc(var (*p)(var))
	{
	    wrapper_ = new var_func_var(p);
	    init_arrays_();
	}
	cfunc::cfunc(var (*p)(var,var))
	{
	    wrapper_ = new var_func_var_var(p);
	    init_arrays_();
	}
	cfunc::cfunc(var (*p)(var,var,var))
	{
	    wrapper_ = new var_func_var_var_var(p);
	    init_arrays_();
	}
	cfunc::cfunc(var (*p)(var,var,var,var))
	{
	    wrapper_ = new var_func_var_var_var_var(p);
	    init_arrays_();
	}

	cfunc::cfunc(double (*p)(double))
	{
	    wrapper_ = new double_func_double(p);
	    init_arrays_();
	}
	cfunc::cfunc(double (*p)(double,double))
	{
	    wrapper_ = new double_func_double_double(p);
	    init_arrays_();
	}
	cfunc::cfunc(double (*p)(double,double,double))
	{
	    wrapper_ = new double_func_double_double_double(p);
	    init_arrays_();
	}
	cfunc::cfunc(double (*p)(double,double,double,double))
	{
	    wrapper_ = new double_func_double_double_double_double(p);
	    init_arrays_();
	}

	cfunc::cfunc(complex<double> (*p)(double))
	{
	    wrapper_ = new complex_func_double(p);
	    init_arrays_();
	}

	cfunc::cfunc(var (*p)(const std::vector<blop::var> &args,
			      const std::vector<blop::var> &pars),
		     int nargs, int npars)
	{
	    wrapper_ = new var_func_varvector_varvector(p,nargs,npars);
	}


	int cfunc::nargs() const
	{
	    if(nargs_>=0) return nargs_;
	    if(wrapper_) return wrapper_->n_args();
	    cerr<<"uninitialized cfunc in cfunc::nargs()"<<endl;
	    return 0;
	}

	int cfunc::npars() const
	{
	    if(npars_>=0) return npars_;
	    if(wrapper_) return wrapper_->n_pars();
	    cerr<<"uninitialized cfunc in cfunc::npars()"<<endl;
	    return 0;
	}

	base *cfunc::clone() const
	{
	    return new cfunc(*this);
	}

	// void cfunc::init_(void *) defined in blop_cint.cc and blop_nocint.cc

	void cfunc::eval(const vector<var> &args,
			 const std::vector<blop::var> &def_args,
			 const vector<var> &pars,
			 vector<var> &result, int *ind) const
	{
	    if(wrapper_ == 0)
	    {
		cerr<<"uninitialized cfunc in cfunc::eval(...)"<<endl;
		for(int i=0; i<n_out(); ++i) result[(*ind)++] = "";
		return;
	    }
	    std::vector<blop::var>::iterator i1 = result.begin()+*ind;
	    std::vector<blop::var>::iterator i2 = i1;

	    // make missing argument substitutions. first take all supplied args
	    std::vector<blop::var> def_substituted_args = args;

	    // then take from the default values
	    for(unsigned int i=args.size(); i<def_args.size(); ++i)
	    {
		def_substituted_args.push_back(def_args[i]);
	    }

	    // if the cfunc_wrapper requies even more than this, then appens zeros
	    for(int i=(int)def_substituted_args.size(); i<wrapper_->n_args(); ++i)
	    {
		def_substituted_args.push_back("");
	    }

	    wrapper_->eval(def_substituted_args,pars,i2);
	    *ind += (i2-i1);
	}
	void cfunc::eval_dbl(const vector<var> &args,
			     const std::vector<blop::var> &def_args,
			     const vector<var> &pars,
			     vector<var> &result, int *ind) const
	{
	    if(wrapper_ == 0)
	    {
		cerr<<"uninitialized cfunc in cfunc::eval(...)"<<endl;
		for(int i=0; i<n_out(); ++i) result[(*ind)++].dbl(0.0);
		return;
	    }
	    std::vector<blop::var>::iterator i1 = result.begin()+*ind;
	    std::vector<blop::var>::iterator i2 = i1;

	    // make missing argument substitutions. first take all supplied args
	    std::vector<blop::var> def_substituted_args = args;

	    // then take from the default values
	    for(unsigned int i=args.size(); i<def_args.size(); ++i)
	    {
		def_substituted_args.push_back(def_args[i]);
	    }

	    // if the cfunc_wrapper requies even more than this, then appens zeros
	    for(int i=(int)def_substituted_args.size(); i<wrapper_->n_args(); ++i)
	    {
		def_substituted_args.push_back("");
	    }

	    wrapper_->eval_dbl(def_substituted_args,pars,i2);
	    *ind += (i2-i1);
	}

	// -----------  derivatives  -------------------------------


	base *Atan2::create_derivative_spec(base *leftderiv, base *rightderiv) const
	{
	    // d/dx (atan2(a,b)) = 1/(1+(a/b)^2) * ( a'/b - a*b'/b^2 )
            //                        t1              t2      t3

	    // t1
	    Div *t1 = new Div;
	    {
		t1->left(new constant(1.0));
		Add *add = new Add;
		t1->right(add);
		add->left(new constant(1.0));
		Div *div2 = new Div;
		div2->left(left_->clone());
		div2->right(right_->clone());
		Ipow *ip = new Ipow(2);
		ip->operand(div2);
		add->right(ip);
	    }

	    // t2
	    Div *t2 = new Div;
	    {
		t2->left(leftderiv);
		t2->right(right_->clone());
	    }

	    // t3
	    Div *t3 = new Div;
	    {
		Mul *m = new Mul;
		m->left(left_->clone());
		m->right(rightderiv);
		t3->left(m);
		t3->right(new Ipow(right_,2));
	    }
	    
	    Mul *result = new Mul;
	    result->left(t1);
	    Sub *sub = new Sub;
	    sub->left(t2);
	    sub->right(t3);
	    result->right(sub);
	    return result;
	}

	base *Sub::create_derivative_spec(base *leftderiv, base *rightderiv) const
	{
	    constant *c;
	    if((c=dynamic_cast<constant*>(leftderiv)) != 0 && c->value_ == 0.0)
	    {
		delete leftderiv;
		Neg *n = new Neg;
		n->operand_ = rightderiv;
		return n;
	    }
	    if((c=dynamic_cast<constant*>(rightderiv)) != 0 && c->value_ == 0.0)
	    {
		delete rightderiv;
		return leftderiv;
	    }
	    Sub *result = new Sub;
	    result->left_ = leftderiv;
	    result->right_ = rightderiv;
	    return result;
	}


	base *Div::create_derivative_spec(base *leftderiv, base *rightderiv) const
	{
	    if(dynamic_cast<const constant *>(right_))
	    {
		Div *result = new Div;
		result->left_ = leftderiv;
		result->right_ = right_->clone();
		delete rightderiv;
		return result;
	    }

	    base *a = left_;
	    base *b = right_;

	    base *aprimeb = 0;
	    {
		base *aprime = leftderiv;
		if(constant *aprime_c = dynamic_cast<constant *>(aprime))
		{
		    if(aprime_c->value_ == 0)
		    {
			aprimeb = new constant(0.0);
			delete aprime;
		    }
		    else if(aprime_c->value_ == 1)
		    {
			aprimeb = b->clone();
			delete aprime;
		    }
		}
		if(aprimeb == 0)
		{
		    Mul *m = new Mul;
		    m->left_  = aprime;
		    m->right_ = b->clone();
		    aprimeb = m;
		}
	    }
	    base *abprime = 0;
	    {
		base *bprime = rightderiv;
		if(constant *bprime_c = dynamic_cast<constant *>(bprime))
		{
		    if(bprime_c->value_ == 0)
		    {
			abprime = new constant(0.0);
			delete bprime;
		    }
		    else if(bprime_c->value_ == 1)
		    {
			abprime = a->clone();
			delete bprime;
		    }
		}
		if(abprime == 0)
		{
		    Mul *m = new Mul;
		    m->left_ = a->clone();
		    m->right_ = bprime;
		    abprime = m;
		}
	    }
	    
	    base *nominator = 0;
	    {
		constant *aprimeb_c = dynamic_cast<constant *>(aprimeb);
		constant *abprime_c = dynamic_cast<constant *>(abprime);
		if(nominator==0 && aprimeb_c && abprime_c) nominator = new constant(aprimeb_c->value_.dbl() - abprime_c->value_.dbl());
		if(nominator==0 && aprimeb_c && aprimeb_c->value_ == 0)
		{
		    Neg *n = new Neg;
		    n->operand_ = abprime;
		    nominator = n;
		}
		if(nominator==0 && abprime_c && abprime_c->value_ == 0) nominator = aprimeb;
		if(nominator==0)
		{
		    Sub *s = new Sub;
		    s->left_ = aprimeb;
		    s->right_ = abprime;
		    nominator = s;
		}
	    }

	    Div *result = new Div;
	    result->left_ = nominator;
	    result->right_ = new Ipow(b,2);
	    return result;
	}

	base *Sin::create_my_derivative() const
	{
	    return new Cos(operand_);
	}
	base *Cos::create_my_derivative() const
	{
	    Mul *mul = new Mul;
	    mul->left_ = new constant(-1.0);
	    mul->right_ = new Sin(operand_);
	    return mul;
	}

	var Ipow::sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			       const var &x, const var &y, const var &z) const
	{
	    var result;
	    {
		const arg *a = dynamic_cast<const arg *>(operand_);
		const constant *c = dynamic_cast<const constant *>(operand_);
		bool leftparen = true;
		if(a || c) leftparen = false;
		
		if(leftparen) result &= "\\left(";
		result &= operand_->sprint_latex(pars, parvalue, x, y, z);
		if(leftparen) result &= "\\right)";
	    }
	    result &= "^{";
	    result &= exponent_;
	    result &= "} ";
	    return result;
	}

	base* Ipow::create_my_derivative() const
	{
	    if(exponent_ == 0) return new constant(0.0);
	    Mul *mul = new Mul;
	    mul->left_ = new constant(exponent_);
	    if(exponent_ == 2) mul->right_ = operand_->clone();
	    else mul->right_ = new Ipow(operand_,exponent_-1);
	    return mul;
	}

	var Pow::sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			      const var &x, const var &y, const var &z) const
	{
	    var result;
	    
	    {
		const arg *a = dynamic_cast<const arg *>(left_);
		const constant *c = dynamic_cast<const constant *>(left_);
		bool leftparen = true;
		if(a || c) leftparen = false;
		
		if(leftparen) result &= "\\left(";
		result &= left_->sprint_latex(pars, parvalue, x, y, z);
		if(leftparen) result &= "\\right)";
	    }
	    result &= "^{";
	    result &= right_->sprint_latex(pars, parvalue, x, y, z);
	    result &= "} ";
	    
	    return result;
	}


	base* Pow::create_derivative_spec(base *aprime, base *bprime) const
	{
	    // (a^b)' = a^b * [ b' * ln(a) + b * a'/a ]
	    Mul *mul = new Mul;
	    Pow *atob = new Pow(left_,right_);
	    mul->left_ = atob;
	    Add *add  = new Add;
	    mul->right_ = add;
	    Mul *mul1 = new Mul;
	    add->left_ = mul1;
	    Mul *mul2 = new Mul;
	    add->right_ = mul2;

	    mul1 -> left_ = bprime;
	    mul1 -> right_ = new Log(left_);

	    mul2 -> left_ = right_->clone();
	    Div *div = new Div;
	    mul2 -> right_ = div;
	    div -> left_ = aprime;
	    div -> right_ = left_->clone();

	    return mul;
	}

	base *Tan::create_my_derivative() const
	{
	    // 1/cos^2
	    Div *div = new Div;
	    div->left(new constant(1.0));
	    Ipow *ipow = new Ipow(2);
	    div->right(ipow);
	    Cos *the_cos = new Cos(operand_);
	    ipow->operand(the_cos);
	    return div;
	}

	base *Atan::create_my_derivative() const
	{
	    // 1/(1+x^2)
	    Div *div = new Div;
	    div->left(new constant(1.0));
	    Add *add = new Add;
	    div->right(add);
	    add->left(new constant(1.0));
	    add->right(new Ipow(operand_,2));
	    return div;
	}

	base *Cot::create_my_derivative() const
	{
	    // -1/sin^2
	    Div *div = new Div;
	    div->left(new constant(-1.0));
	    Ipow *ipow = new Ipow(2);
	    div->right(ipow);
	    Sin *the_sin = new Sin(operand_);
	    ipow->operand(the_sin);
	    return div;
	}


	base *Acot::create_my_derivative() const
	{
	    // -1/(1+x^2)
	    Div *div = new Div;
	    div->left_ = new constant(-1.0);
	    Add *add = new Add;
	    div->right_ = add;
	    add->left_ = new constant(1.0);
	    add->right_ = new Ipow(operand_,2);
	    return div;
	}

	base *Asin::create_my_derivative() const
	{
	    // 1/sqrt(1-x^2)
	    Div *div = new Div;
	    div->left_ = new constant(1.0);
	    Sqrt *sq = new Sqrt;
	    div->right_ = sq;
	    Sub *sub = new Sub;
	    sq->operand_ = sub;
	    sub->left_ = new constant(1.0);
	    sub->right_ = new Ipow(operand_,2);
	    return div;
	}

	base *Acos::create_my_derivative() const
	{
	    // -1/sqrt(1-x^2);
	    Div *div = new Div;
	    div->left_ = new constant(-1.0);
	    Sqrt *sq = new Sqrt;
	    div->right_ = sq;
	    Sub *sub = new Sub;
	    sq->operand_ = sub;
	    sub->left_ = new constant(1.0);
	    sub->right_ = new Ipow(operand_,2);
	    return div;
	}
    


	// -------------------------  argument_subst  ----------------------------------------

	bool argument_subst::uses_arg(int n)
	{
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		if(args_[i]->uses_arg(n)) return true;
	    }
	    return false;
	}
	bool argument_subst::uses_par(int n)
	{
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		if(args_[i]->uses_par (n)) return true;
	    }
	    return false;
	}

	void argument_subst::init_tmp_()
	{
	    int n=0;
	    for(unsigned int i=0; i<args_.size(); ++i) n += args_[i]->n_out();
	    tmp_.resize(n);
	}

	bool argument_subst::equals(const base *o) const
	{
	    const argument_subst *c = dynamic_cast<const argument_subst *>(o);
	    if(c == 0) return false;
	    if(! (base_->equals(c->base_)) ) return false;
	    if(args_.size() != c->args_.size()) return false;
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		if(!(args_[i]->equals(c->args_[i]))) return false;
	    }
	    return true;
	}

	var argument_subst::sprint(const std::vector<blop::var> &pars, bool parvalue) const
	{
	    var result = base_->sprint(pars, parvalue);
	    result &= "[";
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		if(i == 0) result &= " ";
		else result &= " , ";
		result &= "ARG(";
		result &= var(i+1);
		result &= ") ==> ";
		result &= args_[i]->sprint(pars, parvalue);
	    }
	    result &= " ]";
	    return result;
	}

	var argument_subst::sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
					 const var &x, const var &y, const var &z) const
	{
	    var result = base_->sprint_latex(pars, parvalue, x, y, z);
	    result &= "[";
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		if(i == 0) result &= " ";
		else result &= " , ";
		result &= "ARG(";
		result &= var(i+1);
		result &= ") ==> ";
		result &= args_[i]->sprint_latex(pars, parvalue, x, y, z);
	    }
	    result &= " ]";
	    return result;
	}


	base* argument_subst::create_derivative(int ider) const
	{
	    base *result = 0;
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		base *partial_deriv = base_->create_derivative(i+1);
		if(partial_deriv == 0)
		{
		    if(result) delete result;
		    return 0;
		}
		base *arg_deriv = args_[i]->create_derivative(ider);
		if(arg_deriv == 0)
		{
		    if(result) delete result;
		    return 0;
		}

		base *term = 0;
		{
		    constant *c_f = dynamic_cast<constant *>(partial_deriv);
		    constant *c_a = dynamic_cast<constant *>(arg_deriv);
		
		    if(c_f && c_a) {  term = new constant(c_f->value_.dbl() * c_a->value_.dbl()); delete c_a; delete c_f; }
		    if(term == 0 && c_f && c_f->value_ == 1.0) { delete partial_deriv; term = arg_deriv; }
		    if(term == 0 && c_f && c_f->value_ == 0.0) { delete c_f; delete c_a; term = new constant(0.0); }
		    if(term == 0 && c_a && c_a->value_ == 1.0)
		    {
			delete arg_deriv;
			argument_subst *asub = new argument_subst;
			asub->base_ = partial_deriv;
			asub->args_.resize(args_.size());
			for(unsigned int a=0; a<args_.size(); ++a) asub->args_[a] = args_[a]->clone();
			term =asub;
		    }
		    if(term == 0 && c_a && c_a->value_ == 0.0) { delete c_f; delete c_a; term = new constant(0.0); }
		    if(term == 0)
		    {
			argument_subst *asub = new argument_subst;
			asub->base_ = partial_deriv;
			asub->args_.resize(args_.size());
			for(unsigned int a=0; a<args_.size(); ++a) asub->args_[a] = args_[a]->clone();
			Mul *m = new Mul;
			m->left_ = asub;
			m->right_ = arg_deriv;
			term = m;
		    }
		}

		if(result == 0) result = term;
		else
		{
		    // if the current term is not constant 0, add it to the result
		    constant *c = dynamic_cast<constant *>(term);
		    if(!(c && c->value_ == 0.0 ))
		    {
			Add *a = new Add;
			a->left_ = result;
			a->right_ = term;
			result = a;
		    }
		}
	    }
	    return result;
	}

	argument_subst::argument_subst(base *f,int n,const base *ar[])
	{
	    base_ = f->clone();
	    for(int i=0; i<n; ++i) args_.push_back(ar[i]->clone());
/*
  if(f->nargs() > n)
  {
  warning::print("Too few arguments in argument substitution."
  "Missing arguments will be 0!",
  "argument_subst::argument_subst(base *, int, base *[])");
  for(int i=n; i<f->nargs(); ++i) args_.push_back(new constant(0.0));
  }
*/
	    init_tmp_();
	}

	argument_subst::argument_subst(base *f,const std::vector<base*> &ar)
	{
	    base_ = f->clone();
	    for(unsigned int i=0; i<ar.size(); ++i) args_.push_back(ar[i]->clone());
/*
  if((unsigned int)(f->nargs()) > ar.size())
  {
  warning::print("Too few arguments in argument substitution."
  "Missing arguments will be 0!",
  "argument_subst::argument_subst(base *, int, base *[])");
  for(unsigned int i=ar.size(); i<(unsigned int)(f->nargs()); ++i) args_.push_back(new constant(0.0));
  }
*/
	    init_tmp_();
	}

	argument_subst::argument_subst(const argument_subst &o)
	{
	    base_ = o.base_->clone();
	    for(unsigned int i=0; i<o.args_.size(); ++i)
	    {
		args_.push_back(o.args_[i]->clone());
	    }
	    init_tmp_();
	}

	argument_subst::~argument_subst()
	{
	    delete base_;
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		delete args_[i];
	    }
	}

	void argument_subst::eval(const std::vector<blop::var> &arg,
				  const std::vector<blop::var> &def_args,
				  const vector<var> &params,
				  std::vector<blop::var> &result,int *ind) const
	{
	    if(!base_)
	    {
		warning::print("Uninitialized base","argument_subst::eval(...)");
		result[(*ind)++] = "";
		return;
	    }

	    int runind = 0;
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		args_[i]->eval(arg,def_args,params,tmp_,&runind);
	    }
	    base_->eval(tmp_,def_args,params,result,ind);
	}

	void argument_subst::eval_dbl(const std::vector<blop::var> &args,
				      const std::vector<blop::var> &def_args,
				      const std::vector<blop::var> &pars,
				      std::vector<blop::var> &result,int *ind) const
	{
	    if(!base_)
	    {
		warning::print("Uninitialized base","argument_subst::eval(...)");
		result[(*ind)++].dbl(0.0);
		return;
	    }

	    int runind = 0;
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		args_[i]->eval_dbl(args,def_args,pars,tmp_,&runind);
	    }
	    base_->eval_dbl(tmp_, def_args, pars, result, ind);
	}

	int argument_subst::nargs() const
	{
	    if(nargs_>=0) return nargs_;
	    int n=0;
	    for(unsigned int i=0; i<args_.size(); ++i) if(args_[i]->nargs() > n) n = args_[i]->nargs();
	    return n;
	}
	int argument_subst::npars() const
	{
	    if(npars_>=0) return npars_;
	    int n=0;
	    for(unsigned int i=0; i<args_.size(); ++i) if(args_[i]->npars() > n) n = args_[i]->npars();
	    if(base_ && base_->npars() > n) n = base_->npars();
	    return n;
	}

	int argument_subst::n_out() const
	{
	    return base_->n_out();
	}


	base *Sinh::create_my_derivative() const { return new Cosh(operand_); }
	base *Cosh::create_my_derivative() const { return new Sinh(operand_); }
	base *Asinh::create_my_derivative() const
	{ cerr<<"Asinh derivative not yet implemented"<<endl; return new constant(0.0); }
	base *Acosh::create_my_derivative() const
	{ cerr<<"Acosh derivative not yet implemented"<<endl; return new constant(0.0); }
	base *Tanh::create_my_derivative() const
	{ cerr<<"Tanh derivative not yet implemented"<<endl; return new constant(0.0); }
	base *Atanh::create_my_derivative() const
	{ cerr<<"Tanh derivative not yet implemented"<<endl; return new constant(0.0); }
	base *Erf::create_my_derivative() const
	{ cerr<<"Erf derivative not yet implemented"<<endl; return new constant(0.0); }

	double cot(double a) { return 1.0/::tan(a); }
	double acot(double a) { return ::atan(1/a); }


	var binary_operator::sprint(const std::vector<blop::var> &pars, bool parvalue) const
	{
	    bool left_paren = true;
	    if(dynamic_cast<const constant*>(left_) ||
	       dynamic_cast<const arg*>(left_) ||
	       dynamic_cast<const funcparameter *>(left_) ) left_paren = false;

	    bool right_paren = true;
	    if(dynamic_cast<const constant*>(right_) ||
	       dynamic_cast<const arg*>(right_) ||
	       dynamic_cast<const funcparameter *>(right_)) right_paren = false;

	    if( name_ == "*" || name_ == "/" ) 
	    {
		if(dynamic_cast<const Mul *>(left_) ||
		   dynamic_cast<const Div *>(left_) ||
		   dynamic_cast<const Pow *>(left_) ||
		   dynamic_cast<const Ipow*>(left_)) left_paren = false;
		if(dynamic_cast<const Pow *>(right_) ||
		   dynamic_cast<const Ipow*>(right_)) right_paren = false;
		if(name_ == "*")
		{
		    if(dynamic_cast<const Div *>(right_) ||
		       dynamic_cast<const Mul *>(right_)) right_paren = false;
		}

	    }
	    if( name_ == "+" || name_ == "-" )
	    {
		if(dynamic_cast<const Add *>(left_) ||
		   dynamic_cast<const Sub *>(left_) ||
		   dynamic_cast<const Mul *>(left_) ||
		   dynamic_cast<const Div *>(left_) ||
		   dynamic_cast<const Pow *>(left_) ||
		   dynamic_cast<const Ipow*>(left_)) left_paren = false;
		if(name_ == "+" && (
		       dynamic_cast<const Add *>(right_) ||
		       dynamic_cast<const Sub *>(right_) ||
		       dynamic_cast<const Mul *>(right_) ||
		       dynamic_cast<const Div *>(right_) ||
		       dynamic_cast<const Pow *>(right_) ||
		       dynamic_cast<const Ipow*>(right_))) right_paren = false;
		if(name_ == "-" && (
		       dynamic_cast<const Mul *>(right_) ||
		       dynamic_cast<const Div *>(right_) ||
		       dynamic_cast<const Pow *>(right_) ||
		       dynamic_cast<const Ipow*>(right_))) right_paren = false;
	    }

	    if(dynamic_cast<const unary_c_function *>(left_)) left_paren = false;
	    if(dynamic_cast<const unary_c_function *>(right_)) right_paren = false;
	    if(dynamic_cast<const binary_function *>(left_)) left_paren = false;
	    //if(dynamic_cast<const binary_function *>(right_)) right_paren = false;

	    var result;
	    if(left_paren) result &= "(";
	    result &= left_->sprint(pars, parvalue);
	    if(left_paren) result &= ")";
	    result &= name_;
	    if(right_paren) result &= "(";
	    result &= right_->sprint(pars, parvalue);
	    if(right_paren) result &= ")";
	    return result;
	}
	var binary_operator::sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
					  const var &x, const var &y, const var &z) const
	{
	    bool left_paren = true;
	    if(dynamic_cast<const constant*>(left_) ||
	       dynamic_cast<const arg*>(left_) ||
	       dynamic_cast<const funcparameter *>(left_) ) left_paren = false;

	    bool right_paren = true;
	    if(dynamic_cast<const constant*>(right_) ||
	       dynamic_cast<const arg*>(right_) ||
	       dynamic_cast<const funcparameter *>(right_)) right_paren = false;

	    if( name_ == "*" || name_ == "/" ) 
	    {
		if(dynamic_cast<const Mul *>(left_) ||
		   dynamic_cast<const Div *>(left_) ||
		   dynamic_cast<const Pow *>(left_) ||
		   dynamic_cast<const Ipow*>(left_)) left_paren = false;
		if(dynamic_cast<const Pow *>(right_) ||
		   dynamic_cast<const Ipow*>(right_)) right_paren = false;
		if(name_ == "*")
		{
		    if(dynamic_cast<const Div *>(right_) ||
		       dynamic_cast<const Mul *>(right_)) right_paren = false;
		}
	    }
	    if( name_ == "+" || name_ == "-" )
	    {
		if(dynamic_cast<const Add *>(left_) ||
		   dynamic_cast<const Sub *>(left_) ||
		   dynamic_cast<const Mul *>(left_) ||
		   dynamic_cast<const Div *>(left_) ||
		   dynamic_cast<const Pow *>(left_) ||
		   dynamic_cast<const Ipow*>(left_)) left_paren = false;
		if(name_ == "+" && (
		       dynamic_cast<const Add *>(right_) ||
		       dynamic_cast<const Sub *>(right_) ||
		       dynamic_cast<const Mul *>(right_) ||
		       dynamic_cast<const Div *>(right_) ||
		       dynamic_cast<const Pow *>(right_) ||
		       dynamic_cast<const Ipow*>(right_))) right_paren = false;
		if(name_ == "-" && (
		       dynamic_cast<const Mul *>(right_) ||
		       dynamic_cast<const Div *>(right_) ||
		       dynamic_cast<const Pow *>(right_) ||
		       dynamic_cast<const Ipow*>(right_))) right_paren = false;
	    }

	    if(dynamic_cast<const unary_c_function *>(left_)) left_paren = false;
	    if(dynamic_cast<const unary_c_function *>(right_)) right_paren = false;
	    if(dynamic_cast<const binary_function *>(left_)) left_paren = false;
	    //if(dynamic_cast<const binary_function *>(right_)) right_paren = false;

	    var result;
	    if(left_paren) result &= "\\left(";
	    result &= left_->sprint_latex(pars, parvalue,x,y,z);
	    if(left_paren) result &= "\\right)";
	    if(name_.str() == "*") result &= " \\cdot ";
	    else result &= name_;
	    if(right_paren) result &= "\\left(";
	    result &= right_->sprint_latex(pars, parvalue,x,y,z);
	    if(right_paren) result &= "\\right)";
	    return result;
	}


	// -------------------- contained_in -------------------------------------

	contained_in::contained_in(const std::vector<base*> &a)
	{
	    for(unsigned int i=0; i<a.size(); ++i) in_this_.push_back(a[i]->clone());
	}

	contained_in::contained_in(const contained_in &o)
	{
	    for(unsigned int i=0; i<o.in_this_.size(); ++i) in_this_.push_back(o.in_this_[i]->clone());
	}

	contained_in::~contained_in()
	{
	    for(unsigned int i=0; i<in_this_.size(); ++i) delete in_this_[i];
	}

	// evaluate
	void contained_in::eval(const std::vector<blop::var> &args,
				const std::vector<blop::var> &def_args,
				const std::vector<blop::var> &pars,
				std::vector<blop::var> &result,
				int *ind) const
	{
	    vector<var> orig_args = args;
	    for(unsigned int i=0; i<in_this_.size(); ++i)
	    {
		int runind = 0;
		in_this_[i]->eval(orig_args, def_args, pars, tmp, &runind);
		for(int c=0; c<runind; ++c)
		{
		    if(orig_args[0].str() == tmp[c].str())
		    {
			result[(*ind)++] = "1";
			return;
		    }
		}
	    }
	    result[(*ind)++] = "0";
	}

	void contained_in::eval_dbl(const std::vector<blop::var> &args,
				    const std::vector<blop::var> &def_args,
				    const std::vector<blop::var> &pars,
				    std::vector<blop::var> &result,
				    int *ind) const
	{
	    eval(args,def_args,pars,result,ind);
	}

	int contained_in::nargs() const
	{
	    if(nargs_>=0) return nargs_;
	    int n=0;
	    for(unsigned int i=0; i<in_this_.size(); ++i)
		if(in_this_[i]->nargs() > n) n = in_this_[i]->nargs();
	    return n;
	}
	int contained_in::npars() const
	{
	    if(npars_>=0) return npars_;
	    int n=0;
	    for(unsigned int i=0; i<in_this_.size(); ++i)
		if(in_this_[i]->npars() > n) n = in_this_[i]->npars();
	    return n;
	}

	var contained_in::sprint(const std::vector<blop::var> &pars, bool print_parvalue) const
	{
	    var result = "contained_in{";
	    for(unsigned int i=0; i<in_this_.size(); ++i)
	    {
		if(i>0) result &= ",";
		result &= in_this_[i]->sprint(pars,print_parvalue);
	    }
	    result &= "}";
	    return result;
	}

	// ------------------------- value_in_interval --------------------------
	value_in_interval::value_in_interval(const std::vector<base*> &func,
					     const std::vector<base*> &from,
					     const std::vector<base*> &to,
					     const std::vector<base*> &step,
					     const std::string &name)
	{
	    func_.resize(func.size());
	    from_.resize(from.size());
	    to_  .resize(to  .size());
	    step_.resize(step.size());
	    for(unsigned int i=0; i<func.size(); ++i) func_[i] = func[i]->clone();
	    for(unsigned int i=0; i<from.size(); ++i) from_[i] = from[i]->clone();
	    for(unsigned int i=0; i<to.size(); ++i) to_[i] = to[i]->clone();
	    for(unsigned int i=0; i<step.size(); ++i) step_[i] = step[i]->clone();
	    name_ = name;
	}
	value_in_interval::value_in_interval(const value_in_interval &rhs)
	{
	    func_.resize(rhs.func_.size());
	    from_.resize(rhs.from_.size());
	    to_  .resize(rhs.to_  .size());
	    step_.resize(rhs.step_.size());
	    for(unsigned int i=0; i<rhs.func_.size(); ++i) func_[i] = rhs.func_[i]->clone();
	    for(unsigned int i=0; i<rhs.from_.size(); ++i) from_[i] = rhs.from_[i]->clone();
	    for(unsigned int i=0; i<rhs.to_  .size(); ++i) to_[i]   = rhs.to_  [i]->clone();
	    for(unsigned int i=0; i<rhs.step_.size(); ++i) step_[i] = rhs.step_[i]->clone();
	    name_ = rhs.name_;
	}
	int value_in_interval::nargs() const
	{
	    if(nargs_>=0)
	    {
		cerr<<"nargs set by user?"<<endl;
		return nargs_;
	    }
	    int result = 0;
	    //for(unsigned int i=0; i<func_.size(); ++i)
	    //    if(func_[i]->nargs()>result) result = func_[i]->nargs();
	    for(unsigned int i=0; i<from_.size(); ++i)
		if(from_[i]->nargs()>result) result = from_[i]->nargs();
	    for(unsigned int i=0; i<to_.size(); ++i)
		if(to_[i]->nargs()>result) result = to_[i]->nargs();
	    for(unsigned int i=0; i<step_.size(); ++i)
		if(step_[i]->nargs()>result) result = step_[i]->nargs();
	    return result;
	}
	int value_in_interval::npars() const
	{
	    if(npars_>=0) return npars_;
	    int result = 0;
	    //for(unsigned int i=0; i<func_.size(); ++i)
	    //    if(func_[i]->npars()>result) result = func_[i]->npars();
	    for(unsigned int i=0; i<from_.size(); ++i)
		if(from_[i]->npars()>result) result = from_[i]->npars();
	    for(unsigned int i=0; i<to_.size(); ++i)
		if(to_[i]->npars()>result) result = to_[i]->npars();
	    for(unsigned int i=0; i<step_.size(); ++i)
		if(step_[i]->npars()>result) result = step_[i]->npars();
	    return result;
	}
	bool value_in_interval::uses_arg(int argno)
	{
	    for(unsigned int i=0; i<from_.size(); ++i) if(from_[i]->uses_arg(argno)) return true;
	    for(unsigned int i=0; i<to_  .size(); ++i) if(to_  [i]->uses_arg(argno)) return true;
	    for(unsigned int i=0; i<step_.size(); ++i) if(step_[i]->uses_arg(argno)) return true;
	    return false;
	}
	bool value_in_interval::uses_par(int parno)
	{
	    for(unsigned int i=0; i<from_.size(); ++i) if(from_[i]->uses_par(parno)) return true;
	    for(unsigned int i=0; i<to_  .size(); ++i) if(to_  [i]->uses_par(parno)) return true;
	    for(unsigned int i=0; i<step_.size(); ++i) if(step_[i]->uses_par(parno)) return true;
	    return false;
	}
	int value_in_interval::n_out() const
	{
	    int result = 0;
	    for(unsigned int i=0; i<func_.size(); ++i) result += func_[i]->n_out();
	    return result;
	}
	var value_in_interval::sprint(const std::vector<blop::var> &p, bool v) const
	{
	    var result = var(name_) & "(";

	    if(func_.size()>1) result &= "[";
	    for(unsigned int i=0; i<func_.size(); ++i)
	    {
		if(i>0) result &= ",";
		result &= func_[i]->sprint(p,v);
	    }
	    if(func_.size()>1) result &= "]";

	    result &= ",";

	    if(from_.size()>1) result &= "[";
	    for(unsigned int i=0; i<from_.size(); ++i)
	    {
		if(i>0) result &= ",";
		result &= from_[i]->sprint(p,v);
	    }
	    if(from_.size()>1) result &= "]";

	    result &= ",";

	    if(to_.size()>1) result &= "[";
	    for(unsigned int i=0; i<to_.size(); ++i)
	    {
		if(i>0) result &= ",";
		result &= to_[i]->sprint(p,v);
	    }
	    if(to_.size()>1) result &= "]";

	    result &= ",";

	    if(step_.size()>1) result &= "[";
	    for(unsigned int i=0; i<step_.size(); ++i)
	    {
		if(i>0) result &= ",";
		result &= step_[i]->sprint(p,v);
	    }
	    if(step_.size()>1) result &= "]";

	    result &= ")";
	    return result;
	}
	var value_in_interval::sprint_latex(const std::vector<blop::var> &p, bool v,
					    const var &x, const var &y, const var &z) const
	{
	    var result = var("\\mathrm{") & var(name_) & "}\\left(";

	    if(func_.size()>1) result &= "\\left[";
	    for(unsigned int i=0; i<func_.size(); ++i)
	    {
		if(i>0) result &= ",";
		result &= func_[i]->sprint_latex(p,v,x,y,z);
	    }
	    if(func_.size()>1) result &= "\\right]";

	    result &= ",";

	    if(from_.size()>1) result &= "\\left[";
	    for(unsigned int i=0; i<from_.size(); ++i)
	    {
		if(i>0) result &= ",";
		result &= from_[i]->sprint_latex(p,v,x,y,z);
	    }
	    if(from_.size()>1) result &= "\\right]";

	    result &= ",";

	    if(to_.size()>1) result &= "\\left[";
	    for(unsigned int i=0; i<to_.size(); ++i)
	    {
		if(i>0) result &= ",";
		result &= to_[i]->sprint_latex(p,v,x,y,z);
	    }
	    if(to_.size()>1) result &= "\\right]";

	    result &= ",";

	    if(step_.size()>1) result &= "\\left[";
	    for(unsigned int i=0; i<step_.size(); ++i)
	    {
		if(i>0) result &= ",";
		result &= step_[i]->sprint_latex(p,v,x,y,z);
	    }
	    if(step_.size()>1) result &= "\\right]";

	    result &= "\\right)";

	    return result;
	}
	void value_in_interval::eval(const std::vector<blop::var> &args,
				     const std::vector<blop::var> &def_args,
				     const std::vector<blop::var> &pars,
				     std::vector<blop::var> &res,
				     int *ind) const
	{
	    int nout_func = 0;
	    for(unsigned int i=0; i<func_.size(); ++i) nout_func += func_[i]->n_out();
	    int nout_from = 0;
	    for(unsigned int i=0; i<from_.size(); ++i) nout_from += from_[i]->n_out();
	    int nout_to = 0;
	    for(unsigned int i=0; i<to_  .size(); ++i) nout_to   += to_  [i]->n_out();
	    int nout_step = 0;
	    for(unsigned int i=0; i<step_.size(); ++i) nout_step += step_[i]->n_out();

	    std::vector<blop::var>
		from(nout_from),
		to  (nout_to  ),
		step(nout_step);

	    {
		int ind_tmp = 0;
		for(unsigned int i=0; i<from_.size(); ++i)
		    from_[i]->eval(args, def_args, pars, from, &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		for(unsigned int i=0; i<to_  .size(); ++i)
		    to_  [i]->eval(args, def_args, pars, to  , &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		for(unsigned int i=0; i<step_.size(); ++i)
		    step_[i]->eval(args, def_args, pars, step, &ind_tmp);
	    }
		
	    from.resize(::min(nout_from,::min(nout_to,nout_step)));
	    to  .resize(::min(nout_from,::min(nout_to,nout_step)));
	    step.resize(::min(nout_from,::min(nout_to,nout_step)));
		
	    if(evaluate_at_interval_centers())
	    {
		for(unsigned int i=0; i<from.size(); ++i)
		{
		    if(step[i].dbl()>0) from[i] += step[i].dbl()*0.5;
		}
	    }
		
	    std::vector<blop::var> final_result  (n_out());
	    std::vector<blop::var> current_result(n_out());

	    std::vector<blop::var> func_args = from;
	    int count = 0;
	    for(bool could_increment=true; could_increment; ++count)
	    {
		int ind_tmp = 0;
		for(unsigned int i=0; i<func_.size(); ++i)
		{
		    func_[i]->eval(func_args, def_args, pars, current_result, &ind_tmp);
		}
		accumulate_result(count,func_args, step, current_result, final_result);

		could_increment = false;
		for(unsigned int i=0; i<func_args.size(); ++i)
		{
		    // If this argument is not to be incremented, skip it.
		    if(step[i].dbl()<=0) continue;

		    // otherwise increment the given argument by the
		    // corresponding step
		    func_args[i] += step[i];

		    // If 'to' (upper limit of the loop of this argument)
		    // has not been reached, everything is good, stp here
		    if(func_args[i].dbl() < to[i].dbl()+step[i].dbl()*0.1)
		    {
			could_increment = true;
			break;
		    }

		    // Otherwise set it back to the starting value, and let
		    // the loop continue, i.e. increase the next higher (free)
		    // argument
		    func_args[i] = from[i];
		}
	    }

	    for(unsigned int i=0; i<final_result.size(); ++i)
	    {
		res[(*ind)++] = final_result[i];
	    }
	}

	void value_in_interval::eval_dbl(const std::vector<blop::var> &args,
					 const std::vector<blop::var> &def_args,
					 const std::vector<blop::var> &pars,
					 std::vector<blop::var> &res,
					 int *ind) const
	{
	    int nout_func = 0;
	    for(unsigned int i=0; i<func_.size(); ++i) nout_func += func_[i]->n_out();
	    int nout_from = 0;
	    for(unsigned int i=0; i<from_.size(); ++i) nout_from += from_[i]->n_out();
	    int nout_to = 0;
	    for(unsigned int i=0; i<to_  .size(); ++i) nout_to   += to_  [i]->n_out();
	    int nout_step = 0;
	    for(unsigned int i=0; i<step_.size(); ++i) nout_step += step_[i]->n_out();

	    std::vector<blop::var>
		from(nout_from),
		to  (nout_to  ),
		step(nout_step);

	    {
		int ind_tmp = 0;
		for(unsigned int i=0; i<from_.size(); ++i)
		    from_[i]->eval_dbl(args, def_args, pars, from, &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		for(unsigned int i=0; i<to_  .size(); ++i)
		    to_  [i]->eval_dbl(args, def_args, pars, to  , &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		for(unsigned int i=0; i<step_.size(); ++i)
		    step_[i]->eval_dbl(args, def_args, pars, step, &ind_tmp);
	    }
		
	    from.resize(::min(nout_from,::min(nout_to,nout_step)));
	    to  .resize(::min(nout_from,::min(nout_to,nout_step)));
	    step.resize(::min(nout_from,::min(nout_to,nout_step)));
		
	    if(evaluate_at_interval_centers())
	    {
		for(unsigned int i=0; i<from.size(); ++i)
		{
		    if(step[i].dbl()>0) from[i].dbl(from[i].dbl()+step[i].dbl()*0.5);
		}
	    }
		
	    std::vector<blop::var> final_result  (n_out());
	    std::vector<blop::var> current_result(n_out());

	    std::vector<blop::var> func_args = from;
	    int count = 0;
	    for(bool could_increment=true; could_increment; ++count)
	    {
		int ind_tmp = 0;
		for(unsigned int i=0; i<func_.size(); ++i)
		{
		    func_[i]->eval_dbl(func_args, def_args, pars, current_result, &ind_tmp);
		}
		accumulate_result_dbl(count,func_args, step, current_result, final_result);

		could_increment = false;
		for(unsigned int i=0; i<func_args.size(); ++i)
		{
		    // If this argument is not to be incremented, skip it.
		    if(step[i].dbl()<=0) continue;

		    // otherwise increment the given argument by the
		    // corresponding step
		    func_args[i].dbl(func_args[i].dbl()+step[i].dbl());

		    // If 'to' (upper limit of the loop of this argument)
		    // has not been reached, everything is good, stp here
		    if(func_args[i].dbl() < to[i].dbl()+step[i].dbl()*0.1)
		    {
			could_increment = true;
			break;
		    }

		    // Otherwise set it back to the starting value, and let
		    // the loop continue, i.e. increase the next higher (free)
		    // argument
		    func_args[i].dbl(from[i].dbl());
		}
	    }

	    for(unsigned int i=0; i<final_result.size(); ++i)
	    {
		res[(*ind)++].dbl(final_result[i].dbl());
	    }
	}
		
	
    }
}
