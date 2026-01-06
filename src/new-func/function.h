    //                   - func_type derivative(...)
#ifndef __BLOP_FUNCTION_H__
#define __BLOP_FUNCTION_H__

#include <vector>
#include "var.h"
#include <map>
#include "array.h"
#include "constants.h"
#include "interpolate.h"
#ifndef __MAKECINT__
#include <complex>
#endif
#include "config.h"

namespace blop
{
    // forward declaration 
    namespace function_core
    {
	class col;
	class bessel;
    }

    class fgraph;

    // forward declaration of the function_core::base class
    namespace function_core { class base; }

    class dgraph;

class function
{
 private:
    //int nargs_, npars_;

    // cint takes these friend-declarations as function declarations,
    // causing troubles. 
#ifndef __MAKECINT__
    friend function     operator==(const function &left, const function &right);
    friend function     operator==(const function &left, const std::string &right);
    friend function     operator==(const std::string &left, const function &right);
    friend function     operator==(const function &left, const var      &right);
    friend function     operator==(const var      &left, const function &right);
    friend function     operator==(const function &left, const char *right);
    friend function     operator==(const char *left, const function &right);
    friend function     operator==(double left, const function &right);
    friend function     operator==(const function &left, double right);
    friend function     operator==(int left, const function &right);
    friend function     operator==(const function &left, int right);

    friend function     operator!=(const function &left, const function &right);
    friend function     operator< (const function &left, const function &right);
    friend function     operator<=(const function &left, const function &right);
    friend function     operator> (const function &left, const function &right);
    friend function     operator>=(const function &left, const function &right);

    friend function     max       (const function &left, const function &right);
    friend function     min       (const function &left, const function &right);
    friend function     maximize(const function &, const function &,
				 const function &, const function &);
    friend function     minimize(const function &, const function &,
				 const function &, const function &);
    friend function     integrate      (const function &, const function &,
					const function &, const function &);


    friend function     operator+ (const function &left, const function &right);
    friend function     operator- (const function &left, const function &right);
    friend function     operator* (const function &left, const function &right);
    friend function     operator/ (const function &left, const function &right);
    friend function     operator- (const function &operand);

    friend function     operator%  (const function &left, const function &right);
//    friend function     operator%  (const function &left, int right);
//    friend function     operator%  (int left, const function &right);

    friend function     operator&  (const function &left, const function &right);
    friend function     operator&& (const function &left, const function &right);
    friend function     operator|| (const function &left, const function &right);
    friend function     random    ();
    friend function     random    (double);
    friend function     random    (double,double);
    friend function     random_gauss(double a, double s, int parameter_index);
    friend function     random_exponential(double a, int parameter_index);
    friend function     random_idexponential(double a, int parameter_index);
    friend function     exp       (const function &o);
    friend function     log       (const function &o);
    friend function     log10     (const function &o);
    friend function     sqrt      (const function &o);
    friend function     sin       (const function &o);
    friend function     cos       (const function &o);
    friend function     tan       (const function &o);
    friend function     atan      (const function &o);
    friend function     cot       (const function &o);
    friend function     acot      (const function &o);
    friend function     sinh      (const function &o);
    friend function     cosh      (const function &o);
    friend function     tanh      (const function &);
    friend function     atanh     (const function &);
    friend function     asinh     (const function &);
    friend function     acosh     (const function &);
    friend function     floor     (const function &);
    friend function     ceil      (const function &);
    friend function     erf       (const function &);
    friend function     atan2     (const function &, const function &);
    friend function     pow       (const function &, const function &);
    friend function     pow       (const function &, int);
    friend function     pow       (const function &, double);
    friend function     pow       (const function &, const var &);
    friend function     sign      (const function &);
    friend function     abs       (const function &);
    friend function     fabs      (const function &);
#ifdef HAVE_GSL
    friend function     bessel_J(int n, const function &x);
    friend function     bessel_Y(int n, const function &x);
    friend function     bessel_I(int n, const function &x);
    friend function     bessel_K(int n, const function &x);
    friend function     bessel_j(int n, const function &x);
    friend function     bessel_y(int n, const function &x);
    friend function     bessel_i_scaled(int n, const function &x);
    friend function     bessel_k_scaled(int n, const function &x);
    friend class        blop::function_core::bessel;
#endif
    friend function     ARG       (int i);
    friend function     PAR       (int i);

    friend function     cfunc     (void *p);
    friend function     cfunc     (var (*p)(var));
    friend function     cfunc     (var (*p)(var,var));
    friend function     cfunc     (var (*p)(var,var,var));
    friend function     cfunc     (var (*p)(var,var,var,var));
    friend function     cfunc     (double (*p)(double));
    friend function     cfunc     (double (*p)(double,double));
    friend function     cfunc     (double (*p)(double,double,double));
    friend function     cfunc     (double (*p)(double,double,double,double));
    friend function     cfunc     (complex<double> (*p)(double));
    friend function     cfunc     (complex<double> (*p)(double,double));
    friend function     cfunc     (var (*p)(const std::vector<blop::var> &args,
					    const std::vector<blop::var> &pars),
				   int nargs,
				   int npars);

    friend function     charfunc(const function &,const function &, bool, bool);
    friend function     contained_in(const function &);

    friend class        blop::function_core::col;
    friend class        blop::fgraph;
#endif

    static std::vector<std::string> column_names_;
    std::vector<blop::var> default_args_;

    template <class T> void init_binary_(const function &left, const function &right);
    template <class T> void init_unary_ (const function &arg);

    std::vector<function_core::base*> base_;
    typedef std::map<int, std::vector<function_core::base*> > deriv_map;
    deriv_map derivatives_;
//    function_core::base  *clone_base() const;


    bool     print_param_value_;
    static bool default_print_param_value_;

    int components_;
    void calculate_components_();
    void init_();
    mutable std::vector<blop::var>    result_;
    std::vector<blop::var>    parameters_;
    std::vector<blop::var>    parnames_;
    std::map<int, blop::var>  parformats_;

    function(const std::vector<function_core::base*> &bases, bool clone_them=true);

    void copy_arg_ranges_(const function &f1,
			  const function &f2=blop::unset,
			  const function &f3=blop::unset,
			  const function &f4=blop::unset,
			  const function &f5=blop::unset);
    std::vector<double> arg_min_, arg_max_;
    bool make_arg_check_(const std::vector<blop::var> &args) const;
    bool check_args_;
    static bool default_check_args_;
 public:

    class global_initializer
    {
    public:
	global_initializer();
    };

    // ---------------   Constructors  ---------------------------------

    function();                    // the default constructor creates an uninitialized function

    // Initialization to a constant value. If the value (double) is unset, the function
    // is set to an uninitialized state
    function(double value);        
    function(double v1,double v2);  
    function(double v1,double v2,double v3);
    function(double v1,double v2,double v3,double v4);
    function(long double d1);   
    function(long double d1, long double d2);   
    function(long double d1, long double d2, long double d3);   
    function(long double d1, long double d2, long double d3, long double d4);   
    function(int i1);                 
    function(int i1,int i2);                 
    function(int i1,int i2,int i3);                 
    function(int i1,int i2,int i3,int i4);                 
    function(int i1, double d2);
    function(double d1, int i2);
    function(const var &);         
    function(const char *);        
    function(const string &);
    function(const std::vector<blop::var> &);   // multi-valued constant

    function(const function &);    // copy another function
    function(const function &f1, const function &f2);   // concatenate them
    function(const function &f1, const function &f2, const function &f3);
    function(const function &f1, const function &f2, const function &f3, const function &f4);
    function(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5);

    // Initialize from a C-function pointer
    function(void *fptr);

    // Initialize from an interpolator. Do not copy the interpolator
    // itself, just store a pointer
    function(interpolator *interp);


    ~function();

    // Do not call these constructors!!!
    function(const function_core::base &); // clone the given base
    function(      function_core::base *); // do not clone, only store pointer

    
    // ------------ Append more components to it ------------------------

    function &append(const function &);


    // ---- Get or set value of a given parameter  ----------------------

    const var &param(unsigned int parindex) const;             // read parameter
    function  &param(unsigned int parindex, const var &value); // set parameter
    function  &param(unsigned int parindex, double     value); // set parameter
    function  &param(unsigned int parindex, const var &value, const var &name); // set value+name
    function  &param(unsigned int parindex, double     value, const var &name); 
    function  &params(double p1,double p2,
		      double p3=unset,
		      double p4=unset,
		      double p5=unset,
		      double p6=unset);
    var        parname(unsigned int parindex);   // get parameter name
    function  &parname(unsigned int parindex, const var &parname); // set parameter name

    // set the format of a given parameter (for printing by value);
    // if parindex==0, the formats of all parameters are set
    function  &parformat(unsigned int parindex, const var &format);

    // -------  Specify default arguments -------------------------------

    function &def_arg(int arg_index, const var &value);
    var       def_arg(int arg_index) const;
    function &clear_def_args(); 

    // ---- Get the number of required arguments or parameters ----------

    int nargs()         const;
    //function &nargs(int i);

    int npars()         const;
    //function &npars(int i);

    // --- Query wether this function uses a given argument -------------

    bool uses_arg(int)  const;
    bool uses_par(int)  const;

    // ------- Get/set argument ranges ----------------------------------

    function &arg_min(int arg_no, double value);
    function &arg_max(int arg_no, double value);
    function &arg_range(int arg_no, double min, double max);
    double arg_min(int arg_no);
    double arg_max(int arg_no);
    function &check_args(bool flag);           // turn on/off checking if args are in range
    static void default_check_args(bool flag); // make it globally
                                               // (all future functions effected)
	
    // --------  Get maximum value of a given component in given range -----
    

    // --- Printing parameters by value ---------------------------------
    // set this property to true, if you want to substitute the actual value of the
    // parameters of the function, when the function's formula is printed (in 
    // legends, for example), instead of the formal [n] notation (where n is substituted
    // with the index of the parameter)

    static void default_print_param_value(bool i) { default_print_param_value_ = i; }
    function &print_param_value(bool i) { print_param_value_ = i; return *this; }
    bool print_param_value() const { return print_param_value_; }


    // --- Print the function's formula into a 'var' --------------------

    var sprint() const;
    var sprint_latex(const var &x="x", const var &y="y", const var &z="z") const;
    void print(std::ostream &out = cout) const;


    static function create_narg();


    // ------ Provide a formula -----------------------------------------
    // It makes only sense in a compiled code, to make an interactively
    // defineable function. I only works with CINT support of blop
    // (see  blop -h cint)
    // The argument string is passed to the CINT interpreter (it must
    // be a C++ code to build up a function). For example:
    // function f;
    // f.formula("_1*sin(_2) + PAR(1)");
    // the formula string is typically read from stdin, a config file, or
    // whatever
    // Before it is called, G__init_cint should be called, otherwise segfault!

    void formula(const var &f);

    // ------ Derivative ------------------------------------------------
    // Return the derivative function of this function with respect to 
    // the ith argument
    // if 'i' is negative, returns the derivative with respect to the ith parameter
    // The second version can be used to define this function's derivative
    // (if you do not trust blop's calculation, or if it can not calculate the
    // derivative itself, for example in case of a C-function)

    function  derivative(int i) const;                // returns the derivative
    function &derivative(int i, const function &);    // specify derivative explicitely, return *this

    const function &operator=(const function &);
    const function &operator=(const var      &);
    const function &operator=(double);
    const function &operator=(const char *);
    const function &operator=(const string &);

    const function &operator += (const function &);
    const function &operator -= (const function &);
    const function &operator *= (const function &);
    const function &operator /= (const function &);


    // ---- function evaluation operators () ----------------------------
    // You can use the parenthesis operators on a function (with 'var' or 'double' 
    // arguments) to evaluate the function at the provided arguments.
    // The parenthesis operators are defined with up to 6 arguments. If you want
    // to call a function with more arguments, you have to put them into a
    // vector<var>, and call 'eval' instead

    var operator()() const;
    var operator()(const var &) const;
    var operator()(const var &,const var &) const;
    var operator()(const var &,const var &,const var &) const;
    var operator()(const var &,const var &,const var &,const var &) const;
    var operator()(const var &,const var &,const var &,const var &,const var &) const;
    var operator()(const var &,const var &,const var &,const var &,const var &,const var &) const;

    var operator()(const char* v1) const                     { return operator()(var(v1)); }
    var operator()(const char* v1,const char* v2) const           { return operator()(var(v1),var(v2)); }
    var operator()(const char* v1,const char* v2,const char* v3) const { return operator()(var(v1),var(v2),var(v3)); }
    var operator()(const char* v1,const char* v2,const char* v3,const char* v4) const
	{ return operator()(var(v1),var(v2),var(v3),var(v4)); }
    var operator()(const char* v1,const char* v2,const char* v3,const char* v4,const char* v5) const
	{ return operator()(var(v1),var(v2),var(v3),var(v4),var(v5)); }
    var operator()(const char* v1,const char* v2,const char* v3,const char* v4,const char* v5,const char* v6) const
	{ return operator()(var(v1),var(v2),var(v3),var(v4),var(v5),var(v6)); }

    var operator()(double v1) const                     { return operator()(var(v1)); }
    var operator()(double v1,double v2) const           { return operator()(var(v1),var(v2)); }
    var operator()(double v1,double v2,double v3) const { return operator()(var(v1),var(v2),var(v3)); }
    var operator()(double v1,double v2,double v3,double v4) const
	{ return operator()(var(v1),var(v2),var(v3),var(v4)); }
    var operator()(double v1,double v2,double v3,double v4,double v5) const
	{ return operator()(var(v1),var(v2),var(v3),var(v4),var(v5)); }
    var operator()(double v1,double v2,double v3,double v4,double v5,double v6) const
	{ return operator()(var(v1),var(v2),var(v3),var(v4),var(v5),var(v6)); }

    var    eval(const std::vector<blop::var> &args) const;
    double eval_dbl(const std::vector<blop::var> &args) const;
    double eval_dbl(const std::vector<blop::var> &args, const std::vector<blop::var> &pars) const;

    void meval(const std::vector<blop::var>     &args, std::vector<blop::var> &result) const;
    void meval_dbl(const std::vector<blop::var> &args, std::vector<blop::var> &result) const;
    void meval_dbl(const std::vector<blop::var> &args, const std::vector<blop::var> &pars,
		   std::vector<blop::var> &result) const;

    // do the same as meval_dbl(....), for the levenberg_marquardt algorithm
    inline void operator()(const std::vector<blop::var> &args, const std::vector<blop::var> &pars,
		   std::vector<blop::var> &result) const
    {
	meval_dbl(args,pars,result);
    }


    // ------- Argument substitution via () operator -------------------------------
    // These parenthesis operators (with 'function' arguments) DO NOT EVALUATE
    // the function, but return another function instead. This new function
    // is a wrapper for the old one, when it is evaluated, it evaluates
    // the original function, with substituted arguments. For example
    // if f1 is a function, which can handle 2 arguments, and you type:
    // function f2 = f1( _1 + sin(_2), _3 );
    // then f2 will be a new function, which needs 3 arguments, and when it is
    // evaluated: f2(2,4,6), the return value will be the value of f1 with
    // arguments '2+sin(4)' and '6': f1(2+sin(4),6)

    function operator() (const function &) const;
    function operator() (const function &,const function &) const;
    function operator() (const function &,const function &,const function &) const;
    function operator() (const function &,const function &,const function &,const function &) const;
    function operator() (const function &,const function &,const function &,const function &,const function &) const;

    int components() const { return components_; }

    // return the given component (0-based)
    function operator[]  (unsigned int i) const;
    

    bool initialized() const;

    bool equals(const function &f) const;

    static var tmp;

    static void extra_param(const var &v);

    static void column_names(const std::vector<std::string> &v) { column_names_ = v; }



    // ---------------------  interpolated functions -------------------------------

    static function linear(const var &filename,
			   const function &f1=blop::unset,
			   const function &f2=blop::unset,
			   const function &f3=blop::unset,
			   const function &f4=blop::unset);
    static function linear(const array &x, const array &y);
    static function linear(const vector<double> &x, const vector<double> &y);
    static function linear(const double x[], const double y[], int n);

    static function spline(const var &filename,
			   const function &f1=blop::unset,
			   const function &f2=blop::unset,
			   const function &f3=blop::unset,
			   const function &f4=blop::unset);
    static function spline(const array &x, const array &y);
    static function spline(const vector<double> &x, const vector<double> &y);
    static function spline(const double x[], const double y[], int n);

    static function sppchip(const var &filename,
			   const function &f1=blop::unset,
			   const function &f2=blop::unset,
			   const function &f3=blop::unset,
			   const function &f4=blop::unset);
    static function sppchip(const array &x, const array &y);
    static function sppchip(const vector<double> &x, const vector<double> &y);
    static function sppchip(const double x[], const double y[], int n);


};

int sample(dgraph &g, double dx, double from, double to,
	   const function &f1,
	   const function &f2=unset,
	   const function &f3=unset,
	   const function &f4=unset);


typedef function argdef;

function     operator==(const function &left, const function &right);
function     operator==(const function &left, const std::string &right);
function     operator==(const std::string &left, const function &right);
function     operator==(const function &left, const var      &right);
function     operator==(const var      &left, const function &right);
function     operator==(const function &left, const char *right);
function     operator==(const char *left, const function &right);
function     operator==(double left, const function &right);
function     operator==(const function &left, double right);
function     operator==(int left, const function &right);
function     operator==(const function &left, int right);
function     operator!=(const function &left, const function &right);
function     operator< (const function &left, const function &right);
function     operator<=(const function &left, const function &right);
function     operator> (const function &left, const function &right);
function     operator>=(const function &left, const function &right);

function     max       (const function &left, const function &right);
function     min       (const function &left, const function &right);

function     maximize(const function &func,
		      const function &from,
		      const function &to,
		      const function &step);
function     minimize(const function &func,
		      const function &from,
		      const function &to,
		      const function &step);
function     integrate(const function &func,
		       const function &from,
		       const function &to,
		       const function &step);

function     operator+ (const function &left, const function &right);
function     operator- (const function &left, const function &right);
function     operator* (const function &left, const function &right);
function     operator/ (const function &left, const function &right);

function     operator- (const function &operand); 

function     random();
function     random(double to);
function     random(double from,double to);
function     random_gauss(double a, double s, int parindex=0); 
function     random_exponential(double a, int parindex=0);
function     random_idexponential(double a, int parindex=0);
function     exp  (const function &o);
function     log  (const function &o); 
function     log10(const function &o); 
function     sqrt (const function &o);
function     sin  (const function &);
function     cos  (const function &);
function     tan  (const function &);
function     atan (const function &);
function     cot  (const function &);
function     acot (const function &);
function     sinh (const function &);
function     cosh (const function &);
function     tanh (const function &);
function     atanh (const function &);
function     asinh (const function &);
function     acosh (const function &);
function     floor (const function &);
function     ceil  (const function &);
function     erf   (const function &);
function     atan2(const function &y, const function &x);
function     pow  (const function &a,const function &b);
function     pow  (const function &a,int exponent);
function     pow  (const function &a,double exponent);
function     pow  (const function &a,const var &exponent);
function     sign (const function &);
function     abs  (const function &);
function     fabs (const function &);

//html <a name="bessel">
#ifdef HAVE_GSL
double       bessel_J_zero(int n, int s);       // s-th root of Jn
double       bessel_Jprime_zero(int n, int s);  // s-th root of the derivative of Jn

function     bessel_J(int n, const function &x);
double       bessel_J(int n, double x);
function     bessel_Y(int n, const function &x);
double       bessel_Y(int n, double x);
function     bessel_I(int n, const function &x);
double       bessel_I(int n, double x);
function     bessel_K(int n, const function &x);
double       bessel_K(int n, double x);
function     bessel_j(int n, const function &x);
double       bessel_j(int n, double x);
function     bessel_y(int n, const function &x);
double       bessel_y(int n, double x);
function     bessel_i_scaled(int n, const function &x);
double       bessel_i_scaled(int n, double x);
function     bessel_k_scaled(int n, const function &x);
double       bessel_k_scaled(int n, double x);
#endif
//html </a>

function     charfunc(const function &low, const function &high, bool low_in=true, bool high_in=false);

function     operator &  (const function &left, const function &right);
function     operator && (const function &left, const function &right);
function     operator || (const function &left, const function &right);

function     operator%   (const function &left, const function &right);
//function     operator%   (const function &left, int right);
//function     operator%   (int left, const function &right);

// --- Functions returning simply their 1st, 2nd, etc. arguments  --------

extern function _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30;
extern function _0; 


// -----------------------------------------------------------------------
// The symbols _1 ... _20 are defined to access the 1st .. 20th
// arguments of a function (they are functions, which return the
// value of the 1st, .... etc argument).
// If you have more than 20 arguments, you can use ARG(i) instead
// with arbitrary 'i', where 'i' denotes the index of the argument  

function     ARG(int i);
function     PAR(int i);

// ------ Number of arguments --------------------------------------------
// A function, which returns the number of arguments with which it was called

extern function _N;

function     cfunc     (void *p);

// the following two functions need to be explicitely disabled in CINT
// because (although it most of the time calls  cfunc(void*) if called
// with a function pointer, sometimes it calls cfunc(var (*p)(var))
// which leads to problems (namely segfault) 
#ifndef __MAKECINT__
function     cfunc     (var (*p)(var));
function     cfunc     (var (*p)(var,var));
function     cfunc     (var (*p)(var,var,var));
function     cfunc     (var (*p)(var,var,var,var));
function     cfunc     (double (*p)(double));
function     cfunc     (double (*p)(double,double));
function     cfunc     (double (*p)(double,double,double));
function     cfunc     (double (*p)(double,double,double,double));
function     cfunc     (complex<double> (*p)(double));
function     cfunc     (var (*p)(const std::vector<blop::var> &args,
				 const std::vector<blop::var> &pars),
			int nargs,
			int npars);
#endif


std::ostream & operator<< (std::ostream &out, const blop::function &f);

// ------------- contained_in ---------------------------------------------
// The created function will return 1 if its first argument is contained
// in the return-value of fff (which might be a multi-valued result)
// 0 otherwise
function     contained_in(const function &ff);



double tan(double);
}

namespace 
{
    blop::function::global_initializer blop_function_global_initializer_instance;
}


#ifdef __CINT__
blop::function     operator==(const blop::function &left, const blop::function &right);
blop::function     operator==(const blop::function &left, const std::string &right);
blop::function     operator==(const std::string &left, const blop::function &right);
blop::function     operator==(const blop::function &left, const blop::var      &right);
blop::function     operator==(const blop::var      &left, const blop::function &right);
blop::function     operator==(const blop::function &left, const char *right);
blop::function     operator==(const char *left, const blop::function &right);
blop::function     operator==(double left, const blop::function &right);
blop::function     operator==(const blop::function &left, double right);
blop::function     operator==(int left, const blop::function &right);
blop::function     operator==(const blop::function &left, int right);
blop::function     operator!=(const blop::function &left, const blop::function &right);
blop::function     operator< (const blop::function &left, const blop::function &right);
blop::function     operator<=(const blop::function &left, const blop::function &right);
blop::function     operator> (const blop::function &left, const blop::function &right);
blop::function     operator>=(const blop::function &left, const blop::function &right);
#pragma define G__NOALGORITHM
#endif


//#ifdef __MAKECINT__
//#pragma link off namespace blop::function_core
//#endif



#endif
