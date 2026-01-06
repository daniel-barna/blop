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
#include "interpolate.hh"
#include "blop_bookkeeper.h"
#include "geometry.h"
#include "blop_time.hh"
#include "plottable.h"

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

    int function::components() const
    {
	if(!base_) return 0;
	return base_->n_out();
    }

    bool function::default_check_args_ = true;

    void function::named_param(const var &id, const var &v)
    {
	function_core::named_param::values()[id] = v;
    }
    void function::named_params(const std::map<std::string,blop::var> &v)
    {
        function_core::named_param::values() = v;
    }
    void function::clear_named_params()
    {
        function_core::named_param::values().clear();
    }

    bool function::equals(const function &f) const
    {
	if(!base_ || !f.base_) return false;
	return base_->equals(f.base_);
    }

    void function::copy_arg_ranges_(const function &f1,
				    const function &f2,
				    const function &f3,
				    const function &f4,
				    const function &f5,
				    const function &f6)
    {
	vector<const function *> ff;
	if(f1.initialized()) ff.push_back(&f1);
	if(f2.initialized()) ff.push_back(&f2);
	if(f3.initialized()) ff.push_back(&f3);
	if(f4.initialized()) ff.push_back(&f4);
	if(f5.initialized()) ff.push_back(&f5);
	if(f6.initialized()) ff.push_back(&f6);

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
	if(!base_) return;
	result_.resize(base_->n_out());
	parameters_.resize(base_->npars());
	check_args_ = default_check_args_;
    }

#ifdef __MAKECINT__    
    function::function(void *fptr)
    {
	//nargs_ = npars_ = -1;
	base_ = new function_core::cfunc(fptr);
	print_param_value_ = default_print_param_value_;
	init_();
    }
#endif

    function::function(const std::vector<function::core*> &bases, bool clone_them)
    {
	if(bases.size() == 1)
	{
	    if(clone_them) base_ = bases[0]->clone();
	    else base_ = bases[0];
	}
	else
	{
	    function_core::multiple *m = new function_core::multiple;
	    base_ = m;
	    m->base_.resize(bases.size());
	    for(unsigned int i=0; i<bases.size(); ++i)
	    {
		if(clone_them) m->base_[i] = bases[i]->clone();
		else m->base_[i] = bases[i];
	    }
	    print_param_value_ = default_print_param_value_;
	    init_();
	}
    }

    function::function(const std::vector<blop::var> &c)
    {
	if(!c.empty())
	{
	    if(c.size() == 1) base_ = new constant(c[0]);
	    else
	    {
		function_core::multiple *m = new function_core::multiple;
		base_ = m;
		m->base_.resize(c.size());
		for(unsigned int i=0; i<c.size(); ++i) m->base_[i] = new constant(c[i]);
	    }
	    print_param_value_ = default_print_param_value_;
	}
	else base_ = 0;
	init_();
    }

    function::function()
    {
	base_ = 0;
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const function::core &f)
    {
	base_ = f.clone();
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(function::core *f)
    {
	//nargs_ = npars_ = -1;
	base_ = f;
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(double d)
    {
	if(d != unset) base_ = new constant(d);
	else base_ = 0;
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(double d1, double d2)
    {
	if(d1==unset || d2==unset) warning::print("d1 or d2 is unset","function::function(double d1, double d2)");
	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	m->base_.resize(2);
	m->base_[0] = new constant(d1);
	m->base_[1] = new constant(d2);
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(double d1, double d2, double d3)
    {
	if(d1==unset || d2==unset || d3==unset) warning::print("d1 or d2 or d3 is unset","function::function(double d1, double d2, double d3)");
	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	m->base_.resize(3);
	m->base_[0] = new constant(d1);
	m->base_[1] = new constant(d2);
	m->base_[2] = new constant(d3);
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(double d1, double d2, double d3, double d4)
    {
	if(d1==unset || d2==unset || d3==unset || d4==unset) warning::print("d1 or d2 or d3 or d4 is unset","function::function(double d1, double d2, double d3, double d4)");
	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	m->base_.resize(4);
	m->base_[0] = new constant(d1);
	m->base_[1] = new constant(d2);
	m->base_[2] = new constant(d3);
	m->base_[3] = new constant(d4);
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(int d1, double d2)
    {
	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	m->base_.resize(2);
	m->base_[0] = new constant(d1);
	m->base_[1] = new constant(d2);
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(double d1, int d2)
    {
	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	m->base_.resize(2);
	m->base_[0] = new constant(d1);
	m->base_[1] = new constant(d2);
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(int d1)
    {
	base_ = new constant(d1);
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(int d1, int d2)
    {
	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	m->base_.resize(2);
	m->base_[0] = new constant(d1);
	m->base_[1] = new constant(d2);
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(int d1, int d2, int d3)
    {
	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	m->base_.resize(3);
	m->base_[0] = new constant(d1);
	m->base_[1] = new constant(d2);
	m->base_[2] = new constant(d3);
	print_param_value_ = default_print_param_value_;
	init_();
    }
    function::function(int d1, int d2, int d3, int d4)
    {
	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	m->base_.resize(4);
	m->base_[0] = new constant(d1);
	m->base_[1] = new constant(d2);
	m->base_[2] = new constant(d3);
	m->base_[3] = new constant(d4);
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const var &v)
    {
	base_ = new constant(v);
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const char *v)
    {
	base_ = new constant(v);
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const string &s)
    {
	base_ = new constant(s);
	print_param_value_ = default_print_param_value_;
	init_();
    }

    function::function(const function &f)
    {
	parnames_ = f.parnames_;
	if(f.base_) base_ = f.base_->clone();
	else base_ = 0;
	print_param_value_ = f.print_param_value_;
	parameters_ = f.parameters_;

	init_();

	check_args_ = f.check_args_;

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

	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	if(f1.initialized()) m->base_.push_back(f1.base_->clone());
	if(f2.initialized()) m->base_.push_back(f2.base_->clone());

	print_param_value_ = false;
	if(f1.print_param_value_ || f2.print_param_value_) print_param_value_ = true;
	{
	    unsigned i=0;
	    for(; i<f1.parameters_.size(); ++i) parameters_.push_back(f1.parameters_[i]);
	    for(; i<f2.parameters_.size(); ++i) parameters_.push_back(f2.parameters_[i]);
	}
	init_();

	check_args_ = f1.check_args_ || f2.check_args_;

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

	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	if(f1.initialized()) m->base_.push_back(f1.base_->clone());
	if(f2.initialized()) m->base_.push_back(f2.base_->clone());
	if(f3.initialized()) m->base_.push_back(f3.base_->clone());

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

	check_args_ = f1.check_args_ || f2.check_args_ || f3.check_args_;

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

	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	if(f1.initialized()) m->base_.push_back(f1.base_->clone());
	if(f2.initialized()) m->base_.push_back(f2.base_->clone());
	if(f3.initialized()) m->base_.push_back(f3.base_->clone());
	if(f4.initialized()) m->base_.push_back(f4.base_->clone());

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

	check_args_ = f1.check_args_ || f2.check_args_ || f3.check_args_ || f4.check_args_;

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

	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	if(f1.initialized()) m->base_.push_back(f1.base_->clone());
	if(f2.initialized()) m->base_.push_back(f2.base_->clone());
	if(f3.initialized()) m->base_.push_back(f3.base_->clone());
	if(f4.initialized()) m->base_.push_back(f4.base_->clone());
	if(f5.initialized()) m->base_.push_back(f5.base_->clone());

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

	check_args_ = f1.check_args_ || f2.check_args_ || f3.check_args_ || f4.check_args_ || f5.check_args_;

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

    function::function(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6)
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
	for(unsigned int i=parnames_.size(); i<f6.parnames_.size(); ++i)
	    parnames_.push_back(f6.parnames_[i]);

	function_core::multiple *m = new function_core::multiple;
	base_ = m;
	if(f1.initialized()) m->base_.push_back(f1.base_->clone());
	if(f2.initialized()) m->base_.push_back(f2.base_->clone());
	if(f3.initialized()) m->base_.push_back(f3.base_->clone());
	if(f4.initialized()) m->base_.push_back(f4.base_->clone());
	if(f5.initialized()) m->base_.push_back(f5.base_->clone());
	if(f6.initialized()) m->base_.push_back(f6.base_->clone());

	print_param_value_ = false;
	if(f1.print_param_value_ ||
	   f2.print_param_value_ ||
	   f3.print_param_value_ ||
	   f4.print_param_value_ ||
	   f5.print_param_value_ ||
	   f6.print_param_value_ ) print_param_value_ = true;
	   
	{
	    unsigned int i=0;
	    for(; i<f1.parameters_.size(); ++i) parameters_.push_back(f1.parameters_[i]);
	    for(; i<f2.parameters_.size(); ++i) parameters_.push_back(f2.parameters_[i]);
	    for(; i<f3.parameters_.size(); ++i) parameters_.push_back(f3.parameters_[i]);
	    for(; i<f4.parameters_.size(); ++i) parameters_.push_back(f4.parameters_[i]);
	    for(; i<f5.parameters_.size(); ++i) parameters_.push_back(f5.parameters_[i]);
	    for(; i<f6.parameters_.size(); ++i) parameters_.push_back(f6.parameters_[i]);
	}
	init_();

	check_args_ = f1.check_args_ || f2.check_args_ || f3.check_args_ || f4.check_args_ || f5.check_args_ || f6.check_args_;

	if(!f1.derivatives_.empty() || !f2.derivatives_.empty() || !f3.derivatives_.empty() ||
           !f4.derivatives_.empty() ||
           !f5.derivatives_.empty() ||
           !f6.derivatives_.empty())
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
	    for(; i<f6.default_args_.size(); ++i) default_args_.push_back(f6.default_args_[i]);
	}

	copy_arg_ranges_(f1,f2,f3,f4,f5,f6);
    }



    function::~function()
    {
	delete base_;
	for(deriv_map::iterator iter=derivatives_.begin(); iter != derivatives_.end(); ++iter)
	{
	    delete (*iter).second;
	}
    }

    // ---------------------  Append more components -----------------------------------

    function &function::append(const function &f)
    {
	if(f.base_ == 0) return *this;

	for(unsigned int i=parameters_.size(); i<f.parameters_.size(); ++i) parameters_.push_back(f.parameters_[i]);

	if(base_ == 0)
	{
	    base_ = f.base_->clone();
	}
	else if(function_core::multiple *m = dynamic_cast<function_core::multiple*>(base_))
	{
	    m->base_.push_back(f.base_->clone());
	}
	else
	{
	    function_core::multiple *m1 = new function_core::multiple;
	    m1->base_.resize(2);
	    m1->base_[0] = base_;
	    m1->base_[1] = f.base_->clone();
	    base_ = m1;
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

    /*
      // already implemented as parformat
    function &function::param_format(const var &fmt)
    {
        for(unsigned int i=0; i<parameters_.size(); ++i) parameters_[i].format(fmt);
        return *this;
    }
    */

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

    var &function::param(unsigned int i)
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

    function &function::params(const std::vector<blop::var> &p)
    {
        parameters_ = p;
        return *this;
    }
    function &function::params(const std::vector<double> &p)
    {
        parameters_.resize(p.size());
        for(unsigned int i=0; i<p.size(); ++i) parameters_[i] = p[i];
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

    function &function::param(const function &p, var value)
    {
        function_core::funcparameter *P = dynamic_cast<function_core::funcparameter*>(p.base_);
        if(!P)
        {
            warning::print("The provided argument 'p' is not a pure function parameter function","function::param(const function &p, var value)");
            return *this;
        }

        // a simple funcparameter returns npars() which is equal to the parameter index (1-based)
        int i = P->npars();
	if(i<1)
	{
	    warning::print("Parameter index < 1",
			   "function::param(int parindex, const var &value)");
	    return *this;
	}
	--i;
	if(i >= (int)parameters_.size())  parameters_.resize(i+1);
	parameters_[i] = value;
	return *this;
    }

    function &function::param(const function &p, const var &value, const var &name)
    {
        function_core::funcparameter *P = dynamic_cast<function_core::funcparameter*>(p.base_);
        if(!P)
        {
            warning::print("The provided argument 'p' is not a pure function parameter function","function::param(const function &p, var value)");
            return *this;
        }

        // a simple funcparameter returns npars() which is equal to the parameter index (1-based)
        int i = P->npars();
	if(i<1)
	{
	    warning::print("Parameter index < 1",
			   "function::param(int parindex, const var &value)");
	    return *this;
	}
	--i;
	if(i >= (int)parameters_.size())  parameters_.resize(i+1);
	parameters_[i] = value;
        if(i >= (int)parnames_.size()) parnames_.resize(i+1);
        parnames_[i] = name;
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

    const var &function::param(const function &p) const
    {
        static var dummy;

        function_core::funcparameter *P = dynamic_cast<function_core::funcparameter*>(p.base_);
        if(!P)
        {
            warning::print("The provided argument 'p' is not a pure function parameter function","function::param(const function &p, var value)");
            return dummy;
        }

        // a simple funcparameter returns npars() which is equal to the parameter index (1-based)
        int i = P->npars()-1;
        if(i<0 || (int)parameters_.size()<=i)
        {
            warning::print("Parameter index out of range","function::param(const function &p)");
            return dummy;
        }
	return parameters_[i];
    }

    var &function::param(const function &p) 
    {
        static var dummy;

        function_core::funcparameter *P = dynamic_cast<function_core::funcparameter*>(p.base_);
        if(!P)
        {
            warning::print("The provided argument 'p' is not a pure function parameter function","function::param(const function &p, var value)");
            return dummy;
        }

        // a simple funcparameter returns npars() which is equal to the parameter index (1-based)
        int i = P->npars()-1;
        if(i<0 || (int)parameters_.size()<=i)
        {
            warning::print("Parameter index out of range","function::param(const function &p)");
            return dummy;
        }
	return parameters_[i];
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

    int function::nargs() const
    {
	if(!base_)
	{
	    warning::print("Uninitialized function","function::nargs()");
	    return 0;
	}
	return base_->nargs();
    }

    int function::npars() const
    {
	if(!base_)
	{
	    warning::print("Uninitialized function","function::npars()");
	    return 0;
	}
	return base_->npars();
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
	    const double min = (i<arg_min_.size() ? arg_min_[i] : unset);
	    if(min != unset && args[i].dbl() < min) this_arg_ok = false;
	    const double max = (i<arg_max_.size() ? arg_max_[i] : unset);
	    if(max != unset && args[i].dbl() > max) this_arg_ok = false;
	    if(!this_arg_ok)
	    {
		result = false;
		char argvalue[100];
		sprintf(argvalue,"%.15f",args[i].dbl());
		char minvalue[100], maxvalue[100];
		sprintf(minvalue,"%.15f",min);
		sprintf(maxvalue,"%.15f",max);
		warning::print("Function argument #" & var(i+1) &
			       var("=") & argvalue & var(" is out of range [") &
			       minvalue & var(";") & maxvalue & var("]"),
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
	if(!base_)
	{
	    warning::print("Uninitialized function","function::uses_arg(int)");
	    return false;
	}
	return base_->uses_arg(argno);
    }

    bool function::uses_par(int parno) const
    {
	if(!base_)
	{
	    warning::print("Uninitialized function","function::uses_par(int)");
	    return 0;
	}
	return base_->uses_par(parno);
    }


    //---------------------- print the function --------------------------------
    var function::sprint() const
    {
	if(!base_)
	{
	    warning::print("Uninitialized function","function::sprint()");
	    return "";
	}
	string result = base_->sprint(parameters_,print_param_value_);
	if(base_->n_out()>1) result = "[" + result + "]";
	return result;
    }
    var function::sprint(std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
    {
	if(!base_)
	{
	    warning::print("Uninitialized function","function::sprint()");
	    return "";
	}
	string result = base_->sprint(parameters_,print_param_value_,variable_names,param_names);
	if(base_->n_out()>1) result = "[" + result + "]";
	return result;
    }

    var function::sprint_latex(const var &x, const var &y, const var &z) const
    {
	if(!base_)
	{
	    warning::print("Uninitialized function","function::sprint_latex(...)");
	    return "";
	}
	var result = base_->sprint_latex(parameters_,print_param_value_,x,y,z);
	if(base_->n_out()>1) result = var("\\left[") & result & var("\\right]");
	result = var("$") & result & var("$");
	return result;
    }

    void function::print(std::ostream &out) const { out<<sprint(); }

    function function::operator[] (unsigned int comp) const
    {
	if(!base_) return 0.0;

	if((int)comp >= base_->n_out())
	{
	    warning::print("Index out of range",var("function::operator[](") & var(comp) & var(")"));
	    return 0.0;
	}

	if(base_->n_out() == 1)
	{
	    function result(base_->clone());
	    result.parameters_ = parameters_;
	    result.print_param_value_ = print_param_value_;
	    result.default_args_ = default_args_;
	    result.parnames_ = parnames_;
	    return result;
	}

	function result(new function_core::component(*base_,comp));
	result.parameters_ = parameters_;
	result.print_param_value_ = print_param_value_;
	result.default_args_ = default_args_;
	result.parnames_ = parnames_;
	return result;
    }

    double function::eval_meas_error(const std::vector<blop::var> &vals, const std::vector<blop::var> &errors) const
    {
	double result = 0;
	for(unsigned int i=0; i<vals.size(); ++i)
	{
	    function deriv = derivative(i+1);
	    const double d = deriv.eval_dbl(vals);
	    result += d*d*errors[i].dbl()*errors[i].dbl();
	}
	return ::sqrt(result);
    }

    meas function::operator()(const meas &x1) const
    {
	vector<var> vals(1), errs(1);
	vals[0].dbl() = x1.value();
	errs[0].dbl() = x1.error();
	return meas(eval(vals),eval_meas_error(vals,errs));
    }
    meas function::operator()(const meas &x1, const meas &x2) const
    {
	vector<var> vals(2), errs(2);
	vals[0].dbl() = x1.value();
	errs[0].dbl() = x1.error();
	vals[1].dbl() = x2.value();
	errs[1].dbl() = x2.error();
	return meas(eval(vals),eval_meas_error(vals,errs));
    }
    meas function::operator()(const meas &x1, const meas &x2, const meas &x3) const
    {
	vector<var> vals(3), errs(3);
	vals[0].dbl() = x1.value();
	errs[0].dbl() = x1.error();
	vals[1].dbl() = x2.value();
	errs[1].dbl() = x2.error();
	vals[2].dbl() = x3.value();
	errs[2].dbl() = x3.error();
	return meas(eval(vals),eval_meas_error(vals,errs));
    }
    meas function::operator()(const meas &x1, const meas &x2, const meas &x3, const meas &x4) const
    {
	vector<var> vals(4), errs(4);
	vals[0].dbl() = x1.value();
	errs[0].dbl() = x1.error();
	vals[1].dbl() = x2.value();
	errs[1].dbl() = x2.error();
	vals[2].dbl() = x3.value();
	errs[2].dbl() = x3.error();
	vals[3].dbl() = x4.value();
	errs[3].dbl() = x4.error();
	return meas(eval(vals),eval_meas_error(vals,errs));
    }

    // the parenthesis operator, called with functions as arguments,
    // creates another function, with substituted arguments
    function function::operator() (const function &a1) const
    {
	if(a1.is_constant() && npars()==0)
	{
	    if(a1.nargs() > 0) warning::print("a1 claims to be constant, but has arguments");
	    std::vector<blop::var> args;
	    std::vector<blop::var> r1;
	    std::vector<blop::var> r2;
	    a1.meval(args, r1);
	    meval(r1,r2);
	    return r2;
	}

	if(!a1.base_) return unset;
	function result(unset);
	result.print_param_value_ = print_param_value_;
	result.base_ = new function_core::argument_subst(base_, a1.base_);
	result.init_();
	if(parameters_.size() > result.parameters_.size()) result.parameters_.resize(parameters_.size());
	for(unsigned int i=0; i<parameters_.size(); ++i) result.parameters_[i] = parameters_[i];
	for(unsigned int i=result.parameters_.size(); i<a1.parameters_.size(); ++i)
	    result.parameters_.push_back(a1.parameters_[i]);
	result.default_args_ = default_args_;
	return result;
    }
    function function::operator() (const function &a1,const function &a2) const
    {
	return operator()(function(a1,a2));
    }
    function function::operator() (const function &a1,const function &a2,const function &a3) const
    {
	return operator()(function(a1,a2,a3));
    }
    function function::operator() (const function &a1,const function &a2,const function &a3,const function &a4) const
    {
	return operator()(function(a1,a2,a3,a4));
    }
    function function::operator() (const function &a1,const function &a2,const function &a3,const function &a4,const function &a5) const
    {
	return operator()(function(a1,a2,a3,a4,a5));
    }

    // ------------------  function evaluation -----------------------------------------
    var function::eval(const vector<var> &args) const
    {
	if(check_args_) make_arg_check_(args);
	
	if(!base_)
	{
	    warning::print("Uninitialized function is being evaluated","function::eval(...)");
	    return 0;
	}
	int dummy = 0;
	base_->eval(args, default_args_, parameters_, result_, &dummy);
	return result_[0];
    }

    double function::eval_dbl(const std::vector<blop::var> &args, const std::vector<blop::var> &pars) const
    {
	if(!base_)
	{
	    warning::print("Uninitialized function is being evaluated","function::eval_dbl(vector,vector)");
	    return 0;
	}
	int dummy = 0;
	base_->eval_dbl(args,default_args_,pars,result_,&dummy);
	return result_[0].dbl();
    }

    double function::eval_dbl(const std::vector<blop::var> &args) const
    {
	if(!base_)
	{
	    warning::print("Uninitialized function is being evaluated","function::eval_dbl(...)");
	    return 0;
	}
	int dummy = 0;
	base_->eval_dbl(args,default_args_,parameters_,result_,&dummy);
	return result_[0].dbl();
    }

    void function::meval(const std::vector<blop::var> &args, std::vector<blop::var> &result) const
    {
	if(!base_)
	{
	    result.resize(0);
	    warning::print("Uninitialized function is being evaluated","function::meval(...)");
	    return;
	}
	if(check_args_) make_arg_check_(args);
	result.resize(base_->n_out());
	int runind = 0;
	base_->eval(args, default_args_, parameters_, result, &runind);
    }

    void function::meval_dbl(const std::vector<blop::var> &args, std::vector<blop::var> &result) const
    {
	if(!base_)
	{
	    result.resize(0);
	    warning::print("Uninitialized function is being evaluated","function::meval_dbl(...)");
	    return;
	}
	result.resize(base_->n_out());
	int runind = 0;
	base_->eval_dbl(args, default_args_, parameters_, result, &runind);
    }

    void function::meval_dbl(const std::vector<blop::var> &args,
			     const std::vector<blop::var> &pars,
			     std::vector<blop::var> &result) const
    {
	if(!base_)
	{
	    result.resize(0);
	    warning::print("Uninitialized function is being evaluated","function::meval_dbl(...)");
	    return;
	}
	result.resize(base_->n_out());
	int runind = 0;
	base_->eval_dbl(args, default_args_, pars, result, &runind);
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
	delete base_;
	if(f.base_) base_ = f.base_->clone();
	else base_ = 0;
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
	delete base_;
	base_ = new constant(v);
	parameters_.clear();
	init_();

	for(deriv_map::iterator i1 = derivatives_.begin(); i1 != derivatives_.end(); ++i1)
	{
	    delete (*i1).second;
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
	return *this;
    }

    const function &function::operator= (double v)
    {
	if(v == unset)
	{
	    delete base_;
	    base_ = 0;
	    parameters_.clear();
	}
	else operator=(var(v));
	return *this;
    }


    // -------------  operators, functions  ------------------------

    template <class T>
    void function::init_binary_(const function &left, const function &right)
    {
	print_param_value_ = (left.print_param_value_ || right.print_param_value_);
	delete base_;
	if(left.components() != right.components()) warning::print("Component mismatch","function::init_binary");

	base_ = new T(left.base_, right.base_);
	init_();
	parameters_.resize(::max(left.parameters_.size(), right.parameters_.size()));
	unsigned int i=0; 
	for(; i<left .parameters_.size(); ++i) parameters_[i] = left.parameters_[i];
	for(; i<right.parameters_.size(); ++i) parameters_[i] = right.parameters_[i];
    }

    template <class T>
    void function::init_unary_(const function &operand)
    {
	print_param_value_ = operand.print_param_value_;
	delete base_;
	base_ = new T(operand.base_);
	init_();
	parameters_ = operand.parameters_;
    }

    function operator& (const function &left, const function &right)
    {
	function result;
	result.init_binary_<function_core::concatenator>(left,right);
	return result;
    }

    function operator&& (const function &left, const function &right)
    {
	function result;
	result.init_binary_<function_core::logical_and>(left,right);
	return result;
    }

    function operator|| (const function &left, const function &right)
    {
	function result;
	result.init_binary_<function_core::logical_or>(left,right);
	return result;
    }

    function sqrt(const function &o)
    {
	function result;
	result.init_unary_<function_core::Sqrt>(o);
	//result.nargs(o.nargs_);
	//result.npars(o.npars_);
	return result;
    }

    function format(const function &func, const function &form)
    {
        return new function_core::format(func.base_,form.base_);
    }

    function format(const function &func, const var &form)
    {
        return blop::format(func, blop::function(form));
    }

    function max(const function &y, const function &x)
    {
	function result;
	result.init_binary_<function_core::Max>(y,x);
	return result;
    }
    function max(const function &y, double x)
    {
	return max(y,function(x));
    }
    function max(double y, const function &x)
    {
	return max(function(y),x);
    }

    function min(const function &y, const function &x)
    {
	function result;
	result.init_binary_<function_core::Min>(y,x);
	return result;
    }
    function min(const function &y, double x)
    {
	return min(y,function(x));
    }
    function min(double y, const function &x)
    {
	return min(function(y),x);
    }

    function maximum(const function &func,
		     const function &from,
		     const function &to,
		     const function &step)
    {
	if(func.is_constant())
	{
	    vector<var> args, result;
	    func.meval(args,result);
	    return result;
	}
	return new function_core::max_in_interval(func.base_,
						  from.base_,
						  to.base_,
						  step.base_);
    }
    function minimum(const function &func,
		     const function &from,
		     const function &to,
		     const function &step)
    {
	if(func.is_constant())
	{
	    vector<var> args, result;
	    func.meval(args,result);
	    return result;
	}
	return new function_core::min_in_interval(func.base_,
						  from.base_,
						  to.base_,
						  step.base_);
    }
    function integral(const function &func,
		       const function &from,
		       const function &to,
		       const function &step)
    {
	return new function_core::integral(func.base_,
					   from.base_,
					   to.base_,
					   step.base_);
    }

    double integral(const function &func, double from, double to, double step)
    {
        if(func.nargs()>1)
            warning::print("The function to be integrated has more than 1 arguments!","integral(const function &func, double from, double to, double step)");
        if(to<=from)
        {
            warning::print("Upper integration limit is less than lower limit","integral(const function &func, double from, double to, double step)");
            return 0;
        }
        if(step<=0.0) step = (to-from)/100.0;
        double result = 0;
        for(double x=from; x<to+0.1*step; x+=step)
        {
            result += func(x).dbl();
        }
        return result*step;
    }

    function operator==(const function &left, const function &right)
    {
	if(left.is_constant() && right.is_constant())
	{
	    if(left.components() != right.components()) return false;
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    for(unsigned int i=0; i<leftresult.size(); ++i) if(leftresult[i].dbl() != rightresult[i].dbl()) return false;
	    return true;
	}
	function result;
	result.init_binary_<function_core::EqualDbl>(left, right);
	return result;
    }

    function operator==(const function  &left, const var &right)
    {
	return operator==(left,function(right));
    }
    function operator==(const var  &left, const function &right)
    {
	return operator==(function(left),right);
    }
    function operator==(const function  &left, double right)
    {
	return operator==(left,function(right));
    }
    function operator==(double left, const function &right)
    {
	return operator==(function(left),right);
    }
    function operator==(const function  &left, int right)
    {
	return operator==(left,function(right));
    }
    function operator==(int left, const function &right)
    {
	return operator==(function(left),right);
    }

    function operator==(const function &left, const std::string &right)
    {
	function c = right;
	function result;
	result.init_binary_<function_core::EqualStr>(left,c);
	return result;
    }
    function operator==(const std::string &left, const function &right)
    {
	function c = left;
	function result;
	result.init_binary_<function_core::EqualStr>(c,right);
	return result;
    }
    function operator==(const function &left, const char *right)
    {
	function c = right;
	function result;
	result.init_binary_<function_core::EqualStr>(left,c);
	return result;
    }
    function operator==(const char *left, const function &right)
    {
	function c = left;
	function result;
	result.init_binary_<function_core::EqualStr>(c,right);
	return result;
    }

    function operator!=(const function &left, const function &right)
    {
	if(left.is_constant() && right.is_constant())
	{
	    if(left.components() != right.components()) return false;
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    for(unsigned int i=0; i<leftresult.size(); ++i) if(leftresult[i].dbl() == rightresult[i].dbl()) return false;
	    return true;
	}
	function result;
	result.init_binary_<function_core::NotEqualDbl>(left,right);
	return result;
    }
    function operator!=(const function &left, double right)
    {
	return operator!=(left,function(right));
    }
    function operator!=(double left, const function &right)
    {
	return operator!=(function(left),right);
    }

    function operator< (const function &left, const function &right)
    {
	if(left.is_constant() && right.is_constant())
	{
	    if(left.components() != right.components()) return false;
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    for(unsigned int i=0; i<leftresult.size(); ++i) if(leftresult[i].dbl() >= rightresult[i].dbl()) return false;
	    return true;
	}
	function result;
	result.init_binary_<function_core::LessThan>(left,right);
	return result;
    }
    function operator< (const function &left, double right)
    {
	return operator<(left,function(right));
    }
    function operator< (double left, const function &right)
    {
	return operator<(function(left),right);
    }

    function operator<= (const function &left, const function &right)
    {
	if(left.is_constant() && right.is_constant())
	{
	    if(left.components() != right.components()) return false;
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    for(unsigned int i=0; i<leftresult.size(); ++i) if(leftresult[i].dbl() > rightresult[i].dbl()) return false;
	    return true;
	}
	function result;
	result.init_binary_<function_core::LessEqual>(left,right);
	return result;
    }
    function operator<= (const function &left, double right)
    {
	return operator<=(left,function(right));
    }
    function operator<= (double left, const function &right)
    {
	return operator<=(function(left),right);
    }

    function operator> (const function &left, const function &right)
    {
	if(left.is_constant() && right.is_constant())
	{
	    if(left.components() != right.components()) return false;
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    for(unsigned int i=0; i<leftresult.size(); ++i) if(leftresult[i].dbl() <= rightresult[i].dbl()) return false;
	    return true;
	}
	function result;
	result.init_binary_<function_core::GreaterThan>(left,right);
	return result;
    }
    function operator> (const function &left, double right)
    {
	return operator>(left,function(right));
    }
    function operator> (double left, const function &right)
    {
	return operator>(function(left),right);
    }

    function operator>= (const function &left, const function &right)
    {
	if(left.is_constant() && right.is_constant())
	{
	    if(left.components() != right.components()) return false;
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    for(unsigned int i=0; i<leftresult.size(); ++i) if(leftresult[i].dbl() < rightresult[i].dbl()) return false;
	    return true;
	}
	function result;
	result.init_binary_<function_core::GreaterEqual>(left,right);
	return result;
    }
    function operator>= (const function &left, double right)
    {
	return operator>=(left,function(right));
    }
    function operator>= (double left, const function &right)
    {
	return operator>=(function(left),right);
    }

    function operator+ (const function &left, const function &right)
    {
	if(!left.initialized() || !right.initialized())
	{
	    warning::print("Uninitialized function in operator+");
	    return 0.0;
	}
	if(left.components() != right.components()) warning::print("Number of components of the two functions is not equal");

	if(left.is_constant() && right.is_constant())
	{
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    if(leftresult.size()<rightresult.size()) leftresult.resize(rightresult.size());
	    for(unsigned int i=0; i<leftresult.size(); ++i) leftresult[i] = leftresult[i].dbl() + (i<rightresult.size()?rightresult[i].dbl():0);
	    return leftresult;
	}
	
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_);
	if(c1 && c2) return c1->value_.dbl() + c2->value_.dbl();
	if(c1 && c1->value_.dbl() == 0.0) return right;
	if(c2 && c2->value_.dbl() == 0.0) return left;
	function result;
	result.init_binary_<function_core::Add>(left,right);
	return result;
    }
    function operator+ (const function  &left, double right)
    {
	return operator+(left,function(right));
    }
    function operator+ (double left, const function &right)
    {
	return operator+(function(left),right);
    }
    function operator+ (const function  &left, int right)
    {
	return operator+(left,function(right));
    }
    function operator+ (int left, const function &right)
    {
	return operator+(function(left),right);
    }
    function operator+ (const function  &left, const var &right)
    {
	return operator+(left,function(right));
    }
    function operator+ (const var &left, const function &right)
    {
	return operator+(function(left),right);
    }

    function operator- (const function &left, const function &right)
    {
	if(!left.initialized() || !right.initialized())
	{
	    warning::print("Uninitialized function in operator+");
	    return 0.0;
	}
	if(left.components() != right.components()) warning::print("Number of components of the two functions is not equal");

	if(left.is_constant() && right.is_constant())
	{
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    if(leftresult.size()<rightresult.size()) leftresult.resize(rightresult.size());
	    for(unsigned int i=0; i<leftresult.size(); ++i) leftresult[i] = leftresult[i].dbl() - (i<rightresult.size()?rightresult[i].dbl():0);
	    return leftresult;
	}
	
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_);
	if(c1 && c2) return c1->value_.dbl() - c2->value_.dbl();
	if(c2 && c2->value_.dbl() == 0.0) return left;
	if(c1 && c1->value_.dbl() == 0.0)
	{
	    function result;
	    result.init_unary_<function_core::Neg>(right);
	    return result;
	}
	function result;
	result.init_binary_<function_core::Sub>(left,right);
	return result;
    }
    function operator- (const function &left, double right)
    {
	return operator-(left,function(right));
    }
    function operator- (double left, const function &right)
    {
	return operator-(function(left), right);
    }
    function operator- (const function &left, int right)
    {
	return operator-(left,function(right));
    }
    function operator- (int left, const function &right)
    {
	return operator-(function(left), right);
    }
    function operator- (const function &left, const var &right)
    {
	return operator-(left,function(right));
    }
    function operator- (const var &left, const function &right)
    {
	return operator-(function(left), right);
    }

    function operator% (const function &left, const function &right)
    {
	if(!left.initialized() || !right.initialized())
	{
	    warning::print("Uninitialized function in operator+");
	    return 0.0;
	}
	if(left.components() != right.components()) warning::print("Number of components of the two functions is not equal");

	if(left.is_constant() && right.is_constant())
	{
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    if(leftresult.size()<rightresult.size()) leftresult.resize(rightresult.size());
	    for(unsigned int i=0; i<leftresult.size(); ++i) leftresult[i] = leftresult[i].integer() % (i<rightresult.size()?rightresult[i].integer():1);
	    return leftresult;
	}

	function result;
	result.init_binary_<function_core::Mod>(left,right);
	return result;
    }

    function cmp_mul(const function &left, const function &right)
    {
	if(!left.initialized() || !right.initialized())
	{
	    warning::print("Uninitialized function in cmp_mul");
	    return 0.0;
	}
        function result;
        result.init_binary_<function_core::cmp_mul>(left,right);
        return result;
    }
    function cmp_div(const function &left, const function &right)
    {
	if(!left.initialized() || !right.initialized())
	{
	    warning::print("Uninitialized function in cmp_mul");
	    return 0.0;
	}
        function result;
        result.init_binary_<function_core::cmp_div>(left,right);
        return result;
    }

    function operator* (const function &left, const function &right)
    {
	if(!left.initialized() || !right.initialized())
	{
	    warning::print("Uninitialized function in operator+");
	    return 0.0;
	}
	//if(left.components() != right.components()) warning::print("Number of components of the two functions is not equal");

	if(left.is_constant() && right.is_constant())
	{
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    if(leftresult.size()<rightresult.size()) leftresult.resize(rightresult.size());
	    for(unsigned int i=0; i<leftresult.size(); ++i) leftresult[i] = leftresult[i].dbl() * (i<rightresult.size()?rightresult[i].dbl():0);
	    return leftresult;
	}
	
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_);
	if( (c1 && c1->value_.dbl() == 0.0) || (c2 && c2->value_.dbl() == 0.0)) return 0.0;
	if(c1 && c2) return c1->value_.dbl()*c2->value_.dbl();
	if(c1 && c1->value_.dbl() == 1.0) return right;
	if(c2 && c2->value_.dbl() == 1.0) return left;
	function result;
	result.init_binary_<function_core::Mul>(left,right);
	return result;
    }
    function operator* (const function &left, double right)
    {
	return operator*(left, function(right));
    }
    function operator* (double left, const function &right)
    {
	return operator*(function(left),right);
    }
    function operator* (const function &left, int right)
    {
	return operator*(left, function(right));
    }
    function operator* (int left, const function &right)
    {
	return operator*(function(left),right);
    }
    function operator* (const function &left, const var &right)
    {
	return operator*(left, function(right));
    }
    function operator* (const var &left, const function &right)
    {
	return operator*(function(left),right);
    }

    function operator/ (const function &left, const function &right)
    {
	if(!left.initialized() || !right.initialized())
	{
	    warning::print("Uninitialized function in operator+");
	    return 0.0;
	}
	if(left.components() != right.components()) warning::print("Number of components of the two functions is not equal");

	if(left.is_constant() && right.is_constant())
	{
	    vector<var> args, leftresult, rightresult;
	    left.meval(args,leftresult);
	    right.meval(args,rightresult);
	    if(leftresult.size()<rightresult.size()) leftresult.resize(rightresult.size());
	    for(unsigned int i=0; i<leftresult.size(); ++i) leftresult[i] = leftresult[i].dbl() / (i<rightresult.size()?rightresult[i].dbl():0);
	    return leftresult;
	}
	
	function_core::constant *c1 = dynamic_cast<constant *>(left.base_);
	function_core::constant *c2 = dynamic_cast<constant *>(right.base_);
	if(c1 && c2) return c1->value_.dbl()/c2->value_.dbl();
	if(c1 && c1->value_.dbl()==0.0) return 0.0;
	if(c2 && c2->value_.dbl()==1.0) return left;
	function result;
	result.init_binary_<function_core::Div>(left,right);
	return result;
    }
    function operator/ (const function &left, double right)
    {
	return operator/(left,function(right));
    }
    function operator/ (double left, const function &right)
    {
	return operator/(function(left),right);
    }
    function operator/ (const function &left, int right)
    {
	return operator/(left,function(right));
    }
    function operator/ (int left, const function &right)
    {
	return operator/(function(left),right);
    }
    function operator/ (const function &left, const var &right)
    {
	return operator/(left,function(right));
    }
    function operator/ (const var &left, const function &right)
    {
	return operator/(function(left),right);
    }

    function operator- (const function &o)
    {
	if(o.is_constant())
	{
	    vector<var> args, result;
	    o.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = -result[i].dbl();
	    return result;
	}
	
	function result;
	result.init_unary_<function_core::Neg>(o);
	return result;
    }


    function atan2(const function &y, const function &x)
    {
	if(y.components() != x.components()) warning::print("Number of function components is not equal","atan2(function,function)");
	if(y.is_constant() && x.is_constant())
	{
	    vector<var> args, yresult,xresult;
	    y.meval(args,yresult);
	    x.meval(args,xresult);
	    if(xresult.size()>yresult.size()) yresult.resize(xresult.size());
	    for(unsigned int i=0; i<yresult.size(); ++i) yresult[i] = ::atan2(yresult[i].dbl(),(i<xresult.size()?xresult[i].dbl():0.0));
	    return yresult;
	}
	function_core::constant *yc = dynamic_cast<constant *>(y.base_);
	function_core::constant *xc = dynamic_cast<constant *>(x.base_);
	if(yc && xc) return ::atan2(yc->value_.dbl(),xc->value_.dbl());
	function result;
	result.init_binary_<function_core::Atan2>(y,x);
	return result;
    }
    function atan2(double y, const function &x)
    {
	return atan2(function(y),x);
    }
    function atan2(const function &y, double x)
    {
	return atan2(y,function(x));
    }

    function pow(const function &a,const function &exponent)
    {
	if(a.components() != exponent.components()) warning::print("Number of function components is not equal","pow(function,function)");
	if(a.is_constant() && exponent.is_constant())
	{
	    vector<var> args, aresult,eresult;
	    a.meval(args,aresult);
	    exponent.meval(args,eresult);
	    if(eresult.size()>aresult.size()) aresult.resize(eresult.size());
	    for(unsigned int i=0; i<aresult.size(); ++i) aresult[i] = ::pow(aresult[i].dbl(),(i<eresult.size()?eresult[i].dbl():0.0));
	    return aresult;
	}
	function_core::constant *ac = dynamic_cast<constant *>(a.base_);
	function_core::constant *ec = dynamic_cast<constant *>(exponent.base_);
	if(ac && ec)
        {
            return ::pow(ac->value_.dbl(), ec->value_.dbl());
        }
	if(ec)
	{
	    if(ec->value_.dbl() == 0.0) return 1.0;
	    if(ec->value_.dbl() == 1.0) return a;
	}
	function result;
	result.init_binary_<function_core::Pow>(a,exponent);
	return result;
    }
    function pow(double a, const function  &exponent)
    {
	return pow(function(a),exponent);
    }
    function pow(const function &a,double exponent)
    {
	return pow(a,function(exponent));
    }
    function pow(const function &a,int exponent)
    {
	function_core::constant *ac = dynamic_cast<constant *>(a.base_);
	if(ac) return ::pow(ac->value_.dbl(), exponent);

	// this function should be very similar to function::init_unary_
	function result(new Ipow(a.base_,exponent));
	result.print_param_value_ = a.print_param_value_;
	result.parameters_ = a.parameters_;
	return result;
    }
    function pow(const function &a, const var & exponent)
    {
	return pow(a, function(exponent));
    }


    function exp(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::exp(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Exp>(f);
	return result;
    }

    function log(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::log(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Log>(f);
	return result;
    }

    function log10(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::log10(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Log10>(f);
	return result;
    }

    function sin(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::sin(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Sin>(f);
	return result;
    }

    function asin(const function &f)
    {
        if(f.is_constant())
        {
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::asin(result[i].dbl());
	    return result;
        }
	function result;
	result.init_unary_<function_core::Asin>(f);
	return result;
    }

    function cos(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::cos(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Cos>(f);
	return result;
    }

    function acos(const function &f)
    {
        if(f.is_constant())
        {
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::acos(result[i].dbl());
	    return result;
        }
	function result;
	result.init_unary_<function_core::Acos>(f);
	return result;
    }

    function tan(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::tan(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Tan>(f);
	return result;
    }

    function atan(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::atan(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Atan>(f);
	return result;
    }

    function cot(const function &f)
    {
	function result;
	result.init_unary_<function_core::Cot>(f);
	return result;
    }

    function acot(const function &f)
    {
	function result;
	result.init_unary_<function_core::Acot>(f);
	return result;
    }

    function sinh(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::sinh(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Sinh>(f);
	return result;
    }

    function cosh(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::cosh(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Cosh>(f);
	return result;
    }

    function tanh(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::tanh(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Tanh>(f);
	return result;
    }

    function atanh(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::atanh(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Atanh>(f);
	return result;
    }

    function asinh(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::asinh(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Asinh>(f);
	return result;
    }

    function acosh(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::acosh(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Acosh>(f);
	return result;
    }

    function floor(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::floor(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Floor>(f);
	return result;
    }

    function ceil(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::ceil(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Ceil>(f);
	return result;
    }

    function round(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::round(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Round>(f);
	return result;
    }


    function erf(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::erf(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Erf>(f);
	return result;
    }	 

    function sign(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i)
	    {
		if(result[i].dbl()>0) result[i] = 1;
		else if(result[i].dbl()<0) result[i] = -1;
		else result[i] = 0;
	    }
	    return result;
	}
	function result;
	result.init_unary_<function_core::Sign>(f);
	return result;
    }

    function abs(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::fabs(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Abs>(f);
	return result;
    }
    function fabs(const function &f)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    for(unsigned int i=0; i<result.size(); ++i) result[i] = ::fabs(result[i].dbl());
	    return result;
	}
	function result;
	result.init_unary_<function_core::Abs>(f);
	return result;
    }
    function periodic_function(const function &f, double x1, double x2)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    return result;
	}
        return new function_core::periodic(f.base_,x1,x2);
    }
    function periodic_function(const function &f, double x1, double x2, double y1, double y2)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    return result;
	}
        return new function_core::periodic(f.base_,x1,x2,y1,y2);
    }
    function periodic_function(const function &f, double x1, double x2, double y1, double y2, double z1, double z2)
    {
	if(f.is_constant())
	{
	    vector<var> args, result;
	    f.meval(args,result);
	    return result;
	}
        return new function_core::periodic(f.base_,x1,x2,y1,y2,z1,z2);
    }

    function ifelse(const function &condition, const function &iftrue, const function &iffalse)
    {
	return new function_core::ifelse(condition,iftrue,iffalse);
    }

    function join_args(const function &separator, const function &from, const function &to)
    {
        return new function_core::join_args(separator,from,to);
    }

#ifdef HAVE_GSL

    function bessel_J(int nu, const function &a)
    {
	function result;
	bessel::setup_bessel_core(nu, a, bessel::J, result);
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

    function between_lines(const function &start, const function &end)
    {
        return new function_core::between_lines(start.base_,end.base_);
    }

    function charfunc(const function &left, const function &right, bool low_in, bool high_in)
    {
	if(left.components() != right.components()) warning::print("Component mismatch","charfunc");
	function result(new function_core::char_func(left.base_,right.base_,low_in,high_in));
	result.print_param_value_ = (left.print_param_value_ || right.print_param_value_);
	result.parameters_.resize(::max(left.parameters_.size(), right.parameters_.size()));
	unsigned int i=0; 
	for(; i<left .parameters_.size(); ++i) result.parameters_[i] = left .parameters_[i];
	for(; i<right.parameters_.size(); ++i) result.parameters_[i] = right.parameters_[i];
	return result;
    }

    // There is some memory leakage here: the previously created
    // function_core:: base pointers (allocated in
    // function::global_initializer::gobal_initializer() )
    // are not freed, and they are lost
    function _0(new function_core::named_param("linenumber"));
    function _b(new function_core::named_param("blocknumber"));
    function _B(new function_core::named_param("Blocknumber"));
    function _g(new function_core::named_param("groupingvalue"));
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
    function _n(const var &id) { return new function_core::named_param(id); }

    function ARG(int i) {return function_core::arg(i);}
    function PAR(int i) {return function_core::funcparameter(i);}

    // Returns a function which buffers its calling arguments from previous calls, 
    // and returns the arguments from the nth previous call
    function PREV(int n) { return function_core::prev(n); }

    bool function::initialized() const
    {
	return base_ != 0;
    }

    bool function::is_constant() const
    {
	if(!initialized()) return false;
	return base_->is_constant();
    }

/*
    function::operator double() const
    {
	if(!is_constant())
	    warning::print("Non-constant function is converted to double. Its value evaluated at 0 is used");
	return operator()(0.0).dbl();
    }
*/

    const function &function::operator+= (const function &f)
    {
	if(!base_ || !f.base_)
	{
	    warning::print("uninitialized function","function::operator+=");
	    return *this;
	}

	function_core::constant *c2 = dynamic_cast<constant *>(f.base_);
	if(c2 && c2->value_ == 0.0) return *this;

	function_core::constant *c1 = dynamic_cast<constant *>(base_);
	if(c1 && c1->value_ == 0.0)
	{
	    delete base_;
	    base_ = f.base_->clone();
	    return *this;
	}
	
	function_core::Add *add = new function_core::Add;
	add->left(base_);
	add->right(f.base_->clone());
	base_ = add;

	return *this;
    }

    const function &function::operator-= (const function &f)
    {
	if(!base_ || !f.base_)
	{
	    warning::print("uninitialized function","function::operator-=");
	    return *this;
	}

	function_core::constant *c2 = dynamic_cast<constant *>(f.base_);
	if(c2 && c2->value_ == 0.0) return *this;

	function_core::constant *c1 = dynamic_cast<constant *>(base_);
	if(c1 && c1->value_ == 0.0)
	{
	    function_core::Neg *neg = new function_core::Neg;
	    neg->operand(base_);
	    base_ = neg;
	    return *this;
	}
	
	function_core::Sub *sub = new function_core::Sub;
	sub->left(base_);
	sub->right(f.base_->clone());
	base_ = sub;

	return *this;
    }

    const function &function::operator*= (const function &f)
    {
	if(!base_ || !f.base_)
	{
	    warning::print("uninitialized function","function::operator*=");
	    return *this;
	}

	function_core::constant *c2 = dynamic_cast<constant *>(f.base_);
	if(c2 && c2->value_ == 0.0)
	{
	    delete base_;
	    base_ = new constant(0.0);
	    return *this;
	}
	if(c2 && c2->value_ == 1.0) return *this;

	function_core::constant *c1 = dynamic_cast<constant *>(base_);
	if(c1 && c1->value_ == 0.0) return *this;
	if(c1 && c1->value_ == 1.0)
	{
	    delete base_;
	    base_ = f.base_->clone();
	    return *this;
	}
	
	function_core::Mul *mul = new function_core::Mul;
	mul->left(base_);
	mul->right(f.base_->clone());
	base_ = mul;

	return *this;
    }

    const function &function::operator/= (const function &f)
    {
	if(!base_ || !f.base_)
	{
	    warning::print("uninitialized function","function::operator*=");
	    return *this;
	}

	function_core::constant *c2 = dynamic_cast<constant *>(f.base_);
	if(c2 && c2->value_ == 1.0) return *this;

	function_core::constant *c1 = dynamic_cast<constant *>(base_);
	if(c1 && c1->value_ == 0.0) return *this;
	
	function_core::Div *div = new function_core::Div;
	div->left(base_);
	div->right(f.base_->clone());
	base_ = div;

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
	if(!d.initialized())
	{
	    warning::print("Uninitialized function","function::derivative(int,const function &)");
	    return *this;
	}

	deriv_map::const_iterator iter = derivatives_.find(derivi);
	if(iter != derivatives_.end()) delete (*iter).second;

	if(components() != d.components())
	{
	    warning::print("Number of components mismatch when setting derivatives",
			   "function::derivative(int,const function &)");
	    return *this;
	}
	derivatives_[derivi] = d.base_->clone();
	return *this;
    }

    function function::derivative(int derivindex) const
    {
	if(!base_)
	{
	    warning::print("Uninitialized functions derivative...","function::derivative(in)");
	    return constant(0.0);
	}

	// if the user has defined this derivative, return that,
	// instead of trying to calculate it
	deriv_map::const_iterator iter = derivatives_.find(derivindex);
	if(iter != derivatives_.end())
	{
	    function result(*(*iter).second);
	    result.parameters_ = parameters_;
	    result.print_param_value_ = print_param_value_;
	    return result;
	}

	function::core *deriv = base_->create_derivative(derivindex);

	if(deriv == 0)
	{
	    const int di = -derivindex;
	    string s = "th";
	    if(di == 1) s = "st";
	    if(di == 2) s = "nd";
	    if(di == 3) s = "rd";
	    warning::print(var("Could not calculate ") & di & var(s) & " derivative",
			   "function::derivative(int)");
	    deriv = new constant(0.0);
	}
	function result(deriv);
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

    function function::polynomial(int n)
    {
        function result = PAR(1);
        for(int i=1; i<=n; ++i) result += PAR(i+1)*pow(_1,i);
        return result;
    }

    function function::interpolate_linear(const var &filename, const function &x, const function &y)
    {
        istream *file = openin(filename);
        if(!file) return unset;

        bool multidim_x = (x.components()>1);
        bool multidim_y = (y.components()>1);

        // If we do a multidimensional interpolation (in x), the file needs to be read twice:
        // First to get the grid points, and then to store the (x,y) points. That is, if the
        // input stream is a pipe, copy its content first to a temporary file, so that it can
        // be processed twice:
        if(dynamic_cast<ipstream*>(file) && multidim_x)
        {
            string tmpfilename = blop_bookkeeper::tmpfile("ipstream_tmpfile_XXXXXX");
            {
                ofstream otmpfile(tmpfilename.c_str());
                string line;
                while(getline(*file,line)) otmpfile<<line<<endl;
            }
            delete file;
            file = new ifstream(tmpfilename.c_str());
        }
        
        linear_interpolator_md<double,double>                *inter_md_1d = 0;
        linear_interpolator_md<double,valarray<double> >     *inter_md_md = 0;
        linear_interpolator_1d<double,double>                *inter_1d_1d = 0;
        linear_interpolator_1d<double,valarray<double> >     *inter_1d_md = 0;
        
        if(multidim_x)
        {
            if(multidim_y) inter_md_md = new linear_interpolator_md<double,valarray<double> >(x.components());
            else           inter_md_1d = new linear_interpolator_md<double,double>           (x.components());
        }
        else
        {
            if(multidim_y) inter_1d_md = new linear_interpolator_1d<double,valarray<double> >;
            else           inter_1d_1d = new linear_interpolator_1d<double,double>;
        }
        array xxx;
        xxx.resize(x.components());
        array yyy;
        yyy.resize(y.components());

        array line;
        
        // For a multidimensional interpolator, one needs to get first all possible gridpoints,
        // to set up this grid in the interpolator. 
        if(multidim_x)
        {
            vector<vector<double> > xvalues(x.components());
            
            // First loop over the file, to get all the possible gridpoints. 
            while(getline(*file,line))
            {
                // skip empty or commented lines
                if(line.empty()) continue;
                if(dgraph::default_comment_chars().find(line[0][0]) != string::npos) continue;
                //if(line[0][0]=='#') continue;
                
                x.meval(line,xxx);
                for(unsigned int i=0; i<xxx.size(); ++i)
                {
                    if(find(xvalues[i].begin(),xvalues[i].end(),xxx[i].dbl()) == xvalues[i].end()) xvalues[i].push_back(xxx[i].dbl());
                }
            }
            
            for(unsigned int i=0; i<xvalues.size(); ++i)
            {
                if(inter_md_1d) inter_md_1d->x(i+1,xvalues[i]);
                if(inter_md_md) inter_md_md->x(i+1,xvalues[i]);
            }
            
            // rewind the file to the beginning
            file->clear();
            file->seekg(0,ios_base::beg);
        }
        
        vector<double> XXX;
        XXX.resize(xxx.size());
        valarray<double> YYY;
        YYY.resize(yyy.size());
        
        while(getline(*file,line))
        {
            // skip empty or commented lines
            if(line.empty()) continue;
            if(line[0][0]=='#') continue;
            
            x.meval(line,xxx);
            y.meval(line,yyy);
            
            if(multidim_y)
            {
                for(unsigned int i=0; i<yyy.size(); ++i) YYY[i] = yyy[i].dbl();
            }
        
            if(multidim_x)
            {
                for(unsigned int i=0; i<xxx.size(); ++i) XXX[i] = xxx[i].dbl();
                if(inter_md_1d) inter_md_1d->y(XXX,yyy[0].dbl());
                if(inter_md_md) inter_md_md->y(XXX,YYY);
            }
            else
            {
                if(inter_1d_1d) inter_1d_1d->add_point(xxx[0].dbl(),yyy[0].dbl());
                if(inter_1d_md) inter_1d_md->add_point(xxx[0].dbl(),YYY);
            }
        }

        delete file;

/*
        if(inter_1d_1d) return *inter_1d_1d;
        if(inter_1d_md) return *inter_1d_md;
        if(inter_md_1d) return *inter_md_1d;
        if(inter_md_md) return *inter_md_md;
*/
        if(inter_1d_1d) return inter_1d_1d;
        if(inter_1d_md) return inter_1d_md;
        if(inter_md_1d) return inter_md_1d;
        if(inter_md_md) return inter_md_md;
        warning::print("This should never happen","function::interpolate_linear(filename,x,y)");
        return unset;
    }

    function function::interpolate_spline(const var &filename, const function &x, const function &y)
    {
        istream *file = openin(filename);
        if(!file) return unset;
        
        if(x.components() != 1)
        {
            warning::print("Spline interpolation can only be done in 1 dimension!");
            delete file;
            return unset;
        }

        bool multidim_y = (y.components()>1);
        
        spline_interpolator_1d<double,double>                *inter_1d_1d = 0;
        spline_interpolator_1d<double,valarray<double> >     *inter_1d_md = 0;
        
        if(multidim_y) inter_1d_md = new spline_interpolator_1d<double,valarray<double> >;
        else           inter_1d_1d = new spline_interpolator_1d<double,double>;
        
        array yyy;
        yyy.resize(y.components());
        valarray<double> YYY;
        YYY.resize(yyy.size());
        array line;
        
        while(getline(*file,line))
        {
            // skip empty or commented lines
            if(line.empty()) continue;
            if(line[0][0]=='#') continue;
            
            const double xvalue = x.eval(line).dbl();
            y.meval(line,yyy);
            
            if(multidim_y)
            {
                for(unsigned int i=0; i<yyy.size(); ++i) YYY[i] = yyy[i].dbl();
                if(inter_1d_md) inter_1d_md->add_point(xvalue,YYY);
            }
            else
            {
                if(inter_1d_1d) inter_1d_1d->add_point(xvalue,yyy[0].dbl());
            }
        }

        delete file;
/*
        if(inter_1d_1d) return *inter_1d_1d;
        if(inter_1d_md) return *inter_1d_md;
*/
        if(inter_1d_1d) return inter_1d_1d;
        if(inter_1d_md) return inter_1d_md;
        return unset;
    }

    function function::interpolate_sppchip(const var &filename, const function &x, const function &y)
    {
        istream *file = openin(filename);
        if(!file) return unset;
        
        if(x.components() != 1)
        {
            delete file;
            warning::print("sppchip interpolation can only be done in 1 dimension!");
            return unset;
        }

        bool multidim_y = (y.components()>1);
        
        sppchip_interpolator_1d<double,double>                *inter_1d_1d = 0;
        sppchip_interpolator_1d<double,valarray<double> >     *inter_1d_md = 0;
        
        if(multidim_y) inter_1d_md = new sppchip_interpolator_1d<double,valarray<double> >;
        else           inter_1d_1d = new sppchip_interpolator_1d<double,double>;
        
        array yyy;
        yyy.resize(y.components());
        valarray<double> YYY;
        YYY.resize(yyy.size());
        array line;
        
        while(getline(*file,line))
        {
            // skip empty or commented lines
            if(line.empty()) continue;
            if(line[0][0]=='#') continue;
            
            const double xvalue = x.eval(line).dbl();
            y.meval(line,yyy);
            
            if(multidim_y)
            {
                for(unsigned int i=0; i<yyy.size(); ++i) YYY[i] = yyy[i].dbl();
                if(inter_1d_md) inter_1d_md->add_point(xvalue,YYY);
            }
            else
            {
                if(inter_1d_1d) inter_1d_1d->add_point(xvalue,yyy[0].dbl());
            }
        }

        delete file;
/*
        if(inter_1d_1d) return *inter_1d_1d;
        if(inter_1d_md) return *inter_1d_md;
*/
        if(inter_1d_1d) return inter_1d_1d;
        if(inter_1d_md) return inter_1d_md;
        return unset;
    }

    function function::interpolate_linear(const plottable &g)
    {
        if(g.columns()<2)
        {
            warning::print("The dgraph must have at least 2 colums","function::interpolate_linear(const dgraph &)");
            return unset;
        }
        if(g.size()<2)
        {
            warning::print("The dgraph must have at least 2 points","function::interpolate_linear(const dgraph &)");
            return unset;
        }
        vector<double> x(g.size()),y(g.size());
        for(unsigned int i=0; i<g.size(); ++i)
        {
            x[i] = (*g.get(i))[0];
            y[i] = (*g.get(i))[1];
            //y[i] = g[i][1];
        }
        return new linear_interpolator_1d<double,double>(x,y);
    }
    function function::interpolate_linear(const plottable &g, const function &x, const function &y)
    {
        if(g.columns()<2)
        {
            warning::print("The dgraph must have at least 2 colums","function::interpolate_linear(const dgraph &, const function &x, const function &y)");
            return unset;
        }
        if(g.size()<2)
        {
            warning::print("The dgraph must have at least 2 points","function::interpolate_linear(const dgraph &, const function &x const function &y)");
            return unset;
        }
        bool multidim_x = (x.components()>1);
        bool multidim_y = (y.components()>1);

        linear_interpolator_md<double,double>                *inter_md_1d = 0;
        linear_interpolator_md<double,valarray<double> >     *inter_md_md = 0;
        linear_interpolator_1d<double,double>                *inter_1d_1d = 0;
        linear_interpolator_1d<double,valarray<double> >     *inter_1d_md = 0;

        if(multidim_x)
        {
            if(multidim_y) inter_md_md = new linear_interpolator_md<double,valarray<double> >(x.components());
            else           inter_md_1d = new linear_interpolator_md<double,double>           (x.components());
        }
        else
        {
            if(multidim_y) inter_1d_md = new linear_interpolator_1d<double,valarray<double> >;
            else           inter_1d_1d = new linear_interpolator_1d<double,double>;
        }

        array xxx;
        xxx.resize(x.components());
        array yyy;
        yyy.resize(y.components());

        // For a multidimensional interpolator in x, one needs to loop over all gridpoints to set up the grid
        // of the interpolator
        if(multidim_x)
        {
            vector<vector<double> > xvalues(x.components());

            for(unsigned int i=0; i<g.size(); ++i)
            {
                x.meval(*g.get(i),xxx);
                for(unsigned int i=0; i<xxx.size(); ++i)
                {
                    if(find(xvalues[i].begin(),xvalues[i].end(),xxx[i].dbl()) == xvalues[i].end()) xvalues[i].push_back(xxx[i].dbl());
                }
            }
            for(unsigned int i=0; i<xvalues.size(); ++i)
            {
                if(inter_md_1d) inter_md_1d->x(i+1,xvalues[i]);
                if(inter_md_md) inter_md_md->x(i+1,xvalues[i]);
            }

        }

        vector<double> XXX;
        XXX.resize(xxx.size());
        valarray<double> YYY;
        YYY.resize(yyy.size());

        for(unsigned int ig=0; ig<g.size(); ++ig)
        {
            x.meval(*g.get(ig),xxx);
            y.meval(*g.get(ig),yyy);
            if(multidim_y)
            {
                for(unsigned int i=0; i<yyy.size(); ++i) YYY[i] = yyy[i].dbl();
            }
            if(multidim_x)
            {
                for(unsigned int i=0; i<xxx.size(); ++i) XXX[i] = xxx[i].dbl();
                if(inter_md_1d) inter_md_1d->y(XXX,yyy[0].dbl());
                if(inter_md_md) inter_md_md->y(XXX,YYY);
            }
            else
            {
                if(inter_1d_1d) inter_1d_1d->add_point(xxx[0].dbl(),yyy[0].dbl());
                if(inter_1d_md) inter_1d_md->add_point(xxx[0].dbl(),YYY);
            }
        }
        if(inter_1d_1d) return inter_1d_1d;
        if(inter_1d_md) return inter_1d_md;
        if(inter_md_1d) return inter_md_1d;
        if(inter_md_md) return inter_md_md;
        warning::print("This should never happen","funtion::interpolate_linear(filename,x,y)");
        return unset;
    }
    function function::interpolate_linear(const array &x, const array &y)
    {
        return new linear_interpolator_1d<double,double>(x,y,::min(x.size(),y.size()));
    }
    function function::interpolate_linear(const std::vector<double> &x, const std::vector<double> &y)
    {
        return new linear_interpolator_1d<double,double>(x,y);
    }
    function function::interpolate_linear(const std::vector<double> &x, const std::vector<geom::vec3> &y)
    {
        return new linear_interpolator_1d<double,geom::vec3>(x,y);
    }
    function function::interpolate_linear(const std::initializer_list<double> &x, const std::initializer_list<double> &y)
    {
        return new linear_interpolator_1d<double,double>(x,y);
    }
    function function::interpolate_linear(const double x[], const double y[], int n)
    {
        return new linear_interpolator_1d<double,double>(x,y,n);
    }

    function function::interpolate_spline(const dgraph &g)
    {
        if(g.columns()<2)
        {
            warning::print("The dgraph must have at least 2 colums","function::interpolate_linear(const dgraph &)");
            return unset;
        }
        if(g.size()<2)
        {
            warning::print("The dgraph must have at least 2 points","function::interpolate_linear(const dgraph &)");
            return unset;
        }
        vector<double> x(g.size()),y(g.size());
        for(unsigned int i=0; i<g.size(); ++i)
        {
            x[i] = g[i][0];
            y[i] = g[i][1];
        }
        return new spline_interpolator_1d<double,double>(x,y);
    }
    function function::interpolate_spline(const array &x, const array &y)
    {
        return new spline_interpolator_1d<double,double>(x,y,::min(x.size(),y.size()));
    }
    function function::interpolate_spline(const std::vector<double> &x, const std::vector<double> &y)
    {
        return new spline_interpolator_1d<double,double>(x,y);
    }
    function function::interpolate_spline(const std::initializer_list<double> &x, const std::initializer_list<double> &y)
    {
        return new spline_interpolator_1d<double,double>(x,y);
    }
    function function::interpolate_spline(const std::vector<double> &x, const std::vector<geom::vec3> &y)
    {
        return new spline_interpolator_1d<double,geom::vec3>(x,y);
    }
    function function::interpolate_spline(const double x[], const double y[], int n)
    {
        return new spline_interpolator_1d<double,double>(x,y,n);
    }

    function function::interpolate_sppchip(const dgraph &g)
    {
        if(g.columns()<2)
        {
            warning::print("The dgraph must have at least 2 colums","function::interpolate_linear(const dgraph &)");
            return unset;
        }
        if(g.size()<2)
        {
            warning::print("The dgraph must have at least 2 points","function::interpolate_linear(const dgraph &)");
            return unset;
        }
        vector<double> x(g.size()),y(g.size());
        for(unsigned int i=0; i<g.size(); ++i)
        {
            x[i] = g[i][0];
            y[i] = g[i][1];
        }
        return new sppchip_interpolator_1d<double,double>(x,y);
    }
    function function::interpolate_sppchip(const array &x, const array &y)
    {
        return new sppchip_interpolator_1d<double,double>(x,y,::min(x.size(),y.size()));
    }
    function function::interpolate_sppchip(const std::vector<double> &x, const std::vector<double> &y)
    {
        return new sppchip_interpolator_1d<double,double>(x,y);
    }
    function function::interpolate_sppchip(const std::initializer_list<double> &x, const std::initializer_list<double> &y)
    {
        return new sppchip_interpolator_1d<double,double>(x,y);
    }
    function function::interpolate_sppchip(const std::vector<double> &x, const std::vector<geom::vec3> &y)
    {
        return new sppchip_interpolator_1d<double,geom::vec3>(x,y);
    }
    function function::interpolate_sppchip(const double x[], const double y[], int n)
    {
        return new sppchip_interpolator_1d<double,double>(x,y,n);
    }



#ifdef HAVE_GTS_H

    function function::interpolate_delaunay(const var &filename)
    {
        return interpolate_delaunay(filename,function(_1,_2),_3);
    }

    function function::interpolate_delaunay(const var &filename, const function &x, const function &y)
    {
        if(x.components() != 2)
        {
            warning::print("Delaunay-interpolation needs 2 x-values","function::interpolate_delaunay(filename,const function &x, const function &y)");
            return unset;
        }
        if(y.components() != 1)
        {
            warning::print("Delaunay-interpolation needs 1 y-value","function::interpolate_delaunay(filename,const function &x, const function &y)");
            return unset;
        }

        istream *file = openin(filename);
        if(!file) return unset;

        delaunay_interpolator *inter = new delaunay_interpolator;

        array line;
        array xx;
        xx.resize(x.components());
        array yy;
        yy.resize(y.components());
        while(getline(*file,line))
        {
            if(line.empty()) continue;
            if(line[0][0] == '#') continue;

            x.meval(line,xx);
            y.meval(line,yy);
            
            inter->add_point(xx[0].dbl(), xx[1].dbl(), yy[0].dbl());
        }

        return *inter;
    }

#endif


    /*
    function function::linear(const var &filename,
			      const function &f1,
			      const function &f2,
			      const function &f3,
			      const function &f4)
			      
    {
        warning::print("function::linear is deprecated, use function::interpolate_linear(...) instead, which can also do multidimensional interpolation");
	array line, result;

	function filter = unset;
	if(f1.initialized()) filter.append(f1);
	if(f2.initialized()) filter.append(f2);
	if(f3.initialized()) filter.append(f3);
	if(f4.initialized()) filter.append(f4);
	unsigned int N = 0;
	if(filter.initialized())
	{
	    N = filter.components();
	    result.resize(N);
	}

	polynomial_interpolator_1d<double,double>            *interp_double   = 0;
	polynomial_interpolator_1d<double,valarray<double> > *interp_valarray = 0;

	istream *file = openin(filename);

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

	    if(N>2) // valarray is needed
	    {
		if(interp_valarray == 0) interp_valarray = new polynomial_interpolator_1d<double,valarray<double> >;
		valarray<double> r(N-1);
		for(unsigned int i=1; i<result.size(); ++i) r[i-1] = result[i].dbl();
		interp_valarray->add_point(result[0].dbl(), r);
	    }
	    else
	    {
		if(interp_double   == 0) interp_double   = new polynomial_interpolator_1d<double,double>;
		interp_double->add_point(result[0].dbl(), result[1].dbl());
	    }
	}

	if(interp_double && interp_double->linear())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp_double);
	    return c;
	}
	if(interp_valarray && interp_valarray->linear())
	{
	    function_core::interpolated_func<valarray<double> > *c = new function_core::interpolated_func<valarray<double> >(interp_valarray);
	    return c;
	}
	delete interp_double;
	delete interp_valarray;
	return 0.0;
    }

    function function::linear(const array &x, const array &y)
    {
	unsigned int n = ::min(x.size(), y.size());
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>;
	for(unsigned int i=0; i<n; ++i) interp->add_point(x[i].dbl(), y[i].dbl());
	if(interp->linear())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::linear(const vector<double> &x, const vector<double> &y)
    {
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>(x,y);
	if(interp->linear())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::linear(const double x[], const double y[], int n)
    {
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>;
	for(int i=0; i<n; ++i) interp->add_point(x[i],y[i]);
	if(interp->linear())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
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
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>;

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

	    if(result.size()<2) warning::print("Less than 2 values for spline interpolation","function::spline(filename,f1,f2,...)");

	    interp->add_point((result.size()>0?result[0].dbl():0.0), (result.size()>1?result[1].dbl():0.0));
	}

	if(N>2)
	{
	    warning::print("Spline interpolation currently only works for (x,y) pairs (i.e. no more y values, valarray...). Request from the author!");
	}

	if(interp->spline())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::spline(const array &x, const array &y)
    {
	unsigned int n = ::min(x.size(), y.size());
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>;
	for(unsigned int i=0; i<n; ++i) interp->add_point(x[i].dbl(), y[i].dbl());
	if(interp->spline())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::spline(const vector<double> &x, const vector<double> &y)
    {
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>(x,y);
	if(interp->spline())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::spline(const double x[], const double y[], int n)
    {
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>;
	for(int i=0; i<n; ++i) interp->add_point(x[i],y[i]);
	if(interp->spline())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
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
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>;

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
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::sppchip(const array &x, const array &y)
    {
	unsigned int n = ::min(x.size(), y.size());
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>;
	for(unsigned int i=0; i<n; ++i) interp->add_point(x[i].dbl(), y[i].dbl());
	if(interp->sppchip())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::sppchip(const vector<double> &x, const vector<double> &y)
    {
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>(x,y);
	if(interp->sppchip())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }

    function function::sppchip(const double x[], const double y[], int n)
    {
	polynomial_interpolator_1d<double,double> *interp = new polynomial_interpolator_1d<double,double>;
	for(int i=0; i<n; ++i) interp->add_point(x[i],y[i]);
	if(interp->sppchip())
	{
	    function_core::interpolated_func<double> *c = new function_core::interpolated_func<double>(interp);
	    return c;
	}
	delete interp;
	return 0.0;
    }
    */




#ifdef __MAKECINT__
    function cfunc(void *p)             {  return function_core::cfunc(p); }
#endif
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
    function cfunc(void (*p)(double*,double*,double*), int nargs, int npars, int nout)
    {
        return function_core::cfunc(p,nargs, npars, nout);
    }

/*
    function cppfunc(void (*p)(double*,double*,double*), int nargs, int npars, int nout)
    {
        return function_core::cppfunc<void(double*,double*,double*)>(p,nargs, npars, nout);
    }
*/

    function     cppfunc   (const std::function<void(double*,double*,double*)> &func, int nargs, int npars, int nout)
    {
        return function_core::cppfunc<void(double*,double*,double*)>(func, nargs, npars, nout);
    }


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
	    new(&_0)  function(new function_core::named_param("linenumber"));
	    new(&_b)  function(new function_core::named_param("blocknumber"));
	    new(&_B)  function(new function_core::named_param("Blocknumber"));
	    new(&_g)  function(new function_core::named_param("groupingvalue"));
	}
    }

    double tan(double a) { return std::tan(a); }

    function function::find_root(const function &f, const function &x1, const function &x2, const function &epsilon)
    {
        return function_core::find_root(f.base_, x1.base_, x2.base_, epsilon.base_);
    }

    double find_root(const function &f, double x1, double x2, double epsilon)
    {
        if(x2<x1) swap(x1,x2);
        double f1 = f(x1).dbl();
        double f2 = f(x2).dbl();
        if(epsilon<0) epsilon = (x2-x1)/1000000;
        if(f1*f2 >= 0)
        {
            warning::print(var("f(") & x1 & ")=" & f(x1) & " and f(" & x2 & ")=" & f(x2) & " should have opposite signs",
                               "find_root(const function &, double x1, double x2)");
            return x1;
        }

        while(::fabs(x2-x1)>epsilon)
        {
            const double xx = 0.5*(x1+x2);
            const double ff = f(xx).dbl();
            if(ff == 0.0) return xx;
            if(f1*ff<0)
            {
                x2 = xx;
                f2 = ff;
            }
            else if(f2*ff<0)
            {
                x1 = xx;
                f1 = ff;
            }
            else
            {
                warning::print("This should not happen","find_root(const function &, double x1, double x2)");
                return 0.5*(x1+x2);
            }
        }
        return 0.5*(x1+x2);
    }

    double find_root(const std::vector<double> &X, const std::vector<double> &Y, const function &F, double epsilon)
    {
        if(X.size()<=1) return unset;
        function ff = function::interpolate_linear(X,Y)-F;
        return blop::find_root(ff, X.front(), X.back(), epsilon);
    }

    double find_root(const dgraph &g, double yvalue)
    {
        const unsigned int N = g.size();
        for(unsigned int i=1; i<N; ++i)
        {
            const double x1 = g[i-1][0];
            const double y1 = g[i-1][1];
            const double x2 = g[i][0];
            const double y2 = g[i][1];
            if((y2-yvalue)*(y1-yvalue)<=0.0)
            {
                const double a = (y2-y1)/(x2-x1);
                const double b = (x2*y1-x1*y2)/(x2-x1);
                return (yvalue-b)/a;
            }
        }
        return unset;
    }

    unsigned int find_roots(const dgraph &g, std::vector<double> &roots, double yvalue)
    {
        roots.clear();
        const unsigned int N = g.size();
        for(unsigned int i=1; i<N; ++i)
        {
            const double x1 = g[i-1][0];
            const double y1 = g[i-1][1];
            const double x2 = g[i][0];
            const double y2 = g[i][1];
            if((y2-yvalue)*(y1-yvalue)<=0.0)
            {
                const double a = (y2-y1)/(x2-x1);
                const double b = (x2*y1-x1*y2)/(x2-x1);
                roots.push_back((yvalue-b)/a);
            }
        }
        return roots.size();
    }


    double find_root(const std::vector<double> &x, const std::vector<double> &y, double yvalue)
    {
        const unsigned int N = ::min(x.size(),y.size());
        for(unsigned int i=1; i<N; ++i)
        {
            const double x1 = x[i-1];
            const double y1 = y[i-1];
            const double x2 = x[i];
            const double y2 = y[i];
            if((y2-yvalue)*(y1-yvalue)<=0.0)
            {
                const double a = (y2-y1)/(x2-x1);
                const double b = (x2*y1-x1*y2)/(x2-x1);
                return (yvalue-b)/a;
            }
        }
        return unset;
    }

    blop::function implicit_solve(const function &f, double x1, double x2, double y1, double y2,int n1, int n2)
    {
        vector<double> x,y;
        isoline(f,0.0,x,y,x1,x2,y1,y2,n1,n2);
        parallel_sort(x,y);
        for(unsigned int i=1; i<x.size();)
        {
            if(std::abs(x[i]-x[i-1])<(x2-x1)/n1/10)
            {
                x.erase(x.begin()+i);
                y.erase(y.begin()+i);
            }

            else ++i;
        }
        if(x.size()<=1) return unset;
        return function::interpolate_linear(x,y);
    }

    blop::function implicit_solve(const dgraph &g, double zcut)
    {
        std::vector<double> x,y;
        isoline(g,zcut,x,y);
        parallel_sort(x,y);
        if(x.size() <= 1) return unset;
        const double D = (x.back() - x.front())/1000;
        for(unsigned int i=1; i<x.size();)
        {
            if(std::abs(x[i]-x[i-1])<D)
            {
                x.erase(x.begin()+i);
                y.erase(y.begin()+i);
            }

            else ++i;
        }
        if(x.size()<=1) return unset;
        return function::interpolate_linear(x,y);
    }

    blop::function replace(const blop::function &from, const blop::function &to, const blop::function &in)
    {
        return new blop::function_core::replace(from.base_,to.base_,in.base_);
    }

    blop::function substr(const blop::function &s, const blop::function &from, const blop::function &to)
    {
        return new blop::function_core::substr(s.base_,from.base_,to.base_);
    }

    var polylegend(const function &f, int n, const var &independent_variable)
    {
        var result = f.param(1);
        for(int i=1; i<=n; ++i)
        {
            if(f.param(i+1)>=0) result &= "+";
            result &= f.param(i+1) & (i>1?var("$\\cdot ") & independent_variable & "^{" & i & "}$":var("$\\cdot ") & independent_variable & "$");
        }
        return result;
    }

    function date2epoch(const var &format)
    {
        return new function_core::date2epoch(format);
    }

    function cross_product(const function &x1, const function &x2)
    {
        if(x1.components() != 3 || x2.components() != 3) return function(unset,unset,unset);
        return function(
            x1[1]*x2[2]-x1[2]*x2[1],
            x1[2]*x2[0]-x1[0]*x2[2],
            x1[0]*x2[1]-x1[1]*x2[0]
            );
    }

    function dot(const function &x1, const function &x2)
    {
        if(x1.components() != 3 || x2.components() != 3) return unset;
        return x1[0]*x2[0]+x1[1]*x2[1]+x1[2]*x2[2];
    }

    // Normalize the vector-function to the given norm. v must have 3 components
    function normalize(const function &v, double norm)
    {
        if(v.components() != 3) return unset;
        return function (
            v[0]/sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]),
            v[1]/sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]),
            v[2]/sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])
            );
    }
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


