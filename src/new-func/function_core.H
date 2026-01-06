#ifndef __BLOP_FUNCTION_CORE_H__
#define __BLOP_FUNCTION_CORE_H__
#include "warning.h"
#include "array.h"
#include "constants.h"
#include "function.h"
#include "interpolate.h"
#include <cmath>
#include <cstdlib>
#include <complex>

namespace blop
{
    class cfunc_wrapper_base;

    namespace function_core
    {
	extern std::vector<blop::var> tmp;

	class base
	{
	protected:
	    int nargs_, npars_;

	public:
	    base() : nargs_(-1), npars_(-1) {}
	    virtual ~base() {}
    
	    // create a copy of this expression, recursively (daughters are
	    // also cloned)
	    virtual base  *clone()              const = 0;
    
	    // evaluate the expression. the second version avoids the overhead
	    // due to the 'var' return values, it is used in the fitting algorithm
	    // the

	    virtual void       eval(const std::vector<blop::var> &args,
				    const std::vector<blop::var> &def_args,
				    const std::vector<blop::var> &params,
				    std::vector<blop::var> &result, int *ind)  const = 0;

	    virtual void       eval_dbl(const std::vector<blop::var> &args,
					const std::vector<blop::var> &def_args,
					const std::vector<blop::var> &params,
					std::vector<blop::var> &result, int *ind)  const = 0;

	    // return the number of arguments used by this expression
	    // (to be more exact, the highest index of the used arguments)
	    // if the expression uses args 1 and 3, 3 is returned
	    virtual int       nargs()          const = 0;
	    virtual int       npars()          const = 0;

	    virtual void      nargs(int i)     { nargs_ = i; }
	    virtual void      npars(int i)     { npars_ = i; }

	    // check if this expression uses the 'i'th argument
	    virtual bool      uses_arg(int i) {return false;}
	    virtual bool      uses_par(int i) {return false;}
    
	    // return the character-representation of this expression
	    virtual var sprint(const std::vector<blop::var> &pars, bool parvalue) const = 0;
	    virtual var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
				     const var &x="x", const var &y="y", const var &z="z") const
		{
		    return sprint(pars, parvalue);
		}

	    // create the derivative expression. it is dynamically allocated,
	    // so it should not be copied, when attached to a function
	    // derivs should have 0 size when called!
	    virtual void create_derivative(int i, std::vector<base*> &derivs) const = 0;

	    // check if it is the same 
	    virtual bool equals(const base *) const = 0;

	    // return the number of output variables
	    virtual int n_out() const { return 1; }

	};



	class interpolated_func : public base
	{
	private:
	    interpolator *interpolator_;

	public:
	    interpolated_func() : interpolator_(0) {}
	    interpolated_func(const interpolated_func &rhs);
	    interpolated_func(interpolator *interp) : interpolator_(interp) {}

	    base *clone() const;
	    void eval(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &params,
		      std::vector<blop::var> &result, int *ind) const;
	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &params,
			  std::vector<blop::var> &result, int *ind) const;
	    int nargs() const { if(nargs_>=0) return nargs_; return 1; }
	    int npars() const { return 0; }
	    bool uses_arg(int i) { return i==1; }
	    bool uses_par(int i) { return false; }
	    var sprint(const std::vector<blop::var> &pars, bool parvalue) const
		{
		    return (interpolator_?interpolator_->name():"???");
		}
	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue) const
		{
		    return
			var("\\mathrm{") &
			(interpolator_?interpolator_->name():"???") &
			var("}");
		}
	    void create_derivative(int, std::vector<base*> &derivs) const;
	    bool equals(const base *) const { return false; }
	    int n_out() const { return 1; }
	};



	class step_func : public base
	{
	private:
	    vector<double> x_;
	    vector<vector<double> > y_;
	public:
	    step_func(const vector<double> &xx, const vector<double> &yy);
	    step_func(const vector<double> &xx, const vector<vector<double> > &yy);

	    base *clone() const;
	    void eval(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &params,
		      std::vector<blop::var> &result, int *ind) const;
	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &params,
			  std::vector<blop::var> &result, int *ind) const;
	    int nargs() const { if(nargs_>=0) return nargs_; return 1; }
	    int npars() const { return 0; }
	    bool uses_arg(int i) { return i==1; }
	    bool uses_par(int i) { return false; }
	    var sprint(const std::vector<blop::var> &pars, bool parvalue) const
		{
		    return "stepfunc";
		}
	    void create_derivative(int,std::vector<base*> &derivs) const;
	    bool equals(const base *) const { return false; }
	    int n_out() const { return y_.size(); }
	};



	// ----------------  return a specific component of another -------
	// indexing is 0-based!

	class component : public base
	{
	private:
	    int index_;
	    base *base_;
	    
	    component() {}

	public:
	    // ----------  constructor  --------------------------

	    // clone the given base
	    component(const base &b, int i); 
	    ~component();

	    base *clone() const;
	    void eval(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var> &result, int *ind) const;
	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &pars,
			  std::vector<blop::var> &result, int *ind) const;
	    int nargs() const { return base_->nargs(); }
	    int npars() const { return base_->npars(); }
	    bool uses_arg(int i) { return base_->uses_arg(i); }
	    bool uses_par(int i) { return base_->uses_par(i); }
	    var sprint(const std::vector<blop::var> &pars, bool parvalue) const;
	    void create_derivative(int, std::vector<base*> &derivs) const;
	    bool equals(const base *) const { cerr<<"component::equals not implemented"<<endl; return false; }
	    int n_out() const { return 1; }
	};


	//-----------------  cfunc ------------------------------------
	// Evaluate a compiled or interpreted C-function

	class cfunc : public base
	{
	private:
	    cfunc_wrapper_base *wrapper_;
	    void init_arrays_();
	    void init_(void *);

	public:
	    var sprint(const std::vector<blop::var> &par, bool parvalue) const;
    
	    bool uses_arg(int i);
    
	    cfunc(const cfunc &);
	    cfunc(cfunc_wrapper_base *w);   // store the pointer, do NOT copy the object
	    cfunc();
    
	    cfunc(var (*p)(var));
	    cfunc(var (*p)(var,var));
	    cfunc(var (*p)(var,var,var));
	    cfunc(var (*p)(var,var,var,var));
    
	    cfunc(double (*p)(double));
	    cfunc(double (*p)(double,double));
	    cfunc(double (*p)(double,double,double)); 
	    cfunc(double (*p)(double,double,double,double)); 
	    cfunc(complex<double> (*p)(double));

	    cfunc(var (*p)(const std::vector<blop::var> &args,
			   const std::vector<blop::var> &pars),
		  int nargs, int npars);

	    // initialization with a (void *) is needed, because
	    // cint is not willing to initializa with a pointer to
	    // an interpreted function
	    cfunc(void *p);
  
	    ~cfunc();
    
	    base *clone() const;
	    void eval(const std::vector<blop::var> &,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &,
		      std::vector<blop::var> &result,
		      int *ind) const;
	    void eval_dbl(const std::vector<blop::var> &,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &,
			  std::vector<blop::var> &result,
			  int *ind) const;

	    int nargs() const;
	    int npars() const;

	    int n_out() const;

	    void create_derivative(int i, std::vector<base*> &derivs) const;

	    bool equals(const base *) const;
	};

	
	// ---------------------  Constant  ----------------------------
	// a constant expression, always returns a given constant value
	// regardless of the arguments provided to its 'eval' function
	class constant : public base
	{
	public:
	    // the value to be returned
	    var value_;

	public:

	    // initialization
	    constant(double v) : value_(v) {}
	    constant(int v) : value_(v) {}
	    constant(const var &v) : value_(v) {}
	    constant(const char *v) : value_(v) {}
	    constant(const string &v) : value_(v) {}

	    // clone
	    base *clone() const {return new constant(value_);}

	    // evaluate
	    void eval(const std::vector<blop::var> &,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &,
		      std::vector<blop::var> &result,
		      int *ind) const
		{
		    result[(*ind)++] = value_;
		}
	    void eval_dbl(const std::vector<blop::var> &,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &,
			  std::vector<blop::var> &result,
			  int *ind) const
		{
		    result[(*ind)++].dbl(value_.dbl());
		}

	    // it does not use any of the arguments, no params
	    int nargs() const { return 0; }
	    int npars() const { return 0; }

	    // its character representation. 
	    var sprint(const std::vector<blop::var> &, bool) const
		{
		    if(value_.is_dbl()) return value_;
		    var result = "\""; result &= value_; result &= "\""; return result;
		}

	    // its derivative is a constant 0 
	    void create_derivative(int, std::vector<base*> &derivs) const { derivs.push_back(new constant(0.0)); }

	    bool equals(const base *o) const
		{
		    const constant *c = dynamic_cast<const constant *> (o);
		    if(c && c->value_ == value_) return true;
		    return false;
		}

	    const var &value() const { return value_; }
	};
	

	// ---------------------  Random number ---------------------------
	class random : public base
	{
	private:
	    double from_, to_;
	    static double get_();
	public:
	    random() : from_(0), to_(1) {}
	    random(double from, double to) : from_(from), to_(to) {}

	    // clone
	    base *clone() const { return new random(*this); }

	    // evaluate
	    void eval(const std::vector<blop::var> &,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &,
		      std::vector<blop::var> &result,
		      int *ind) const
		{
		    result[(*ind)++] = get_()*(to_-from_) + from_;
		}
	    void eval_dbl(const std::vector<blop::var> &,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &,
			  std::vector<blop::var> &result,
			  int *ind) const
		{
		    result[(*ind)++].dbl(get_()*(to_-from_) + from_);
		}

	    // it does not use any of the arguments, no params
	    int nargs() const { return 0; }
	    int npars() const { return 0; }

	    // its character representation. 
	    var sprint(const std::vector<blop::var> &, bool) const
		{
		    var result = "random(";
		    result &= from_;
		    result &= ",";
		    result &= to_;
		    result &= ")";
		    return result;
		}

	    // its derivative is a constant 0 
	    void create_derivative(int, std::vector<base*> &derivs) const { derivs.push_back(new constant(0.0)); }

	    bool equals(const base *o) const
		{
		    const random *c = dynamic_cast<const random *> (o);
		    if(c == 0) return false;
		    if(c->from_ != from_ || c->to_ != to_) return false;
		    return true;
		}
	};
	
	// ---------------------  extra_param ------------------------
	class extra_param : public base
	{
	public:
	    static var value;

	    // clone
	    base *clone() const { return new extra_param; }

	    // evaluate
	    void eval(const std::vector<blop::var> &,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &,
		      std::vector<blop::var> &result,
		      int *ind) const
		{
		    result[(*ind)++] = value;
		}
	    void eval_dbl(const std::vector<blop::var> &,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &,
			  std::vector<blop::var> &result,
			  int *ind) const
		{
		    result[(*ind)++].dbl(value.dbl());
		}

	    // it does not use any of the arguments, no params
	    int nargs() const { return 0; }
	    int npars() const { return 0; }

	    // its character representation. 
	    var sprint(const std::vector<blop::var> &, bool) const
		{
		    return "_0";
		}
	    var sprint_latex(const std::vector<blop::var> &, bool) const
		{
		    return " \\_0 ";
		}

	    // its derivative is a constant 0 
	    void create_derivative(int, std::vector<base*> &derivs) const { derivs.push_back(new constant(0.0)); }

	    bool equals(const base *o) const
		{
		    if(dynamic_cast<const extra_param *> (o)) return true;
		    return false;
		}
	};


	// ---------------  characteristic function ---------------------
	// the characteristic function of an interval returns 1 if its
	// (first) argument falls within the interval, 0 otherwise

	class char_func : public base
	{
	private:
	    base *low_, *high_;      // interval limits
	    bool low_in_, high_in_;  // whether the limits are 'inside' the interval

	public:
	    char_func(base *low,base *high,bool low_in = true, bool high_in = false)
		: low_in_(low_in), high_in_(high_in)
		{
		    low_ = low->clone();
		    high_ = high->clone();
		    unsigned int n = ::max(low_->n_out(),high_->n_out());
		    if(n > tmp.size()) tmp.resize(n);
		}

	    ~char_func() { delete low_; delete high_; }

	    base *clone() const { return new char_func(low_,high_,low_in_,high_in_); }

	    void      eval(const std::vector<blop::var> &args,
			   const std::vector<blop::var> &def_args,
			   const std::vector<blop::var> &pars,
			   std::vector<blop::var> &result,
			   int *ind) const;

	    void     eval_dbl(const std::vector<blop::var> &args,
			      const std::vector<blop::var> &def_args,
			      const std::vector<blop::var> &pars,
			      std::vector<blop::var> &result,
			      int *ind) const;
	    
	    // it uses the first argument
	    int nargs() const
		{
		    if(nargs_>=0) return nargs_;
		    return ::max(::max(low_->nargs(),high_->nargs()),1);
		}
	    int npars() const
		{
		    if(npars_>=0) return npars_;
		    return ::max(low_->npars(), high_->npars());
		}
	    
	    bool uses_arg(int i) { return (i==1 || low_->uses_arg(i) || high_->uses_arg(i) ); }

	    // character representation
	    var sprint(const std::vector<blop::var> &p, bool v) const
		{
		    var result = "charfunc";
		    if(low_in_) result &= "[";
		    else result &= "]";
		    result &= low_->sprint(p,v);
		    result &= ";";
		    result &= high_->sprint(p,v);
		    if(high_in_) result &= "]";
		    else result &= "[";
		    return result;
		}

	    var sprint_latex(const std::vector<blop::var> &p, bool v,
			     const var &x, const var &y, const var &z) const
		{
		    var result = "\\mathrm{charfunc}";
		    if(low_in_) result &= "[";
		    else result &= "]";
		    result &= low_->sprint_latex(p,v,x,y,z);
		    result &= ";";
		    result &= high_->sprint_latex(p,v,x,y,z);
		    if(high_in_) result &= "]";
		    else result &= "[";
		    return result;
		}


	    // its derivative is a constant 0
	    void create_derivative(int i, std::vector<base*> &derivs) const { derivs.push_back(new constant(0.0)); }

	    bool equals(const base *o) const
		{
		    const char_func *c = dynamic_cast<const char_func *> (o);
		    if(c &&
		       low_->equals(c->low_) && high_->equals(c->high_) &&
		       c->low_in_ == low_in_ && c->high_in_ == high_in_) return true;
		    return false;
		}
	};

	// -------------------------  NARGS  --------------------------------
	// this is a utility function, it returns the actual number of
	// arguments in the current call of the function (remember, the
	// functions in blop can be called with an arbitrary number of
	// parameters provided in an array)

	class actual_nargs : public base
	{
	public:
	    // clone
	    base *clone() const { return new actual_nargs; }

	    // evaluate: return the actual number of arguments in this function call
	    void eval(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &,
		      std::vector<blop::var> &result,
		      int *ind) const
		{ result[(*ind)++] = (int)(args.size()); }
	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &,
			  std::vector<blop::var> &result,
			  int *ind) const
		{ result[(*ind)++].dbl(args.size()); }

	    // it does not use any of the arguments
	    int nargs() const { return 0; }
	    int npars() const { return 0; }

	    // Its representation is _N (this symbol is defined in the C++ interface
	    // to refer to this expression)
	    var sprint(const std::vector<blop::var> &, bool) const { return "_N"; }
	    var sprint_latex(const std::vector<blop::var> &, bool,
			     const var &, const var &, const var &) const
		{
		    return " \\_N ";
		}

	    // and its derivative is 0
	    void create_derivative(int, std::vector<base*> &derivs) const { derivs.push_back(new constant(0.0)); }

	    bool equals(const base *o) const
		{
		    return(dynamic_cast<const actual_nargs *>(o) != 0);
		}
	};

	
	// ------------------------  ARG ------------------------------
	// This expression returns the value of a given argument from the
	// many possible arguments in the function call. 

	class arg : public base
	{
	private:
	    int arg_index_;

	public:
	    int arg_index() const { return arg_index_+1; }

	    // initialize: i=1 refers to the first argument (index 0 in the
	    // c++ vector of arguments provided in the function call)
	    arg(int i) : arg_index_(::max(i-1,0)) {}

	    // clone it
	    base *clone() const { return new arg(arg_index_+1); }

	    // evaluate: 
	    void eval(const std::vector<blop::var> &arg,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &,
		      std::vector<blop::var> &result,
		      int *ind) const
		{
		    if((int)arg.size() <= arg_index_) // not sufficient arguments
		    {
			if((int)def_args.size() <= arg_index_) // no default argument either
			{
			    result[(*ind)].dbl(0);
			    result[(*ind)++].str("");
			}
			else
			{
			    result[(*ind)++] = def_args[arg_index_];
			}
		    }
		    else result[(*ind)++] = arg[arg_index_];
		}
	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &,
			  std::vector<blop::var> &result,
			  int *ind) const
		{
		    double val = 0;
		    if(arg_index_<(int)args.size()) val = args[arg_index_].dbl();
		    else if(arg_index_<(int)def_args.size()) val = def_args[arg_index_].dbl();
		    result[(*ind)++].dbl(val);
		}

	    // 
	    int nargs() const { if(nargs_>=0) return nargs_; return arg_index_ + 1;}
	    int npars() const { return 0; }

	    // 
	    bool uses_arg(int i);

	    // character representation is _n, where n is a number
	    // determining the argument index. This symbol is defined in the
	    // c++ interface to access this expression, therefore this character
	    // representation can again be pasted as it is to the c++ interpreter
	    var sprint(const std::vector<blop::var> &, bool) const
		{
		    var result = "_";
		    result &= (arg_index_+1);
		    return result;
		}

	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x="x", const var &y="y", const var &z="z") const
		{
		    var result = var(" \\_") & (arg_index_+1);
		    switch(arg_index_)
		    {
		    case 0: result = x; break;
		    case 1: result = y; break;
		    case 2: result = z; break;
		    }
		    return result;
		}


	    // derivative: if we derive with respect to this argument, then return
	    // the constant 1, otherwise return the constant 0
	    void create_derivative(int i, std::vector<base*> &derivs) const
		{
		    if(i-1 == arg_index_) derivs.push_back(new constant(1.0));
		    else derivs.push_back(new constant(0.0));
		}

	    // check equality
	    bool equals(const base *o) const
		{
		    const arg *c = dynamic_cast<const arg *>(o);
		    if(c && c->arg_index_ == arg_index_) return true;
		    return false;
		}
	};

	// ------------------------  COL ------------------------------
	// returns the nth element of the array, based on names stored
	// in function::column_names_

	class col : public base
	{
	private:
	    std::string name_;

	public:

	    // initialize: i=1 refers to the first argument (index 0 in the
	    // c++ vector of arguments provided in the function call)
	    col(const string &s) : name_(s) {}

	    // clone it
	    base *clone() const { return new col(*this); }

	    // evaluate: 
	    void eval(const std::vector<blop::var> &arg,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &,
		      std::vector<blop::var> &result,
		      int *ind) const
		{
		    for(unsigned int i=0; i<function::column_names_.size(); ++i)
		    {
			if(function::column_names_[i] == name_)
			{
			    if(i>=arg.size())
			    {
				result[(*ind)].dbl(0);
				result[(*ind)++].str("");
				return;
			    }
			    result[(*ind)++] = arg[i];
			    return;
			}
		    }
		    result[(*ind)].dbl(0);
		    result[(*ind)++].str("");
		}
	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &,
			  std::vector<blop::var> &result,
			  int *ind) const
		{
		    for(unsigned int i=0; i<function::column_names_.size(); ++i)
		    {
			if(function::column_names_[i] == name_)
			{
			    if(i>=args.size())
			    {
				result[(*ind)].dbl(0);
				result[(*ind)++].str("");
				return;
			    }
			    result[(*ind)++] = args[i];
			    return;
			}
		    }
		    result[(*ind)].dbl(0);
		    result[(*ind)++].str("");
		}

	    // 
	    int nargs() const
		{
		    for(unsigned int i=0; i<function::column_names_.size(); ++i)
		    {
			if(function::column_names_[i] == name_) return i+1;
		    }
		    return 0;
		}
	    int npars() const { return 0; }

	    // 
	    bool uses_arg(int i)
		{
		    if(0<i && i<=(int)function::column_names_.size() &&
		       function::column_names_[i-1] == name_) return true;
		    return false;
		}

	    // character representation is _n, where n is a number
	    // determining the argument index. This symbol is defined in the
	    // c++ interface to access this expression, therefore this character
	    // representation can again be pasted as it is to the c++ interpreter
	    var sprint(const std::vector<blop::var> &, bool) const
		{
		    return string("_c(\"") + name_ + string("\")");
		}

	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x="x", const var &y="y", const var &z="z") const
		{
		    return string("\\_c(\"") + name_ + string("\")");
		}


	    // derivative: if we derive with respect to this argument, then return
	    // the constant 1, otherwise return the constant 0
	    void create_derivative(int i, std::vector<base*> &derivs) const
		{
		    if(0<i && i<=(int)function::column_names_.size() &&
		       function::column_names_[i-1] == name_) derivs.push_back( new constant(1.0));
		    else derivs.push_back(new constant(0.0));
		}

	    // check equality
	    bool equals(const base *o) const
		{
		    const col *c = dynamic_cast<const col *>(o);
		    if(c && c->name_ == name_) return true;
		    return false;
		}
	};



	// ---------------  binary-base ---------------------------------
	// this is an abstract class serving as a base for those expressions,
	// which have two operands (called left and right). 
	// it implements some common features of these

	class binary_base : public base
	{
	public:
	    var name_;
	    int n_out_;
	    std::vector<base*> left_,right_;
	    mutable std::vector<var> tmp_left_, tmp_right_;

	    void calc_n_()
		{
		    int n_out_left=0, n_out_right=0;
		    for(unsigned int i=0; i<left_.size(); ++i) n_out_left += left_[i]->n_out();
		    for(unsigned int i=0; i<right_.size(); ++i) n_out_right += right_[i]->n_out();
		    if(n_out_left != n_out_right)
		    {
			warning::print("The left and right arguments of binary_base have different n_out! Results can be unpredictable");
		    }
		    n_out_ = ::max(n_out_left, n_out_right);

		    if((unsigned int)(n_out_) > tmp.size()) tmp.resize(n_out_);
		    if(tmp.size() > tmp_left_.size()) tmp_left_.resize(tmp.size());
		    if(tmp.size() > tmp_right_.size()) tmp_right_.resize(tmp.size());

		    nargs_ = 0;
		    npars_ = 0;
		    for(unsigned int i=0; i<left_.size(); ++i)
		    {
			if(left_[i]->nargs() > nargs_) nargs_ = left_[i]->nargs();
			if(left_[i]->npars() > npars_) npars_ = left_[i]->npars();
		    }
		    for(unsigned int i=0; i<right_.size(); ++i)
		    {
			if(right_[i]->nargs() > nargs_) nargs_ = right_[i]->nargs();
			if(right_[i]->npars() > npars_) npars_ = right_[i]->npars();
		    }
		}

	    // initialize. if 'left' and 'right' are provided, they are CLONED
	    // and their clone's pointer is stored in the left_ and right_ members
	    void init_binary_base_(const var &name, const std::vector<base*> &left, const std::vector<base*> &right)
		{
		    name_ = name;

		    left_.resize(left.size());
		    right_.resize(right.size());
		    for(unsigned int i=0; i<left.size(); ++i) left_[i] = left[i]->clone();
		    for(unsigned int i=0; i<right.size(); ++i) right_[i] = right[i]->clone();

		    calc_n_();
		}

	public:

	    binary_base(const var &name, const std::vector<base*> &left, const std::vector<base*> &right)
		{
		    init_binary_base_(name,left,right);
		}
	    binary_base(const var &name, const base *left, const base *right)
		{
		    vector<base *> l;
		    if(left)
		    {
			l.resize(1);
			l[0] = left->clone();
		    }
		    vector<base *> r;
		    if(right)
		    {
			r.resize(1);
			r[0] = right->clone();
		    }
		    init_binary_base_(name,l,r);
		    for(unsigned int i=0; i<l.size(); ++i) delete l[i];
		    for(unsigned int i=0; i<r.size(); ++i) delete r[i];
		}
	    binary_base(const binary_base &o)
		{
		    init_binary_base_(o.name_,o.left_,o.right_);
		}
		     
	    void left(const std::vector<base*> &left)
		{
		    left_.resize(left.size());
		    for(unsigned int i=0; i<left.size(); ++i) left_[i] = left[i]->clone();
		    calc_n_();
		}
	    void left(base *left)
		{
		    left_.resize(1);
		    left_[0] = left->clone();
		    calc_n_();
		}
	    void right(const std::vector<base*> &right)
		{
		    right_.resize(right.size());
		    for(unsigned int i=0; i<right.size(); ++i) right_[i] = right[i]->clone();
		    calc_n_();
		}
	    void right(base *right)
		{
		    right_.resize(1);
		    right_[0] = right->clone();
		    calc_n_();
		}

	    // desctuctor: (recursively) delete both operands
	    ~binary_base()       
		{
		    for(unsigned int i=0; i<left_.size(); ++i) delete left_[i];
		    for(unsigned int i=0; i<right_.size(); ++i) delete right_[i];
		}

	    int n_out() const { return n_out_; }

	    // number of args, etc, are deduced from the response of the two operands
	    int nargs() const { return nargs_; }
	    int npars() const { return npars_; }

	    bool uses_arg(int i)
		{
		    for(unsigned int i=0; i<left_.size(); ++i) if(left_[i]->uses_arg(i)) return true;
		    for(unsigned int i=0; i<right_.size(); ++i) if(right_[i]->uses_arg(i)) return true;
		    return false;
		}
	    bool uses_par(int i) 
		{
		    for(unsigned int i=0; i<left_.size(); ++i) if(left_[i]->uses_par(i)) return true;
		    for(unsigned int i=0; i<right_.size(); ++i) if(right_[i]->uses_par(i)) return true;
		    return false;
		}

	    bool equals(const base *o) const
		{
		    const binary_base *c = dynamic_cast<const binary_base *>(o);
		    if(!c) return false;
		    if(c->name_.str() != name_.str()) return false;
		    if(left_.size() != c->left_.size()) return false;
		    if(right_.size() != c->right_.size()) return false;
		    for(unsigned int i=0; i<left_.size(); ++i) if(!left_[i]->equals(c->left_[i])) return false;
		    for(unsigned int i=0; i<right_.size(); ++i) if(!right_[i]->equals(c->right_[i])) return false;
		    return true;
		}

	    void create_derivative(int a, std::vector<base*> &derivs) const
		{
		    vector<base*> leftderiv;
		    for(unsigned int i=0; i<left_.size(); ++i) left_[i]->create_derivative(a, leftderiv);
		    vector<base*> rightderiv;
		    for(unsigned int i=0; i<right_.size(); ++i) right_[i]->create_derivative(a, rightderiv);
		    create_derivative_spec(leftderiv, rightderiv, derivs);
		    for(unsigned int i=0; i<leftderiv.size(); ++i) delete leftderiv[i];
		    for(unsigned int i=0; i<rightderiv.size(); ++i) delete rightderiv[i];
		}

	    virtual var    exec    (const var &left, const var &right) const = 0;
	    virtual double exec_dbl(const double left, const double right) const = 0;
		     
	    void eval(const std::vector<var> &args,
		      const std::vector<blop::var> &def_args,
		      const std::vector<var> &params,
		      std::vector<blop::var> &result,int *ind) const
		{
		    int n_out_left = 0;
		    for(unsigned int i=0; i<left_.size(); ++i) left_[i]->eval(args,def_args,params,tmp, &n_out_left);
		    for(int i=0; i<n_out_left; ++i) tmp_left_[i] = tmp[i];
		    int n_out_right = 0;
		    for(unsigned int i=0; i<right_.size(); ++i) right_[i]->eval(args,def_args,params,tmp, &n_out_right);
		    for(int i=0; i<n_out_right; ++i) tmp_right_[i] = tmp[i];
		    for(int i=0; i<std::max(n_out_left,n_out_right); ++i)
		    {
			const var r = exec( (i<n_out_left ?tmp_left_ [i]:var("0") ),
					    (i<n_out_right?tmp_right_[i]:var("0")) );
			cerr<<"Calculating component ["<<i<<"] = "<<r<<endl;
			result[(*ind)++] = r;
		    }
		}
	    void eval_dbl(const std::vector<var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<var> &params,
			  std::vector<blop::var> &result,int *ind) const
		{
		    int n_out_left = 0;
		    for(unsigned int i=0; i<left_.size(); ++i) left_[i]->eval_dbl(args,def_args,params,tmp, &n_out_left);
		    for(int i=0; i<n_out_left; ++i) tmp_left_[i].dbl(tmp[i].dbl());
		    int n_out_right = 0;
		    for(unsigned int i=0; i<right_.size(); ++i) right_[i]->eval_dbl(args,def_args,params,tmp, &n_out_right);
		    for(int i=0; i<n_out_right; ++i) tmp_right_[i].dbl(tmp[i].dbl());
		    for(int i=0; i<std::max(n_out_left,n_out_right); ++i)
		    {
			result[(*ind)++].dbl(exec_dbl( (i<n_out_left?tmp_left_[i].dbl():0.0),
						       (i<n_out_right?tmp_right_[i].dbl():0.0)));
		    }
		}

	    void create_derivative_spec(const std::vector<base*> &leftderiv, const std::vector<base*> &rightderiv, std::vector<base*> &derivs) const
		{
		    for(int i=0; i<n_out_; ++i) derivs.push_back(new constant(0.0));
		}
	};


	// ---------------  binary_operator -----------------------------
	// This class is derived from binary_base (has two operands), and
	// serves as a base class for those binary expressions, which are
	// character-represented as *operators*, that is, an operator symbol
	// is put *between* the two operands
	// this class therefore only implements the sprint method, anything
	// else is left for the specialized derived classes

	class binary_operator : public binary_base
	{
	public:
	    // initialize. the left and right operands, if provided, are CLONED
	    // 'name' is the symbol for the operator (for example +, -, *, etc)
	    binary_operator(const var &name, const base *left, const base *right) : binary_base(name,left,right) {}
	    binary_operator(const var &name, const std::vector<base*> &left, const std::vector<base*> &right) : binary_base(name,left,right) {}
	    binary_operator(const binary_operator &o) : binary_base(o.name_,o.left_,o.right_) {}

	    // return the character representation, the operator symbol (name above)
	    // placed between the two operands
	    var sprint(const std::vector<blop::var> &, bool) const;
	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x="x", const var &y="y", const var &z="z") const;

	};


	// ---------------  binary_function -----------------------------
	// this class is derived from binary_base (two operands), and serves
	// as a base class for those binary expressions, which are represented
	// as functions [like function(a,b)]
	// it therefore only implements the sprint method

	class binary_function : public binary_base
	{
	public:
	    // initialize. left and right are CLONED
	    binary_function(const var &name, const base *left, const base *right) : binary_base(name,left,right) {}
	    binary_function(const var &name, const std::vector<base*> &left, const std::vector<base*> &right) : binary_base(name,left,right) {}
	    binary_function(const binary_function &o) : binary_base(o.name_,o.left_,o.right_) {}

	    // return the character representation
	    var sprint(const std::vector<blop::var> &pars, bool parvalue) const
		{
		    var result = name_;
		    result &= "(";
		    result &= left_->sprint(pars, parvalue);
		    result &= ",";
		    result &= right_->sprint(pars, parvalue);
		    result &= ")";
		    return result;
		}
	    // return the character representation
	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x, const var &y, const var &z) const
		{
		    var result = var("\\mathrm{") & name_ & var("}");
		    result &= "\\left(";
		    result &= left_->sprint_latex(pars, parvalue, x, y, z);
		    result &= ",";
		    result &= right_->sprint_latex(pars, parvalue, x, y, z);
		    result &= "\\right)";
		    return result;
		}

	};


	// ---------------  EqualDbl -----------------------------------------

	class EqualDbl : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    EqualDbl(const base *left=0, const base *right=0) : binary_operator("==",left,right) {}
	    EqualDbl(const EqualDbl &o) : binary_operator("==",o.left_,o.right_) {}

	    // clone it: return a new EqualDbl
	    base *clone() const { return new EqualDbl(*this); }

	    var    exec    (const var &left, const var &right) const { return left.dbl() == right.dbl(); }
	    double exec_dbl(double left, double right)         const { return left == right; }
	};


	// ---------------  NotEqualDbl -----------------------------------------

	class NotEqualDbl : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    NotEqualDbl(const base *left=0, const base *right=0) : binary_operator("!=",left,right) {}
	    NotEqualDbl(const NotEqualDbl &o) : binary_operator("!=",o.left_,o.right_) {}

	    // clone it: return a new NotEqualDbl
	    base *clone() const { return new NotEqualDbl(*this); }

	    // evaluate: return 1 if 2 args are equal, 0 otherwise
	    var exec(const var &left, const var &right) const { return left.dbl() != right.dbl(); }
	    double exec_dbl(double left, double right)  const { return left != right; }
	};



	// ---------------  EqualStr -----------------------------------------

	class EqualStr : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    EqualStr(const base *left=0, const base *right=0) : binary_operator("==",left,right) {}
	    EqualStr(const EqualStr &o) : binary_operator("==",o.left_,o.right_) {}

	    // clone it: return a new EqualStr
	    base *clone() const { return new EqualStr(*this); }

	    // evaluate: return 1 if 2 args are equal, 0 otherwise
	    var exec(const var &left,const var &right) const  { return left.str() == right.str(); }
	    double exec_dbl(double,double) const
		{
		    cerr<<"EqualStr::exec_dbl should never be called"<<endl;
		    return 0;
		}
	    double eval_dbl(double left, double right)
		{ cerr<<"EqualStr::eval_dbl should never be called"<<endl; return left == right; }

	    // for string comparison eval_dbl makes no sense. alias to eval
	    void eval_dbl(const std::vector<var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<var> &params,
			  std::vector<blop::var> &result,int *ind) const
		{
		    eval(args, def_args, params, result, ind);
		}
	};

	// ---------------  LessThan -----------------------------------------------

	class LessThan : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    LessThan(const base *left=0, const base *right=0) : binary_operator("<",left,right) {}
	    LessThan(const LessThan &o) : binary_operator("<",o.left_,o.right_) {}

	    // clone it: return a new LessThan
	    base *clone() const { return new LessThan(*this); }

	    // evaluate: return 1 if arg1 < arg2, 0 otherwise
	    var exec(const var &left, const var &right) const { return left.dbl() < right.dbl(); }
	    double exec_dbl(double left, double right)  const { return left < right; }
	};

	// ---------------  LessEqual -----------------------------------------------

	class LessEqual : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    LessEqual(const base *left=0, const base *right=0) : binary_operator("<=",left,right) {}
	    LessEqual(const LessEqual &o) : binary_operator("<=",o.left_,o.right_) {}

	    // clone it: return a new LessEqual
	    base *clone() const { return new LessEqual(*this); }

	    // evaluate: return 1 if arg1 < arg2, 0 otherwise
	    var exec(const var &left, const var &right) const { return left.dbl() <= right.dbl(); }
	    double exec_dbl(double left, double right)  const { return left <= right; }
	};

	// ---------------  max/min/integral values over a range -------------------------------

	class value_in_interval : public base
	{
	protected:
	    std::vector<base*> func_, from_, to_, step_;
	    std::string name_;
	public:
	    value_in_interval(const std::vector<base*> &func,
			      const std::vector<base*> &from,
			      const std::vector<base*> &to,
			      const std::vector<base*> &step,
			      const std::string &name);
	    value_in_interval(const value_in_interval &rhs);
	    int nargs() const;
	    int npars() const;
	    bool uses_arg(int i);
	    bool uses_par(int i);
	    int n_out() const;
	    var sprint(const std::vector<blop::var> &p, bool v) const;
	    var sprint_latex(const std::vector<blop::var> &p, bool v,
			     const var &x, const var &y, const var &z) const;
	    
	    void create_derivative(int a, std::vector<base*> &derivs) const
	    {
		warning::print("Derivative of value_in_interval not implemented");
		for(int i=0; i<func_.components(); ++i) derivs.push_back(new constant(0.0));
	    }

	    virtual bool   evaluate_at_interval_centers() const = 0;

	    virtual void   accumulate_result(int count,
					     const std::vector<blop::var> &func_args,
					     const std::vector<blop::var> &step,
					     const std::vector<blop::var> &current_result,
					     std::vector<blop::var>       &final_result) const = 0;
	    virtual void   accumulate_result_dbl(int count,
					     const std::vector<blop::var> &func_args,
					     const std::vector<blop::var> &step,
					     const std::vector<blop::var> &current_result,
					     std::vector<blop::var>       &final_result) const = 0;

	    void      eval(const std::vector<blop::var> &args,
			   const std::vector<blop::var> &def_args,
			   const std::vector<blop::var> &pars,
			   std::vector<blop::var> &res,
			   int *ind) const;

	    void      eval_dbl(const std::vector<blop::var> &args,
			       const std::vector<blop::var> &def_args,
			       const std::vector<blop::var> &pars,
			       std::vector<blop::var> &res,
			       int *ind) const;


	    bool equals(const base *rhs) const {return false;}

	};

	class max_in_interval : public value_in_interval
	{
	public:
	    max_in_interval(const std::vector<base*> &func,
			    const std::vector<base*> &from,
			    const std::vector<base*> &to,
			    const std::vector<base*> &step)
		: value_in_interval(func,from,to,step,"maxininterval") {}
	    max_in_interval(const max_in_interval &rhs) : value_in_interval(rhs) {}
	    max_in_interval *clone() const { return new max_in_interval(*this); }


	    bool   evaluate_at_interval_centers() const { return false; }
	    void   accumulate_result(int count,
				     const std::vector<blop::var> &func_args,
				     const std::vector<blop::var> &step,
				     const std::vector<blop::var> &current_result,
				     std::vector<blop::var>       &final_result) const
	    {
		if(count==0)
		{
		    for(unsigned int i=0; i<current_result.size(); ++i)
		    {
			final_result[i] = current_result[i];
		    }
		}
		else
		{
		    for(unsigned int i=0; i<current_result.size(); ++i)
		    {
			if(current_result[i].dbl()>final_result[i].dbl())
			    final_result[i] = current_result[i];
		    }
		}
	    }
	    void   accumulate_result_dbl(int count,
					 const std::vector<blop::var> &func_args,
					 const std::vector<blop::var> &step,
					 const std::vector<blop::var> &current_result,
					 std::vector<blop::var>       &final_result) const
	    {
		if(count==0)
		{
		    for(unsigned int i=0; i<current_result.size(); ++i)
		    {
			final_result[i].dbl(current_result[i].dbl());
		    }
		}
		else
		{
		    for(unsigned int i=0; i<current_result.size(); ++i)
		    {
			if(current_result[i].dbl()>final_result[i].dbl())
			    final_result[i].dbl(current_result[i].dbl());
		    }
		}
	    }

	};

	class min_in_interval : public value_in_interval
	{
	public:
	    min_in_interval(const std::vector<base*> &func,
			    const std::vector<base*> &from,
			    const std::vector<base*> &to,
			    const std::vector<base*> &step)
		: value_in_interval(func,from,to,step,"minininterval") {}
	    min_in_interval(const min_in_interval &rhs) : value_in_interval(rhs) {}
	    min_in_interval *clone() const { return new min_in_interval(*this); }

	    bool   evaluate_at_interval_centers() const { return false; }
	    void   accumulate_result(int count,
				     const std::vector<blop::var> &func_args,
				     const std::vector<blop::var> &step,
				     const std::vector<blop::var> &current_result,
				     std::vector<blop::var>       &final_result) const
	    {
		if(count==0)
		{
		    for(unsigned int i=0; i<current_result.size(); ++i)
		    {
			final_result[i] = current_result[i];
		    }
		}
		else
		{
		    for(unsigned int i=0; i<current_result.size(); ++i)
		    {
			if(current_result[i].dbl()<final_result[i].dbl())
			    final_result[i] = current_result[i];
		    }
		}
	    }
	    void   accumulate_result_dbl(int count,
					 const std::vector<blop::var> &func_args,
					 const std::vector<blop::var> &step,
					 const std::vector<blop::var> &current_result,
					 std::vector<blop::var>       &final_result) const
	    {
		if(count==0)
		{
		    for(unsigned int i=0; i<current_result.size(); ++i)
		    {
			final_result[i].dbl(current_result[i].dbl());
		    }
		}
		else
		{
		    for(unsigned int i=0; i<current_result.size(); ++i)
		    {
			if(current_result[i].dbl()<final_result[i].dbl())
			    final_result[i].dbl(current_result[i].dbl());
		    }
		}
	    }

	};

	class integral : public value_in_interval
	{
	public:
	    integral(const std::vector<base*> &func,
		     const std::vector<base*> &from,
		     const std::vector<base*> &to,
		     const std::vector<base*> &step)
		: value_in_interval(func,from,to,step,"integral") {}
	    integral(const integral &rhs) : value_in_interval(rhs) {}
	    integral *clone() const { return new integral(*this); }

	    bool   evaluate_at_interval_centers() const { return true; }
	    void   accumulate_result(int count,
				     const std::vector<blop::var> &func_args,
				     const std::vector<blop::var> &step,
				     const std::vector<blop::var> &current_result,
				     std::vector<blop::var>       &final_result) const
	    {
		double w = 1;
		for(unsigned int i=0; i<step.size(); ++i)
		{
		    if(step[i].dbl()>0) w *= step[i].dbl();
		}
		for(unsigned int i=0; i<current_result.size(); ++i)
		{
		    final_result[i] += current_result[i].dbl()*w;
		}
	    }
	    void   accumulate_result_dbl(int count,
					 const std::vector<blop::var> &func_args,
					 const std::vector<blop::var> &step,
					 const std::vector<blop::var> &current_result,
					 std::vector<blop::var>       &final_result) const
	    {
		double w = 1;
		for(unsigned int i=0; i<step.size(); ++i)
		{
		    if(step[i].dbl()>0) w *= step[i].dbl();
		}
		for(unsigned int i=0; i<current_result.size(); ++i)
		{
		    final_result[i].dbl(final_result[i].dbl()+current_result[i].dbl()*w);
		}
	    }

	};




	// ------------------ max/min -------------------------------------------------

	class Max : public binary_function
	{
	public:
	    Max(const base *left=0, const base *right=0) : binary_function("max",left,right) {}
	    Max(const Max &o) : binary_function("max",o.left_,o.right_) {}
	    base *clone() const { return new Max(*this); }
	    var exec(const var &left, const var &right) const
		{ return ::max(left.dbl(), right.dbl()); }
	    double exec_dbl(double left, double right) const
		{ return ::max(left,right); }

	    void create_derivative_spec(const std::vector<base*> &leftderiv, const std::vector<base*> &rightderiv, std::vector<base*> &derivs) const
		{
		    warning::print("The derivative of max is not yet implemented");
		    for(int i=0; i<n_out_; ++i) derivs.push_back(new constant(0.0));
		}
	};

	class Min : public binary_function
	{
	public:
	    Min(const base *left=0, const base *right=0) : binary_function("min",left,right) {}
	    Min(const Min &o) : binary_function("min",o.left_,o.right_) {}
	    base *clone() const { return new Min(*this); }
	    var exec(const var &left, const var &right) const
		{ return ::min(left.dbl(), right.dbl()); }
	    double exec_dbl(double left, double right) const
		{ return ::min(left,right); }
	    void create_derivative_spec(const std::vector<base*> &leftderiv, const std::vector<base*> &rightderiv, std::vector<base*> &derivs) const
		{
		    warning::print("The derivative of min is not yet implemented");
		    for(int i=0; i<n_out_; ++i) derivs.push_back(new constant(0.0));
		}
	};


	// ---------------  GreaterThan -----------------------------------------------

	class GreaterThan : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    GreaterThan(const base *left=0, const base *right=0) : binary_operator(">",left,right) {}
	    GreaterThan(const GreaterThan &o) : binary_operator(">",o.left_,o.right_) {}

	    // clone it: return a new GreaterThan
	    base *clone() const { return new GreaterThan(*this); }

	    // evaluate: return 1 if arg1 < arg2, 0 otherwise
	    var exec(const var &left, const var &right) const { return left.dbl() > right.dbl(); }
	    double exec_dbl(double left, double right)  const { return left > right; }
	};

	// ---------------  GreaterEqual -----------------------------------------------

	class GreaterEqual : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    GreaterEqual(const base *left=0, const base *right=0) : binary_operator(">=",left,right) {}
	    GreaterEqual(const GreaterEqual &o) : binary_operator(">=",o.left_,o.right_) {}

	    // clone it: return a new GreaterEqual
	    base *clone() const { return new GreaterEqual(*this); }

	    // evaluate: return 1 if arg1 < arg2, 0 otherwise
	    var exec(const var &left, const var &right) const { return left.dbl() >= right.dbl(); }
	    double exec_dbl(double left, double right)  const { return left >= right; }
	};


	// ---------------  Add -----------------------------------------
	// Addition. What else could be said?

	class Add : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    Add(const base *left=0, const base *right=0) : binary_operator("+",left,right) {}
	    Add(const std::vector<base*> &left, const std:;vector<base*> &right) : binary_operator("+",left,right) {}
	    Add(const Add &o) : binary_operator("+",o.left_,o.right_) {}

	    // clone it: return a new Add
	    base *clone() const { return new Add(*this); }

	    // evaluate: return the sum of the two arguments
	    var exec(const var &left, const var &right) const { return left.dbl() + right.dbl(); }
	    double exec_dbl(double left, double right)  const { return left + right; }

	    void create_derivative_spec(const std::vector<base*> &leftderiv, const std::vector<base*> &rightderiv, std::vector<base*> &derivs) const
		{
		    derivs.push_back(new Add(leftderiv,rightderiv));
		}
	};


	// ---------------  Sub -----------------------------------------
	// Subtraction. 

	class Sub : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    Sub(const base *left=0, const base *right=0) : binary_operator("-",left,right) {}
	    Sub(const std::vector<base*> &left, const std:;vector<base*> &right) : binary_operator("-",left,right) {}
	    Sub(const Sub &o) : binary_operator("-",o.left_,o.right_) {}

	    // clone: create a new Sub
	    base *clone() const { return new Sub(*this); }

	    var exec(const var &left, const var &right) const { return left.dbl() - right.dbl(); }
	    double exec_dbl(double left, double right)  const { return left - right; }

	    void create_derivative_spec(const std::vector<base*> &leftderiv, const std::vector<base*> &rightderiv, std::vector<base*> &derivs) const
		{
		    derivs.push_back(new Sub(leftderiv,rightderiv));
		}
	};


	// --------------- Mod  -----------------------------------------

	class Mod : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    Mod(const base *left=0, const base *right=0) : binary_operator("%",left,right) {}
	    Mod(const Mod &o) : binary_operator("%",o.left_,o.right_) {}

	    // clone: create a new Mod
	    base *clone() const { return new Mod(*this); }

	    var exec(const var &, const var &) const 
		{
		    return left.integer()%right.integer();
		}
	    double exec_dbl(double,double) const 
		{
		    return left.integer()%right.integer();
		}
	};


	// ---------------  Mul -----------------------------------------
	// Multiplication

	class Mul : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    Mul(const base *left=0, const base *right=0) : binary_operator("*",left,right) {}
	    Mul(const Mul &o) : binary_operator("*",o.left_,o.right_) {}

	    // clone: create a new Mul
	    base *clone() const { return new Mul(*this); }

	    var exec(const var &l, const var &r) const 
		{
		    return l*r;
		}
	    double exec_dbl(double,double) const 
		{
		    return l.dbl()*r.dbl();
		}
	    void create_derivative_spec(const std::vector<base*> &leftderiv, const std::vector<base*> &rightderiv, std::vector<base*> &derivs) const
		{
		    Mul *leftprime_right = new Mul(leftderiv,right_);
		    Mul *left_rightprime = new Mul(left_,rightderiv);
		    derivs.push_back(new Add(leftprime_right,left_rightprime));
		}
/*
	    // derivative, with some optimization to avoid complicated expression
	    // trees with unuseful elements
	    base *create_derivative_spec(base *aprime, base *bprime) const
		{
		    base *aprimeb=0, *abprime=0;

		    constant *aprime_c = dynamic_cast<constant *>(aprime);
		    constant *bprime_c = dynamic_cast<constant *>(bprime);

		    if(aprime_c && aprime_c->value_ == 1.0)
		    {
			delete aprime;
			aprimeb = right_->clone();
		    }
		    else if(aprime_c && aprime_c->value_ == 0.0)
		    {
			delete aprime;
			aprimeb = 0;
		    }
		    else
		    {
			Mul *mul = new Mul;
			mul->left_ = aprime;
			mul->right_ = right_->clone();
			aprimeb = mul;
		    }

		    if(bprime_c && bprime_c->value_ == 1.0)
		    {
			delete bprime;
			abprime = left_->clone();
		    }
		    else if(bprime_c && bprime_c->value_ == 0.0)
		    {
			delete bprime;
			abprime = 0;
		    }
		    else
		    {
			Mul *mul = new Mul;
			mul->left_ = left_->clone();
			mul->right_ = bprime;
			abprime = mul;
		    }
	
		    if(aprimeb == 0 && abprime == 0) return new constant(0.0);
		    if(aprimeb == 0) return abprime;
		    if(abprime == 0) return aprimeb;

		    Add *result = new Add;
		    result->left_  = aprimeb;
		    result->right_ = abprime;
		    return result;
		}
*/
	};


	// ---------------  Div -----------------------------------------
	// Division

	class Div : public binary_operator
	{
	public:
	    // initialization. left and right are cloned (if they are not 0)
	    Div(const base *left=0, const base *right=0) : binary_operator("/",left,right) {}
	    Div(const Div &o) : binary_operator("/",o.left_,o.right_) {}

	    // clone: create a new Div
	    base *clone() const { return new Div(*this); }

	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x="x", const var &y="y", const var &z="z") const
		{
		    var l = left_->sprint_latex(pars, parvalue, x, y, z);
		    var r = right_->sprint_latex(pars, parvalue, x, y, z);
		    if(l.size() > 4 || r.size() > 4)
		    {
			return var(" \\frac{") & l & var("}{") & r & var("} ");
		    }
		    return binary_operator::sprint_latex(pars, parvalue, x, y, z);
		}
	    

	    var exec(const var &, const var &) const 
		{
		    cerr<<"Div::exec should never be called"<<endl;
		    return "";
		}
	    double exec_dbl(double,double) const 
		{
		    cerr<<"Div::exec_dbl should never be called"<<endl;
		    return 0;
		}

	    // evaluate: left/right
	    void eval(const std::vector<var> &args,
		      const std::vector<blop::var> &def_args,
		      const std::vector<var> &params,
		      std::vector<blop::var> &result,
		      int *ind) const
		{
		    // This is already updated to the new code!!!!!!!!!!!!
		    int dummy_ind = 0;
		    left_->eval(args,def_args,params,tmp, &dummy_ind);
		    const double left = tmp[0].dbl();
		    if(left == 0.0)
		    {
			result[(*ind)++] = 0;
			return;
		    }

		    dummy_ind = 0;
		    right_->eval(args,def_args,params,tmp, &dummy_ind);
		    const double right = tmp[0].dbl();
		    result[(*ind)++] = left/right;
		}
	    void eval_dbl(const std::vector<var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<var> &params,
			  std::vector<blop::var> &result,
			  int *ind) const
		{
		    // This is already updated to the new code!!!!!!!!!!!!
		    int dummy_ind = 0;
		    left_->eval_dbl(args,def_args,params,tmp, &dummy_ind);
		    const double left = tmp[0].dbl();
		    if(left == 0.0)
		    {
			result[(*ind)++].dbl(0);
			return;
		    }

		    dummy_ind = 0;
		    right_->eval_dbl(args,def_args,params,tmp, &dummy_ind);
		    const double right = tmp[0].dbl();
		    result[(*ind)++].dbl(left/right);
		}


	    // derivative. Definition at the end since it needs
	    // other expressions, not yet defined
	    void create_derivative_spec(const std::vector<base*> &leftderiv, const std::vector<base*> &rightderiv, std::vector<base*> &derivs) const;
	};

	// ---------------  Atan2 -----------------------------------------
	// Return the value atan(y/x), where y is the first argument,
	// x is the second one

	class Atan2 : public binary_function
	{
	public:
	    // initialization: left and right are cloned (if they are not 0)
	    Atan2(const base *left=0, const base *right=0)
		: binary_function("atan2",left,right) {}
	    Atan2(const Atan2 &o) : binary_function("atan2",o.left_,o.right_) {}

	    // clone: create a new Atan2
	    base *clone() const { return new Atan2(*this); }

	    var exec(const var &left, const var &right) const
		{ return ::atan2(left.dbl(),right.dbl()); }
	    double exec_dbl(double left,double right)   const
		{ return ::atan2(left,right); }

	    // derivative: definition later, since it needs other expressions,
	    // which are not yet defined
	    void create_derivative_spec(const std::vector<base*> &leftderiv, const std::vector<base*> &rightderiv, std::vector<base*> &derivs) const;
	};
    

	// ---------------  Pow -----------------------------------------
	// Return the value left^right

	class Pow : public binary_function
	{
	public:
	    // initialization: left and right are cloned (if they are not 0)
	    Pow(const base *left=0, const base *right=0) : binary_function("pow",left,right) {}
	    Pow(const Pow &o) : binary_function("pow",o.left_,o.right_) {}

	    // clone: create a new Pow
	    base *clone() const { return new Pow(*this); }

	    var exec(const var &left, const var &right) const { return ::pow(left.dbl(),right.dbl()); }
	    double exec_dbl(double left,double right)   const { return ::pow(left,right); }

	    // derivative: definition later, since it needs other expressions,
	    // which are not yet defined
	    void create_derivative_spec(const std::vector<base*> &leftderiv, const std::vector<base*> &rightderiv, std::vector<base*> &derivs) const;


	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x, const var &y, const var &z) const;

	};



	// ---------------  unary_base ----------------------------------
	// An abstract class serving as base for expressions taking only
	// one argument (operand)

	class unary_base : public base
	{
	public:
	    var name_;
	    std::vector<base*> operand_;

	    virtual var exec(const var &op) const = 0;
	    virtual double exec_dbl(double op) const = 0;

	    int n_out_;

	    void calc_n_()
		{
		    n_out_ = 0;
		    nargs_ = 0;
		    npars_ = 0;
		    for(unsigned int i=0; i<operand_.size(); ++i)
		    {
			n_out_ += operand_[i]->n_out();
			if(operand_[i]->nargs() > nargs_) nargs_ = operand_[i]->nargs();
			if(operand_[i]->npars() > npars_) npars_ = operand_[i]->npars();
			if((unsigned int)(operand_[i]->n_out()) > tmp.size()) tmp.resize(operand_[i]->n_out());
		    }
		}

	    void init_unary_base_(const var &name, const std::vector<base*> &op)
		{
		    name_ = name;
		    operand_.resize(op.size());
		    for(unsigned int i=0; i<op.size(); ++i) operand_[i] = op[i]->clone();
		    calc_n_();
		}

	public:
	    // initialization. the operand 'op' is cloned, and this cloned pointer
	    // is stored
	    unary_base(const var &name, const std::vector<base*> &op)
		{
		    init_unary_base_(name,op);
		}
	    unary_base(const unary_base &o)
		{
		    init_unary_base_(o.name_,o.operand_);
		}
	    void operand(const std::vector<base*> &op)
		{
		    init_unary_base(name_,op);
		}

	    // destructor: the operand is (recursively) deleted
	    ~unary_base()
		{
		    for(unsigned int i=0; i<operand_.size(); ++i) delete operand_[i];
		}

	    int n_out() const { return n_out_; }

	    bool uses_arg(int i) { for(unsigned int i=0; i<operand_.size(); ++i) if(operand_[i]->uses_arg(i)) return true; return false; }
	    bool uses_par(int i) { for(unsigned int i=0; i<operand_.size(); ++i) if(operand_[i]->uses_par(i)) return true; return false; }

	    void eval(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var> &result, int *ind) const
		{
		    int dummy_ind = 0;
		    operand_->eval(args,def_args,pars,tmp,&dummy_ind);
		    for(int i=0; i<dummy_ind; ++i) result[(*ind)++] = exec(tmp[i]);
		}
	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &pars,
			  std::vector<blop::var> &result, int *ind) const
		{
		    int dummy_ind = 0;
		    operand_->eval(args,def_args,pars,tmp,&dummy_ind);
		    for(int i=0; i<dummy_ind; ++i) result[(*ind)++].dbl(exec_dbl(tmp[i].dbl()));
		}

	    // the derivative of the composite function f(g(x)) is f'(g(x)) * g'(x)
	    // where f refers to this base, and g is its operand
	    // create_my_derivative returns the derivative of this expression,
	    // that is, f'(g(x)) [this is specific to the different unary
	    // expressions], and create_derivative will use this to assemble
	    // f'(g(x)) * g'(x)
	    virtual void create_my_derivative(std::vector<base*> &derivs) const = 0;

	    // return the derivative f'(g(x)) * g'(x)
	    void create_derivative(int i, std::vector<base*> &derivs) const
		{
		    // calculate the operand's derivative
		    std::vector<base*> op_deriv;
		    operand_->create_derivative(i, op_deriv);

		    std::vector<base*> my_deriv;
		    create_my_derivative(my_deriv);

		    derivs.push_back(new Mul(my_deriv,op_deriv));
		}


	    bool equals(const base *o) const
		{
		    const unary_base *c = dynamic_cast<const unary_base *>(o);
		    return (c && c->name_.str() == name_.str() && operand_->equals(c->operand_));
		}
	};


	// ---------------------  bessel ---------------------------------
#ifdef HAVE_GSL	
	class bessel : public unary_base
	{
	public:
	    enum type { J, Y, I, K, j, y, i_scaled, k_scaled };
	    static void setup_bessel_core(int nu, const function &a, bessel::type type,
					  function &result);
	private:
	    type type_;
	    int n_;
	public:
	    bessel(const base *operand, type t=J, int n=0)
		: unary_base("bessel",operand), type_(t), n_(n) {}
	    bessel(type t, int n) : unary_base("bessel",0), type_(t), n_(n) {}
	    bessel(const bessel &o) : unary_base("bessel",o.operand_), type_(o.type_), n_(o.n_) {}
	    base *clone() const { return new bessel(*this); }

	    var exec(const var &) const 
		{
		    cerr<<"bessel::exec should never be called"<<endl;
		    return "";
		}
	    double exec_dbl(double) const 
		{
		    cerr<<"bessel::exec_dbl should never be called"<<endl;
		    return 0;
		}

	    // evaluate: return arg^exponent
	    void eval(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &params,
		      std::vector<blop::var> &result,
		      int *ind) const;
	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &params,
			  std::vector<blop::var> &result,
			  int *ind) const;

	    // derivative: definition later, since needs other expressions, which are
	    // not yet defined
	    void create_my_derivative(std::vector<base*> &my_deriv) const;

	    // character representation is pow(operand,n). This function is defined in the C++
	    // interface (and returns an Ipow), so this representation can be directly pasted
	    // into the interpreter
	    var sprint(const std::vector<blop::var> &pars, bool parvalue) const
		{
		    var result = "";
		    if(type_ == J) result = "bessel_J";
		    else if(type_ == Y) result = "bessel_Y";
		    else if(type_ == I) result = "bessel_I";
		    else if(type_ == K) result = "bessel_K";
		    else if(type_ == j) result = "bessel_j";
		    else if(type_ == y) result = "bessel_y";
		    else if(type_ == i_scaled) result = "bessel_i_scaled";
		    else if(type_ == k_scaled) result = "bessel_k_scaled";
		    else result = "bessel_?";
		    result &= "(" & var(n_) & ",";
		    result &= operand_->sprint(pars, parvalue);
		    result &= ")";
		    return result;
		}
	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x, const var &y, const var &z) const
		{
		    var result = "";
		    if(type_ == J) result = "bessel\\_J";
		    else if(type_ == Y) result = "bessel\\_Y";
		    else if(type_ == I) result = "bessel\\_I";
		    else if(type_ == K) result = "bessel\\_K";
		    else if(type_ == j) result = "bessel\\_j";
		    else if(type_ == y) result = "bessel\\_y";
		    else if(type_ == i_scaled) result = "bessel\\_i\\_scaled";
		    else if(type_ == k_scaled) result = "bessel\\_k\\_scaled";
		    else result = "bessel\\_?";
		    result &= "(" & var(n_) & ",";
		    result &= operand_->sprint_latex(pars, parvalue, x, y, z);
		    result &= ")";
		    return result;
		}
	    

	};
#endif


	// ---------------  Ipow ----------------------------------------
	// Integer power: operand^n

	class Ipow : public unary_base
	{
	private:
	    int exponent_;
	public:
	    // initialization. operand is cloned (if not 0)
	    Ipow(const base *operand, int exponent=0) : unary_base("pow",operand), exponent_(exponent) {}
	    Ipow(int exponent) : unary_base("pow",0), exponent_(exponent){}
	    Ipow(const Ipow &o) : unary_base("pow",o.operand_), exponent_(o.exponent_) {}

	    // clone: create a new Ipow
	    base *clone() const {return new Ipow(*this); }

	    var exec(const var &) const 
		{
		    cerr<<"Ipow::exec should never be called"<<endl;
		    return "";
		}
	    double exec_dbl(double) const 
		{
		    cerr<<"Ipow::exec_dbl should never be called"<<endl;
		    return 0;
		}

	    // evaluate: return arg^exponent
	    void eval(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &params,
		      std::vector<blop::var> &result,
		      int *ind) const
		{
		    if(exponent_ == 0) {result[(*ind)++] = 1; return; }
		    int dummy = 0;
		    operand_->eval(args,def_args,params,tmp,&dummy);
		    const double val = tmp[0].dbl();
		    double res = 1;
		    if(exponent_ > 0) for(int i=0; i<exponent_; ++i) res *= val;
		    else for(int i=0; i<exponent_; ++i) res /= val;
		    result[(*ind)++] = res;
		}		
	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &params,
			  std::vector<blop::var> &result,
			  int *ind) const
		{
		    if(exponent_ == 0) {result[(*ind)++] = 1; return; }
		    int dummy = 0;
		    operand_->eval_dbl(args,def_args,params,tmp,&dummy);
		    const double val = tmp[0].dbl();
		    double res = 1;
		    if(exponent_ > 0) for(int i=0; i<exponent_; ++i) res *= val;
		    else for(int i=0; i<exponent_; ++i) res /= val;
		    result[(*ind)++].dbl(res);
		}		
	    

	    // derivative: definition later, since needs other expressions, which are
	    // not yet defined
	    void create_my_derivative(std::vector<base*> &my_deriv) const;


	    // character representation is pow(operand,n). This function is defined in the C++
	    // interface (and returns an Ipow), so this representation can be directly pasted
	    // into the interpreter
	    var sprint(const std::vector<blop::var> &pars, bool parvalue) const
		{
		    var result = name_;
		    result &= "(";
		    result &= operand_->sprint(pars, parvalue);
		    result &= ",";
		    result &= var(exponent_);
		    result &= ")";
		    return result;
		}
	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x, const var &y, const var &z) const;
	};



	// ---------------  unary_function ------------------------------
	// this is an abstract class serving as base for those unary expressions,
	// which are represented as function calls (it defines only sprint method)

	class unary_function : public unary_base
	{
	public:
	    // initialization. operand is cloned (if not 0)
	    unary_function(const var &name,base *operand) : unary_base(name,operand) {}
	    unary_function(const unary_function &o) : unary_base(o.name_, o.operand_) {}

	    // character representation: functionname(arg)
	    var sprint(const std::vector<blop::var> &pars, bool parvalue) const
		{
		    var result = name_;
		    result &= "(";
		    result &= operand_->sprint(pars, parvalue);
		    result &= ")";
		    return result;
		}
	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x, const var &y, const var &z) const
		{
		    var result = var("\\mathrm{") & name_ & var("}");
		    result &= "\\left(";
		    result &= operand_->sprint_latex(pars, parvalue, x, y, z);
		    result &= "\\right)";
		    return result;
		}
	};


	// ---------------  unary_operator ------------------------------
	// this is an abstract class serving as base for those unary expressions,
	// which are represented as operators (currently, only the - operator)

	class unary_operator : public unary_base
	{
	public:
	    // initialization. operand is cloned (if not 0)
	    unary_operator(const var &name,base *op) : unary_base(name,op) {}
	    unary_operator(const unary_operator &o) : unary_base(o.name_, o.operand_) {}

	    // character representation: operatorsymbol(arg)
	    var sprint(const std::vector<blop::var> &pars, bool parvalue) const
		{
		    var result = name_;
		    result &= "(";
		    result &= operand_->sprint(pars, parvalue);
		    result &= ")";
		    return result;
		}
	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x, const var &y, const var &z) const
		{
		    var result = name_;
		    result &= "(";
		    result &= operand_->sprint_latex(pars, parvalue, x, y, z);
		    result &= ")";
		    return result;
		}
	};


	// ---------------  Neg -----------------------------------------
	// Negation: multiplication by -1

	class Neg : public unary_operator
	{
	public:
	    // initialization. operand is cloned (if not 0)
	    Neg(base *operand = 0) : unary_operator("-",operand) {}
	    Neg(const Neg &o) : unary_operator("-",o.operand_) {}

	    // clone: create a new Neg
	    base *clone() const { return new Neg(*this); }

	    var exec(const var &op) const { return -op.dbl(); }
	    double exec_dbl(double op) const { return -op; }
	    // my own derivative is -1
	    void create_my_derivative(std::vector<base*> &my_deriv) const
		{
		    for(int i=0; i<op_->n_out(); ++i) my_deriv.push_back(new constant(-1.0)); 
		}
	};


	// -------------------   sign  ----------------------------------
	// Returns the sign of the argument

	class Sign : public unary_function
	{
	public:
	    // initialization. operand is cloned
	    Sign(base *operand = 0) : unary_function("sign",operand) {}
	    Sign(const Sign &o) : unary_function("sign",o.operand_) {}
	    base *clone() const { return new Sign(*this); }
	    void create_my_derivative(std::vector<base*> &derivs) { for(int i=0; i<op_->n_out(); ++i) derivs.push_back(new constant(0.0)); }

	    var exec(const var &op) const { if(op.dbl()<0) return -1; if(op.dbl()>0) return 1; return 0; }
	    double exec_dbl(double op) const { if(op<0) return -1; if(op>0) return 1; return 0; }
	};


	// ---------------  unary_c_function ----------------------------
	// This is a class serving as a base class for those expressions,
	// which are implemented as a 1-argument C-library math function
	// (for example sin, cos, exp, etc)
	// This class takes the pointer to the C-function, and defines
	// the eval function.

	class unary_c_function : public unary_function
	{
	private:
	    double (*func_)(double);
	public:
	    // initialization. operand is cloned
	    unary_c_function(double (*f)(double), const var &name, base *operand = 0)
		: unary_function(name,operand) {func_ = f;}
	    unary_c_function(const unary_c_function &o)
		: unary_function(o.name_,o.operand_) { func_ = o.func_; }

	    var exec(const var &op)    const
		{
		    double v = func_(op.dbl());
		    if(!isfinite(v)) v = unset;
		    return v;
		}
	    double exec_dbl(double op) const { double v = func_(op);       if(!isfinite(v)) v = unset; return v; }

	};

	// ---------------  C-math functions ----------------------------

	class Sin : public unary_c_function
	{
	public:
	    Sin(base *operand = 0) : unary_c_function(::sin,"sin",operand) {}
	    Sin(const Sin &o) : unary_c_function(::sin,"sin",o.operand_) {}
	    base *clone() const {return new Sin(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Asin : public unary_c_function
	{
	public:
	    Asin(base *operand = 0) : unary_c_function(::asin,"asin",operand) {}
	    Asin(const Asin &o) : unary_c_function(::asin,"asin",o.operand_) {}
	    base *clone() const { return new Asin(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Cos : public unary_c_function
	{
	public:
	    Cos(base *operand = 0) : unary_c_function(::cos,"cos",operand) {}
	    Cos(const Cos &o) : unary_c_function(::cos,"cos",o.operand_) {}
	    base *clone() const { return new Cos(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Acos : public unary_c_function
	{
	public:
	    Acos(base *operand = 0) : unary_c_function(::acos,"acos",operand) {}
	    Acos(const Acos &o) : unary_c_function(::acos,"acos",o.operand_) {}
	    base *clone() const { return new Acos(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Tan : public unary_c_function
	{
	public:
	    Tan(base *operand = 0) : unary_c_function(::tan,"tan",operand) {}
	    Tan(const Tan &o) : unary_c_function(::tan,"tan",o.operand_) {}
	    base *clone() const { return new Tan(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Atan : public unary_c_function
	{
	public:
	    Atan(base *operand = 0) : unary_c_function(::atan,"atan",operand) {}
	    Atan(const Atan &o) : unary_c_function(::atan,"atan",o.operand_) {}
	    base *clone() const { return new Atan(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	double cot(double a);
	class Cot : public unary_c_function
	{
	public:
	    Cot(base *operand = 0) : unary_c_function(blop::function_core::cot,"cot",operand) {}
	    Cot(const Cot &o) : unary_c_function(blop::function_core::cot,"cot",o.operand_) {}
	    base *clone() const { return new Cot(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	double acot(double a);
	class Acot : public unary_c_function
	{
	public:
	    Acot(base *operand = 0) : unary_c_function(blop::function_core::acot,"acot",operand) {}
	    Acot(const Acot &o) : unary_c_function(blop::function_core::acot,"acot",o.operand_) {}
	    base *clone() const { return new Acot(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Sinh : public unary_c_function
	{
	public:
	    Sinh(base *operand = 0) : unary_c_function(::sinh,"sinh",operand) {}
	    Sinh(const Sinh &o) : unary_c_function(::sinh,"sinh",o.operand_) {}
	    base *clone() const { return new Sinh(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Cosh : public unary_c_function
	{
	public:
	    Cosh(base *operand = 0) : unary_c_function(::cosh,"cosh",operand) {}
	    Cosh(const Cosh &o) : unary_c_function(::cosh,"cosh",o.operand_) {}
	    base *clone() const { return new Cosh(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Asinh : public unary_c_function
	{
	public:
	    Asinh(base *operand = 0) : unary_c_function(::asinh,"asinh",operand) {}
	    Asinh(const Asinh &o) : unary_c_function(::asinh,"asinh",o.operand_) {}
	    base *clone() const { return new Asinh(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Acosh : public unary_c_function
	{
	public:
	    Acosh(base *operand = 0) : unary_c_function(::acosh,"acosh",operand) {}
	    Acosh(const Acosh &o) : unary_c_function(::acosh,"acosh",o.operand_) {}
	    base *clone() const { return new Acosh(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Tanh : public unary_c_function
	{
	public:
	    Tanh(base *operand = 0) : unary_c_function(::tanh,"tanh",operand) {}
	    Tanh(const Tanh &o) : unary_c_function(::tanh,"tanh",o.operand_) {}
	    base *clone() const { return new Tanh(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Atanh : public unary_c_function
	{
	public:
	    Atanh(base *operand = 0) : unary_c_function(::atanh,"atanh",operand) {}
	    Atanh(const Atanh &o) : unary_c_function(::atanh,"atanh",o.operand_) {}
	    base *clone() const { return new Atanh(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Floor : public unary_c_function
	{
	public:
	    Floor(base *operand = 0) : unary_c_function(::floor,"floor",operand) {}
	    Floor(const Floor &o) : unary_c_function(::floor,"floor",o.operand_) {}
	    base *clone() const { return new Floor(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs);
	};

	class Ceil : public unary_c_function
	{
	public:
	    Ceil(base *operand = 0) : unary_c_function(::ceil,"ceil",operand) {}
	    Ceil(const Ceil &o) : unary_c_function(::ceil,"ceil",o.operand_) {}
	    base *clone() const { return new Ceil(*this); }
	    void create_my_derivative(std::vector<base*> &my_derivs) { 
	};

	class Erf : public unary_c_function
	{
	public:
	    Erf(base *operand = 0) : unary_c_function(::erf,"erf",operand) {}
	    Erf(const Erf &o) : unary_c_function(::erf,"erf",o.operand_) {}
	    base *clone() const { return new Erf(*this); }
	    base *create_my_derivative() const;
	};

	class Exp : public unary_c_function
	{
	public:
	    Exp(base *operand = 0) : unary_c_function(::exp,"exp",operand) {}
	    Exp(const Exp &o) : unary_c_function(::exp,"exp",o.operand_) {}
	    base *clone() const { return new Exp(operand_); }
	    base *create_my_derivative() const { return new Exp(operand_); }
	};

	class Log : public unary_c_function
	{
	public:
	    Log(base *operand = 0) : unary_c_function(::log,"log",operand) {}
	    Log(const Log &o) : unary_c_function(::log,"log",o.operand_) {}
	    base *clone() const { return new Log(*this); }
	    base *create_my_derivative() const
		{
		    Div *div = new Div;
		    div->left(new constant(1.0));
		    div->right(operand_->clone());
		    return div;
		}
	};

	class Log10 : public unary_c_function
	{
	public:
	    Log10(base *operand = 0) : unary_c_function(::log10,"log10",operand) {}
	    Log10(const Log10 &o) : unary_c_function(::log10,"log10",o.operand_) {}
	    base *clone() const { return new Log10(*this); }
	    base *create_my_derivative() const
		{
		    Div *div = new Div;
		    div->left(new constant(1.0/::log(10.0)));
		    div->right(operand_->clone());
		    return div;
		}
	};

	class Sqrt : public unary_c_function
	{
	public:
	    Sqrt(base *operand = 0) : unary_c_function(::sqrt,"sqrt",operand) {}
	    Sqrt(const Sqrt &o) : unary_c_function(::sqrt,"sqrt",o.operand_) {}
	    base *clone() const { return new Sqrt(*this); }
	    base *create_my_derivative() const
		{
		    Div *div = new Div;
		    div->left(new constant(0.5));
		    div->right(new Sqrt(operand_));
		    return div;
		}
	};

	class Abs : public unary_c_function
	{
	public:
	    Abs(base *operand = 0) : unary_c_function(::fabs,"abs",operand) {}
	    Abs(const Abs &o) : unary_c_function(::fabs,"abs",o.operand_) {}
	    base *clone() const { return new Abs(*this); }
	    base *create_my_derivative() const
		{
		    Sign *s = new Sign;
		    s->operand(operand_->clone());
		    return s;
		}
	};


	// ---------------  funcparameter -------------------------------
	// this expression returns the value of a given PARAMETER
	// of the function (the second argument of base::eval)

	class funcparameter : public base
	{
	private:
	    int parameter_index_;
	public:
	    funcparameter(int i) : parameter_index_(::max(i-1,0)) {}
	    base *clone() const {return new funcparameter(*this);}
	    void   eval(const std::vector<blop::var> &,
			const std::vector<blop::var> &def_args,
			const std::vector<blop::var> &pars,
			std::vector<blop::var> &result,int *ind) const
		{
		    if((int)pars.size() <= parameter_index_)
		    {
			var msg = "Function does not have so many [";
			msg &= var(parameter_index_+1);
			msg &= "] parameters";
			warning::print(msg, "function::funcparameter::eval(...)");
			result[(*ind)++] = "";
			return;
		    }
		    result[(*ind)++] = pars[parameter_index_];
		}
	    void   eval_dbl(const std::vector<blop::var> &args,
			    const std::vector<blop::var> &def_args,
			    const std::vector<blop::var> &pars,
			    std::vector<blop::var> &result,int *ind) const
		{
		    if((int)pars.size() <= parameter_index_) result[(*ind)++].dbl(0);
		    else result[(*ind)++].dbl(pars[parameter_index_].dbl());
		}

	    // it does not require any arguments, only parameter
	    int nargs() const {return 0;}
	    int npars() const { if(npars_>=0) return npars_; return parameter_index_ + 1; }
	    bool uses_par(int i) const { return i-1==(int)parameter_index_;}

	    var sprint(const std::vector<blop::var> &pars, bool parvalue) const;
	    var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
			     const var &x, const var &y, const var &z) const;

	    base *create_derivative(int i) const
		{
		    if(-i-1 == parameter_index_) return new constant(1.0);
		    return new constant(0.0);
		}
		     

	    bool equals(const base *o) const
		{
		    const funcparameter *c = dynamic_cast<const funcparameter *>(o);
		    return (c && c->parameter_index_ == parameter_index_);
		}
	};



	// ---------------  argument_subst ------------------------------
	// this expression is a wrapper around another expression:
	// it calls that other expression with replaced arguments

	class argument_subst : public base
	{
	private:
	    base *base_;               // the expression to be called
	    std::vector<base *> args_; // the expressions to calculate the replaced arguments
	    argument_subst() { base_ = 0; }
	    mutable std::vector<blop::var> tmp_;
	    void init_tmp_();
	public:
	    argument_subst(base *f,int n,const base *ar[]);        // clone 'f', and clone all elements of 'ar'
	    argument_subst(base *f,const std::vector<base*> &ar);  // clone 'f', and clone all elements of 'ar'
	    argument_subst(const argument_subst &);
	    ~argument_subst();
	    base *clone() const { return new argument_subst(*this); }
	    void eval(const std::vector<blop::var> &,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &,
		      std::vector<blop::var> &result,int *ind) const;
	    void eval_dbl(const std::vector<blop::var> &,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &,
			  std::vector<blop::var> &result,int *ind) const;

	    int nargs() const;
	    int npars() const;

	    base *create_derivative(int) const;
	    var sprint(const std::vector<blop::var> &, bool parvalue) const;
	    virtual var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
				     const var &x="x", const var &y="y", const var &z="z") const;

	    bool equals(const base *o) const;

	    int n_out() const;

	    bool uses_arg(int i);
	    bool uses_par(int i);

	};



	// ---------------  concatenator --------------------------------
	// concatenates its two arguments (as strings)

	class concatenator : public binary_operator
	{
	public:
	    concatenator(const function_core::base *l=0,const function_core::base *r=0) : binary_operator("&",l,r) {}
	    concatenator(const concatenator &o) : binary_operator("&",o.left_,o.right_) {}
	    function_core::base *clone() const {return new concatenator(*this);}

	    var exec(const var &left, const var &right) const
		{
		    var result = left & right;
		    return result;
		}
	    double exec_dbl(double left, double right) const
		{
		    cerr<<"concatenator::exec_dbl should never be called"<<endl;
		    return 0;
		}

	    void eval_dbl(const std::vector<blop::var> &args,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &pars,
			  std::vector<blop::var> &result, int *runind) const
		{
		    eval(args, def_args, pars, result, runind);
		}
	    function_core::base *create_derivative_spec(base *, base *) const { return new constant(0.0); }
	};


	// ---------------  logical_and ---------------------------------

	class logical_and : public binary_operator
	{
	public:
	    logical_and(const base *l=0,const base *r=0) : binary_operator("&&",l,r) {}
	    logical_and(const logical_and &o) : binary_operator("&&",o.left_,o.right_) {}
	    base *clone() const {return new logical_and(*this);}

	    var exec(const var &left, const var &right) const { return (left.dbl()!=0.0) && (right.dbl()!=0.0); }
	    double exec_dbl(double left, double right) const { return (left!=0.0) && (right!=0.0); }

	    base *create_derivative_spec(base*,base*) const { return new constant(0.0); }
	};

	// ---------------  logical_or ----------------------------------

	class logical_or : public binary_operator
	{
	public:
	    logical_or(const base *l=0,const base *r=0) : binary_operator("||",l,r) {}
	    logical_or(const logical_or &o) : binary_operator("||",o.left_,o.right_) {}
	    base *clone() const {return new logical_or(*this);}

	    var exec(const var &left, const var &right) const { return (left.dbl()!=0.0) || (right.dbl()!=0.0); }
	    double exec_dbl(double left, double right) const { return (left!=0.0) || (right!=0.0); }

	    base* create_derivative_spec(base*,base*) const { return new constant(0.0); }
	};


	// ---------------------  contained ---------------------------------
	class contained_in : public base
	{
	private:
	    std::vector<base*> in_this_;
	    contained_in() {}

	public:
	    contained_in(const std::vector<base*> &a); // clone all elements of 'a'
	    contained_in(const contained_in &o);
	    ~contained_in();
	    base *clone() const {return new contained_in(*this); }

	    // evaluate
	    void eval(const std::vector<blop::var> &,
		      const std::vector<blop::var> &def_args,
		      const std::vector<blop::var> &,
		      std::vector<blop::var> &result,
		      int *ind) const;
	    void eval_dbl(const std::vector<blop::var> &,
			  const std::vector<blop::var> &def_args,
			  const std::vector<blop::var> &,
			  std::vector<blop::var> &result,
			  int *ind) const;

	    int nargs() const;
	    int npars() const;
	    var sprint(const std::vector<blop::var> &, bool) const;
	    base *create_derivative(int) const { return new constant(0.0); }
	    bool equals(const base *o) const { return false; }
	};
	

    }
}

#endif
