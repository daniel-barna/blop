#ifndef __BLOP_CFUNC_WRAPPER_HH__
#define __BLOP_CFUNC_WRAPPER_HH__

#ifndef __MAKECINT__

#include "config.h"
#include "var.h"
#ifdef BLOP_ENABLE_CINT
#ifdef CINT_INCLUDE_PREFIX
#include "cint/Api.h"
#else
#include "Api.h"
#endif
#endif
#include <iostream>
#include <string>
#include <complex>
//#include "global.h"

namespace blop
{
    extern var    temporary_result_var;
    extern std::complex<double> temporary_result_complex;

    class cfunc_wrapper_base
    {
    public:
	int  functype_;
	void *ptr_;
	std::string fname_, arglist_;

    public:
	enum {compiled=1, interpreted=2};

	cfunc_wrapper_base()
	    {
		functype_ = 0;
		ptr_ = 0;
		fname_ = "UNINITIALIZED";
		arglist_ = "";
	    }
	cfunc_wrapper_base(const cfunc_wrapper_base &o)
	    {
		functype_ = o.functype_;
		ptr_ = o.ptr_;
		fname_ = o.fname_;
		arglist_ = o.arglist_;
	    }
	void operator=(const cfunc_wrapper_base &o)
	    {
		functype_ = o.functype_;
		ptr_ = o.ptr_;
		fname_ = o.fname_;
		arglist_ = o.arglist_;
	    }

	const std::string name() const { return fname_; }
	const std::string arglist() const { return arglist_; }

#ifdef BLOP_ENABLE_CINT
	static void find(void *pointer,
			 std::vector<G__MethodInfo> &results,
			 std::string &function_name_global,
			 const std::string &arglist_spec);
#endif
	static cfunc_wrapper_base *find(void *pointer, const std::string &arglist = "");


	// --------------- Number of input/output variables -------------------------
	// The following member functions should be overwritten in derived classes,
	// if it needs to be used as a plottable function, or in a 'function' or 'mfunction'

	virtual int   n_args() const { return 0; }
	virtual int   n_pars() const { return 0; }
	virtual int   n_out() const { return 0; }


	// ------------- Evaluate the C-function ------------------------------------
	// The lengths of the 'in' and 'out' arrays should be >= n_args() and n_out(),
	// respectively. This is the responsibility of the calling process to
	// size these arrays accordingly

	virtual void  eval    (const std::vector<blop::var> &args,
			       const std::vector<blop::var> &pars,
			       std::vector<blop::var>::iterator &out) const {}
	virtual void  eval_dbl(const std::vector<blop::var> &args,
			       const std::vector<blop::var> &pars,
			       std::vector<blop::var>::iterator &out) const {}
	virtual cfunc_wrapper_base *clone() const { return 0; }

	virtual ~cfunc_wrapper_base() {}
    };

    template <class R,class F>
    class cfunc_wrapper : public cfunc_wrapper_base
    {
    public:

	// --------  Typedefs  ---------------------------------------------

	typedef R return_type;  
	typedef F function_type;


	// ---------  Check validity ---------------------------------------
	// Check wether the initialization succeeded (a function exists with
	// the given argument list, etc)

	bool valid() const { return (ptr_ != 0); }


	//==================================================================
	// a variable which will hold the return value after the
	// evaluation of the function (do not use the result_ variable, but the
	// result() member function instead!!

	mutable return_type result_;
	const return_type &result() const { return result_; }

	void init(F funcpointer = 0)
	    {
		ptr_ = (void*)funcpointer;
		functype_ = compiled;
		fname_ = "COMPILEDFUNCTION";
		if(ptr_ == 0) fname_ = "UNINITIALIZED";
		arglist_ = "no_arglist_for_compiled_function";
	    }
	void init(void *pointer, const std::string &arglist)
	    {
		cfunc_wrapper_base *f = find(pointer,arglist);
		if(f)
		{
		    ptr_ = f->ptr_;
		    functype_ = f->functype_;
		    fname_ = f->fname_;
		    arglist_ = f->arglist_;
		    delete f;
		}
		else
		{
		    ptr_ = 0;
		    functype_ = 0;
		    fname_ = "UNKNOWN FUNCTION";
		    arglist_ = "";
		}
	    }
    };

    class double_func_aref_aref_aref_aref_aref
	: public cfunc_wrapper<double, double (*)(const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &)>
    {
    public:
	double_func_aref_aref_aref_aref_aref (void *p)
	    {init(p,
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&"); }

	double_func_aref_aref_aref_aref_aref( double (*p)( const std::vector<blop::var> &,
							   const std::vector<blop::var> &,
							   const std::vector<blop::var> &,
							   const std::vector<blop::var> &,
							   const std::vector<blop::var> &)=0 )
	    {init(p);}
	double operator()(const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &);
    };

    class double_func_aref_aref_aref_aref_aref_aref
	: public cfunc_wrapper<double, double (*)(const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &)>
    {
    public:
	double_func_aref_aref_aref_aref_aref_aref (void *p)
	    {init(p,
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&"); }

	double_func_aref_aref_aref_aref_aref_aref( double (*p)( const std::vector<blop::var> &,
								const std::vector<blop::var> &,
								const std::vector<blop::var> &,
								const std::vector<blop::var> &,
								const std::vector<blop::var> &,
								const std::vector<blop::var> &)=0 )
	    {init(p);}
	double operator()(const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &);
    };

    class double_func_aref_aref_aref_aref_aref_aref_aref
	: public cfunc_wrapper<double, double (*)(const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &,
						  const std::vector<blop::var> &)>
    {
    public:
	double_func_aref_aref_aref_aref_aref_aref_aref (void *p)
	    {init(p,
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&"); }

	double_func_aref_aref_aref_aref_aref_aref_aref( double (*p)( const std::vector<blop::var> &,
								     const std::vector<blop::var> &,
								     const std::vector<blop::var> &,
								     const std::vector<blop::var> &,
								     const std::vector<blop::var> &,
								     const std::vector<blop::var> &,
								     const std::vector<blop::var> &)=0 )
	    {init(p);}
	double operator()(const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &,
			  const std::vector<blop::var> &);
    };

    class int_func_double_double_aref_aref_int
	: public cfunc_wrapper<int, int (*)(double,
					       double,
					       const std::vector<blop::var> &,
					       const std::vector<blop::var> &,
					       int)>
    {
    public:
	int_func_double_double_aref_aref_int(void *p)
	    {init(p,
		  "double,double,"
		  "const std::vector<blop::var>&,"
		  "const std::vector<blop::var>&,"
		  "int"); }

	int_func_double_double_aref_aref_int( int (*p)( double, double, 
							const std::vector<blop::var> &,
							const std::vector<blop::var> &,
							int ) )
	    {init(p);}
	int operator()(double,double,
		       const std::vector<blop::var> &,
		       const std::vector<blop::var> &,
		       int) const;
    };



    class complex_func_double : public cfunc_wrapper<complex<double>, complex<double> (*)(double)>
    {
    public:
	complex_func_double(void *p) {init(p,"double");}
	complex_func_double(complex<double> (*p)(double)=0)
	    { init(p); }
	complex<double> operator() (double) const;

	cfunc_wrapper_base *clone() const { return new complex_func_double(*this); }
	int n_out() const { return 2; }
	int n_args () const { return 1; }
	void eval(const std::vector<blop::var> &in,
		  const std::vector<blop::var> &, //pars
		  std::vector<blop::var>::iterator &out) const
	    {
		operator()(in[0].dbl());
		(*(out++)) = result().real();
		(*(out++)) = result().imag();
	    }
	void eval_dbl(const std::vector<blop::var> &in,
		      const std::vector<blop::var> &, //pars
		      std::vector<blop::var>::iterator &out) const
	    {
		operator()(in[0].dbl());
		(*(out++)).dbl(result().real());
		(*(out++)).dbl(result().imag());
	    }
    };

    class complex_func_double_double : public cfunc_wrapper<complex<double>, complex<double> (*)(double,double)>
    {
    public:
	complex_func_double_double(void *p) {init(p,"double");}
	complex_func_double_double(complex<double> (*p)(double,double)=0)
	    { init(p); }
	complex<double> operator() (double,double) const;

	cfunc_wrapper_base *clone() const { return new complex_func_double_double(*this); }
	int n_out() const { return 2; }
	int n_args () const { return 2; }
	void eval(const std::vector<blop::var> &in,
		  const std::vector<blop::var> &, //pars
		  std::vector<blop::var>::iterator &out) const
	    {
		operator()(in[0].dbl(),in[1].dbl());
		(*(out++)) = result().real();
		(*(out++)) = result().imag();
	    }
	void eval_dbl(const std::vector<blop::var> &in,
		      const std::vector<blop::var> &, //pars
		      std::vector<blop::var>::iterator &out) const
	    {
		operator()(in[0].dbl(),in[1].dbl());
		(*(out++)).dbl(result().real());
		(*(out++)).dbl(result().imag());
	    }
    };

    class double_func_double : public cfunc_wrapper<double,double (*)(double)>
    {
    public:
	double_func_double(void *p) {init(p,"double"); }
	double_func_double(double (*p)(double)=0)
	    {init(p);}
	double operator() (double v) const;
	cfunc_wrapper_base *clone() const { return new double_func_double(*this); }
	int n_out() const { return 1; }
	int n_args () const { return 1; }
	void eval    (const std::vector<blop::var> &args,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)     const
	    {*(out++) = operator()(args[0].dbl());}
	void eval_dbl(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out) const
	    {(*(out++)).dbl(operator()(args[0].dbl()));}
    };

    class double_func_double_double : public cfunc_wrapper<double,double (*)(double,double)>
    {
    public:
	double_func_double_double(void *p) {init(p,"double,double"); }
	double_func_double_double(double (*p)(double,double)=0)
	    {init(p);}
	double operator() (double v1,double v2) const;
	cfunc_wrapper_base *clone() const { return new double_func_double_double(*this); }
	int n_out() const { return 1; }
	int n_args () const { return 2; }
	void eval    (const std::vector<blop::var> &args,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)     const
	    {*(out++) = operator()(args[0].dbl(),args[1].dbl());}
	void eval_dbl(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)     const
	    {(*(out++)).dbl(operator()(args[0].dbl(),args[1].dbl()));}
    };

    class double_func_double_double_double : public cfunc_wrapper<double,double (*)(double,double,double)>
    {
    public:
	double_func_double_double_double(void *p) {init(p,"double,double,double"); }
	double_func_double_double_double(double (*p)(double,double,double)=0)
	    {init(p);}
	double operator() (double v1,double v2,double v3) const;
	cfunc_wrapper_base *clone() const { return new double_func_double_double_double(*this); }
	int n_out() const { return 1; }
	int n_args () const { return 3; }
	void eval    (const std::vector<blop::var> &args,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)    const
	    { *(out++) = operator()(args[0].dbl(),args[1].dbl(),args[2].dbl()); }
	void eval_dbl(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)    const
	    { (*(out++)).dbl(operator()(args[0].dbl(),args[1].dbl(),args[2].dbl())); }
    };

    class double_func_double_double_double_double : public cfunc_wrapper<double,double (*)(double,double,double,double)>
    {
    public:
	double_func_double_double_double_double(void *p) {init(p,"double,double,double,double"); }
	double_func_double_double_double_double(double (*p)(double,double,double,double)=0)
	    {init(p);}
	double operator() (double v1,double v2,double v3,double v4) const;
	cfunc_wrapper_base *clone() const { return new double_func_double_double_double_double(*this); }
	int n_out() const { return 1; }
	int n_args () const { return 4; }
	void eval    (const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out) const
	    { *(out++) = operator()(in[0].dbl(),in[1].dbl(),in[2].dbl(),in[3].dbl()); }
	void eval_dbl(const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out) const
	    { (*(out++)).dbl(operator()(in[0].dbl(),in[1].dbl(),in[2].dbl(),in[3].dbl())); }
    };


    class var_func_varvector_varvector
	: public cfunc_wrapper<var, var (*)(const std::vector<blop::var>&,
					    const std::vector<blop::var>&)>
    {
    private:
	int nargs_, npars_;
    public:
	var_func_varvector_varvector(void *p, int nargs, int npars)
	    : nargs_(nargs), npars_(npars)
	    {init(p,"const std::vector<blop::var>&,const std::vector<blop::var>&");}
	var_func_varvector_varvector
	    (var (*p)(const std::vector<blop::var>&,const std::vector<blop::var>&)=0,
	     int nargs=0, int npars=0)
	    : nargs_(nargs), npars_(npars) {init(p);}

	var operator()(const std::vector<blop::var> &, const std::vector<blop::var> &) const;

	cfunc_wrapper_base *clone() const { return new var_func_varvector_varvector(*this); }
	int n_out() const { return 1; }
	int n_args()  const { return nargs_; }
	int n_pars()  const { return npars_; }
	void eval (const std::vector<blop::var> &args,
		   const std::vector<blop::var> &pars,
		   std::vector<blop::var>::iterator &out) const
	    {
		*(out++) = operator()(args,pars); 
	    }
	void eval_dbl(const std::vector<blop::var> &args,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)     const
	    {(*(out++)).dbl(operator()(args,pars).dbl());}
	
    };


    class var_func_var : public cfunc_wrapper<var,var (*)(var)>
    {
    public:
	var_func_var(void *p) {init(p,"var"); }
	var_func_var(var (*p)(var)=0)
	    {init(p);}
	var operator() (var v) const;
	cfunc_wrapper_base *clone() const { return new var_func_var(*this); }
	int n_out() const { return 1; }
	int n_args () const { return 1; }
	void eval    (const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)     const
	    {*(out++) = operator()(in[0]);}
	void eval_dbl(const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)     const
	    {(*(out++)).dbl(operator()(in[0]).dbl());}
    };

    class var_func_var_var : public cfunc_wrapper<var,var (*)(var,var)>
    {
    public:
	var_func_var_var(void *p) {init(p,"var,var"); }
	var_func_var_var(var (*p)(var,var)=0)
	    {init(p);}
	var operator() (var v1,var v2) const;
	cfunc_wrapper_base *clone() const { return new var_func_var_var(*this); }
	int n_out() const { return 1; }
	int n_args () const { return 2; }
	void eval    (const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)     const
	    {*(out++) = operator()(in[0],in[1]);}
	void eval_dbl(const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)     const
	    {(*(out++)).dbl(operator()(in[0],in[1]).dbl());}
    };

    class var_func_var_var_var : public cfunc_wrapper<var,var (*)(var,var,var)>
    {
    public:
	var_func_var_var_var(void *p) {init(p,"var,var,var"); }
	var_func_var_var_var(function_type p=0)	    {init(p);}
	var operator() (var v1,var v2,var v3) const;
	cfunc_wrapper_base *clone() const { return new var_func_var_var_var(*this); }
	int n_out() const { return 1; }
	int n_args () const { return 3; }
	void eval    (const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)    const
	    { *(out++) = operator()(in[0],in[1],in[2]); }
	void eval_dbl(const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)    const
	    { (*(out++)).dbl(operator()(in[0],in[1],in[2]).dbl()); }
    };

    class var_func_var_var_var_var : public cfunc_wrapper<var,var (*)(var,var,var,var)>
    {
    public:
	var_func_var_var_var_var(void *p) {init(p,"var,var,var,var"); }
	var_func_var_var_var_var(function_type p = 0) 	    {init(p);}
	var operator() (var v1,var v2,var v3,var v4) const;
	cfunc_wrapper_base *clone() const { return new var_func_var_var_var_var(*this); }
	int n_out() const { return 1; }
	int n_args () const { return 4; }
	void eval    (const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)    const
	    { *(out++) = operator()(in[0],in[1],in[2],in[3]); }
	void eval_dbl(const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)    const
	    { (*(out++)).dbl(operator()(in[0],in[1],in[2],in[3]).dbl()); }
    };

    class void_func_doublep_doublep_doublep : public cfunc_wrapper<int,void (*)(double*, double*, double*)>
    {
    private:
	int n_args_, n_pars_, n_out_;
	double 
            *in_dbl_,    // temporary array for the input arguments
            *pars_dbl_,   // temporary array for the input parameters
            *out_dbl_;   // temporary array for othe output results
    public:
        void_func_doublep_doublep_doublep(const void_func_doublep_doublep_doublep &rhs)
            {
                init((function_type)rhs.ptr_);
                n_args_ = rhs.n_args_;
                n_pars_ = rhs.n_pars_;
                n_out_ = rhs.n_out_;
		in_dbl_ = new double[n_args_];
                pars_dbl_ = new double[n_pars_]; 
		out_dbl_ = new double[n_out_];
            }
#ifdef __MAKECINT__
	void_func_doublep_doublep_doublep(void *p,int n_in, int n_pars, int n_out)
	    {
		init(p,"double*,double*,double*");
		n_args_ = n_in;
                n_pars_ = n_pars;
		n_out_ = n_out;
		in_dbl_ = new double[n_args_];
                pars_dbl_ = new double[n_pars_]; 
		out_dbl_ = new double[n_out_];
	    }
#endif
	void_func_doublep_doublep_doublep(function_type p, int nin, int n_pars, int nout)
	    {
		init(p);
		n_args_ = nin;
                n_pars_ = n_pars;
		n_out_ = nout;
		in_dbl_ = new double[n_args_];
                pars_dbl_ = new double[n_pars_];
		out_dbl_ = new double[n_out_];
	    }
	void_func_doublep_doublep_doublep()
	    {
		init(0);
		n_args_ = n_pars_ = n_out_ = 0;
		in_dbl_ = pars_dbl_ = out_dbl_ = 0;
	    }
	~void_func_doublep_doublep_doublep()
	    {
		delete [] in_dbl_;
                delete [] pars_dbl_;
		delete [] out_dbl_;
	    }
	void operator() (double *args,double *pars,double *out) const;
	cfunc_wrapper_base *clone() const { return new void_func_doublep_doublep_doublep(*this); }
	int n_out() const { return n_out_; }
	int n_args () const { return n_args_ ; }
        int n_pars() const { return n_pars_; }
	void eval(const std::vector<blop::var> &in,
		  const std::vector<blop::var> &pars,
		  std::vector<blop::var>::iterator &out)    const
	    {
		for(int i=0; i<n_args_; ++i) in_dbl_[i] = in[i].dbl();
                for(int i=0; i<n_pars_; ++i) pars_dbl_[i] = pars[i].dbl();
		operator()(in_dbl_,pars_dbl_,out_dbl_);
		for(int i=0; i<n_out_; ++i) *(out++) = out_dbl_[i];
	    }
	void eval_dbl(const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)    const
	    {
		for(int i=0; i<n_args_; ++i) in_dbl_[i] = in[i].dbl();
                for(int i=0; i<n_pars_; ++i) pars_dbl_[i] = pars[i].dbl();
		operator()(in_dbl_,pars_dbl_,out_dbl_);
		for(int i=0; i<n_out_; ++i) (*(out++)).dbl(out_dbl_[i]);
	    }
    };


    class void_func_doublep_doublep : public cfunc_wrapper<int,void (*)(double *, double *)>
    {
    private:
	int n_args_, n_out_;
	double *in_dbl_, *out_dbl_;
    public:
	void_func_doublep_doublep(void *p,int nin, int nout)
	    {
		init(p,"double*,double*");
		n_args_ = nin;
		n_out_ = nout;
		in_dbl_ = new double[n_args_];
		out_dbl_ = new double[n_out_];
	    }
	void_func_doublep_doublep(function_type p, int nin, int nout)
	    {
		init(p);
		n_args_ = nin;
		n_out_ = nout;
		in_dbl_ = new double[n_args_];
		out_dbl_ = new double[n_out_];
	    }
	void_func_doublep_doublep()
	    {
		init(0);
		n_args_ = n_out_ = 0;
		in_dbl_ = out_dbl_ = 0;
	    }
	~void_func_doublep_doublep()
	    {
		delete [] in_dbl_;
		delete [] out_dbl_;
	    }
	void operator() (double *in,double *out) const;
	cfunc_wrapper_base *clone() const { return new void_func_doublep_doublep(*this); }
	int n_out() const { return n_out_; }
	int n_args () const { return n_args_ ; }
	void eval(const std::vector<blop::var> &in,
		  const std::vector<blop::var> &pars,
		  std::vector<blop::var>::iterator &out)    const
	    {
		for(int i=0; i<n_args_; ++i) in_dbl_[i] = in[i].dbl();
		operator()(in_dbl_,out_dbl_);
		for(int i=0; i<n_out_; ++i) *(out++) = out_dbl_[i];
	    }
	void eval_dbl(const std::vector<blop::var> &in,
		      const std::vector<blop::var> &pars,
		      std::vector<blop::var>::iterator &out)    const
	    {
		for(int i=0; i<n_args_; ++i) in_dbl_[i] = in[i].dbl();
		operator()(in_dbl_,out_dbl_);
		for(int i=0; i<n_out_; ++i) (*(out++)).dbl(out_dbl_[i]);
	    }
    };

}

#endif
#endif
