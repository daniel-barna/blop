#include "blop_bookkeeper.h"
#include "graph_drawer.h"
#include "function_core.hh"
#include "cfunc_wrapper.hh"
#include "function.h"
#include <iostream>

using namespace std;

namespace blop
{
    void blop_initializer::call_INIT() {}

    color_mapping_function::color_mapping_function(void *)
    {
	cerr<<"color_mapping::set(void *) should not be called"<<endl;
	cerr<<"in a compiled program ! It is only intended for CINT !"<<endl;
    }

    color color_mapping_function::map_interpreted(double val,double mini,double maxi) const
    {
	cerr<<"graphd_cbox_base::map_color_interpreted(double,double,double)"<<endl;
	cerr<<"should not be called in a compiled program!"<<endl;
	return color(0,0,0);
    }

    // -------------  function::formula ---------------------------

    void function::formula(const var &)
    {
	cerr<<"function::formula(const var &) should be called in a compiled "<<endl;
	cerr<<"code only if CINT support is enabled (blop-config --cint --cflags --libs)"<<endl;
	*this = 0.0;
    }

    //--------------  function_core::cfunc ------------------------

    void function_core::cfunc::init_(void *)
    {
	cerr<<"function::cfunc::init_(void *) should not be called from a compiled code"<<endl;
	cerr<<"with no CINT interpreter support"<<endl;
    }

    // -----------------  cfunc_wrappers  ------------------------------

    double double_func_aref_aref_aref_aref_aref
    ::operator() (const std::vector<blop::var> &a1,
		  const std::vector<blop::var> &a2,
		  const std::vector<blop::var> &a3,
		  const std::vector<blop::var> &a4,
		  const std::vector<blop::var> &a5)
    {
	if(ptr_) return (result_=((function_type)ptr_)(a1,a2,a3,a4,a5));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }
    
    double double_func_aref_aref_aref_aref_aref_aref
    ::operator() (const std::vector<blop::var> &a1,
		  const std::vector<blop::var> &a2,
		  const std::vector<blop::var> &a3,
		  const std::vector<blop::var> &a4,
		  const std::vector<blop::var> &a5,
		  const std::vector<blop::var> &a6)
    {
	if(ptr_) return (result_=((function_type)ptr_)(a1,a2,a3,a4,a5,a6));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
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
	if(ptr_) return (result_=((function_type)ptr_)(a1,a2,a3,a4,a5,a6,a7));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }
    
    int int_func_double_double_aref_aref_int
    ::operator() (double a1, double a2,
		  const std::vector<blop::var> &a3,
		  const std::vector<blop::var> &a4,
		  int a5) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(a1,a2,a3,a4,a5));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }


/*
    double double_func_int_doublep_doublep_int_doublep_doublep_doublep_doublep
    ::operator() (int nx, double *x, double *sigma_x, int ny, double *y, double *sigma_y, double *f, double *g1)
    {
	if(ptr_) return (result_=((function_type)ptr_)(nx,x,sigma_x,ny,y,sigma_y,f,g1));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }

    double double_func_int_doublep_doublep_int_doublep_doublep_doublep_doublep_doublep
    ::operator() (int nx, double *x, double *sigma_x, int ny, double *y, double *sigma_y, double *f, double *g1, double *g2)
    {
	if(ptr_) return (result_=((function_type)ptr_)(nx,x,sigma_x,ny,y,sigma_y,f,g1,g2));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }

    int int_func_double_double_doublep_doublep_int_int::operator()
	(double d1, double d2, double *p1, double *p2, int i1, int i2) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(d1, d2, p1, p2, i1, i2));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }
*/

    complex<double> complex_func_double::operator()(double v) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v));
	warning::print("Uninitialized cfunc_wrapper called");
	return (result_=complex<double>(0,0));
    }
    complex<double> complex_func_double_double::operator()(double v1,double v2) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v1,v2));
	warning::print("Uninitialized cfunc_wrapper called");
	return (result_=complex<double>(0,0));
    }
    double double_func_double::operator()(double v) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }
    double double_func_double_double::operator()(double v1,double v2) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v1,v2));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }
    double double_func_double_double_double::operator()(double v1,double v2,double v3) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v1,v2,v3));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }
    double double_func_double_double_double_double::operator()(double v1,double v2,double v3,double v4) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v1,v2,v3,v4));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }

    var var_func_varvector_varvector::operator()(const std::vector<blop::var> &args,
						 const std::vector<blop::var> &pars) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(args,pars));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }

    var var_func_var::operator()(var v) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }
    var var_func_var_var::operator()(var v1,var v2) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v1,v2));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }
    var var_func_var_var_var::operator()(var v1,var v2,var v3) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v1,v2,v3));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }
    var var_func_var_var_var_var::operator()(var v1,var v2,var v3,var v4) const
    {
	if(ptr_) return (result_=((function_type)ptr_)(v1,v2,v3,v4));
	cerr<<"Uninitialized cfunc_wrapper called"<<endl;
	return (result_=0);
    }

    void void_func_doublep_doublep::operator()(double *in, double *out) const
    {
	if(ptr_) { ((function_type)ptr_)(in,out); return; }
	warning::print("Uninitialized cfunc_wrapper called","void_func_doublep_doublep::operator()(double*,double*)");
    }

    void void_func_doublep_doublep_doublep::operator()(double *in, double *pars, double *out) const
    {
	if(ptr_) { ((function_type)ptr_)(in,pars,out); return; }
	warning::print("Uninitialized cfunc_wrapper called","void_func_doublep_doublep::operator()(double*,double*)");
    }


#ifdef BLOP_ENABLE_CINT
    void cfunc_wrapper_base::find(void *pointer,
				  std::vector<G__MethodInfo> &results,
				  std::string &function_name_global,
				  const std::string &arglist_spec)
    {
	warning::print("cfunc_wrapper_base::find(void*, ...) should not be called in a code without CINT interpreter support");
    }
#endif
    
    cfunc_wrapper_base *cfunc_wrapper_base::find(void *pointer, const std::string &arglist)
    {
	warning::print("cfunc_wrapper_base::find(void*, ...) should not be called in a code without CINT interpreter support");
	return 0;
    }



}
