#include "function.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "config.h"
#include "exc.hh"
#include "pstream.h"
#include "constants.h"
#include "warning.h"
#include "cfunc_wrapper.hh" 
#include "function_core.hh"
#include "dgraph.h"
#include "bloputils.h"

#ifdef HAVE_GSL
#include <gsl/gsl_sf_bessel.h>
#endif

namespace blop
{
    // void function::formula(const var &)    -- is defined in blop_cint.cc and blop_nocint.cc

    using namespace function_core;

    // --------------------  constructors and destructor  --------------------------------

    var function::tmp;

    std::vector<std::string> function::column_names_;

    int sample(dgraph &g, double dx, double from, double to,
	       const function &f1,
	       const function &f2,
	       const function &f3,
	       const function &f4)
    {
	function f(f1,f2,f3,f4);
	int n = (int)((to-from)/dx)+1;
	g.columns(f.components());
	g.resize(n);
	std::vector<blop::var> args(1);
	double x = from;
	for(int i=0; i<n; ++i, x += dx)
	{
	    args[0] = x;
	    f.meval(args,g[i]);
	}
	return n;
    }

    bool function::default_check_args_ = true;

    void function::extra_param(const var &v)
    {
	function_core::extra_param::value = v;
    }

    bool function::equals(const function &f) const
    {
	if(base_.size() != f.base_.size()) return false;
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    if(!base_[i]->equals(f.base_[i])) return false;
	}
	return true;
    }

    void function::copy_arg_ranges_(const function &f1,
				    const function &f2,
				    const function &f3,
				    const function &f4,
				    const function &f5)
    {
	vector<const function *> ff;
	if(f1.initialized()) ff.push_back(&f1);
	if(f2.initialized()) ff.push_back(&f2);
	if(f3.initialized()) ff.push_back(&f3);
	if(f4.initialized()) ff.push_back(&f4);
	if(f5.initialized()) ff.push_back(&f5);

	{
	    unsigned int min_size = 0;
	    for(int i=0; i<(int)ff.size(); ++i)
		if(min_size<ff[i]->arg_min_.size()) min_size = ff[i]->arg_min_.size();
	    arg_min_.resize(min_size,unset);
	    for(unsigned int arg_i=0; arg_i<arg_min_.size(); ++arg_i)
	    {
		arg_min_[arg_i] = unset;
		for(unsigned int f_i=0; f_i<ff.size(); ++f_i)
		{
		    if(arg_i<ff[f_i]->arg_min_.size() &&
		       ff[f_i]->arg_min_[arg_i] != unset &&
		       ( ff[f_i]->arg_min_[arg_i] > arg_min_[arg_i] ||
			 arg_min_[arg_i] == unset))
		    {
			arg_min_[arg_i] = ff[f_i]->arg_min_[arg_i];
		    }
		}
	    }
	}
	{
	    unsigned int max_size = 0;
	    for(int i=0; i<(int)ff.size(); ++i)
		if(max_size<ff[i]->arg_max_.size()) max_size = ff[i]->arg_max_.size();
	    arg_max_.resize(max_size,unset);
	    for(unsigned int arg_i=0; arg_i<arg_max_.size(); ++arg_i)
	    {
		arg_max_[arg_i] = unset;
		for(unsigned int f_i=0; f_i<ff.size(); ++f_i)
		{
		    if(arg_i<ff[f_i]->arg_max_.size() &&
		       ff[f_i]->arg_max_[arg_i] != unset &&
		       ( ff[f_i]->arg_max_[arg_i] < arg_max_[arg_i] ||
			 arg_max_[arg_i] == unset))
		    {
			arg_max_[arg_i] = ff[f_i]->arg_max_[arg_i];
		    }
		}
	    }
	}

    }


    // whenever the 'base_' vector is modified (or any of its element), this
    // function should be called!!!!!
    void function::init_()
    {
	components_ = 0;
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    components_ += base_[i]->n_out();
	}
	if(components_ > 0) result_.resize(components_);

	int npars = 0;
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    if(base_[i]->npars() > npars) npars = base_[i]->npars();
	}
	if((unsigned int)npars > parameters_.size()) parameters_.resize(npars);
	check_args_ = default_check_args_;
    }

    function::function(void *fptr)
    {
	//nargs_ = npars_ = -1;
	base_.resize(1);
	base_[0] = new function_core::cfunc(fptr);
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const std::vector<function_core::base*> &bases, bool clone_them)
    {
	//nargs_ = npars_ = -1;
	base_.resize(bases.size());
	for(unsigned int i=0; i<bases.size(); ++i)
	{
	    if(clone_them) base_[i] = bases[i]->clone();
	    else base_[i] = bases[i];
	}
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const std::vector<blop::var> &c)
    {
	//nargs_ = npars_ = -1;
	for(unsigned int i=0; i<c.size(); ++i)
	{
	    base_.push_back(new constant(c[i]));
	}
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function()
    {
	//nargs_ = npars_ = -1;
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const function_core::base &f)
    {
	//nargs_ = npars_ = -1;
	base_.push_back(f.clone());
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(function_core::base *f)
    {
	//nargs_ = npars_ = -1;
	base_.push_back(f);
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(double d)
    {
	if(d != unset) base_.push_back(new constant(d));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(double d1, double d2)
    {
	if(d1 != unset) base_.push_back(new constant(d1));
	if(d2 != unset) base_.push_back(new constant(d2));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(double d1, double d2, double d3)
    {
	if(d1 != unset) base_.push_back(new constant(d1));
	if(d2 != unset) base_.push_back(new constant(d2));
	if(d3 != unset) base_.push_back(new constant(d3));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(double d1, double d2, double d3, double d4)
    {
	if(d1 != unset) base_.push_back(new constant(d1));
	if(d2 != unset) base_.push_back(new constant(d2));
	if(d3 != unset) base_.push_back(new constant(d3));
	if(d4 != unset) base_.push_back(new constant(d4));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(long double d)
    {
	if((double)d != unset) base_.push_back(new constant((double)d));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(long double d1, long double d2)
    {
	if((double)d1 != unset) base_.push_back(new constant((double)d1));
	if((double)d2 != unset) base_.push_back(new constant((double)d2));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(long double d1, long double d2, long double d3)
    {
	if((double)d1 != unset) base_.push_back(new constant((double)d1));
	if((double)d2 != unset) base_.push_back(new constant((double)d2));
	if((double)d3 != unset) base_.push_back(new constant((double)d3));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(long double d1, long double d2, long double d3, long double d4)
    {
	if((double)d1 != unset) base_.push_back(new constant((double)d1));
	if((double)d2 != unset) base_.push_back(new constant((double)d2));
	if((double)d3 != unset) base_.push_back(new constant((double)d3));
	if((double)d4 != unset) base_.push_back(new constant((double)d4));
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(int d1, double d2)
    {
	base_.push_back(new constant(d1));
	base_.push_back(new constant(d2));
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(double d1, int d2)
    {
	base_.push_back(new constant(d1));
	base_.push_back(new constant(d2));
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(int d1)
    {
	base_.push_back(new constant(d1));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(int d1, int d2)
    {
	base_.push_back(new constant(d1));
	base_.push_back(new constant(d2));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(int d1, int d2, int d3)
    {
	base_.push_back(new constant(d1));
	base_.push_back(new constant(d2));
	base_.push_back(new constant(d3));
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(int d1, int d2, int d3, int d4)
    {
	base_.push_back(new constant(d1));
	base_.push_back(new constant(d2));
	base_.push_back(new constant(d3));
	base_.push_back(new constant(d4));
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const var &v)
    {
	base_.push_back(new constant(v));
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const char *v)
    {
	base_.push_back(new constant(v));
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const string &s)
    {
	base_.push_back(new constant(s));
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(interpolator *interp)
    {
	base_.push_back(new function_core::interpolated_func(interp));
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const function &f)
    {
	parnames_ = f.parnames_;
	for(unsigned int i=0; i<f.base_.size(); ++i) base_.push_back(f.base_[i]->clone());
	print_param_value_ = f.print_param_value_;
	parameters_ = f.parameters_;

	init_();

	if(!f.derivatives_.empty())
	{
	    cerr<<"function::function(const function &) does not yet copy the derivatives!"<<endl;
	    cerr<<"Please strongly request the implementation of it by the author!!!"<<endl;
	    exit(1);
	}
	default_args_ = f.default_args_;

	copy_arg_ranges_(f);
    }

    function::function(const function &f1, const function &f2)
    {
	parnames_ = f1.parnames_;
	for(unsigned int i=parnames_.size(); i<f2.parnames_.size(); ++i)
	    parnames_.push_back(f2.parnames_[i]);
	//nargs_ = max(f1.nargs_,f2.nargs_);
	//npars_ = max(f1.npars_,f2.npars_);
	for(unsigned int i=0; i<f1.base_.size(); ++i) base_.push_back(f1.base_[i]->clone());
	for(unsigned int i=0; i<f2.base_.size(); ++i) base_.push_back(f2.base_[i]->clone());

	print_param_value_ = false;
	if(f1.print_param_value_ || f2.print_param_value_) print_param_value_ = true;
	{
	    unsigned i=0;
	    for(; i<f1.parameters_.size(); ++i) parameters_.push_back(f1.parameters_[i]);
	    for(; i<f2.parameters_.size(); ++i) parameters_.push_back(f2.parameters_[i]);
	}
	init_();

	if(!f1.derivatives_.empty() || !f2.derivatives_.empty())
	{
	    cerr<<"function::function(const function &) does not yet copy the derivatives!"<<endl;
	    cerr<<"Please strongly request the implementation of it by the author!!!"<<endl;
	    exit(1);
	}

	{
	    unsigned int i=0;
	    for(; i<f1.default_args_.size(); ++i) default_args_.push_back(f1.default_args_[i]);
	    for(; i<f2.default_args_.size(); ++i) default_args_.push_back(f2.default_args_[i]);
	}
	copy_arg_ranges_(f1,f2);
    }

    function::function(const function &f1, const function &f2, const function &f3)
    {
	parnames_ = f1.parnames_;
	for(unsigned int i=parnames_.size(); i<f2.parnames_.size(); ++i)
	    parnames_.push_back(f2.parnames_[i]);
	for(unsigned int i=parnames_.size(); i<f3.parnames_.size(); ++i)
	    parnames_.push_back(f3.parnames_[i]);
	//nargs_ = max(f1.nargs_,max(f2.nargs_,f3.nargs_));
	//npars_ = max(f1.npars_,max(f2.npars_,f3.npars_));
	for(unsigned int i=0; i<f1.base_.size(); ++i) base_.push_back(f1.base_[i]->clone());
	for(unsigned int i=0; i<f2.base_.size(); ++i) base_.push_back(f2.base_[i]->clone());
	for(unsigned int i=0; i<f3.base_.size(); ++i) base_.push_back(f3.base_[i]->clone());

	print_param_value_ = false;
	if(f1.print_param_value_ || f2.print_param_value_ || f3.print_param_value_)
	    print_param_value_ = true;
	{
	    unsigned int i=0;
	    for(; i<f1.parameters_.size(); ++i) parameters_.push_back(f1.parameters_[i]);
	    for(; i<f2.parameters_.size(); ++i) parameters_.push_back(f2.parameters_[i]);
	    for(; i<f3.parameters_.size(); ++i) parameters_.push_back(f3.parameters_[i]);
	}
	init_();

	if(!f1.derivatives_.empty() || !f2.derivatives_.empty() || !f3.derivatives_.empty())
	{
	    cerr<<"function::function(const function &) does not yet copy the derivatives!"<<endl;
	    cerr<<"Please strongly request the implementation of it by the author!!!"<<endl;
	    exit(1);
	}

	{
	    unsigned int i=0;
	    for(; i<f1.default_args_.size(); ++i) default_args_.push_back(f1.default_args_[i]);
	    for(; i<f2.default_args_.size(); ++i) default_args_.push_back(f2.default_args_[i]);
	    for(; i<f3.default_args_.size(); ++i) default_args_.push_back(f3.default_args_[i]);
	}

	copy_arg_ranges_(f1,f2,f3);
    }



    function::function(const function &f1, const function &f2, const function &f3, const function &f4)
    {
	parnames_ = f1.parnames_;
	for(unsigned int i=parnames_.size(); i<f2.parnames_.size(); ++i)
	    parnames_.push_back(f2.parnames_[i]);
	for(unsigned int i=parnames_.size(); i<f3.parnames_.size(); ++i)
	    parnames_.push_back(f3.parnames_[i]);
	for(unsigned int i=parnames_.size(); i<f4.parnames_.size(); ++i)
	    parnames_.push_back(f4.parnames_[i]);

	//nargs_ = max(max(f1.nargs_,f2.nargs_),max(f3.nargs_,f4.nargs_));
	//npars_ = max(max(f1.npars_,f2.npars_),max(f3.npars_,f4.npars_));
	for(unsigned int i=0; i<f1.base_.size(); ++i) base_.push_back(f1.base_[i]->clone());
	for(unsigned int i=0; i<f2.base_.size(); ++i) base_.push_back(f2.base_[i]->clone());
	for(unsigned int i=0; i<f3.base_.size(); ++i) base_.push_back(f3.base_[i]->clone());
	for(unsigned int i=0; i<f4.base_.size(); ++i) base_.push_back(f4.base_[i]->clone());

	print_param_value_ = false;
	if(f1.print_param_value_ ||
	   f2.print_param_value_ ||
	   f3.print_param_value_ ||
	   f4.print_param_value_) print_param_value_ = true;
	   
	{
	    unsigned int i=0;
	    for(; i<f1.parameters_.size(); ++i) parameters_.push_back(f1.parameters_[i]);
	    for(; i<f2.parameters_.size(); ++i) parameters_.push_back(f2.parameters_[i]);
	    for(; i<f3.parameters_.size(); ++i) parameters_.push_back(f3.parameters_[i]);
	    for(; i<f4.parameters_.size(); ++i) parameters_.push_back(f4.parameters_[i]);
	}
	init_();

	if(!f1.derivatives_.empty() || !f2.derivatives_.empty() || !f3.derivatives_.empty() || !f4.derivatives_.empty())
	{
	    cerr<<"function::function(const function &) does not yet copy the derivatives!"<<endl;
	    cerr<<"Please strongly request the implementation of it by the author!!!"<<endl;
	    exit(1);
	}

	{
	    unsigned int i=0;
	    for(; i<f1.default_args_.size(); ++i) default_args_.push_back(f1.default_args_[i]);
	    for(; i<f2.default_args_.size(); ++i) default_args_.push_back(f2.default_args_[i]);
	    for(; i<f3.default_args_.size(); ++i) default_args_.push_back(f3.default_args_[i]);
	    for(; i<f4.default_args_.size(); ++i) default_args_.push_back(f4.default_args_[i]);
	}

	copy_arg_ranges_(f1,f2,f3,f4);
    }

    function::function(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5)
    {
	parnames_ = f1.parnames_;
	for(unsigned int i=parnames_.size(); i<f2.parnames_.size(); ++i)
	    parnames_.push_back(f2.parnames_[i]);
	for(unsigned int i=parnames_.size(); i<f3.parnames_.size(); ++i)
	    parnames_.push_back(f3.parnames_[i]);
	for(unsigned int i=parnames_.size(); i<f4.parnames_.size(); ++i)
	    parnames_.push_back(f4.parnames_[i]);
	for(unsigned int i=parnames_.size(); i<f5.parnames_.size(); ++i)
	    parnames_.push_back(f5.parnames_[i]);

	//nargs_ = max(max(f1.nargs_,f2.nargs_),max(f3.nargs_,f4.nargs_));
	//npars_ = max(max(f1.npars_,f2.npars_),max(f3.npars_,f4.npars_));
	for(unsigned int i=0; i<f1.base_.size(); ++i) base_.push_back(f1.base_[i]->clone());
	for(unsigned int i=0; i<f2.base_.size(); ++i) base_.push_back(f2.base_[i]->clone());
	for(unsigned int i=0; i<f3.base_.size(); ++i) base_.push_back(f3.base_[i]->clone());
	for(unsigned int i=0; i<f4.base_.size(); ++i) base_.push_back(f4.base_[i]->clone());
	for(unsigned int i=0; i<f5.base_.size(); ++i) base_.push_back(f5.base_[i]->clone());

	print_param_value_ = false;
	if(f1.print_param_value_ ||
	   f2.print_param_value_ ||
	   f3.print_param_value_ ||
	   f4.print_param_value_ ||
	   f5.print_param_value_ ) print_param_value_ = true;
	   
	{
	    unsigned int i=0;
	    for(; i<f1.parameters_.size(); ++i) parameters_.push_back(f1.parameters_[i]);
	    for(; i<f2.parameters_.size(); ++i) parameters_.push_back(f2.parameters_[i]);
	    for(; i<f3.parameters_.size(); ++i) parameters_.push_back(f3.parameters_[i]);
	    for(; i<f4.parameters_.size(); ++i) parameters_.push_back(f4.parameters_[i]);
	    for(; i<f5.parameters_.size(); ++i) parameters_.push_back(f5.parameters_[i]);
	}
	init_();

	if(!f1.derivatives_.empty() || !f2.derivatives_.empty() || !f3.derivatives_.empty() || !f4.derivatives_.empty() || !f5.derivatives_.empty())
	{
	    cerr<<"function::function(const function &) does not yet copy the derivatives!"<<endl;
	    cerr<<"Please strongly request the implementation of it by the author!!!"<<endl;
	    exit(1);
	}

	{
	    unsigned int i=0;
	    for(; i<f1.default_args_.size(); ++i) default_args_.push_back(f1.default_args_[i]);
	    for(; i<f2.default_args_.size(); ++i) default_args_.push_back(f2.default_args_[i]);
	    for(; i<f3.default_args_.size(); ++i) default_args_.push_back(f3.default_args_[i]);
	    for(; i<f4.default_args_.size(); ++i) default_args_.push_back(f4.default_args_[i]);
	    for(; i<f5.default_args_.size(); ++i) default_args_.push_back(f5.default_args_[i]);
	}

	copy_arg_ranges_(f1,f2,f3,f4,f5);
    }


    function::~function()
    {
	for(unsigned int i=0; i<base_.size(); ++i) delete base_[i];
	for(deriv_map::iterator iter=derivatives_.begin(); iter != derivatives_.end(); ++iter)
	{
	    for(unsigned int i=0; i<(*iter).second.size(); ++i) delete (*iter).second[i];
	}
    }

    // ---------------------  Append more components -----------------------------------

    function &function::append(const function &f)
    {
	for(unsigned int i=parameters_.size(); i<f.parameters_.size(); ++i)
	{
	    parameters_.push_back(f.parameters_[i]);
	}

	for(unsigned int i=0; i<f.base_.size(); ++i)
	{
	    base_.push_back(f.base_[i]->clone());
	}

	if(!f.derivatives_.empty() || !derivatives_.empty())
	{
	    cerr<<"function::append does not correctly handle the user-set derivatives"<<endl;
	    cerr<<"Strongly request this feature from the author"<<endl;
	    cerr<<"Set the derivatives now after the call to function::append(...)"<<endl;
	}

	init_();
	return *this;
    }

    // -------------------  get/set parameter values  ----------------------------------

    function &function::parformat(unsigned int parindex, const var &format)
    {
	if(parindex==0)
	{
	    for(unsigned int i=0; i<parameters_.size(); ++i) parameters_[i].format(format);
	}
	else
	{
	    if(parindex > parameters_.size()) parameters_.resize(parindex);
	    parameters_[parindex-1].format(format);
	}
	return *this;
    }

    const var &function::param(unsigned int i) const
    {
	--i;
	if(i<0 || parameters_.size()<=i)
	{
	    static var dummy;
	    return dummy;
	}
	return parameters_[i];
    }

    function &function::params(double p1,
			       double p2,
			       double p3,
			       double p4,
			       double p5,
			       double p6)
    {
	param(1,p1);
	param(2,p2);
	if(p3!=unset) param(3,p3);
	if(p4!=unset) param(4,p4);
	if(p5!=unset) param(5,p5);
	if(p6!=unset) param(6,p6);
	return *this;
    }

    function &function::param(unsigned int i, const var &value)
    {
	if(i<1)
	{
	    warning::print("Parameter index < 1",
			   "function::param(int parindex, const var &value)");
	    return *this;
	}
	--i;
	if(i >= parameters_.size())  parameters_.resize(i+1);
	parameters_[i] = value;
	return *this;
    }

    function &function::param(unsigned int i, double value)
    {
	if(i<1)
	{
	    warning::print("Parameter index < 1",
			   "function::param(int parindex, const var &value)");
	    return *this;
	}
	--i;
	if(i >= parameters_.size())  parameters_.resize(i+1);
	parameters_[i] = value;
	return *this;
    }

    function &function::param(unsigned int i, const var &value, const var &name)
    {
	param(i,value);
	parname(i,name);
	return *this;
    }
    function &function::param(unsigned int i, double value, const var &name)
    {
	param(i,value);
	parname(i,name);
	return *this;
    }


    function &function::parname(unsigned int i, const var &name)
    {
	if(i<1) return *this;
	--i;
	if(i>=parnames_.size()) parnames_.resize(i+1);
	parnames_[i] = name;
	return *this;
    }
    var function::parname(unsigned int i)
    {
	if(i<1 || parnames_.size()<i) return var("parameter[") & var(i) & var("]");
	return parnames_[i-1];
    }


    // ----------------- set default arguments --------------------------------------

    function &function::def_arg(int arg_index, const var &value)
    {
	if(--arg_index >= (int)default_args_.size()) default_args_.resize(arg_index+1);
	default_args_[arg_index] = value;
	return  *this;
    }
    var function::def_arg(int arg_index) const
    {
	--arg_index;
	if(arg_index < 0) return "";
	if(arg_index >=(int)default_args_.size()) return "";
	return default_args_[arg_index];
    }

    function &function::clear_def_args()
    {
	default_args_.clear();
	return *this;
    }


    // ----------------  query of required number of args/pars ----------------------

/*
    function &function::nargs(int n)
    {
	for(unsigned int i=0; i<base_.size(); ++i) base_[i]->nargs(n);
	nargs_ = n;
	return *this;
    }
*/
    int function::nargs() const
    {
	//if(nargs_ >= 0) return nargs_;
	if(base_.empty())
	{
	    warning::print("Uninitialized function","function::nargs()");
	    return 0;
	}
	int result = 0;
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    if(base_[i]->nargs() > result) result = base_[i]->nargs();
	}
	return result;
    }

    /*
    function &function::npars(int n)
    {
	for(unsigned int i=0; i<base_.size(); ++i) base_[i]->nargs(n);
	npars_ = n;
	return *this;
    }
    */
    int function::npars() const
    {
	//if(npars_ >= 0) return npars_;
	if(base_.empty())
	{
	    warning::print("Uninitialized function","function::npars()");
	    return 0;
	}
	int result = 0;
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    if(base_[i]->npars() > result) result = base_[i]->npars();
	}
	return result;
    }

    function function::create_narg()
    {
	return actual_nargs();
    }

    bool function::make_arg_check_(const std::vector<blop::var> &args) const
    {
	if(arg_min_.empty() && arg_max_.empty()) return true;
	bool result = true;
	for(unsigned int i=0; i<args.size(); ++i)
	{
	    bool this_arg_ok = true;
	    double min = (i<arg_min_.size() ? arg_min_[i] : unset);
	    if(min != unset && args[i].dbl() < min) this_arg_ok = false;
	    double max = (i<arg_max_.size() ? arg_max_[i] : unset);
	    if(max != unset && args[i].dbl() > max) this_arg_ok = false;
	    if(!this_arg_ok)
	    {
		result = false;
		warning::print("Function argument #" & var(i+1) &
			       var("=") & args[i] & var(" is out of range [") &
			       var(min) & var(";") & var(max) & var("]"),
			       "function::make_arg_check_(...)");
	    }
	}
	return result;
    }

    function &function::check_args(bool flag)
    {
	check_args_ = flag;
	return *this;
    }

    void function::default_check_args(bool flag)
    {
	default_check_args_ = flag;
    }
	 

    function &function::arg_min(int arg_no, double value)
    {
	if(arg_no<1 || !uses_arg(arg_no))
	{
	    warning::print(var("Argument ") & var(arg_no) & var(" is not used"),
			   "function::arg_min(int arg_no, double value)");
	    return *this;
	}
	--arg_no;
	if((unsigned int)arg_no >= arg_min_.size()) arg_min_.resize(arg_no+1,unset);
	arg_min_[arg_no] = value;
	return *this;
    }

    function &function::arg_max(int arg_no, double value)
    {
	if(arg_no<1 || !uses_arg(arg_no))
	{
	    warning::print(var("Argument ") & var(arg_no) & var(" is not used"),
			   "function::arg_max(int arg_no, double value)");
	    return *this;
	}
	--arg_no;
	if((unsigned int)arg_no >= arg_max_.size()) arg_max_.resize(arg_no+1,unset);
	arg_max_[arg_no] = value;
	return *this;
    }

    function &function::arg_range(int arg_no, double min, double max)
    {
	arg_min(arg_no, min);
	arg_max(arg_no, max);
	return *this;
    }

    double function::arg_min(int arg_no)
    {
	--arg_no;
	if(arg_no<0 || (int)arg_min_.size() <= arg_no) return unset;
	return arg_min_[arg_no];
    }

    double function::arg_max(int arg_no)
    {
	--arg_no;
	if(arg_no<0 || (int)arg_max_.size() <= arg_no) return unset;
	return arg_max_[arg_no];
    }

    bool function::uses_arg(int argno) const
    {
	if(base_.empty())
	{
	    warning::print("Uninitialized function","function::uses_arg(int)");
	    return 0;
	}
	for(unsigned int i = 0; i<base_.size(); ++i)
	{
	    if(base_[i]->uses_arg(argno)) return true;
	}
	return false;
    }

    bool function::uses_par(int parno) const
    {
	if(base_.empty())
	{
	    warning::print("Uninitialized function","function::uses_par(int)");
	    return 0;
	}
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    if(base_[i]->uses_par(parno)) return true;
	}
	return false;
    }


    //---------------------- print the function --------------------------------
    var function::sprint() const
    {
	if(base_.empty())
	{
	    warning::print("Uninitialized function","function::sprint()");
	    return "";
	}
	if(base_.size() == 1) return base_[0]->sprint(parameters_,print_param_value_);
	var result = "[";
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    if(i>0) result &= ",";
	    result &= base_[i]->sprint(parameters_,print_param_value_);
	}
	result &= "]";
	return result;
    }

    var function::sprint_latex(const var &x, const var &y, const var &z) const
    {
	if(base_.empty())
	{
	    warning::print("Uninitialized function","function::sprint_latex(...)");
	    return "";
	}
	if(base_.size() == 1)
	    return
		var(" $ ") &
		base_[0]->sprint_latex(parameters_,print_param_value_,x,y,z) &
		var(" $ ");

	var result = " $\\left[";
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    if(i>0) result &= ",";
	    result &= base_[i]->sprint_latex(parameters_,print_param_value_,x,y,z);
	}
	result &= "\\right]$ ";
	return result;
    }

    void function::print(std::ostream &out) const { out<<sprint(); }
    

    function function::operator[] (unsigned int comp) const
    {
	unsigned int N = 0;
	for(unsigned int a=0; a<base_.size(); ++a)
	{
	    N += base_[a]->n_out();
	    if(N>comp)
	    {
		if(base_[a]->n_out() == 1)
		{
		    function result(*base_[a]);
		    result.parameters_ = parameters_;
		    result.print_param_value_ = print_param_value_;
		    //result.nargs(nargs_);
		    //result.npars(npars_);
		    result.default_args_ = default_args_;
		    result.parnames_ = parnames_;
		    return result;
		}
		else
		{
		    int index = base_[a]->n_out()-(N-comp);
		    if(index<0 || base_[a]->n_out()<=index)
		    {
			warning::print("This should not happen",var("function::operator[](") & var(comp) & var(")"));
			return 0.;
		    }
		    function result(new function_core::component(*base_[a],index));
		    result.parameters_ = parameters_;
		    result.print_param_value_ = print_param_value_;
		    //result.nargs(nargs_);
		    //result.npars(npars_);
		    result.default_args_ = default_args_;
		    result.parnames_ = parnames_;
		    return result;
		}
	    }
	}
	warning::print("Index out of range",var("function::operator[](") & var(comp) & var(")"));
	return 0.0;
    }

    // the parenthesis operator, called with functions as arguments,
    // creates another function, with substituted arguments
    function function::operator() (const function &a1) const
    {
	if(a1.base_.empty()) return unset;
	function result(unset);

	result.print_param_value_ = print_param_value_;
	result.base_.resize(base_.size());
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    result.base_[i] = new function_core::argument_subst(base_[i], a1.base_);
	}
	result.init_();
	if(parameters_.size() > result.parameters_.size()) result.parameters_.resize(parameters_.size());
	for(unsigned int i=0; i<parameters_.size(); ++i) result.parameters_[i] = parameters_[i];
	for(unsigned int i=result.parameters_.size(); i<a1.parameters_.size(); ++i)
	    result.parameters_.push_back(a1.parameters_[i]);

	//result.nargs(nargs_);
	//result.npars(npars_);

	result.default_args_ = default_args_;

	return result;
    }

    function function::operator() (const function &a1,const function &a2) const
    {
	if(a1.base_.empty() || a2.base_.empty() ) return unset;
	function result(unset);

	vector<function_core::base*> args;
	// since the constructor of argument_subst will anyway clone these elements,
	// no need to clone them now, just store a pointer;
	for(unsigned int i=0; i<a1.base_.size(); ++i) args.push_back(a1.base_[i]);
	for(unsigned int i=0; i<a2.base_.size(); ++i) args.push_back(a2.base_[i]);

	result.print_param_value_ = print_param_value_;
	result.base_.resize(base_.size());
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    result.base_[i] = new function_core::argument_subst(base_[i], args);
	}
	result.init_();
	if(parameters_.size() > result.parameters_.size()) result.parameters_.resize(parameters_.size());
	for(unsigned int i=0; i<parameters_.size(); ++i) result.parameters_[i] = parameters_[i];
	if(a1.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a1.parameters_.size(); ++i)
		result.parameters_.push_back(a1.parameters_[i]);
	}
	if(a2.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a2.parameters_.size(); ++i)
		result.parameters_.push_back(a2.parameters_[i]);
	}

	//result.nargs(nargs_);
	//result.npars(npars_);

	result.default_args_ = default_args_;

	return result;
    }
    function function::operator() (const function &a1,const function &a2,const function &a3) const
    {
	if(a1.base_.empty() || a2.base_.empty() || a3.base_.empty() ) return unset;
	function result(unset);

	vector<function_core::base*> args;
	// since the constructor of argument_subst will anyway clone these elements,
	// no need to clone them now, just store a pointer;
	for(unsigned int i=0; i<a1.base_.size(); ++i) args.push_back(a1.base_[i]);
	for(unsigned int i=0; i<a2.base_.size(); ++i) args.push_back(a2.base_[i]);
	for(unsigned int i=0; i<a3.base_.size(); ++i) args.push_back(a3.base_[i]);

	result.print_param_value_ = print_param_value_;
	result.base_.resize(base_.size());
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    result.base_[i] = new function_core::argument_subst(base_[i], args);
	}
	result.init_();
	if(parameters_.size() > result.parameters_.size()) result.parameters_.resize(parameters_.size());
	for(unsigned int i=0; i<parameters_.size(); ++i) result.parameters_[i] = parameters_[i];
	if(a1.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a1.parameters_.size(); ++i)
		result.parameters_.push_back(a1.parameters_[i]);
	}
	if(a2.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a2.parameters_.size(); ++i)
		result.parameters_.push_back(a2.parameters_[i]);
	}
	if(a3.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a3.parameters_.size(); ++i)
		result.parameters_.push_back(a3.parameters_[i]);
	}

	//result.nargs(nargs_);
	//result.npars(npars_);

	result.default_args_ = default_args_;

	return result;
    }
    function function::operator() (const function &a1,const function &a2,const function &a3,const function &a4) const
    {
	if(a1.base_.empty() || a2.base_.empty() || a3.base_.empty() || a4.base_.empty() ) return unset;
	function result(unset);

	vector<function_core::base*> args;
	// since the constructor of argument_subst will anyway clone these elements,
	// no need to clone them now, just store a pointer;
	for(unsigned int i=0; i<a1.base_.size(); ++i) args.push_back(a1.base_[i]);
	for(unsigned int i=0; i<a2.base_.size(); ++i) args.push_back(a2.base_[i]);
	for(unsigned int i=0; i<a3.base_.size(); ++i) args.push_back(a3.base_[i]);
	for(unsigned int i=0; i<a4.base_.size(); ++i) args.push_back(a4.base_[i]);

	result.print_param_value_ = print_param_value_;
	result.base_.resize(base_.size());
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    result.base_[i] = new function_core::argument_subst(base_[i], args);
	}
	result.init_();
	if(parameters_.size() > result.parameters_.size()) result.parameters_.resize(parameters_.size());
	for(unsigned int i=0; i<parameters_.size(); ++i) result.parameters_[i] = parameters_[i];
	if(a1.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a1.parameters_.size(); ++i)
		result.parameters_.push_back(a1.parameters_[i]);
	}
	if(a2.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a2.parameters_.size(); ++i)
		result.parameters_.push_back(a2.parameters_[i]);
	}
	if(a3.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a3.parameters_.size(); ++i)
		result.parameters_.push_back(a3.parameters_[i]);
	}
	if(a4.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a4.parameters_.size(); ++i)
		result.parameters_.push_back(a4.parameters_[i]);
	}

	//result.nargs(nargs_);
	//result.npars(npars_);

	result.default_args_ = default_args_;

	return result;
    }
    function function::operator() (const function &a1,const function &a2,const function &a3,const function &a4,const function &a5) const
    {
	if(a1.base_.empty() ||
	   a2.base_.empty() ||
	   a3.base_.empty() ||
	   a4.base_.empty() ||
	   a5.base_.empty() ) return unset;
	function result(unset);

	vector<function_core::base*> args;
	// since the constructor of argument_subst will anyway clone these elements,
	// no need to clone them now, just store a pointer;
	for(unsigned int i=0; i<a1.base_.size(); ++i) args.push_back(a1.base_[i]);
	for(unsigned int i=0; i<a2.base_.size(); ++i) args.push_back(a2.base_[i]);
	for(unsigned int i=0; i<a3.base_.size(); ++i) args.push_back(a3.base_[i]);
	for(unsigned int i=0; i<a4.base_.size(); ++i) args.push_back(a4.base_[i]);
	for(unsigned int i=0; i<a5.base_.size(); ++i) args.push_back(a5.base_[i]);

	result.print_param_value_ = print_param_value_;
	result.base_.resize(base_.size());
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    result.base_[i] = new function_core::argument_subst(base_[i], args);
	}
	result.init_();
	if(parameters_.size() > result.parameters_.size()) result.parameters_.resize(parameters_.size());
	for(unsigned int i=0; i<parameters_.size(); ++i) result.parameters_[i] = parameters_[i];
	if(a1.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a1.parameters_.size(); ++i)
		result.parameters_.push_back(a1.parameters_[i]);
	}
	if(a2.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a2.parameters_.size(); ++i)
		result.parameters_.push_back(a2.parameters_[i]);
	}
	if(a3.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a3.parameters_.size(); ++i)
		result.parameters_.push_back(a3.parameters_[i]);
	}
	if(a4.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a4.parameters_.size(); ++i)
		result.parameters_.push_back(a4.parameters_[i]);
	}
	if(a5.parameters_.size() > result.parameters_.size())
	{
	    for(unsigned int i=result.parameters_.size(); i<a5.parameters_.size(); ++i)
		result.parameters_.push_back(a5.parameters_[i]);
	}

	//result.nargs(nargs_);
	//result.npars(npars_);

	result.default_args_ = default_args_;

	return result;
    }

    // ------------------  function evaluation -----------------------------------------
    var function::eval(const vector<var> &args) const
    {
	if(check_args_) make_arg_check_(args);
	
	if(base_.empty())
	{
	    warning::print("Uninitialized function is being evaluated","function::eval(...)");
	    return 0;
	}
	int dummy = 0;
	base_[0]->eval(args, default_args_, parameters_, result_, &dummy);
	return result_[0];
    }

    double function::eval_dbl(const std::vector<blop::var> &args, const std::vector<blop::var> &pars) const
    {
	if(base_.empty())
	{
	    warning::print("Uninitialized function is being evaluated","function::eval_dbl(vector,vector)");
	    return 0;
	}
	int dummy = 0;
	base_[0]->eval_dbl(args,default_args_,pars,result_,&dummy);
	return result_[0].dbl();
    }

    double function::eval_dbl(const std::vector<blop::var> &args) const
    {
	if(base_.empty())
	{
	    warning::print("Uninitialized function is being evaluated","function::eval_dbl(...)");
	    return 0;
	}
	int dummy = 0;
	base_[0]->eval_dbl(args,default_args_,parameters_,result_,&dummy);
	return result_[0].dbl();
    }

    void function::meval(const std::vector<blop::var> &args, std::vector<blop::var> &result) const
    {
	if(check_args_) make_arg_check_(args);

	result.resize(components_);
	if(base_.empty())
	{
	    warning::print("Uninitialized function is being evaluated","function::meval(...)");
	    return;
	}
	int runind = 0;
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    base_[i]->eval(args, default_args_, parameters_, result, &runind);
	}
    }

    void function::meval_dbl(const std::vector<blop::var> &args, std::vector<blop::var> &result) const
    {
	result.resize(components_);
	if(base_.empty())
	{
	    warning::print("Uninitialized function is being evaluated","function::meval_dbl(...)");
	    return;
	}
	int runind = 0;
	for(unsigned int i=0; i<base_.size(); ++i)
	    base_[i]->eval_dbl(args, default_args_, parameters_, result, &runind);
    }

    void function::meval_dbl(const std::vector<blop::var> &args,
			     const std::vector<blop::var> &pars,
			     std::vector<blop::var> &result) const
    {
	result.resize(components_);
	if(base_.empty())
	{
	    warning::print("Uninitialized function is being evaluated","function::meval_dbl(...)");
	    return;
	}
	int runind = 0;
	for(unsigned int i=0; i<base_.size(); ++i)
	    base_[i]->eval_dbl(args, default_args_, pars, result, &runind);
    }

    var function::operator()() const
    {
	vector<var> a;
	return eval(a);
    }
    var function::operator()(const var &x) const
    {
	vector<var> a;
	a.push_back(x);
	return eval(a);
    }
    var function::operator()(const var &x,const var &y) const
    {
	vector<var> a;
	a.push_back(x);
	a.push_back(y);
	return eval(a);
    }
    var function::operator()(const var &x,const var &y,const var &z) const
    {
	vector<var> arg;
	arg.push_back(x);
	arg.push_back(y);
	arg.push_back(z);
	return eval(arg);
    }
    var function::operator()(const var &x,const var &y,const var &z,const var &w) const
    {
	vector<var> arg;
	arg.push_back(x);
	arg.push_back(y);
	arg.push_back(z);
	arg.push_back(w);
	return eval(arg);
    }
    var function::operator()(const var &x,const var &y,const var &z,const var &w,const var &v) const
    {
	vector<var> arg;
	arg.push_back(x);
	arg.push_back(y);
	arg.push_back(z);
	arg.push_back(w);
	arg.push_back(v);
	return eval(arg);
    }
    var function::operator()(const var &x,const var &y,const var &z,const var &w,const var &v,const var &a) const
    {
	vector<var> arg;
	arg.push_back(x);
	arg.push_back(y);
	arg.push_back(z);
	arg.push_back(w);
	arg.push_back(v);
	arg.push_back(a);
	return eval(arg);
    }


    const function &function::operator= (const function &f)
    {
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    delete base_[i];
	}
	base_.resize(f.base_.size());
	for(unsigned int i=0; i<f.base_.size(); ++i)
	{
	    base_[i] = f.base_[i]->clone();
	}
	parameters_ = f.parameters_;
	init_();

	if(!f.derivatives_.empty())
	{
	    cerr<<"function::operator= does not yet copy derivatives, request it strongly at the author"<<endl;
	    exit(1);
	}

	//nargs(nargs_);
	//npars(npars_);

	default_args_ = f.default_args_;
	arg_min_ = f.arg_min_;
	arg_max_ = f.arg_max_;

        return *this;
    }

    const function &function::operator= (const var &v)
    {
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    delete base_[i];
	}
	base_.clear();
	base_.push_back(new constant(v));
	parameters_.clear();
	init_();

	typedef std::map<int, function_core::base*>::iterator iter2;
	for(deriv_map::iterator i1 = derivatives_.begin(); i1 != derivatives_.end(); ++i1)
	{
	    for(unsigned int i2=0; i2<(*i1).second.size(); ++i2) delete (*i1).second[i2];
	}

	//nargs(nargs_);
	//npars(npars_);
        return *this;
    }
    const function &function::operator= (const string &v)
    {
	operator=(var(v));
	//nargs(nargs_);
	//npars(npars_);
	return *this;
    }
    const function &function::operator= (const char *v)
    {
	operator=(var(v));
	//nargs(nargs_);
	//npars(npars_);
	return *this;
    }

    const function &function::operator= (double v)
    {
	if(v == unset)
	{
	    for(unsigned int i=0; i<base_.size();++i) delete base_[i];
	    base_.clear();
	    parameters_.clear();
	    components_ = 0;
	}
	else operator=(var(v));
	//nargs(nargs_);
	//npars(npars_);
	return *this;
    }


    // -------------  operators, functions  ------------------------

    template <class T>
    void function::init_binary_(const function &left, const function &right)
    {
	print_param_value_ = (left.print_param_value_ || right.print_param_value_);
	unsigned int n = ::min(left.base_.size(), right.base_.size());
	base_.clear();
	for(unsigned int i=0; i<n; ++i)
	{
	    if(left.base_[i]->n_out() != right.base_[i]->n_out())
	    {
		warning::print("Component mismatch","function::init_binary_");
		break;
	    }
	    base_.push_back(new T(left.base_[i], right.base_[i]));
	}
	init_();
	parameters_.resize(::max(left.parameters_.size(), right.parameters_.size()));
	unsigned int i=0; 
	for(; i<left .parameters_.size(); ++i) parameters_[i] = left.parameters_[i];
	for(; i<right.parameters_.size(); ++i) parameters_[i] = right.parameters_[i];
	//nargs(max(left.nargs_,right.nargs_));
	//npars(max(left.npars_,right.npars_));
    }

    template <class T>
    void function::init_unary_(const function &operand)
    {
	print_param_value_ = operand.print_param_value_;
	unsigned int n = operand.base_.size();
	base_.resize(n);
	for(unsigned int i=0; i<n; ++i) base_[i] = new T(operand.base_[i]);
	init_();
	parameters_ = operand.parameters_;
    }

    function operator& (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_ & c2->value_);
	function result;
	result.init_binary_<function_core::concatenator>(left,right);
	return result;
    }

    function operator && (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_.dbl() != 0.0 && c2->value_.dbl() != 0.0);
	if( (c1 && c1->value_==0.0) || (c2 && c2->value_==0.0) ) return constant(0.0);
	function result;
	result.init_binary_<function_core::logical_and>(left,right);
	return result;
    }

    function operator || (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_.dbl() != 0.0 || c2->value_.dbl() != 0.0);
	function result;
	result.init_binary_<function_core::logical_or>(left,right);
	return result;
    }

    function sqrt(const function &o)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(o.base_[0])) return constant(::sqrt(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Sqrt>(o);
	//result.nargs(o.nargs_);
	//result.npars(o.npars_);
	return result;
    }

    function max(const function &y, const function &x)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(y.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(x.base_[0]);
	if(c1 && c2) return constant(::max(c1->value_.dbl(),c2->value_.dbl()));
	function result;
	result.init_binary_<function_core::Max>(y,x);
	return result;
    }

    function min(const function &y, const function &x)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(y.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(x.base_[0]);
	if(c1 && c2) return constant(::min(c1->value_.dbl(),c2->value_.dbl()));
	function result;
	result.init_binary_<function_core::Min>(y,x);
	return result;
    }

    function maximize(const function &func,
		      const function &from,
		      const function &to,
		      const function &step)
    {
	// For the time being, handle just the simple case when func has 1 component....
	// (lacking time)
	if(func.components() == 1)
	{
	    if(function_core::constant *c = dynamic_cast<function_core::constant *>(func.base_[0])) return constant(c->value_.dbl());
	}

	return new function_core::max_in_interval(func.base_,
						  from.base_,
						  to.base_,
						  step.base_);
    }
    function minimize(const function &func,
		      const function &from,
		      const function &to,
		      const function &step)
    {
	// For the time being, handle just the simple case when func has 1 component....
	// (lacking time)
	if(func.components() == 1)
	{
	    if(function_core::constant *c = dynamic_cast<function_core::constant *>(func.base_[0])) return constant(c->value_.dbl());
	}

	return new function_core::min_in_interval(func.base_,
						  from.base_,
						  to.base_,
						  step.base_);
    }
    function integrate(const function &func,
		       const function &from,
		       const function &to,
		       const function &step)
    {
	return new function_core::integral(func.base_,
					   from.base_,
					   to.base_,
					   step.base_);
    }

    function operator==(const function  &left, const var &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	if(c1) return constant(c1->value_.dbl() == right.dbl());
	function result;
	result.init_binary_<function_core::EqualDbl>(left,right);
	return result;
    }
    function operator==(const var  &left, const function &right)
    {
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c2) return constant(left.dbl() == c2->value_.dbl());
	function result;
	result.init_binary_<function_core::EqualDbl>(left,right);
	return result;
    }

    function operator==(const function  &left, double right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	if(c1) return constant(c1->value_.dbl() == right);
	function result;
	result.init_binary_<function_core::EqualDbl>(left,right);
	return result;
    }
    function operator==(double left, const function &right)
    {
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c2) return constant(left == c2->value_.dbl());
	function result;
	result.init_binary_<function_core::EqualDbl>(left,right);
	return result;
    }

    function operator==(const function  &left, int right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	if(c1) return constant(c1->value_.dbl() == right);
	function result;
	result.init_binary_<function_core::EqualDbl>(left,right);
	return result;
    }
    function operator==(int left, const function &right)
    {
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c2) return constant(left == c2->value_.dbl());
	function result;
	result.init_binary_<function_core::EqualDbl>(left,right);
	return result;
    }

    function operator==(const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_.dbl() == c2->value_.dbl());
	function result;
	result.init_binary_<function_core::EqualDbl>(left, right);
	return result;
    }

    function operator==(const function &left, const std::string &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	if(c1) return constant(c1->value_.str() == right);
	function c = right;
	function result;
	result.init_binary_<function_core::EqualStr>(left,c);
	return result;
    }
    function operator==(const std::string &left, const function &right)
    {
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c2) return constant(left == c2->value_.str());
	function c = left;
	function result;
	result.init_binary_<function_core::EqualStr>(c,right);
	return result;
    }
    function operator==(const function &left, const char *right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	if(c1) return constant(c1->value_.str() == right);
	function c = right;
	function result;
	result.init_binary_<function_core::EqualStr>(left,c);
	return result;
    }
    function operator==(const char *left, const function &right)
    {
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c2) return constant(left == c2->value_.str());
	function c = left;
	function result;
	result.init_binary_<function_core::EqualStr>(c,right);
	return result;
    }

    function operator!=(const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_.dbl() != c2->value_.dbl());
	function result;
	result.init_binary_<function_core::NotEqualDbl>(left,right);
	return result;
    }

    function operator< (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_.dbl() < c2->value_.dbl());
	function result;
	result.init_binary_<function_core::LessThan>(left,right);
	return result;
    }

    function operator<= (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_.dbl() <= c2->value_.dbl());
	function result;
	result.init_binary_<function_core::LessEqual>(left,right);
	return result;
    }

    function operator> (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_.dbl() > c2->value_.dbl());
	function result;
	result.init_binary_<function_core::GreaterThan>(left,right);
	return result;
    }

    function operator>= (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_.dbl() >= c2->value_.dbl());
	function result;
	result.init_binary_<function_core::GreaterEqual>(left,right);
	return result;
    }

    function operator+ (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value().dbl()+c2->value().dbl());
	if(c1 && c1->value_ == 0.0) return right;
	if(c2 && c2->value_ == 0.0) return left;
	
	function result;
	result.init_binary_<function_core::Add>(left,right);
	return result;
    }

    function operator- (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value().dbl()-c2->value().dbl());
	if(c2 && c2->value_ == 0.0) return left;

	function result;
	result.init_binary_<function_core::Sub>(left,right);
	return result;
    }

    function operator% (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value().integer()%c2->value().integer());

	function result;
	result.init_binary_<function_core::Mod>(left,right);
	return result;
    }

    function operator* (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value().dbl()*c2->value().dbl());
	if( (c1 && c1->value_ == 0.0) || (c2 && c2->value_ == 0.0) ) return 0.0;
	if(c1 && c1->value_ == 1.0) return right;
	if(c2 && c2->value_ == 1.0) return left;
	function result;
	result.init_binary_<function_core::Mul>(left,right);
	return result;
    }

    function operator/ (const function &left, const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(c1->value_.dbl()/c2->value_.dbl());
	if(c1 && c1->value_ == 0.0) return 0.0;

	function result;
	result.init_binary_<function_core::Div>(left,right);
	return result;
    }

    function operator- (const function &o)
    {
	function_core::constant *c = dynamic_cast<constant *>(o.base_[0]);
	if(c) return constant(-c->value_.dbl());
	function result;
	result.init_unary_<function_core::Neg>(o);
	//result.nargs(o.nargs_);
	//result.npars(o.npars_);
	return result;
    }

    function exp(const function &o)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(o.base_[0])) return constant(::sqrt(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Exp>(o);
	//result.nargs(o.nargs_);
	//result.npars(o.npars_);
	return result;
    }

    function log(const function &o)
    {
	function result;
	result.init_unary_<function_core::Log>(o);
	//result.nargs(o.nargs_);
	//result.npars(o.npars_);
	return result;
    }

    function log10(const function &o)
    {
	function result;
	result.init_unary_<function_core::Log10>(o);
	//result.nargs(o.nargs_);
	//result.npars(o.npars_);
	return result;
    }

    function atan2(const function &y, const function &x)
    {
	function result;
	result.init_binary_<function_core::Atan2>(y,x);
	return result;
    }

    function pow(const function &left,const function &right)
    {
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_[0]);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_[0]);
	if(c1 && c2) return constant(::pow(c1->value_.dbl(), c2->value_.dbl()));
	if(c2 && c2->value_ == 0.0) return constant(1.0);
	if(c2 && c2->value_ == 1.0) return left;
	function result;
	result.init_binary_<function_core::Pow>(left,right);
	return result;
    }
    function pow(const function &a,int exponent)
    {
	if(exponent == 0) return constant(1.0);
	if(exponent == 1) return a;
	// this function should be very similar to function::init_unary_
	function result;
	result.print_param_value_ = a.print_param_value_;
	unsigned int n = a.base_.size();
	result.base_.resize(n);
	for(unsigned int i=0; i<n; ++i) result.base_[i] = new Ipow(a.base_[i],exponent);
	result.init_();
	result.parameters_ = a.parameters_;
	//result.nargs(a.nargs_);
	//result.npars(a.npars_);
	return result;
    }
    function pow(const function &a,double exponent)
    {
	if(exponent == 0.0) return constant(1.0);
	if(exponent == 1.0) return a;
	function_core::constant c(exponent);
	function result;
	result.init_binary_<function_core::Pow>(a,c);
	return result;
    }
    function pow(const function &a, const var & exponent)
    {
	if(exponent.dbl() == 0.0) return constant(1.0);
	if(exponent.dbl() == 1.0) return a;
	function_core::constant c(exponent);
	function result;
	result.init_binary_<function_core::Pow>(a,c);
	return result;
    }

    function sin(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::sin(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Sin>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function cos(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::cos(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Cos>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function tan(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::tan(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Tan>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function atan(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::atan(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Atan>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function cot(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(blop::function_core::cot(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Cot>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function acot(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(blop::function_core::acot(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Acot>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function sinh(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::sinh(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Sinh>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function cosh(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::cosh(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Cosh>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function tanh(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::tanh(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Tanh>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function atanh(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::atanh(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Atanh>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function asinh(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::asinh(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Asinh>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function acosh(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::acosh(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Acosh>(f);
	//result.nargs(f.nargs_);
	//result.npars(f.npars_);
	return result;
    }

    function floor(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::floor(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Floor>(f);
	return result;
    }

    function ceil(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::ceil(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Ceil>(f);
	return result;
    }

    function erf(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::erf(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Erf>(f);
	return result;
    }	 

    function sign(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0]))
	{
	    if(c->value_.dbl()<0) return constant(-1);
	    if(c->value_.dbl()>0) return constant(1);
	    return constant(0.0);
	}
	function result;
	result.init_unary_<function_core::Sign>(f);
	return result;
    }

    function abs(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::fabs(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Abs>(f);
	return result;
    }
    function fabs(const function &f)
    {
	if(function_core::constant *c = dynamic_cast<constant *>(f.base_[0])) return constant(::fabs(c->value_.dbl()));
	function result;
	result.init_unary_<function_core::Abs>(f);
	return result;
    }

#ifdef HAVE_GSL

    function bessel_J(int nu, const function &a)
    {
	function result;
	bessel::setup_bessel_core(nu, a, bessel::J, result);
/*
	result.print_param_value_ = a.print_param_value_;
	unsigned int n = a.base_.size();
	result.base_.resize(n);
	for(unsigned int i=0; i<n; ++i) result.base_[i] = new bessel(a.base_[i],bessel::J,nu);
	result.init_();
	result.parameters_ = a.parameters_;
*/
	return result;
    }
    function bessel_Y(int nu, const function &a)
    {
	function result;
	bessel::setup_bessel_core(nu, a, bessel::Y, result);
	return result;
    }
    function bessel_I(int nu, const function &a)
    {
	function result;
	bessel::setup_bessel_core(nu, a, bessel::I, result);
	return result;
    }
    function bessel_K(int nu, const function &a)
    {
	function result;
	bessel::setup_bessel_core(nu, a, bessel::K, result);
	return result;
    }
    function bessel_j(int nu, const function &a)
    {
	function result;
	bessel::setup_bessel_core(nu, a, bessel::j, result);
	return result;
    }
    function bessel_y(int nu, const function &a)
    {
	function result;
	bessel::setup_bessel_core(nu, a, bessel::y, result);
	return result;
    }
    function bessel_i_scaled(int nu, const function &a)
    {
	function result;
	bessel::setup_bessel_core(nu, a, bessel::i_scaled, result);
	return result;
    }
    function bessel_k_scaled(int nu, const function &a)
    {
	function result;
	bessel::setup_bessel_core(nu, a, bessel::k_scaled, result);
	return result;
    }




    double bessel_J(int n, double x)
    {
	if(n==0) return gsl_sf_bessel_J0(x);
	if(n==1) return gsl_sf_bessel_J1(x);
	return gsl_sf_bessel_Jn(n,x);
    }
    double bessel_Y(int n, double x)
    {
	if(n==0) return gsl_sf_bessel_Y0(x);
	if(n==1) return gsl_sf_bessel_Y1(x);
	return gsl_sf_bessel_Yn(n,x);
    }
    double bessel_I(int n, double x)
    {
	if(n==0) return gsl_sf_bessel_I0(x);
	if(n==1) return gsl_sf_bessel_I1(x);
	return gsl_sf_bessel_In(n,x);
    }
    double bessel_K(int n, double x)
    {
	if(n==0) return gsl_sf_bessel_K0(x);
	if(n==1) return gsl_sf_bessel_K1(x);
	return gsl_sf_bessel_Kn(n,x);
    }
    double bessel_j(int n, double x)
    {
	if(n==0) return gsl_sf_bessel_j0(x);
	if(n==1) return gsl_sf_bessel_j1(x);
	if(n==2) return gsl_sf_bessel_j2(x);
	return gsl_sf_bessel_jl(n,x);
    }
    double bessel_y(int n, double x)
    {
	if(n==0) return gsl_sf_bessel_y0(x);
	if(n==1) return gsl_sf_bessel_y1(x);
	if(n==2) return gsl_sf_bessel_y2(x);
	return gsl_sf_bessel_yl(n,x);
    }
    double bessel_i_scaled(int n, double x)
    {
	if(n==0) return gsl_sf_bessel_i0_scaled(x);
	if(n==1) return gsl_sf_bessel_i1_scaled(x);
	if(n==2) return gsl_sf_bessel_i2_scaled(x);
	return gsl_sf_bessel_il_scaled(n,x);
    }
    double bessel_k_scaled(int n, double x)
    {
	if(n==0) return gsl_sf_bessel_k0_scaled(x);
	if(n==1) return gsl_sf_bessel_k1_scaled(x);
	if(n==2) return gsl_sf_bessel_k2_scaled(x);
	return gsl_sf_bessel_kl_scaled(n,x);
    }

    double   bessel_J_zero(int n, int s)
    {
	if(n == 0) return gsl_sf_bessel_zero_J0(s);
	if(n == 1) return gsl_sf_bessel_zero_J1(s);
	return gsl_sf_bessel_zero_Jnu(n,s);
    }

    double bessel_J_derivative(int n, double x)
    {
	// http://en.wikipedia.org/wiki/Bessel_function#Derivatives_of_J.2CY.2CI.2CH.2CK
	if(n>=1) return bessel_J(n-1,x)-n/x*bessel_J(n,x);
	return -bessel_J(n+1,x); //+n/x*bessel_J(n,x);
    }

    double   bessel_Jprime_zero(int n, int s)
    {
	// Starting point: for n>=1, the s-th zero of the derivative
	// is bracketed by the s-1 and s-th zero of the function.
	// For n==0, the s-th zero is bracked by the s-th and (s+1)-th
	// zero of the function.
	// make an interval-division root finding, then interpolate
	// linearly in the bracketing interval.
	double x1=0,x2=0;
	if(n==0)
	{
	    x1 = bessel_J_zero(0,s);
	    x2 = bessel_J_zero(0,s+1);
	}
	else
	{
	    x2 = bessel_J_zero(n,s);
	    x1  = (s==1?0.1*x2:bessel_J_zero(n,s-1));
	}

	double y1 = bessel_J_derivative(n,x1);
	double y2 = bessel_J_derivative(n,x2);
	
	if(y1*y2>0)
	{
	    cerr<<"This should not happen in bessel_Jprime_zero("<<n<<","<<s<<")"<<endl;
	    return 0;
	}

	// determine the position up to 2^10 precision
	for(int i=0; i<10; ++i)
	{
	    // take midpoint of interval
	    const double x = 0.5*(x1+x2);
	    const double y = bessel_J_derivative(n,x);

	    if(y*y2<0)
	    {
		x1 = x;
		y1 = y;
	    }
	    else
	    {
		x2 = x;
		y2 = y;
	    }
	}

	return (x1*y2-x2*y1)/(y2-y1);
    }

#endif

    function charfunc(const function &left, const function &right, bool low_in, bool high_in)
    {
	// this function should be very similar to function::init_binary_
	function result;
	result.print_param_value_ = (left.print_param_value_ || right.print_param_value_);
	unsigned int n = ::min(left.base_.size(), right.base_.size());
	result.base_.clear();
	for(unsigned int i=0; i<n; ++i)
	{
	    if(left.base_[i]->n_out() != right.base_[i]->n_out())
	    {
		warning::print("Component mismatch","charfunc");
		break;
	    }
	    result.base_.push_back(new function_core::char_func(left.base_[i], right.base_[i], low_in, high_in));
	}
	result.init_();
	result.parameters_.resize(::max(left.parameters_.size(), right.parameters_.size()));
	unsigned int i=0; 
	for(; i<left .parameters_.size(); ++i) result.parameters_[i] = left .parameters_[i];
	for(; i<right.parameters_.size(); ++i) result.parameters_[i] = right.parameters_[i];
	//result.nargs(::max(left.nargs_,right.nargs_));
	//result.npars(::max(left.npars_,right.npars_));
	return result;
    }

    // There is some memory leakage here: the previously created
    // function_core:: base pointers (allocated in
    // function::global_initializer::gobal_initializer() )
    // are not freed, and they are lost
    function _0(new function_core::extra_param);
    function _1(new function_core::arg(1));
    function _2(new function_core::arg(2));
    function _3(new function_core::arg(3));
    function _4(new function_core::arg(4));
    function _5(new function_core::arg(5));
    function _6(new function_core::arg(6));
    function _7(new function_core::arg(7));
    function _8(new function_core::arg(8));
    function _9(new function_core::arg(9));
    function _10(new function_core::arg(10));
    function _11(new function_core::arg(11));
    function _12(new function_core::arg(12));
    function _13(new function_core::arg(13));
    function _14(new function_core::arg(14));
    function _15(new function_core::arg(15));
    function _16(new function_core::arg(16));
    function _17(new function_core::arg(17));
    function _18(new function_core::arg(18));
    function _19(new function_core::arg(19));
    function _20(new function_core::arg(20));
    function _21(new function_core::arg(21));
    function _22(new function_core::arg(22));
    function _23(new function_core::arg(23));
    function _24(new function_core::arg(24));
    function _25(new function_core::arg(25));
    function _26(new function_core::arg(26));
    function _27(new function_core::arg(27));
    function _28(new function_core::arg(28));
    function _29(new function_core::arg(29));
    function _30(new function_core::arg(30));
    function _N(new function_core::actual_nargs);

    function ARG(int i) {return function_core::arg(i);}
    function PAR(int i) {return function_core::funcparameter(i);}

    bool function::initialized() const
    {
	return !base_.empty();
    }

    const function &function::operator+= (const function &f)
    {
	if(base_.empty())
	{
	    warning::print("uninitialized function","function::operator+=");
	    return *this;
	}

	const unsigned int n = ::min(base_.size(), f.base_.size());
	for(unsigned int i=0; i<n; ++i)
	{
	    function_core::constant *c2 = dynamic_cast<constant *>(f.base_[i]);
	    if(c2 && c2->value_ == 0.0)
	    {
		// add 0 ==> do nothing
		continue;
	    }

	    function_core::constant *c1 = dynamic_cast<constant *>(base_[i]);
	    if(c1 && c1->value_ == 0.0)
	    {
		delete base_[i];
		base_[i] = f.base_[i]->clone();
		continue;
	    }

	    function_core::Add *add = new function_core::Add;
	    add->left(base_[i]);
	    add->right(f.base_[i]->clone());
	    base_[i] = add;
	}
	//nargs(nargs_);
	//npars(npars_);
	return *this;
    }

    const function &function::operator-= (const function &f)
    {
	if(base_.empty())
	{
	    warning::print("uninitialized function","function::operator-=");
	    return *this;
	}

	const unsigned int n = ::min(base_.size(), f.base_.size());
	for(unsigned int i=0; i<n; ++i)
	{
	    function_core::constant *c2 = dynamic_cast<constant *>(f.base_[i]);
	    if(c2 && c2->value_ == 0.0)
	    {
		// subtract 0 ==> do nothing
		continue;
	    }

	    function_core::constant *c1 = dynamic_cast<constant *>(base_[i]);
	    if(c1 && c1->value_ == 0.0)
	    {
		delete base_[i];
		base_[i] = new function_core::Neg(f.base_[i]);
		continue;
	    }

	    function_core::Sub *sub = new function_core::Sub;
	    sub->left(base_[i]);
	    sub->right(f.base_[i]->clone());
	    base_[i] = sub;
	}
	//nargs(nargs_);
	//npars(npars_);
	return *this;
    }

    const function &function::operator*= (const function &f)
    {
	if(base_.empty())
	{
	    warning::print("uninitialized function","function::operator*=");
	    return *this;
	}

	const unsigned int n = ::min(base_.size(), f.base_.size());
	for(unsigned int i=0; i<n; ++i)
	{
	    function_core::constant *c1 = dynamic_cast<constant *>(  base_[i]);
	    function_core::constant *c2 = dynamic_cast<constant *>(f.base_[i]);

	    // if this is 0, do nothing
	    if(c1 && c1->value_ == 0.0) continue;

	    // if this is 1, clone the other
	    if(c1 && c1->value_ == 1.0)
	    {
		delete base_[i];
		base_[i] = f.base_[i]->clone();
		continue;
	    }

	    // multiply by 1 ==> do nothing
	    if(c2 && c2->value_ == 1.0) continue;

	    // multipily by 0.0
	    if(c2 && c2->value_ == 0.0)
	    {
		delete base_[i];
		base_[i] = new constant(0.0);
		continue;
	    }

	    function_core::Mul *mul = new function_core::Mul;
	    mul->left(base_[i]);
	    mul->right(f.base_[i]->clone());
	    base_[i] = mul;
	}
	//nargs(nargs_);
	//npars(npars_);
	return *this;
    }

    const function &function::operator/= (const function &f)
    {
	if(base_.empty())
	{
	    warning::print("uninitialized function","function::operator*=");
	    return *this;
	}

	const unsigned int n = ::min(base_.size(), f.base_.size());
	for(unsigned int i=0; i<n; ++i)
	{
	    function_core::constant *c1 = dynamic_cast<constant *>(  base_[i]);
	    function_core::constant *c2 = dynamic_cast<constant *>(f.base_[i]);

	    // if this is 0, do nothing
	    if(c1 && c1->value_ == 0.0) continue;

	    // divide by 1 ==> do nothing
	    if(c2 && c2->value_ == 1.0) continue;

	    function_core::Div *div = new function_core::Div;
	    div->left(base_[i]);
	    div->right(f.base_[i]->clone());
	    base_[i] = div;
	}
	//nargs(nargs_);
	//npars(npars_);
	return *this;
    }

    function random()
    {
	return new function_core::random;
    }
    function random(double to)
    {
	return new function_core::random(0,to);
    }
    function random(double from, double to)
    {
	return new function_core::random(from,to);
    }
    function random_gauss(double a, double s, int parameter_index)
    {
	function result;
	if(parameter_index>0)
	{
	    result =
		PAR(parameter_index)+
		PAR(parameter_index+1)*sqrt(2.0*fabs(log(1.0-blop::random(0.0, 1.0))))*
		cos((2.0*M_PI)*blop::random(0.0, 1.0));
	    result.param(parameter_index,1);
	    result.param(parameter_index+1,s);
	}
	else
	{
	    result =
		a+
		s*sqrt(2.0*fabs(log(1.0-blop::random(0.0, 1.0))))*
		cos((2.0*M_PI)*blop::random(0.0, 1.0));
	}
	return result;
    }
    function random_exponential(double d, int parameter_index)
    {
	function result;
	if(parameter_index>0)
	{
	    result = PAR(parameter_index)*log(1.0/(1.0-blop::random(0.0, 1.0)));
	    result.param(parameter_index,d);
	}
	else
	{
	    result = d*log(1.0/(1.0-blop::random(0.0, 1.0)));
	}
	return result;

    }
    function random_idexponential(double d, int parameter_index)
    {
	function result;
	if(parameter_index>0)
	{
	    result = PAR(parameter_index)*
		log(1.0/((1.0-blop::random(0.0, 1.0))*(1.0-blop::random(0.0, 1.0))));
	    result.param(parameter_index,d);
	}
	else
	{
	    result = d * log(1.0/((1.0-blop::random(0.0, 1.0))*(1.0-blop::random(0.0, 1.0))));
	}
	return result;
    }

    function &function::derivative(int derivi, const function &d)
    {
	if(!derivatives_[derivi].empty())
	{
	    for(unsigned int i=0; i<derivatives_[derivi].size(); ++i) delete derivatives_[derivi][i];
	    derivatives_[derivi].clear();
	}
	if(d.base_.size() != base_.size())
	{
	    warning::print("Number of components mismatch when setting derivatives",
			   "function::derivative(int,const function &)");
	    return *this;
	}
	derivatives_[derivi].resize(d.base_.size());
	for(unsigned int i=0; i<d.base_.size(); ++i)
	{
	    derivatives_[derivi][i] = d.base_[i]->clone();
	}
	return *this;
    }

    function function::derivative(int derivindex) const
    {
	if(base_.empty())
	{
	    warning::print("Uninitialized functions derivative...","function::derivative(in)");
	    return constant(0.0);
	}

	// if the user has defined this derivative, return that,
	// instead of trying to calculate it
	deriv_map::const_iterator iter = derivatives_.find(derivindex);
	if(iter != derivatives_.end())
	{
	    function result((*iter).second);
	    result.parameters_ = parameters_;
	    result.print_param_value_ = print_param_value_;
	    return result;
	}

	std::vector<function_core::base*> derivs(base_.size());
	for(unsigned int i=0; i<base_.size(); ++i)
	{
	    derivs[i] = base_[i] -> create_derivative(derivindex);
	    if(derivs[i] == 0)
	    {
		const int di = -derivindex;
		string s = "th";
		if(di == 1) s = "st";
		if(di == 2) s = "nd";
		if(di == 3) s = "rd";
		warning::print(var("Could not calculate ") & di & var(s) & " derivative",
			       "function::derivative(int)");
		derivs[i] = new constant(0.0);
	    }
	}
	bool clone_them;
	function result(derivs, clone_them=false);
	result.parameters_ = parameters_;
	result.print_param_value_ = print_param_value_;
	return result;
    }


    // -----------------  cfunc  ----------------------


    std::ostream &operator<< (std::ostream &out, const blop::function &f)
    {
	f.print(out);
	return out;
    }

    bool function::default_print_param_value_ = false;

    function contained_in(const function &f)
    {
	return new function_core::contained_in(f.base_);
    }


    function function::linear(const var &filename,
			      const function &f1,
			      const function &f2,
			      const function &f3,
			      const function &f4)
			      
    {
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d;

	istream *file = openin(filename);

	function filter = unset;
	if(f1.initialized()) filter.append(f1);
	if(f2.initialized()) filter.append(f2);
	if(f3.initialized()) filter.append(f3);
	if(f4.initialized()) filter.append(f4);

	array line, result;

	unsigned int N = 0;
	if(filter.initialized())
	{
	    N = filter.components();
	    result.resize(N);
	}
	while(getline(*file,line))
	{
	    if(line.empty()) continue;
	    if(line[0].str()[0] == '#') continue;
	    if(filter.initialized()) filter.meval(line,result);
	    else
	    {
		if(N == 0) N = line.size();
		result = line;
	    }

	    interp->add_point(result[0].dbl(), result[1].dbl());
	}

	if(interp->linear())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::linear(const array &x, const array &y)
    {
	unsigned int n = ::min(x.size(), y.size());
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d;
	for(unsigned int i=0; i<n; ++i) interp->add_point(x[i].dbl(), y[i].dbl());
	if(interp->linear())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::linear(const vector<double> &x, const vector<double> &y)
    {
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d(x,y);
	if(interp->linear())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::linear(const double x[], const double y[], int n)
    {
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d;
	for(int i=0; i<n; ++i) interp->add_point(x[i],y[i]);
	if(interp->linear())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::spline(const var &filename,
			      const function &f1,
			      const function &f2,
			      const function &f3,
			      const function &f4)
			      
    {
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d;

	istream *file = openin(filename);

	function filter = unset;
	if(f1.initialized()) filter.append(f1);
	if(f2.initialized()) filter.append(f2);
	if(f3.initialized()) filter.append(f3);
	if(f4.initialized()) filter.append(f4);

	array line, result;

	unsigned int N = 0;
	if(filter.initialized())
	{
	    N = filter.components();
	    result.resize(N);
	}
	while(getline(*file,line))
	{
	    if(line.empty()) continue;
	    if(line[0].str()[0] == '#') continue;
	    if(filter.initialized()) filter.meval(line,result);
	    else
	    {
		if(N == 0) N = line.size();
		result = line;
	    }

	    interp->add_point(result[0].dbl(), result[1].dbl());
	}

	if(interp->spline())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::spline(const array &x, const array &y)
    {
	unsigned int n = ::min(x.size(), y.size());
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d;
	for(unsigned int i=0; i<n; ++i) interp->add_point(x[i].dbl(), y[i].dbl());
	if(interp->spline())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::spline(const vector<double> &x, const vector<double> &y)
    {
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d(x,y);
	if(interp->spline())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::spline(const double x[], const double y[], int n)
    {
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d;
	for(int i=0; i<n; ++i) interp->add_point(x[i],y[i]);
	if(interp->spline())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    // --------------  shape-preserving piecewise cubic hermite interpolating polynomial ----
    function function::sppchip(const var &filename,
			      const function &f1,
			      const function &f2,
			      const function &f3,
			      const function &f4)
			      
    {
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d;

	istream *file = openin(filename);

	function filter = unset;
	if(f1.initialized()) filter.append(f1);
	if(f2.initialized()) filter.append(f2);
	if(f3.initialized()) filter.append(f3);
	if(f4.initialized()) filter.append(f4);

	array line, result;

	unsigned int N = 0;
	if(filter.initialized())
	{
	    N = filter.components();
	    result.resize(N);
	}
	while(getline(*file,line))
	{
	    if(line.empty()) continue;
	    if(line[0].str()[0] == '#') continue;
	    if(filter.initialized()) filter.meval(line,result);
	    else
	    {
		if(N == 0) N = line.size();
		result = line;
	    }

	    interp->add_point(result[0].dbl(), result[1].dbl());
	}

	if(interp->sppchip())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::sppchip(const array &x, const array &y)
    {
	unsigned int n = ::min(x.size(), y.size());
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d;
	for(unsigned int i=0; i<n; ++i) interp->add_point(x[i].dbl(), y[i].dbl());
	if(interp->sppchip())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::sppchip(const vector<double> &x, const vector<double> &y)
    {
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d(x,y);
	if(interp->sppchip())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::sppchip(const double x[], const double y[], int n)
    {
	polynomial_interpolator_1d *interp = new polynomial_interpolator_1d;
	for(int i=0; i<n; ++i) interp->add_point(x[i],y[i]);
	if(interp->sppchip())
	{
	    function_core::interpolated_func *c = new function_core::interpolated_func(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }





    function cfunc(void *p)             {  return function_core::cfunc(p); }
    function cfunc(var (*p)(var))       {  return function_core::cfunc(p); }
    function cfunc(var (*p)(var,var))   {  return function_core::cfunc(p); }
    function cfunc(var (*p)(var,var,var)) {  return function_core::cfunc(p); }
    function cfunc(var (*p)(var,var,var,var)) {  return function_core::cfunc(p); }
    function cfunc(double (*p)(double)) {  return function_core::cfunc(p); }
    function cfunc(double (*p)(double,double)) {  return function_core::cfunc(p); }
    function cfunc(double (*p)(double,double,double)) {  return function_core::cfunc(p); }
    function cfunc(double (*p)(double,double,double,double)) {  return function_core::cfunc(p); }
    function cfunc(complex<double> (*p)(double)) { return function_core::cfunc(p); }
    function cfunc(var (*p)(const std::vector<blop::var> &args,
			    const std::vector<blop::var> &pars),
		   int nargs, int npars) { return function_core::cfunc(p,nargs,npars); }

    function::global_initializer::global_initializer()
    {
	static int count = 0;
	if(count++ == 0)
	{
	    new(&_1) function(new function_core::arg(1));
	    new(&_2) function(new function_core::arg(2));
	    new(&_3) function(new function_core::arg(3));
	    new(&_4) function(new function_core::arg(4));
	    new(&_5) function(new function_core::arg(5));
	    new(&_6) function(new function_core::arg(6));
	    new(&_7) function(new function_core::arg(7));
	    new(&_8) function(new function_core::arg(8));
	    new(&_9) function(new function_core::arg(9));
	    new(&_10) function(new function_core::arg(10));
	    new(&_11) function(new function_core::arg(11));
	    new(&_12) function(new function_core::arg(12));
	    new(&_13) function(new function_core::arg(13));
	    new(&_14) function(new function_core::arg(14));
	    new(&_15) function(new function_core::arg(15));
	    new(&_16) function(new function_core::arg(16));
	    new(&_17) function(new function_core::arg(17));
	    new(&_18) function(new function_core::arg(18));
	    new(&_19) function(new function_core::arg(19));
	    new(&_20) function(new function_core::arg(20));
	    new(&_N)  function(new function_core::actual_nargs);
	    new(&_0)  function(new function_core::extra_param);
	}
    }

    double tan(double a) { return std::tan(a); }
}


blop::function operator==(const blop::function &f1, const blop::function &f2)
{
    return blop::operator==(f1,f2);
}
blop::function operator<(const blop::function &f1, const blop::function &f2)
{
    return blop::operator<(f1,f2);
}
blop::function operator>(const blop::function &f1, const blop::function &f2)
{
    return blop::operator>(f1,f2);
}


