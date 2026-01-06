#define __MAKECINT__
#include <cstring>
#include <sstream>
#include "var.h"
#include "blop_bookkeeper.h"
#include "graph_drawer.h"
#include "exc.hh"
#include "function_core.hh"
#include "function.h"
#include "cfunc_wrapper.hh"
#include "cfunc_wrapper.h"
#ifdef CINT_INCLUDE_PREFIX
#include "cint/Api.h"
#else
#include "Api.h"
#endif
#include "fit.h"
#include "bloputils.h"
#include "plot.h"

using namespace std;

namespace blop
{


    fgraph &plot(void *f1, void *f2, void *f3, void *f4, void *f5)
    {
	function f1_ = cfunc(f1);
	function f2_ = (f2?cfunc(f2):function(unset));
	function f3_ = (f3?cfunc(f3):function(unset));
	function f4_ = (f4?cfunc(f4):function(unset));
	function f5_ = (f5?cfunc(f5):function(unset));
	return plot(f1_,f2_,f3_,f4_,f5_);
    }
    fgraph &plot(void *f1, const function &f2, void *f3, void *f4,void *f5)
    {
	function f1_ = cfunc(f1);
	function f3_ = (f3?cfunc(f3):function(unset));
	function f4_ = (f4?cfunc(f4):function(unset));
	function f5_ = (f5?cfunc(f5):function(unset));
	return plot(f1_,f2,f3_,f4_,f5_);
    }
    fgraph &plot(const function &f1, void *f2, void *f3, void *f4, void *f5)
    {
	function f2_ = cfunc(f2);
	function f3_ = (f3?cfunc(f3):function(unset));
	function f4_ = (f4?cfunc(f4):function(unset));
	function f5_ = (f5?cfunc(f5):function(unset));
	return plot(f1,f2_,f3_,f4_,f5_);
    }

    fgraph &mplot(void *f1, void *f2, void *f3, void *f4, void *f5)
    {
	function f1_ = cfunc(f1);
	function f2_ = (f2?cfunc(f2):function(unset));
	function f3_ = (f3?cfunc(f3):function(unset));
	function f4_ = (f4?cfunc(f4):function(unset));
	function f5_ = (f5?cfunc(f5):function(unset));
	return mplot(f1_,f2_,f3_,f4_,f5_);
    }


    void blop_initializer::call_INIT()
    {
	G__ClassInfo globalscope;
	G__MethodInfo method;
	long dummy;

	method = globalscope.GetMethod("BLOP_USER_INIT","",&dummy);

	if(method.IsValid())
	{
	    void *p2f = method.PointerToFunc();
	    int type = G__isinterpretedp2f(p2f);
	    if(type == G__BYTECODEFUNC || type == G__INTERPRETEDFUNC)
	    {
		G__calc("BLOP_USER_INIT()");
	    }
	}
    }


    //SEP
    // color mapping

    color color_mapping_function::the_color_;

    color color_mapping_function::map_interpreted(double val,double mini,double maxi,bool logscale) const
    {
	var cmd = "color_mapping::the_color_ = ";
        if(logscale)
        {
            val = std::log10(val);
            mini = std::log10(mini);
            maxi = std::log10(maxi);
        }
	cmd &= fname_;
	cmd &= "(";
	cmd &= val;
	cmd &= ",";
	cmd &= mini;
	cmd &= ",";
	cmd &= maxi;
	cmd &= ");";
	G__calc(cmd.c_str());
	return color_mapping::the_color_;
    }

    void color_mapping_function::color_mapping_function(void *pointer)
    {
	if(pointer == 0)
	{
	    p2f_ = 0;
	    p2f_cint_ = 0;
	    fname_ = "this_should_not_happen";
	    return;
	}

	// if it is the pointer of the compiled default mapping function, simply store
	// this pointer

	if(pointer == (void*)color_mapping::def)
	{
	    p2f_ = color_mapping::def;
	    p2f_cint_ = 0;
	    fname_ = "this_should_not_happen";
	    return;
	}

	// otherwise, it can be an interpreted, or whatever function, so try to
	// figure out, what kind of function it is.

	char *fname;

	if((fname = G__p2f2funcname(pointer)) == 0)
	{
	    warning::print("Unknown pointer to function in","color_mapping::set(void*)");
	    p2f_ = 0;
	    p2f_cint_ = 0;
	    fname_ = "this_should_not_happen";
	    return;
	}

	fname_ = fname;

	G__ClassInfo globalscope;
	G__MethodInfo method;
	long dummy;
	method = globalscope.GetMethod(fname,"double,double,double",&dummy);

	if(!method.IsValid())
	{
	    warning::print(var("Function '") & var(fname) &
			   var("(double,double,double)' is not found"));
	    p2f_ = 0;
	    p2f_cint_ = 0;
	    fname_ = "this_should_not_happen";
	    return;
	}

	pointer = method.PointerToFunc();

	switch(G__isinterpretedp2f(pointer))
	{
	case G__BYTECODEFUNC:        // reconstruct function call as string
	case G__INTERPRETEDFUNC: 
	case G__COMPILEDINTERFACEMETHOD:
	{
	    p2f_cint_ = pointer;
	    p2f_ = 0;
	    break;
	}
	case G__COMPILEDTRUEFUNC: // using true pointer to function
	case G__UNKNOWNFUNC:
	{
	    typedef color (*ppp)(double,double,double);
	    p2f_cint_ = 0;
	    p2f_ = (ppp)pointer;
	    break;
	}

	default:
	    err("This should never happen");

	}
    }

    // -------------  function::formula ---------------------------

    void function::formula(const var &f)
    {
	char ptr[30];
	sprintf(ptr,"(*(function*)%p) = ",this);
	var cmd = ptr;
	cmd &= f;
	G__exec_text(cmd.c_str());
    }

    // -------------  function_core::cfunc ------------------------

    void function_core::cfunc::init_(void *p)
    {
	wrapper_ = cfunc_wrapper_base::find(p);

	if(wrapper_ == 0)
	{
	    warning::print("No function found","function_core::cfunc::init_(void*)");
	    return;
	}

	if(wrapper_->n_args() <= 0 ||wrapper_->n_out() <= 0)
	{
	    warning::print("This cfunc_wrapper is not prepared to work with function",
			   "function_core::cfunc::init(void *)");
	    delete wrapper_;
	    wrapper_ = 0;
	    return;
	}
    }


    // ------------------- cfunc_wrappers ----------------------------------------------

    double double_func_aref_aref_aref_aref_aref
    ::operator() (const std::vector<blop::var> &a1,
		  const std::vector<blop::var> &a2,
		  const std::vector<blop::var> &a3,
		  const std::vector<blop::var> &a4,
		  const std::vector<blop::var> &a5)
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(a1,a2,a3,a4,a5));
	std::string cmd = fname_ + "(";
	char buf[1000];
	const char *t = "*(std::vector<blop::var>*)";
	sprintf(buf,"%s%p,%s%p,%s%p,%s%p,%s%p", t,&a1, t,&a2, t,&a3, t,&a4, t,&a5);
	cmd += buf;
	cmd += ");";
	result_ = G__double(G__calc(cmd.c_str()));
	return result_;
    }

    double double_func_aref_aref_aref_aref_aref_aref
    ::operator() (const std::vector<blop::var> &a1,
		  const std::vector<blop::var> &a2,
		  const std::vector<blop::var> &a3,
		  const std::vector<blop::var> &a4,
		  const std::vector<blop::var> &a5,
		  const std::vector<blop::var> &a6)
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(a1,a2,a3,a4,a5,a6));
	std::string cmd = fname_ + "(";
	char buf[1000];
	const char *t = "*(std::vector<blop::var>*)";
	sprintf(buf,"%s%p,%s%p,%s%p,%s%p,%s%p,%s%p", t,&a1, t,&a2, t,&a3, t,&a4, t,&a5, t,&a6);
	cmd += buf;
	cmd += ");";
	result_ = G__double(G__calc(cmd.c_str()));
	return result_;
    }

    double double_func_aref_aref_aref_aref_aref_aref_aref
    ::operator() (const std::vector<blop::var> &a1,
		  const std::vector<blop::var> &a2,
		  const std::vector<blop::var> &a3,
		  const std::vector<blop::var> &a4,
		  const std::vector<blop::var> &a5,
		  const std::vector<blop::var> &a6,
		  const std::vector<blop::var> &a7)
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(a1,a2,a3,a4,a5,a6,a7));
	std::string cmd = fname_ + "(";
	char buf[1000];
	const char *t = "*(std::vector<blop::var>*)";
	sprintf(buf,"%s%p,%s%p,%s%p,%s%p,%s%p,%s%p,%s%p", t,&a1, t,&a2, t,&a3, t,&a4, t,&a5, t,&a6, t,&a7);
	cmd += buf;
	cmd += ");";
	result_ = G__double(G__calc(cmd.c_str()));
	return result_;
    }

    int int_func_double_double_aref_aref_int
    ::operator() (double a1, double a2,
		  const std::vector<blop::var> &a3,
		  const std::vector<blop::var> &a4,
		  int a5) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(a1,a2,a3,a4,a5));
	std::string cmd = fname_ + "(";
	char buf[1000];
	const char *t = "*(std::vector<blop::var>*)";
	sprintf(buf,"*(double*)%p,*(double*)%p,%s%p,%s%p,*(int*)%p", &a1, &a2, t,&a3, t,&a4, &a5);
	cmd += buf;
	cmd += ");";
	result_ = G__int(G__calc(cmd.c_str()));
	return result_;
    }

    /*
    // --
    double double_func_int_doublep_doublep_int_doublep_doublep_doublep_doublep
    ::operator() (int nx, double *x, double *sigma_x, int ny, double *y, double *sigma_y, double *f, double *g1)
    {
	if(ptr_ == 0) { cerr<<"Uninitialized cfunc_wrapper called"<<endl; return (result_=0);}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(nx,x,sigma_x,ny,y,sigma_y,f, g1));
	std::string cmd = fname_ + "(";
	char buf[1000];
	sprintf(buf,"%i,(double*)%p,(double*)%p,%i,(double*)%p,(double*)%p,(double*)%p,(double*)%p",
		nx,x,sigma_x,ny,y,sigma_y,f,g1);
	cmd += buf;
	cmd += ");";
	result_ = G__double(G__calc(cmd.c_str()));
	return result_;
    }


    // --
    double double_func_int_doublep_doublep_int_doublep_doublep_doublep_doublep_doublep::operator()
	(int nx, double *x, double *sigma_x, int ny, double *y, double *sigma_y, double *f, double *g1, double *g2)
    {
	if(ptr_ == 0) { cerr<<"Uninitialized cfunc_wrapper called"<<endl; return (result_=0);}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(nx,x,sigma_x,ny,y,sigma_y,f, g1, g2));
	std::string cmd = fname_ + "(";
	char buf[1000];
	sprintf(buf,"%i,(double*)%p,(double*)%p,%i,(double*)%p,(double*)%p,(double*)%p,(double*)%p,(double*)%p",
		nx,x,sigma_x,ny,y,sigma_y,f,g1, g2);
	cmd += buf;
	cmd += ");";
	result_ = G__double(G__calc(cmd.c_str()));
	return result_;
    }

    // --
    int int_func_double_double_doublep_doublep_int_int::operator()
	(double d1, double d2, double *p1, double *p2, int i1, int i2) const
    {
	if(ptr_ == 0) { cerr<<"Uninitialized cfunc_wrapper called"<<endl; return (result_=0);}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(d1, d2, p1, p2, i1, i2));
	std::string cmd = fname_ + "(";
	char buf[1000];
	sprintf(buf,"%f,%f,(double*)%p,(double*)%p,%i,%i",d1, d2, p1, p2, i1, i2);
	cmd += buf;
	cmd += ");";
	result_ = G__int(G__calc(cmd.c_str()));
	return result_;
    }
    */

    //---

    complex<double> complex_func_double::operator()(double v) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called",
			   "complex_func_double::operator()(double)");
	    return (result_=0);
	}
	if(functype_ == compiled)
	{
	    return (result_=((function_type)ptr_)(v));
	}
	char cmd[1000];
	sprintf(cmd,"temporary_result_complex = %s(%f);",fname_.c_str(),v);
	G__calc(cmd);
	return (result_=temporary_result_complex);
    }
    complex<double> complex_func_double_double::operator()(double v1,double v2) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called",
			   "complex_func_double_double::operator()(double,double)");
	    return (result_=0);
	}
	if(functype_ == compiled)
	{
	    return (result_=((function_type)ptr_)(v1,v2));
	}
	char cmd[1000];
	sprintf(cmd,"temporary_result_complex = %s(%f,%f);",fname_.c_str(),v1,v2);
	G__calc(cmd);
	return (result_=temporary_result_complex);
    }


    double double_func_double::operator()(double v) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called",
			   "double_func_double::operator()(double)");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(v));
	var cmd = var(fname_) & "(" & v & ")";
	result_ = G__double(G__calc(cmd.c_str()));
	return result_;
    }

    double double_func_double_double::operator()(double v1,double v2) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(v1,v2));
	var cmd = var(fname_) & "(" & v1 & "," & v2 & ")";
	result_ = G__double(G__calc(cmd.c_str()));
	return result_;
    }

    double double_func_double_double_double::operator()(double v1,double v2,double v3) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(v1,v2,v3));
	var cmd = var(fname_) & "(" & v1 & "," & v2 & "," & v3 & ")";
	result_ = G__double(G__calc(cmd.c_str()));
	return result_;
    }

    double double_func_double_double_double_double::operator()(double v1,double v2,double v3,double v4) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(v1,v2,v3,v4));
	var cmd = var(fname_) & "(" & v1 & "," & v2 & "," & v3 & "," & v4 & ")";
	result_ = G__double(G__calc(cmd.c_str()));
	return result_;
    }

    var var_func_varvector_varvector::operator()(const std::vector<blop::var> &args,
						 const std::vector<blop::var> &pars) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled)
	{
	    return (result_=((function_type)ptr_)(args,pars));
	}
	ostringstream ptr1, ptr2;
	ptr1<<&args;
	ptr2<<&pars;
	var cmd = var("temporary_result_var = ") & fname_ & "(" &
	    var("*((const std::vector<blop::var> *)") & ptr1.str() & ")," &
	    var("*((const std::vector<blop::var> *)") & ptr2.str() & "))";
	G__calc(cmd.c_str());
	return (result_=temporary_result_var);
    }

    var var_func_var::operator()(var v) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled)
	{
	    return (result_=((function_type)ptr_)(v));
	}
	var cmd = var("temporary_result_var = ") & fname_ & "(" & v & ")";
	G__calc(cmd.c_str());
	return (result_=temporary_result_var);
    }

    var var_func_var_var::operator()(var v1,var v2) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(v1,v2));
	var cmd = var("temporary_result_var = ") & fname_ & "(" & v1 & "," & v2 & ")";
	G__calc(cmd.c_str());
	return (result_=temporary_result_var);
    }

    var var_func_var_var_var::operator()(var v1,var v2,var v3) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(v1,v2,v3));
	var cmd = var("temporary_result_var = ") & fname_ & "(" & v1 & "," & v2 & "," & v3 & ")";
	G__calc(cmd.c_str());
	return (result_=temporary_result_var);
    }

    var var_func_var_var_var_var::operator()(var v1,var v2,var v3,var v4) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called");
	    return (result_=0);
	}
	if(functype_ == compiled) return (result_=((function_type)ptr_)(v1,v2,v3,v4));
	var cmd = var("temporary_result_var = ") & fname_ & "(" & v1 & "," & v2 & "," & v3 & "," & v4 & ")";
	G__calc(cmd.c_str());
	return (result_=temporary_result_var);
    }

    void void_func_doublep_doublep::operator()(double *in, double *out) const
    {
	if(ptr_ == 0)
	{
	    warning::print("Uninitialized cfunc_wrapper called","void_func_doublep_doublep::operator()");
	    result_ = 0;
	    return;
	}

	result_ = 1;
	if(functype_ == compiled) { ((function_type)ptr_)(in,out); return; }
	char cmd[1000];
	sprintf(cmd,"%s((double*)%p,(double*)%p)",fname_.c_str(),in,out);
	G__calc(cmd);
    }

    std::string create_arglist(G__MethodInfo &func)
    {
	string alist;
	G__MethodArgInfo arg(func);
	int count=0;
	while(arg.Next())
	{
	    if(count > 0) alist += ",";
	    long int property = arg.Property();
	    if((property&G__BIT_ISCONSTANT) &&
	       strstr(arg.Type()->Name(),"const ") != arg.Type()->Name())
	    {
		alist += "const ";
	    }
	    alist += arg.Type()->Name();
	    ++count;
	}
	return alist;
    }

    bool arglist_matches(var a1, var a2)
    {
	if(a1.str() == a2.str()) return true;
	a1.replace("std::","");
	a2.replace("std::","");
	if(a1.str() == a2.str()) return true;
	return false;
    }


    void cfunc_wrapper_base::find(void *pointer,
				  std::vector<G__MethodInfo> &results,
				  std::string &function_name_global,
				  const std::string &arglist_spec)
    {
	results.clear();
	function_name_global = "UNKNOWN FUNCTION";

	if(pointer == 0)
	{
	    warning::print("0 pointer","cfunc_wrapper_base::find(...)");
	    return;
	}

	function_name_global = G__p2f2funcname(pointer);
	if(function_name_global == "")
	{
	    warning::print("Unknown pointer to function",
			   "cfunc_wrapper_base::find"
			   "(void *,const string &,int&,void*&,string&,string&)");
	    function_name_global == "UNKNOWN FUNCTION";
	    return;
	}

	std::string fname = function_name_global;

	G__MethodInfo *method = 0;
	G__ClassInfo *scope = 0;
	string::size_type pos;
	if( (pos = function_name_global.find("::")) != string::npos )
	{
	    string classname = function_name_global.substr(0,pos);
	    scope = new G__ClassInfo(classname.c_str());
	    method =  new G__MethodInfo(*scope);
	    fname = function_name_global.substr(pos+2);
	}
	else
	{
	    method = new G__MethodInfo;
	    fname = function_name_global;
	}

	array arglists = split(arglist_spec,"|");

	while(method->Next())
	{
	    // accept only methods with the same name
	    if(fname != method->Name()) continue;

	    // if no arglist specifications were given, then store this into
	    // the output list
	    if(arglists.empty())
	    {
		results.push_back(*method);
		continue;
	    }

	    // otherwise check if the arglist of this method matches any of the
	    // arglist specifications

	    std::string alist = create_arglist(*method);

	    for(unsigned int i=0; i<arglists.size(); ++i)
	    {
		//if(arglists[i].str() == alist)
		if(arglist_matches(arglists[i], alist))
		{
		    results.push_back(*method);
		    break;
		}
	    }
	}

	delete method;
	delete scope;

    }
    

    cfunc_wrapper_base *cfunc_wrapper_base::find(void *pointer, const std::string &arglist_spec)
    {
	string function_name_global;
	std::vector<G__MethodInfo> methods;

	// find all functions with mathing name
	find(pointer,
	     methods,
	     function_name_global,
	     arglist_spec);


	if(debug::on())
	{
	    debug::print(var((long int)methods.size()) & var(" function candidates found!"),
			     var("cfunc_wrapper_base::find(void*,\"") &
						  arglist_spec &
						  var("\")"));
	}


	cfunc_wrapper_base *f = 0;
	for(unsigned int i=0; i<methods.size(); ++i)
	{
	    const string returntype = methods[i].Type()->Name();
	    int functype = G__isinterpretedp2f(methods[i].PointerToFunc());

	    // An interpreted static member function of a class was taken as G__UNKNOWNFUNC
	    // Therefore it needed to be taken out from the 'compiled' category, and
	    // only G__COMPILEDTRUEFUNC is treated as a compiled function


	    const std::string arglist = create_arglist(methods[i]);

	    string ftmessage = string("Function: ") + returntype + " " + function_name_global +
		"(" + arglist + ") [";

	    if(functype == G__COMPILEDTRUEFUNC
	       //|| functype == G__UNKNOWNFUNC
		)
	    {
		ftmessage += "compiled";
		functype = compiled;
	    }
	    else
	    {
		functype = interpreted;
		ftmessage += "interpreted";
	    }
	    ftmessage += "]";

	    debug::print(ftmessage,"cfunc_wrapper_base::find(void*,string arglist)");

	    if(returntype == "double" && arglist == "double")
	    {
		f = new double_func_double;
	    }
	    else if(returntype == "double" && arglist == "double,double")
	    {
		f = new double_func_double_double;
	    }
	    else if(returntype == "double" && arglist == "double,double,double")
	    {
		f = new double_func_double_double_double;
	    }
	    else if(returntype == "double" && arglist == "double,double,double,double")
	    {
		f = new double_func_double_double_double_double;
	    }
	    else if((returntype == "var" || returntype == "blop::var") &&
		    (arglist == "var" || arglist == "blop::var"))
	    {
		f = new var_func_var;
	    }
	    else if((returntype == "var" || returntype == "blop::var") &&
		    (arglist == "var,var" || arglist == "blop::var,blop::var"))
	    {
		f = new var_func_var_var;
	    }
	    else if((returntype == "var" || returntype == "blop::var") &&
		    (arglist == "var,var,var" || arglist == "blop::var,blop::var,blop::var"))
	    {
		f = new var_func_var_var_var;
	    }
	    else if((returntype == "var" || returntype == "blop::var") &&
		    (arglist == "var,var,var,var" || arglist == "blop::var,blop::var,blop::var,blop::var"))
	    {
		f = new var_func_var_var_var_var;
	    }

	    else if( (returntype == "double") &&
		     (
			 arglist == "const std::vector<blop::var>&,"
		                    "const std::vector<blop::var>&,"
                                    "const std::vector<blop::var>&,"
                                    "const std::vector<blop::var>&,"
                                    "const std::vector<blop::var>&" ||
			 arglist == "const vector<blop::var>&,"
		                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&"
		      )
		)
	    {
		f = new double_func_aref_aref_aref_aref_aref;
	    }

	    else if( (returntype == "double") &&
		     (
			 arglist == "const std::vector<blop::var>&,"
		                    "const std::vector<blop::var>&,"
		                    "const std::vector<blop::var>&,"
                                    "const std::vector<blop::var>&,"
                                    "const std::vector<blop::var>&,"
                                    "const std::vector<blop::var>&" ||
			 arglist == "const vector<blop::var>&,"
		                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&"
		      )
		)
	    {
		f = new double_func_aref_aref_aref_aref_aref_aref;
	    }

	    else if( (returntype == "double") &&
		     (
			 arglist == "const std::vector<blop::var>&,"
		                    "const std::vector<blop::var>&,"
		                    "const std::vector<blop::var>&,"
		                    "const std::vector<blop::var>&,"
                                    "const std::vector<blop::var>&,"
                                    "const std::vector<blop::var>&,"
                                    "const std::vector<blop::var>&" ||
			 arglist == "const vector<blop::var>&,"
		                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&,"
                                    "const vector<blop::var>&"
		      )
		)
	    {
		f = new double_func_aref_aref_aref_aref_aref_aref_aref;
	    }



/*
	    else if(returntype == "double" && arglist == "int,double*,double*,int,double*,double*,double*")
	    {
		f = new double_func_int_doublep_doublep_int_doublep_doublep_doublep;
	    }
	    else if(returntype == "double" && arglist == "int,double*,double*,int,double*,double*,double*,double*")
	    {
		f = new double_func_int_doublep_doublep_int_doublep_doublep_doublep_doublep;
	    }
	    else if(returntype == "double" &&
		    arglist == "int,double*,double*,int,double*,double*,double*,double*,double*")
	    {
		f = new double_func_int_doublep_doublep_int_doublep_doublep_doublep_doublep_doublep;
	    }
	    else if(returntype == "int" && arglist == "double,double,double*,double*,int,int")
	    {
		f = new int_func_double_double_doublep_doublep_int_int;
	    }
*/


	    else if((returntype == "complex" ||
		     returntype == "complex<double>" ||
		     returntype == "std::complex<double>")
		    && arglist == "double")
	    {
		f = new complex_func_double;
	    }
	    else if(returntype == "void"      && arglist == "double*,double*")
	    {
		f = new void_func_doublep_doublep;
	    }


	    if(f != 0)
	    {
		f->functype_ = functype;
		f->ptr_ = methods[i].PointerToFunc();
		f->fname_ = function_name_global;
		f->arglist_ = arglist;
		break;
	    }
	}
	return f;
    }


    
}
