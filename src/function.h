#ifndef __BLOP_FUNCTION_H__
#define __BLOP_FUNCTION_H__

#include <vector>
#include <valarray>
#include <map>
#include <functional>
#include "var.h"
#include "array.h"
#include "constants.h"
//#include "geometry.h"
#include "meas.h"
#ifndef __MAKECINT__
#include <complex>
#endif
#include "config.h"

namespace blop
{
    class plottable;

    namespace geom
    {
        class vec3;
    }

    // forward declaration 
    namespace function_core
    {
	class col;
	class bessel;
	class ifelse;
        class join_args;
    }

    class fgraph;

    class dgraph;

    class function;
    function ARG(int i);
    function PAR(int i);

    // --- Functions returning simply their 1st, 2nd, etc. arguments  --------

    extern function _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30;
    extern function _0, _b, _B, _g;
    extern function _n(const var &id);  // named parameter
    
    function PREV(int nprevious=1);

    class function
    {

    public:
        class core
        {
        protected:
            int nargs_, npars_;
	
        public:
            core() : nargs_(-1), npars_(-1) {}
            virtual ~core() {}
	
            // create a copy of this expression, recursively (daughters are
            // also cloned)
            virtual function::core  *clone()              const = 0;
	
            // evaluate the expression. the second version avoids the overhead
            // due to the 'var' return values, it is used in the fitting algorithm
            // 
	
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
            virtual bool      uses_arg(int i) const = 0; //{return false;}
            virtual bool      uses_par(int i) const {return false;}
	
            // return the character-representation of this expression
            virtual var sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names = {}, std::map<int,blop::var> param_names = {}) const = 0;
            virtual var sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
                                     const var &x="x", const var &y="y", const var &z="z") const
            {
                return sprint(pars, parvalue);
            }
	
            // create the derivative expression. it is dynamically allocated,
            // so it should not be copied, when attached to a function
            virtual function::core* create_derivative(int i) const { return 0; }
	
            // check if it is the same 
            virtual bool equals(const function::core *) const = 0;
	
            // return the number of output variables
            virtual int n_out() const { return 1; }

            // Hint about how many returned values a caller (argument_subst) requires
            virtual void n_out_hint(int n) 
            {
                if(n_out() < n) warning::print("Function might return less values than required");
            }
	
            virtual bool is_constant() const { return false; }

        };
        

    private:
        

        //int nargs_, npars_;

        // cint takes these friend-declarations as function declarations,
        // causing troubles. 
#ifndef __MAKECINT__
        friend function     format(const function &func, const function &form);
        friend function     format(const function &func, const var &form);

        friend function     ifelse(const function &condition, const function &iftrue, const function &iffalse);
        friend blop::function     replace(const blop::function &from, const blop::function &to, const blop::function &in);
        friend blop::function     substr(const blop::function &s, const blop::function &from, const blop::function &to);

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
        friend function     maximum(const function &, const function &,
                                    const function &, const function &);
        friend function     minimum(const function &, const function &,
                                    const function &, const function &);
        friend function     integral      (const function &, const function &,
                                           const function &, const function &);


        friend function     operator+ (const function &left, const function &right);
        friend function     operator+ (const function &left, double right);
        friend function     operator+ (double left, const function &right);
        friend function     operator+ (const function &left, int right);
        friend function     operator+ (int left, const function &right);
        friend function     operator- (const function &left, const function &right);
        friend function     operator- (const function &left, double right);
        friend function     operator- (double left, const function &right);
        friend function     operator- (const function &left, int right);
        friend function     operator- (int left, const function &right);
        friend function     operator* (const function &left, const function &right);
        friend function     operator* (const function &left, double right);
        friend function     operator* (double left, const function &right);
        friend function     operator* (const function &left, int right);
        friend function     operator* (int left, const function &right);
        friend function     operator/ (const function &left, const function &right);
        friend function     operator/ (const function &left, double right);
        friend function     operator/ (double left, const function &right);
        friend function     operator/ (const function &left, int right);
        friend function     operator/ (int left, const function &right);
        friend function     operator- (const function &operand);

        friend function     cmp_mul(const function &left, const function &right);
        friend function     cmp_div(const function &left, const function &right);

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
        friend function     asin      (const function &o);
        friend function     cos       (const function &o);
        friend function     acos      (const function &o);
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
        friend function     round     (const function &);
        friend function     erf       (const function &);
        friend function     atan2     (const function &, const function &);
        friend function     pow       (const function &, const function &);
        friend function     pow       (const function &, int);
        friend function     pow       (const function &, double);
        friend function     pow       (double, const function &);
        friend function     pow       (const function &, const var &);
        friend function     sign      (const function &);
        friend function     abs       (const function &);
        friend function     fabs      (const function &);
        friend function     periodic_function(const function &, double x1, double x2);
        friend function     periodic_function(const function &, double x1, double x2, double y1, double y2);
        friend function     periodic_function(const function &, double x1, double x2, double y1, double y2, double z1, double z2);
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
        friend function     cfunc     (void (*p)(double*, double*, double*), int nargs, int npars, int nout);

//        friend function     cppfunc   (void (*p)(double*,double*,double*), int nargs, int npars, int nout);
        friend function     cppfunc   (const std::function<void(double*,double*,double*)> &func, int nargs, int npars, int nout);

        friend function     charfunc(const function &,const function &, bool, bool);
        friend function     between_lines(const function &, const function &);
        friend function     contained_in(const function &);

        friend class        blop::function_core::col;
        friend class        blop::fgraph;
        friend class        blop::function_core::ifelse;
        friend class        blop::function_core::join_args;

//        friend blop::function     replace(const blop::function &from, const blop::function &to, const blop::function &in);

#endif

        static std::vector<std::string> column_names_;
        std::vector<blop::var> default_args_;

        template <class T> void init_binary_(const function &left, const function &right);
        template <class T> void init_unary_ (const function &arg);

        core* base_;
        typedef std::map<int, blop::function::core*> deriv_map;
        deriv_map derivatives_;

        bool     print_param_value_;
        static bool default_print_param_value_;

        void init_();
        mutable std::vector<blop::var>    result_;
        std::vector<blop::var>    parameters_;
        std::vector<blop::var>    parnames_;
        std::map<int, blop::var>  parformats_;

        function(const std::vector<function::core*> &bases, bool clone_them=true);

        void copy_arg_ranges_(const function &f1,
                              const function &f2=blop::unset,
                              const function &f3=blop::unset,
                              const function &f4=blop::unset,
                              const function &f5=blop::unset,
                              const function &f6=blop::unset);
        std::vector<double> arg_min_, arg_max_;
        bool make_arg_check_(const std::vector<blop::var> &args) const;
        bool check_args_;
        static bool default_check_args_;

        double eval_meas_error(const std::vector<blop::var> &vals, const std::vector<blop::var> &errors) const;

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
        function(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5, const function &f6);

        // Initialize from a C-function pointer
        function(void *fptr);

        ~function();

        function(const function::core &); // clone the given base

        // Do not call this function!!! It should in fact be private
        // (but many internal classes use it, so for the moment it is public....
        // it's a design flaw)
        function(      function::core *); // do not clone, only store pointer



        // ------------ Append more components to it ------------------------

        function &append(const function &);

        // ---- Set the format of the parameters (if they are set later to a numerial
        // value, for example during a fit, this format will be used for the string-representation)

        //function &param_format(const var &fmt); // already implemented as parformat

        // ---- Get or set value of a given parameter  ----------------------


        function  &param(unsigned int parindex, const var &value); // set parameter
        function  &param(unsigned int parindex, double     value); // set parameter
        function  &param(unsigned int parindex, const var &value, const var &name); // set value+name
        function  &param(unsigned int parindex, double     value, const var &name); 
        function  &params(double p1,double p2,
                          double p3=unset,
                          double p4=unset,
                          double p5=unset,
                          double p6=unset);

        function  &params(const std::vector<double> &p);
        function  &params(const std::vector<blop::var> &p);

        // It is often useful to use more descriptive names for parameters, for example:
        // function center = PAR(1);
        // function sigma = PAR(2);
        // function x = _1;
        // function gauss = exp(-(x-center)*(x-center)/(2*sigma*sigma));
        // This function makes it possible to also set the parameter value using these
        // descriptive names:
        // gauss.param(sigma,1.1);
        // The first argument of this function MUST be a pure function-parameter function, i.e.
        // PAR(...), as above

        function &param(const function &p, var value);
        function &param(const function &p, double value) { return param(p,var(value)); }
        function &param(const function &p, const string &value) { return param(p,var(value)); }

        function &param(const function &p, const var &value, const var &name);
        function &param(const function &p, double value, const var &name) { return param(p,var(value),name); }
        function &param(const function &p, const string &value, const var &name) { return param(p,var(value),name); }



        const var &param(unsigned int parindex) const;             // read parameter
        var &param(unsigned int parindex);                         // read parameter
        const var &param(const function &p) const;
        var &param(const function &p);

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
        var sprint(std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names={}) const;
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

        meas operator() (const meas &) const;
        meas operator() (const meas &, const meas &) const;
        meas operator() (const meas &, const meas &, const meas &) const;
        meas operator() (const meas &, const meas &, const meas &, const meas &) const;


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

//    operator double() const;

        int components() const;

        // return the given component (0-based)
        function operator[]  (unsigned int i) const;
    
        bool is_constant() const;

        bool initialized() const;

        bool equals(const function &f) const;

        static var tmp;

        static void clear_named_params();
        static void named_param(const var &id, const var &value);
        static void named_params(const std::map<std::string,blop::var> &v);

        static void column_names(const std::vector<std::string> &v) { column_names_ = v; }


        // ---------------------  interpolated functions -------------------------------

        // Read the content of the file (or any stream, see 'blop -h openin' about opening pipes, remote files, etc)
        // and return a linear-interpolated function from these values.
        // The functions 'x' and 'y' specify the independent and dependent variables. If 'x' is multi-valued,
        // the interpolation will be done in multiple dimensions. (Note that spline and sppchip can only be done in
        // one dimension!)
        // For example to make a linear interpolation on a 2D grid (x,y values stored in the columns 1&2,
        // z value stored in the 3rd column) function::interpolate_linear("filename",function(_1,_2),_3);

        static function interpolate_linear (const var &filename, const function &x=_1, const function &y=_2);
        static function interpolate_linear (const array &x, const array &y);
        static function interpolate_linear (const std::vector<double> &x, const std::vector<double> &y);
#ifndef __MAKECINT__        
        static function interpolate_linear (const std::initializer_list<double> &x, const std::initializer_list<double> &y);
#endif
        static function interpolate_linear (const std::vector<double> &x, const std::vector<blop::geom::vec3> &y);
        static function interpolate_linear (const double x[], const double y[], int n);
        static function interpolate_linear (const plottable &g);
        static function interpolate_linear (const plottable &g, const function &x, const function &y);

        static function interpolate_spline (const var &filename, const function &x=_1, const function &y=_2);
        static function interpolate_spline (const array &x, const array &y);
        static function interpolate_spline (const std::vector<double> &x, const std::vector<double> &y);
        static function interpolate_spline (const std::vector<double> &x, const std::vector<blop::geom::vec3> &y);
        static function interpolate_spline (const double x[], const double y[], int n);
        static function interpolate_spline (const dgraph &g);
#ifndef __MAKECINT__
        static function interpolate_spline (const std::initializer_list<double> &x, const std::initializer_list<double> &y);
#endif        

        static function interpolate_sppchip(const var &filename, const function &x=_1, const function &y=_2);
        static function interpolate_sppchip(const array &x, const array &y);
        static function interpolate_sppchip(const std::vector<double> &x, const std::vector<double> &y);
        static function interpolate_sppchip(const std::vector<double> &x, const std::vector<blop::geom::vec3> &y);
        static function interpolate_sppchip(const double x[], const double y[], int n);
        static function interpolate_sppchip(const dgraph &g);
#ifndef __MAKECINT__
        static function interpolate_sppchip(const std::initializer_list<double> &x, const std::initializer_list<double> &y);
#endif        

#ifdef HAVE_GTS_H    
        static function interpolate_delaunay(const var &filename);  // (x,y) is in the first 2 columns, z is in the 3rd one
        static function interpolate_delaunay(const var &filename, const function &x, const function &y);
#endif

        //html <a name="polynomial">
        // Define a polynomial function: PAR(1)+PAR(2)*x+PAR(3)*x^2+...PAR(n+1)*x^n
        static function polynomial(int n);
        //html </a>

        //html <a name="find_root_func
        // Create a blop function, which returns the root of a given equation, specified by 'f'. 
        // Upon evaluation, the <b>arguments</b> will be substituted into the <b>parameters</b> of the function 'f', then
        // the root-finding limits (x1, x2) and stepsize (epsilon) are calculated by calling the corresponding
        // functions with the actual arguments, then the root-finding algorithm is executed, and the result is returned
        // So for example if a data file "data.dat" contains the parameters of a linear function a*x+b, the parameter 'a' being stored
        // in the 3rd column, and the parameter 'b' being stored in the 4th column, and we want to plot the root of a*x+b=3 in
        // the range 0-10 as a function of the 1st and 2nd columns, then  we can do
        // plot("data.dat",_1,_2,function::find_root(PAR(3)*_1+PAR(4)-3,0,10,1e-3));
        static function find_root(const function &f, const function &x1, const function &x2, const function &epsilon);
        //html </a>

        /*
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

        */

    };

    // Create an expanded legend, assuming that the function is a polynomial of degree n (i.e. PAR(1)+PAR(2)*x+PAR(3)*x^2+ ... +PAR(n+1)*x^n
    var polylegend(const function &f, int n, const var &independent_variable="x");

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
    function     operator!=(const function &left, double right);
    function     operator!=(double left, const function &right);
    function     operator< (const function &left, const function &right);
    function     operator< (const function &left, double right);
    function     operator< (double left, const function &right);
    function     operator<=(const function &left, const function &right);
    function     operator<=(const function &left, double right);
    function     operator<=(double left, const function &right);
    function     operator> (const function &left, const function &right);
    function     operator> (const function &left, double right);
    function     operator> (double left, const function &right);
    function     operator>=(const function &left, const function &right);
    function     operator>=(const function &left, double right);
    function     operator>=(double left, const function &right);

    function     format    (const function &f, const var &form);

    function     max       (const function &left, const function &right);
    function     max       (const function &left, double right);
    function     max       (double left, const function &right);
    function     min       (const function &left, const function &right);
    function     min       (const function &left, double right);
    function     min       (double left, const function &right);

    function     maximum(const function &func,
                         const function &from,
                         const function &to,
                         const function &step=0.0);
    function     minimum(const function &func,
                         const function &from,
                         const function &to,
                         const function &step=0.0);
    function     integral(const function &func,
                          const function &from,
                          const function &to,
                          const function &step=0.0);  
    double       integral(const function &func,
                          double from,
                          double to,
                          double step=0.0);

    function     operator+ (const function &left, const function &right);
    function     operator+ (const function &left, double right);
    function     operator+ (double left, const function &right);
    function     operator+ (const function &left, int right);
    function     operator+ (int left, const function &right);
    function     operator+ (const function &left, const var &right);
    function     operator+ (const var &left, const function &right);

    function     operator- (const function &left, const function &right);
    function     operator- (const function &left, double right);
    function     operator- (double left, const function &right);
    function     operator- (const function &left, int right);
    function     operator- (int left, const function &right);
    function     operator- (const function &left, const var &right);
    function     operator- (const var &left, const function &right);

    function     operator* (const function &left, const function &right);
    function     operator* (const function &left, double right);
    function     operator* (double left, const function &right);
    function     operator* (const function &left, int right);
    function     operator* (int left, const function &right);
    function     operator* (const function &left, const var &right);
    function     operator* (const var &left, const function &right);

    function     cmp_mul(const function &left, const function &right);
    function     cmp_div(const function &left, const function &right);

    function     operator/ (const function &left, const function &right);
    function     operator/ (const function &left, double right);
    function     operator/ (double left, const function &right);
    function     operator/ (const function &left, int right);
    function     operator/ (int left, const function &right);
    function     operator/ (const function &left, const var &right);
    function     operator/ (const var &left, const function &right);

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
    function     asin (const function &);
    function     cos  (const function &);
    function     acos (const function &);
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
    function     round (const function &);
    function     erf   (const function &);
    function     atan2(const function &y, const function &x);
    function     atan2(double y, const function &x);
    function     atan2(const function &y, double x);
    function     pow  (const function &a,const function &b);
    function     pow  (const function &a,int exponent);
    function     pow  (const function &a,double exponent);
    function     pow  (const function &a,const var &exponent);
    function     pow  (double a, const function &exponent);
    function     sign (const function &);
    function     abs  (const function &);
    function     fabs (const function &);
//html <a name="periodic">
// create a periodic version of 'func' (take it between the values [x1,x2], and
// repeat this infinitely
    function     periodic_function(const function &func, double x1, double x2);
    function     periodic_function(const function &func, double x1, double x2, double y1, double y2);
    function     periodic_function(const function &func, double x1, double x2, double y1, double y2, double z1, double z2);
//html </a>

    function     ifelse(const function &condition, const function &iftrue, const function &iffalse);


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

    function     between_lines(const function &start, const function &end=true);

    function     operator&  (const function &left, const function &right);
    function     operator&& (const function &left, const function &right);
    function     operator|| (const function &left, const function &right);

    function     operator%   (const function &left, const function &right);
//function     operator%   (const function &left, int right);
//function     operator%   (int left, const function &right);



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

#ifdef __MAKECINT__
    function     cfunc     (void *p);
#endif

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

    function     cfunc     (void (*p)(double*, double*, double*), int nargs, int npars, int nout);
//    function     cppfunc   (void (*p)(double*,double*,double*), int nargs, int npars, int nout);
    function     cppfunc   (const std::function<void(double*,double*,double*)> &func, int nargs, int npars, int nout);

#endif


    std::ostream & operator<< (std::ostream &out, const blop::function &f);

    // -------------------- join_args -------------------------------------
    // Join the arguments using a separator. The range specified by 'from' and 'to'
    // is inclusive, i.e. join_args(" ",1,3) will join the arguments 1,2 & 3 separated
    // by a space

    function     join_args(const function &separator="", const function &from=1, const function &to=_N);


// ------------- contained_in ---------------------------------------------
// The created function will return 1 if its first argument is contained
// in the return-value of fff (which might be a multi-valued result)
// 0 otherwise
    function     contained_in(const function &ff);

    // The returned function returns a substring of its argument. 
    function substr(const function &s, const function &from, const function &to);

    //html <a name='replace'>
    //SEP
    // Returns a function, which replaces 'from' to 'to' in 'in'
    //html </a>
    function replace(const function &from, const function &to, const function &in);

    double tan(double);

    //html <a name=find_root>
    //SEP
    // Find the root of the provided function (i.e. where f(x)=0), within
    // the range [x1,x2]. The function values at these two points must have
    // opposite sign: f(x1)*f(x2)<0
    // epsilon is the error, the algorithm stops when the 0-crossing is found
    // with less than this error.
    // If it is left at its default value, (x2-x1)/1e6 is used
    double find_root(const function &f, double x1, double x2, double epsilon=-1);

    // find the root of the interpolated function (given by the points x,y), for value 'yvalue',
    // that is, x0 which satisfies function::interpolate_linear(x,y)(x0)-yvalue==0
    double find_root(const std::vector<double> &x, const std::vector<double> &y, double yvalue=0.0);

    // find the root of an interpolated function given by the (x,y) pairs stored in this dgraph
    double find_root(const dgraph &g, double yvalue=0.0);

    // Find all roots (i.e. where the graph crosses 'yvalue') and return them in the 'roots' vector.
    // return the number of roots found (i.e. the size of the vector)
    unsigned int find_roots(const dgraph &g, std::vector<double> &roots, double yvalue=0.0);

    // find the x-value where the piecewise linearly interpolated curve given by the arrays X and Y, and the
    // function F cross each other. 
    double find_root(const std::vector<double> &X, const std::vector<double> &Y, const function &F, double epsilon=-1);

    //html </a>

    //html <a name='implicit_solve'>
    // Numerically solve the equation f(x,y(x))=0 for y(x), by first calculating the 0-isoline of the two-argument
    // function 'f' in the range [x1,x2] and [y1,y2] with number of samples along the two axes 'n1' and 'n2'
    // Then create an interpolated function using the isoline. The x,y values of the isoline are sorted
    // by the x values, so make sure the solution function is single-valued.
    function implicit_solve(const function &f, double x1, double x2, double y1, double y2, int n1=100, int n2=100); 

    // Search the isoline (at the value 'zcut') of the given graph, and create a linearly interpolated function
    // from that iso-line
    function implicit_solve(const dgraph   &g, double zcut);
    //html </a>

    function date2epoch(const var &format="%y/%m/%d %H:%M:%S");

    // Create the vectorial cross-product of two functions. Both input functions, and the returned
    // function have 3 components
    blop::function cross_product(const blop::function &x1, const blop::function &x2);

    // Create the scalar product of two vectors. x1 and x2 must have 3 components
    blop::function dot(const blop::function &x1, const blop::function &x2);

    // Normalize the vector-function to the given norm. v must have 3 components
    blop::function normalize(const function &v, double norm=1);
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
