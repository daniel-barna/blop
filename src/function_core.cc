#include "function.h"
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
        std::map<std::string,blop::var> &named_param::values()
        {
            static std::map<std::string,blop::var> v;
            return v;
        }

//	std::vector<blop::var> tmp(20);

        // -------------------- unary_base ---------------------------------

        void unary_base::eval(const std::vector<blop::var> &args,
                              const std::vector<blop::var> &def_args,
                              const std::vector<blop::var> &pars,
                              std::vector<blop::var> &result, int *ind) const
        {
//            cerr<<"unary_base::eval"<<endl;
            int dummy_ind = 0;
            operand_->eval(args,def_args,pars,tmp_,&dummy_ind);
            for(int i=0; i<operand_->n_out(); ++i)
            {
                if(tmp_[i].str() == "unset" && tmp_[i].dbl() == unset) result[(*ind)++] = unset;
                else
                {
                    auto fff = exec( tmp_[i] );
//                    cerr<<"Writing into position ["<<(*ind)<<"] --> "<<fff<<endl;
                    result[(*ind)++] = fff;
                }
            }
//            for(int i=0; i<*ind; ++i)
//            {
//                cerr<<">> result["<<i<<"] = "<<result_[i]<<endl;
//            }
        }
        void unary_base::eval_dbl(const std::vector<blop::var> &args,
                                  const std::vector<blop::var> &def_args,
                                  const std::vector<blop::var> &pars,
                                  std::vector<blop::var> &result, int *ind) const
        {
            int dummy_ind = 0;
            operand_->eval(args,def_args,pars,tmp_,&dummy_ind);
            for(int i=0; i<operand_->n_out(); ++i)
            {
                if(tmp_[i].dbl() == unset) result[(*ind)++].dbl(unset);
                else result[(*ind)++].dbl(exec_dbl( tmp_[i].dbl() ) );
            }
        }
        
        // ------------------- unary_c_function ----------------------------

        var unary_c_function::exec(const var &op)    const
        {
//            cerr<<"Evaluating "<<name_<<endl;
            double v = func_(op.dbl());
//            cerr<<v<<endl;
            if(!isfinite(v)) v = unset;
            return v;
        }
        

        // -------------------  format  ------------------------------------

        format::format(const function::core *func, const function::core *form)
        {
            func_ = func->clone();
            format_ = form->clone();
        }
        format::format(const format &rhs)
        {
            func_ = rhs.func_->clone();
            format_ = rhs.format_->clone();
        }
        
	void       format::eval(const std::vector<blop::var> &args,
                                const std::vector<blop::var> &def_args,
                                const std::vector<blop::var> &params,
                                std::vector<blop::var> &result, int *ind)  const
	{
            std::vector<blop::var> result_tmp(std::max(func_->n_out(),format_->n_out())+2); 
            int ind_tmp=0;
            const int n_func = func_->n_out();
            const int n_format = format_->n_out();
            func_->eval(args, def_args, params, result_tmp, &ind_tmp);
            format_->eval(args, def_args, params, result_tmp, &ind_tmp);
            
            for(int i_func=0; i_func<n_func; ++i_func)
            {
                var func_value = result_tmp[i_func];
                const var format_value = result_tmp[n_func + (i_func<n_format ? i_func : n_format-1)];
                func_value.reformat(format_value);
                result[(*ind)++] = func_value;
            }
	}

	void       format::eval_dbl(const std::vector<blop::var> &args,
                                    const std::vector<blop::var> &def_args,
                                    const std::vector<blop::var> &params,
                                    std::vector<blop::var> &result, int *ind)  const
	{
            std::vector<blop::var> result_tmp(std::max(func_->n_out(),format_->n_out())+2); 
            int ind_tmp=0;
            const int n_func = func_->n_out();
            const int n_format = format_->n_out();
            func_->eval(args, def_args, params, result_tmp, &ind_tmp);
            format_->eval(args, def_args, params, result_tmp, &ind_tmp);
            
            for(int i_func=0; i_func<n_func; ++i_func)
            {
                var func_value = result_tmp[i_func];
                const var format_value = result_tmp[n_func + (i_func<n_format ? i_func : n_format-1)];
                func_value.reformat(format_value);
                result[(*ind)++] = func_value;
            }
	}

        var format::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
        {
            var result = "\\mbox{format}(" & func_->sprint(pars,parvalue,variable_names,param_names) & "," & format_->sprint(pars,parvalue,variable_names,param_names) & ")";
            return result;
        }

        bool format::equals(const function::core *rhs) const
        {
            const format *p = dynamic_cast<const format *>(rhs);
            if(!p) return false;
            return func_->equals(p->func_) && format_->equals(p->format_);
        }        
        

        // -------------------- between_lines -------------------------------
        between_lines::between_lines(const function::core *start, const function::core *end)
        {
            start_ = start->clone();
            end_ = end->clone();
        }
        between_lines::between_lines(const between_lines &rhs)
        {
            start_ = rhs.start_->clone();
            end_ = rhs.end_->clone();
        }
        
	void       between_lines::eval(const std::vector<blop::var> &args,
                                       const std::vector<blop::var> &def_args,
                                       const std::vector<blop::var> &params,
                                       std::vector<blop::var> &result, int *ind)  const
	{
//            cerr<<endl;
//            cerr<<"between lines: ";
//            for(unsigned int i=0; i<args.size(); ++i) cerr<<args[i]<<" ";
//            cerr<<endl;

            std::vector<blop::var> result_tmp(std::max(start_->n_out(),end_->n_out())+2); // +2 is just for safety. Stupid, I know
            int ind_tmp=0;

            // Evaluate the end condition. If it is true, then set the flag to false and return false
            if(args.size() >= end_->nargs())
            {
//                cerr<<"Sufficient args to evaluate end condition"<<endl;
                end_->eval(args, def_args, params, result_tmp, &ind_tmp);
//                cerr<<"End condition: "<<result_tmp[0]<<endl;
                if(result_tmp[0])
                {
                    flag_ = false;
                    result[(*ind)++] = 0;
                    return;
                }
            }

            if(flag_)
            {
//                cerr<<"Flat is true, returning true"<<endl;
                result[(*ind)++] = 1;
                return;
            }

            // Evaluate the start condition. If it is true, then set the flag to true (i.e. following lines will be 
            ind_tmp = 0;
            if(args.size() >= start_->nargs())
            {
//                cerr<<"Sufficient args to evaluate start condition"<<endl;
                start_->eval(args, def_args, params, result_tmp, &ind_tmp);
//                cerr<<"Start condition: "<<result_tmp[0]<<endl;
                if(result_tmp[0]) flag_ = true;
            }
            result[(*ind)++] = 0;
	}

	void       between_lines::eval_dbl(const std::vector<blop::var> &args,
                                       const std::vector<blop::var> &def_args,
                                       const std::vector<blop::var> &params,
                                       std::vector<blop::var> &result, int *ind)  const
	{

            std::vector<blop::var> result_tmp(1);
            int ind_tmp=0;

            // Evaluate the end condition. If it is true, then set the flag to false and return false
            if(args.size() >= end_->nargs())
            {
                end_->eval(args, def_args, params, result_tmp, &ind_tmp);
                if(result_tmp[0])
                {
                    flag_ = false;
                    result[(*ind)++].dbl() = 0;
                    return;
                }
            }

            if(flag_)
            {
                result[(*ind)++].dbl() = 1;
                return;
            }

            // Evaluate the start condition. If it is true, then set the flag to true (i.e. following lines will be 
            if(args.size() >= start_->nargs())
            {
                start_->eval(args, def_args, params, result_tmp, &ind_tmp);
                if(result_tmp[0]) flag_ = true;
            }
            result[(*ind)++].dbl() = 0;
	}

        var between_lines::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
        {
            var result = "\\mbox{BETWEENLINES}(" & start_->sprint(pars,parvalue,variable_names,param_names) & "," & end_->sprint(pars,parvalue,variable_names,param_names) & ")";
            return result;
        }

        bool between_lines::equals(const function::core *rhs) const
        {
            const between_lines *p = dynamic_cast<const between_lines *>(rhs);
            if(!p) return false;
            return start_->equals(p->start_) && end_->equals(p->end_);
        }        

        // --------------------- periodic -----------------------------------

        periodic::periodic(const function::core *f, double x1, double x2)
            : func_(f->clone())
        {
            from_.push_back(x1);
            to_.push_back(x2);
        }
        periodic::periodic(const function::core *f, double x1, double x2, double y1, double y2)
            : func_(f->clone())
        {
            from_.push_back(x1);
            from_.push_back(y1);
            to_.push_back(x2);
            to_.push_back(y2);
        }
        periodic::periodic(const function::core *f, double x1, double x2, double y1, double y2, double z1, double z2)
            : func_(f->clone())
        {
            from_.push_back(x1);
            from_.push_back(y1);
            from_.push_back(z1);
            to_.push_back(x2);
            to_.push_back(y2);
            to_.push_back(z2);
        }
        periodic::periodic(const periodic &rhs)
            : func_(rhs.func_->clone()), from_(rhs.from_), to_(rhs.to_)
        {
        }
        periodic::~periodic()
        {
            delete func_;
        }

        
	void       periodic::eval(const std::vector<blop::var> &args_orig,
                                  const std::vector<blop::var> &def_args,
                                  const std::vector<blop::var> &params,
                                  std::vector<blop::var> &result, int *ind)  const
	{
            vector<var> args(args_orig);
            for(unsigned int i=0; i<args.size(); ++i)
            {
                // if a period is specified for this argument...
                if(i<from_.size())
                {
                    const double T = to_[i] - from_[i];
                    double a = args_orig[i].dbl();
                    while(a<from_[i]) a += T;
                    while(a>=to_[i])   a -= T;
                    args[i] = a;
                }
                else
                {
                    args[i] = args_orig[i];
                }
            }
            func_->eval(args, def_args, params, result, ind);
	}
	void       periodic::eval_dbl(const std::vector<blop::var> &args_orig,
                                      const std::vector<blop::var> &def_args,
                                      const std::vector<blop::var> &params,
                                      std::vector<blop::var> &result, int *ind)  const
	{
            vector<var> args(args_orig);
            for(unsigned int i=0; i<args.size(); ++i)
            {
                // if a period is specified for this argument...
                if(i<from_.size())
                {
                    const double T = to_[i] - from_[i];
                    double a = args_orig[i].dbl();
                    while(a<from_[i]) a += T;
                    while(a>=to_[i])   a -= T;
                    args[i].dbl(a);
                }
                else
                {
                    args[i].dbl(args_orig[i].dbl());
                }
            }
            func_->eval_dbl(args, def_args, params, result, ind);
	}

        int periodic::nargs() const { return func_->nargs(); }
        int periodic::npars() const { return func_->npars(); }
        bool periodic::uses_arg(int i) const { return func_->uses_arg(i); }
        bool periodic::uses_par(int i) const { return func_->uses_par(i); }
        var periodic::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
        {
            var result = "\\mbox{PERIODIC}(" & func_->sprint(pars,parvalue,variable_names,param_names) & ")";
            return result;
        }
        bool periodic::equals(const function::core *rhs) const
        {
            const periodic *p = dynamic_cast<const periodic *>(rhs);
            if(!p) return false;
            if(! (func_->equals(p->func_))) return false;
            if(from_.size() != p->from_.size()) return false;
            for(unsigned int i=0; i<from_.size(); ++i)
            {
                if(from_[i] != p->from_[i] || to_[i] != p->to_[i]) return false;
            }
            return true;
        }
        int periodic::n_out() const
        {
            return func_->n_out();
        }
        bool periodic::is_constant() const
        {
            return func_->is_constant();
        }
        

	// --------------------- ifelse -------------------------------------

        function::core *ifelse::create_derivative(int i) const
        {
            ifelse *result = new ifelse;
            result->condition_ = condition_->clone();
            result->iftrue_    = iftrue_->create_derivative(i);
            result->iffalse_   = iffalse_->create_derivative(i);
            return result;
        }

	void       ifelse::eval(const std::vector<blop::var> &args,
				const std::vector<blop::var> &def_args,
				const std::vector<blop::var> &params,
				std::vector<blop::var> &result, int *ind)  const
	{
	    bool istrue = false;
	    if(condition_->n_out() > 0)
	    {
		vector<var> cond(condition_->n_out()+2); // +2 is just for safety. stupid, I know.
		int dummy = 0;
		condition_->eval(args,def_args,params,cond,&dummy);
		istrue = cond[0];
	    }
	    if(istrue) iftrue_ ->eval(args,def_args,params,result,ind);
	    else       iffalse_->eval(args,def_args,params,result,ind);
	}
	void       ifelse::eval_dbl(const std::vector<blop::var> &args,
				    const std::vector<blop::var> &def_args,
				    const std::vector<blop::var> &params,
				    std::vector<blop::var> &result, int *ind)  const
	{
	    bool istrue = false;
	    if(condition_->n_out() > 0)
	    {
		vector<var> cond(condition_->n_out()+2); // +2 is just for safety. stupid, I know.
		int dummy = 0;
		condition_->eval_dbl(args,def_args,params,cond,&dummy);
		istrue = cond[0].dbl() != 0.0;
	    }
	    if(istrue) iftrue_->eval_dbl(args,def_args,params,result,ind);
	    else       iffalse_->eval_dbl(args,def_args,params,result,ind);
	}
	
	int ifelse::nargs() const { return std::max(condition_->nargs(), std::max(iftrue_->nargs(), iffalse_->nargs())); }
	int ifelse::npars() const { return std::max(condition_->npars(), std::max(iftrue_->npars(), iffalse_->npars())); }
	bool ifelse::uses_arg(int i) const { return condition_->uses_arg(i) || iftrue_->uses_arg(i) || iffalse_->uses_arg(i); }
	bool ifelse::uses_par(int i) const { return condition_->uses_par(i) || iftrue_->uses_par(i) || iffalse_->uses_par(i); }

	var ifelse::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
	{
	    return var("(") & condition_->sprint(pars,parvalue,variable_names,param_names) & "?" & iftrue_->sprint(pars,parvalue,variable_names,param_names) & ":" & iffalse_->sprint(pars,parvalue,variable_names,param_names) & ")";
	}

	bool ifelse::equals(const function::core *rhs) const
	{
	    const ifelse *ie = dynamic_cast<const ifelse*>(rhs);
	    if(!ie) return false;
	    return
		condition_->equals(ie->condition_) &&
		iftrue_   ->equals(ie->iftrue_   ) &&
		iffalse_  ->equals(ie->iffalse_  );
	}

	ifelse::ifelse(const ifelse &rhs)
	{
	    condition_ = rhs.condition_->clone();
	    iftrue_    = rhs.iftrue_->clone();
	    iffalse_   = rhs.iffalse_->clone();
	}

	ifelse::ifelse(const function &condition, const function &iftrue, const function &iffalse)
	{
	    condition_ = condition.base_->clone();
	    iftrue_ = iftrue.base_->clone();
	    iffalse_ = iffalse.base_->clone();
	}

	ifelse::~ifelse()
	{
	    delete condition_;
	    delete iftrue_;
	    delete iffalse_;
	}

	int ifelse::n_out() const
	{
	    return std::max(iftrue_->n_out(), iffalse_->n_out());
	}

	bool ifelse::is_constant() const
	{
	    return iffalse_->is_constant() && iftrue_->is_constant();
	}



	// --------------------- join -------------------------------------

	void       join_args::eval(const std::vector<blop::var> &args,
                                   const std::vector<blop::var> &def_args,
                                   const std::vector<blop::var> &params,
                                   std::vector<blop::var> &result, int *ind)  const
	{
            vector<var> dummy_vector(std::max(std::max(separator_->n_out(),from_->n_out()),to_->n_out()));
            int dummy_index = 0;

            separator_->eval(args,def_args,params,dummy_vector,&dummy_index);
            const var separator = dummy_vector[0];

            dummy_index = 0;
            from_->eval(args,def_args,params,dummy_vector,&dummy_index);
            int from = dummy_vector[0].integer()-1;
            if(from<0) from=0;

            dummy_index = 0;
            to_->eval(args,def_args,params,dummy_vector,&dummy_index);
            int to = dummy_vector[0].integer();
            if(to>(int)std::max(args.size(),def_args.size())) to = (int)std::max(args.size(),def_args.size());

            var res = "";
            for(int i=from; i<to; ++i)
            {
                if(i>from) res &= separator;
                res &= (i<(int)args.size()?args[i]:def_args[i]);
            }
            result[(*ind)++] = res;
	}
	void       join_args::eval_dbl(const std::vector<blop::var> &args,
                                       const std::vector<blop::var> &def_args,
                                       const std::vector<blop::var> &params,
                                       std::vector<blop::var> &result, int *ind)  const
	{
            eval(args,def_args,params,result,ind);
	}

	int join_args::nargs() const { return std::max(from_->nargs(), std::max(to_->nargs(), separator_->nargs())); }
	int join_args::npars() const { return std::max(from_->npars(), std::max(to_->npars(), separator_->npars())); }
	bool join_args::uses_arg(int i) const { return from_->uses_arg(i) || to_->uses_arg(i) || separator_->uses_arg(i); }
	bool join_args::uses_par(int i) const { return from_->uses_par(i) || to_->uses_par(i) || separator_->uses_par(i); }

	var join_args::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
	{
	    return var("join_args(") & separator_->sprint(pars,parvalue,variable_names,param_names) & "," & from_->sprint(pars,parvalue,variable_names,param_names) & "," & to_->sprint(pars,parvalue,variable_names,param_names) & ")";
	}

	bool join_args::equals(const function::core *rhs) const
	{
	    const join_args *j = dynamic_cast<const join_args*>(rhs);
	    if(!j) return false;
	    return
		from_->equals(j->from_) &&
		to_  ->equals(j->to_  ) &&
		separator_  ->equals(j->separator_  );
	}

        join_args::join_args(const join_args &rhs)
	{
	    from_ = rhs.from_->clone();
	    to_   = rhs.to_->clone();
	    separator_   = rhs.separator_->clone();
	}

        join_args::join_args(const function &separator, const function &from, const function &to)
	{
	    separator_ = separator.base_->clone();
	    from_      = from.base_->clone();
	    to_        = to.base_->clone();
	}

        join_args::~join_args()
	{
	    delete separator_;
	    delete from_;
	    delete to_;
	}

	bool join_args::is_constant() const
	{
	    return separator_->is_constant() && from_->is_constant() && to_->is_constant();
	}




        // -------------------  replace  -------------------------------

	void       replace::eval(const std::vector<blop::var> &args,
				const std::vector<blop::var> &def_args,
				const std::vector<blop::var> &params,
				std::vector<blop::var> &result, int *ind)  const
	{
            int n = std::max(base_->n_out(),std::max(from_->n_out(),to_->n_out()));
            vector<var> tmp(n);
            int dummy_ind = 0;
            from_->eval(args, def_args, params, tmp, &dummy_ind);
            const var from = tmp[0];
            dummy_ind = 0;
            to_->eval(args, def_args, params, tmp, &dummy_ind);
            const var to = tmp[0];

            const int orig_ind = *ind;
            base_->eval(args, def_args, params, result, ind);
            for(int i=orig_ind; i<*ind; ++i) result[i].replace(from,to);
	}
        
	void       replace::eval_dbl(const std::vector<blop::var> &args,
				const std::vector<blop::var> &def_args,
				const std::vector<blop::var> &params,
				std::vector<blop::var> &result, int *ind)  const
	{
            eval(args, def_args, params, result, ind);
        }

        var replace::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
        {
            return var("replace(") & from_->sprint(pars,parvalue,variable_names,param_names) & "," & to_->sprint(pars,parvalue,variable_names,param_names) & "," & base_->sprint(pars,parvalue,variable_names,param_names) & ")";
        }

        bool replace::equals(const function::core *rhs) const
        {
            const replace *r = dynamic_cast<const replace*>(rhs);
            if(!r) return false;
            return from_->equals(r->from_) && to_->equals(r->to_) && base_->equals(r->base_);
        }

        // -------------------------- substr ----------------------------------------------

	void       substr::eval(const std::vector<blop::var> &args,
				const std::vector<blop::var> &def_args,
				const std::vector<blop::var> &params,
				std::vector<blop::var> &result, int *ind)  const
	{
            int n = std::max(string_->n_out(),std::max(from_->n_out(),to_->n_out()));
            vector<var> tmp(n);
            int dummy_ind = 0;
            from_->eval(args, def_args, params, tmp, &dummy_ind);
            const int from = atoi(tmp[0].c_str());
            dummy_ind = 0;
            to_->eval(args, def_args, params, tmp, &dummy_ind);
            const int to   = atoi(tmp[0].c_str());

            const int orig_ind = *ind;
            string_->eval(args, def_args, params, result, ind);
            for(int i=orig_ind; i<*ind; ++i) result[i] = result[i].str().substr(from,to);
	}
        
	void       substr::eval_dbl(const std::vector<blop::var> &args,
				const std::vector<blop::var> &def_args,
				const std::vector<blop::var> &params,
				std::vector<blop::var> &result, int *ind)  const
	{
            eval(args, def_args, params, result, ind);
        }

        var substr::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
        {
            return var("substr(") & string_->sprint(pars,parvalue,variable_names,param_names) & "," & from_->sprint(pars,parvalue,variable_names,param_names) & "," & to_->sprint(pars,parvalue,variable_names,param_names) & ")";
        }

        bool substr::equals(const function::core *rhs) const
        {
            const substr *r = dynamic_cast<const substr*>(rhs);
            if(!r) return false;
            return from_->equals(r->from_) && to_->equals(r->to_) && string_->equals(r->string_);
        }
        
        
	// -------------------  bessel ---------------------------------


	void bessel::setup_bessel_core(int nu, const function &a,
				       bessel::type type, function &result)
	{
	    // this function should be very similar to function::init_unary_
	    result.print_param_value_ = a.print_param_value_;
	    result.base_ = new bessel(a.base_, type, nu);
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
	    operand_->eval(args,def_args,params,tmp_,&dummy);
	    const double x = tmp_[0].dbl();
	    
	    double res = 0;
            if(x == unset) res = unset;
            else
            {
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
	    operand_->eval_dbl(args,def_args,params,tmp_,&dummy);
	    const double x = tmp_[0].dbl();
	    
	    double res = 0;
            if(x == unset) res = unset;
            else
            {
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
            }
	    result[(*ind)++] = res;
	}		
	
	function::core* bessel::create_my_derivative() const
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

	function::core *step_func::clone() const
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
	    const int p=std::max((long int)(pos-x_.begin()-1),(long int)0);
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
	    const int p=std::max((long int)(pos-x_.begin()-1),(long int)0);
	    for(unsigned int i=0; i<y_.size(); ++i)
	    {
		result[(*ind)++].dbl() = y_[i][p];
	    }
	}

	function::core *step_func::create_derivative(int i) const
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
	    int dummy_ind = 0;
	    low_->eval_dbl(args,def_args,pars,tmp_,&dummy_ind);
	    const double low = tmp_[0].dbl();

	    dummy_ind = 0;
	    high_->eval_dbl(args,def_args,pars,tmp_,&dummy_ind);
	    const double high = tmp_[0].dbl();

            if(low == unset || high == unset)
            {
		result[(*ind)++].dbl(0);
		return;
            }

	    if(args[0].dbl() < low ||
	       (!low_in_ && args[0].dbl() <= low) ||
	       high < args[0].dbl() ||
	       (!high_in_ && high <= args[0].dbl()))
	    {
		result[(*ind)++].dbl(0);
		return;
	    }
	    result[(*ind)++].dbl(1);
	}


	void      char_func::eval(const std::vector<blop::var> &args,
				  const std::vector<blop::var> &def_args,
				  const std::vector<blop::var> &pars,
				  std::vector<blop::var> &result,
				  int *ind) const
	{
	    int dummy_ind = 0;
	    low_->eval(args,def_args,pars,tmp_,&dummy_ind);
	    const double low = tmp_[0].dbl();

	    dummy_ind = 0;
	    high_->eval(args,def_args,pars,tmp_,&dummy_ind);
	    const double high = tmp_[0].dbl();

            if(low == unset || high == unset)
            {
		result[(*ind)++] = 0;
		return;
            }

	    if(args[0].dbl() < low ||
	       (!low_in_ && args[0].dbl() <= low) ||
	       high < args[0].dbl() ||
	       (!high_in_ && high <= args[0].dbl()))
	    {
		result[(*ind)++] = 0;
		return;
	    }
	    result[(*ind)++] = 1;
	}
	

	// -----------------  function parameter -----------------------------

	var funcparameter::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
	{
            auto pos = param_names.find(parameter_index_+1);
            if(pos != param_names.end()) return (*pos).second;

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

	bool arg::uses_arg(int i) const
	{
	    return ( i-1 == arg_index_);
	}


	// ----------------  component ---------------------------------------

	bool component::equals(const function::core *rhs) const
	{
	    if(multiple *m = dynamic_cast<multiple*>(base_))
	    {
		int n = 0;
		for(unsigned int i=0; i<m->base_.size(); ++i)
		{
		    if(n+m->base_[i]->n_out() > index_)
		    {
			if(m->base_[i]->n_out()==1 &&
			   (m->base_[i]->equals(rhs) || rhs->equals(m->base_[i]))) return true;
			break;
		    }
		    n += m->base_[i]->n_out();
		}
	    }
	    else
	    {
		if(base_->n_out() == 1 && (base_->equals(rhs) || rhs->equals(base_))) return true;
	    }
	    return false;
	}

	component::component(const function::core &b, int i) : tmp_(b.n_out())
	{
	    index_ = i;
	    base_ = b.clone();
	    if(index_ >= base_->n_out())
	    {
		warning::print("Function component is out of range. Using index=0 instead","component::component(const base &, int index)");
		index_ = 0;
	    }
	    //if((unsigned int)(base_->n_out()) > tmp_.size()) tmp_.resize(base_->n_out());
	}

	component::~component()
	{
	    delete base_;
	}

	function::core *component::clone() const
	{
	    return new component(*base_,index_);
	}

	void component::eval(const std::vector<blop::var> &args,
			     const std::vector<blop::var> &def_args,
			     const std::vector<blop::var> &pars,
			     std::vector<blop::var> &result,
			     int *ind) const
	{
//            cerr<<"Evaluating component "<<index_<<endl;
	    int dummy = 0;
//            cerr<<"BASE: "<<base_->sprint(pars,false)<<endl;
	    base_->eval(args,def_args,pars,tmp_,&dummy);
//            cerr<<"Dummy after call: "<<dummy<<endl;
//            cerr<<" --> "<<tmp_[index_]<<endl;
	    result[(*ind)++] = tmp_[index_];
	}

	void component::eval_dbl(const std::vector<blop::var> &args,
				 const std::vector<blop::var> &def_args,
				 const std::vector<blop::var> &pars,
				 std::vector<blop::var> &result,
				 int *ind) const
	{
	    int dummy = 0;
	    base_->eval_dbl(args,def_args,pars,tmp_,&dummy);
	    result[(*ind)++].dbl(tmp_[index_].dbl());
	}

	var component::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
	{
            function_core::multiple *m = dynamic_cast<function_core::multiple*>(base_);
            if(m)
            {
                if(index_ < m->base_.size()) return m->base_[index_]->sprint(pars,parvalue,variable_names,param_names);
                else return "outofrange";
            }

	    var result = var("(") & base_->sprint(pars,parvalue,variable_names,param_names) & var(")[") & var(index_) & var("]");
	    return result;
	}

	function::core *component::create_derivative(int i) const
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
	    if((unsigned int)(wrapper_->n_out()*2) > tmp_.size()) tmp_.resize(wrapper_->n_out()*2);
	}

	bool cfunc::equals(const function::core *o) const
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

	function::core* cfunc::create_derivative(int) const
	{
	    return 0;
	}

	var cfunc::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names)  const
	{
	    if(wrapper_ == 0) return "UNINITIALIZED_CFUNC";
	    var result = wrapper_->name();
	    result &= var("(");
	    result &= ")";
	    return result;
	}


	bool cfunc::uses_arg(int arg_ind) const
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

#ifdef __MAKECINT__        
	cfunc::cfunc(void *p)
	{
	    init_(p);
	    init_arrays_();
	}
#endif

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

        cfunc::cfunc(void (*p)(double*,double*,double*),int nargs, int npars, int nout)
        {
            wrapper_ = new void_func_doublep_doublep_doublep(p,nargs,npars,nout);
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

	function::core *cfunc::clone() const
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


	function::core *Atan2::create_derivative_spec(function::core *leftderiv, function::core *rightderiv) const
	{
            // here r means right (2nd), l means left (1st) argument
            // d/dl atan2(l,r) =  r/(r^2+l^2)
            // d/dr atan2(l,r) = -l/(r^2+l^2)

            Add *denominator = new Add;
            {
                Pow *pow1 = new Pow;
                pow1->left(left_->clone());
                pow1->right(new constant(2.0));
                denominator->left(pow1);
                Pow *pow2 = new Pow;
                pow2->left(right_->clone());
                pow2->right(new constant(2.0));
                denominator->right(pow2);
            }

            Sub *nominator = new Sub;
            {
                nominator->left  (new Mul(right_,leftderiv));
                nominator->right(new Mul(left_,rightderiv));
            }

            Div *result = new Div;
            result->left(nominator);
            result->right(denominator);
            return result;

	}

	function::core *Sub::create_derivative_spec(function::core *leftderiv, function::core *rightderiv) const
	{
	    constant *c;
	    if((c=dynamic_cast<constant*>(leftderiv)) != 0 && c->value_ == 0.0)
	    {
		delete leftderiv;
		return new Neg(rightderiv);
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


	function::core *Div::create_derivative_spec(function::core *leftderiv, function::core *rightderiv) const
	{
	    if(dynamic_cast<const constant *>(right_))
	    {
		Div *result = new Div;
		result->left_ = leftderiv;
		result->right_ = right_->clone();
		delete rightderiv;
		return result;
	    }

	    function::core *a = left_;
	    function::core *b = right_;

	    function::core *aprimeb = 0;
	    {
		function::core *aprime = leftderiv;
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
	    function::core *abprime = 0;
	    {
		function::core *bprime = rightderiv;
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
	    
	    function::core *nominator = 0;
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

	function::core *Sin::create_my_derivative() const
	{
	    return new Cos(operand_);
	}
	function::core *Cos::create_my_derivative() const
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

	function::core* Ipow::create_my_derivative() const
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


	function::core* Pow::create_derivative_spec(function::core *aprime, function::core *bprime) const
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

	function::core *Tan::create_my_derivative() const
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

	function::core *Atan::create_my_derivative() const
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

	function::core *Cot::create_my_derivative() const
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


	function::core *Acot::create_my_derivative() const
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

	function::core *Asin::create_my_derivative() const
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

	function::core *Acos::create_my_derivative() const
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

	bool argument_subst::uses_arg(int n) const
	{
	    return args_->uses_arg(n);
	}
	bool argument_subst::uses_par(int n) const
	{
	    return args_->uses_par(n);
	}

	void argument_subst::init_tmp_()
	{
	    tmp_.resize(args_->n_out());
	}

	bool argument_subst::equals(const function::core *o) const
	{
	    const argument_subst *c = dynamic_cast<const argument_subst *>(o);
	    if(c == 0) return false;
	    if(! (base_->equals(c->base_)) ) return false;
	    if(! (args_->equals(c->args_)) ) return false;
	    return true;
	}

	var argument_subst::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
	{
	    if(!base_ || !args_) return "";
	    var result = base_->sprint(pars, parvalue,variable_names,param_names);
	    result &= "(";
            result &= args_->sprint(pars, parvalue,variable_names,param_names);
	    result &= ")";
	    return result;
	}

	var argument_subst::sprint_latex(const std::vector<blop::var> &pars, bool parvalue,
					 const var &x, const var &y, const var &z) const
	{
	    if(!base_ || !args_) return "";
	    var result = base_->sprint_latex(pars, parvalue, x, y, z);
	    result &= "(";
	    result &= args_->sprint_latex(pars, parvalue, x, y, z);
	    result &= ")";
	    return result;
	}


	function::core* argument_subst::create_derivative(int ider) const
	{
            if(args_->is_constant())
            {
                if(base_->n_out()>1)
                {
                    multiple *m = new multiple;
                    m->base_.resize(base_->n_out());
                    for(int i=0; i<base_->n_out(); ++i) m->base_[i] = new constant(0.0);
                    return m;
                }
                else return new constant(0.0);
            }

            if(base_->n_out() > 1)
            {
                cerr<<"FIXME: can not create derivative for an argument-substituted multi-valued function"<<endl;
                return 0;
            }

	    function::core *result = 0;

            // Derivation by a parameter
            if(ider<0)
            {
                argument_subst *asub = new argument_subst;
                asub->base_ = base_->create_derivative(ider);
                asub->args_ = args_->clone();
                result = asub;
            }

            function::core *arg_deriv = args_->create_derivative(ider);
            
            if(arg_deriv->n_out() != args_->n_out())
            {
                warning::print("Derivative doesn't have the same number of output variables as original function",
                               "argument_subst::create_derivative(int)");
                return 0;
            }

	    for(int i=0; i<base_->nargs(); ++i)
	    {
		function::core *partial_deriv = base_->create_derivative(i+1);
                // Failed to create a partial derivative
		if(partial_deriv == 0)
		{
		    if(result) delete result;
		    return 0;
		}

                // If the base function's ith partial derivative is 0, skip this term
                {
                    constant *c = dynamic_cast<constant*>(partial_deriv);
                    if(c && c->value_ == 0.0) continue;
                }

                {
                    constant *c = dynamic_cast<constant*>(arg_deriv); 
                    if(c && c->value_ == 0.0) continue;
                }

                Mul *term = new Mul;
                argument_subst *asub = new argument_subst;
                term->left_ = asub;
                asub->base_ = partial_deriv;
                asub->args_ = args_->clone();
                term->right_ = 0;
                if(arg_deriv->n_out() == 1)
                {
                    term->right_ = arg_deriv->clone();
                }
                else if(multiple *m = dynamic_cast<multiple*>(arg_deriv))
                {
                    bool ok = true;
                    // Check if this is a multiple function of single functions (i.e. each
                    // component has only 1 further component
                    for(unsigned int a=0; a<m->base_.size(); ++a)
                    {
                        if(m->base_[a]->n_out() != 1)
                        {
                            ok=false;
                            break;
                        }
                    }
                    // If yes, then multiply by that component
                    if(ok)
                    {
                        {
                            constant *c = dynamic_cast<constant*>(m->base_[i]);
                            if(c && c->value_ == 0.0) continue;
                        }
                        term->right_ = m->base_[i]->clone();
                    }
                }
                if(term->right_ == 0)
                {
                    term->right_ = new component(*arg_deriv,i);
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


/*
	    for(int i=0; i<args_->n_out(); ++i)
	    {
		function::core *partial_deriv = base_->create_derivative(i+1);
		if(partial_deriv == 0)
		{
		    if(result) delete result;
		    return 0;
		}

                Mul *term = new Mul;
                argument_subst *asub = new argument_subst;
                term->left_ = asub;
                asub->base_ = partial_deriv;
                asub->args_ = args_->clone();
                term->right_ = 0;
                if(arg_deriv->n_out() == 1)
                {
                    term->right_ = arg_deriv->clone();
                }
                else if(multiple *m = dynamic_cast<multiple*>(arg_deriv))
                {
                    bool ok = true;
                    for(unsigned int a=0; a<m->base_.size(); ++a) if(m->base_[a]->n_out() != 1) { ok=false; break; }
                    if(ok) term->right_ = m->base_[i]->clone();
                }
                if(term->right_ == 0)
                {
                    term->right_ = new component(*arg_deriv,i);
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
*/

	    return result;
/* fixme
	    function::core *result = 0;
	    for(unsigned int i=0; i<args_.size(); ++i)
	    {
		function::core *partial_deriv = base_->create_derivative(i+1);
		if(partial_deriv == 0)
		{
		    if(result) delete result;
		    return 0;
		}
		function::core *arg_deriv = args_[i]->create_derivative(ider);
		if(arg_deriv == 0)
		{
		    if(result) delete result;
		    return 0;
		}

		function::core *term = 0;
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
*/
	}

	argument_subst::argument_subst(function::core *f, function::core *arg)
	{
	    base_ = f->clone();
	    args_ = arg->clone();
            args_->n_out_hint(base_->nargs());
	    init_tmp_();
	}

	argument_subst::argument_subst(const argument_subst &o)
	{
	    base_ = o.base_->clone();
	    args_ = o.args_->clone();
            args_->n_out_hint(base_->nargs());
	    init_tmp_();
	}

	argument_subst::~argument_subst()
	{
	    delete base_;
	    delete args_;
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
	    args_->eval(arg,def_args,params,tmp_,&runind);
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
	    args_->eval_dbl(args,def_args,pars,tmp_,&runind);
	    base_->eval_dbl(tmp_, def_args, pars, result, ind);
	}

	int argument_subst::nargs() const
	{
	    if(nargs_>=0) return nargs_;
	    return args_->nargs();
	}
	int argument_subst::npars() const
	{
	    if(npars_>=0) return npars_;
	    return std::max(base_->npars(),args_->npars());
	}

	int argument_subst::n_out() const
	{
	    return base_->n_out();
	}


	function::core *Sinh::create_my_derivative() const { return new Cosh(operand_); }
	function::core *Cosh::create_my_derivative() const { return new Sinh(operand_); }
	function::core *Asinh::create_my_derivative() const
	{ cerr<<"Asinh derivative not yet implemented"<<endl; return new constant(0.0); }
	function::core *Acosh::create_my_derivative() const
	{ cerr<<"Acosh derivative not yet implemented"<<endl; return new constant(0.0); }
	function::core *Tanh::create_my_derivative() const
	{ cerr<<"Tanh derivative not yet implemented"<<endl; return new constant(0.0); }
	function::core *Atanh::create_my_derivative() const
	{ cerr<<"Tanh derivative not yet implemented"<<endl; return new constant(0.0); }
	function::core *Erf::create_my_derivative() const
	{ cerr<<"Erf derivative not yet implemented"<<endl; return new constant(0.0); }

	double cot(double a) { return 1.0/::tan(a); }
	double acot(double a) { return ::atan(1/a); }

        // -------------------- function_core::col ----------------------------------

        int col::nargs() const
        {
            for(unsigned int i=0; i<function::column_names_.size(); ++i)
            {
                if(function::column_names_[i] == name_) return i+1;
            }
            return 0;
        }

        bool col::uses_arg(int argno) const
        {
            if(0<argno && argno<=(int)function::column_names_.size() &&
               function::column_names_[argno-1] == name_) return true;
            return false;
        }
        
        void col::eval(const std::vector<blop::var> &arg,
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

        void col::eval_dbl(const std::vector<blop::var> &args,
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

        
        function::core* col::create_derivative(int i) const
        {
            if(0<i && i<=(int)function::column_names_.size() &&
               function::column_names_[i-1] == name_) return new constant(0.0);
            return new constant(0.0);
        }
        bool col::equals(const function::core *o) const
        {
            const col *c = dynamic_cast<const col *>(o);
            if(c && c->name_ == name_) return true;
            return false;
        }
        

        // ----------------------- date2epoch -------------------------------------------

	void date2epoch::eval(const vector<var> &args,
                              const std::vector<blop::var> &def_args,
                              const vector<var> &pars,
                              vector<var> &result, int *ind) const
        {
            std::chrono::system_clock::time_point t = date2timepoint(args[0],format_);
            result[(*ind)++] = chrono::duration_cast<chrono::seconds>(t.time_since_epoch()).count();
        }
	void date2epoch::eval_dbl(const vector<var> &args,
                  const std::vector<blop::var> &def_args,
                  const vector<var> &pars,
                  vector<var> &result, int *ind) const
                {
                    std::chrono::system_clock::time_point t = date2timepoint(args[0],format_);
                    result[(*ind)++].dbl() = chrono::duration_cast<chrono::seconds>(t.time_since_epoch()).count();
                }

        // ------------------- function_core::binary_operator -----------------------

	var binary_operator::sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
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
	    result &= left_->sprint(pars, parvalue,variable_names,param_names);
	    if(left_paren) result &= ")";
	    result &= name_;
	    if(right_paren) result &= "(";
	    result &= right_->sprint(pars, parvalue,variable_names,param_names);
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

	contained_in::contained_in(function::core *a) : tmp_(a->n_out())
	{
	    in_this_ = a->clone();
            //if(in_this_->n_out() > tmp_.size()) tmp_.resize(in_this_->n_out());
	}

	contained_in::contained_in(const contained_in &o) : tmp_(o.in_this_->n_out())
	{
	    in_this_ = o.in_this_->clone();
            //if(in_this_->n_out() > tmp_.size()) tmp_.resize(in_this_->n_out());
	}

	contained_in::~contained_in()
	{
	    delete in_this_;
	}

	// evaluate
	void contained_in::eval(const std::vector<blop::var> &args,
				const std::vector<blop::var> &def_args,
				const std::vector<blop::var> &pars,
				std::vector<blop::var> &result,
				int *ind) const
	{
	    vector<var> orig_args = args;

	    int runind = 0;
	    in_this_->eval(orig_args, def_args, pars, tmp_, &runind);

	    for(int c=0; c<runind; ++c)
	    {
		if(orig_args[0].str() == tmp_[c].str())
		{
		    result[(*ind)++] = "1";
		    return;
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
	    return in_this_->nargs();
	}
	int contained_in::npars() const
	{
	    if(npars_>=0) return npars_;
	    return in_this_->npars();
	}

	var contained_in::sprint(const std::vector<blop::var> &pars, bool print_parvalue, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
	{
	    var result = "contained_in{";
	    result &= in_this_->sprint(pars,print_parvalue,variable_names,param_names);
	    result &= "}";
	    return result;
	}


        // ------------------------- find_root ----------------------------------
        find_root::find_root(const function::core *func,
                             const function::core *x1,
                             const function::core *x2,
                             const function::core *epsilon)

	{
	    func_ = func->clone();
	    x1_ = x1->clone();
	    x2_ = x2->clone();
	    epsilon_ = epsilon->clone();
	}
	find_root::find_root(const find_root &rhs)
	{
	    func_ = rhs.func_->clone();
	    x1_   = rhs.x1_->clone();
	    x2_   = rhs.x2_  ->clone();
	    epsilon_ = rhs.epsilon_->clone();
	}
	int find_root::nargs() const
	{
	    if(nargs_>=0) return nargs_;
	    return std::max(std::max(x1_->nargs(),x2_->nargs()),std::max(func_->nargs(),epsilon_->nargs()));
	}
	int find_root::npars() const
	{
	    if(npars_>=0) return npars_;
	    return std::max(std::max(x1_->npars(),x2_->npars()),std::max(func_->npars(),epsilon_->npars()));
	}
	bool find_root::uses_arg(int argno) const
	{
	    if(func_->uses_arg(argno)) return true;
	    if(x1_->uses_arg(argno)) return true;
	    if(x2_->uses_arg(argno)) return true;
	    if(epsilon_->uses_arg(argno)) return true;
	    return false;
	}
	bool find_root::uses_par(int parno) const
	{
	    if(func_->uses_par(parno)) return true;
	    if(x1_->uses_par(parno)) return true;
	    if(x2_->uses_par(parno)) return true;
	    if(epsilon_->uses_par(parno)) return true;
	    return false;
	}
	int find_root::n_out() const
	{
	    return 1;
	}
	var find_root::sprint(const std::vector<blop::var> &p, bool v, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
        {
            return "find_root::sprint is not yet implemented";
        }
	var find_root::sprint_latex(const std::vector<blop::var> &p, bool v,
					    const var &x, const var &y, const var &z) const
	{
            return "find_root::sprint_latex is not yet implemented";
        }
        
	void find_root::eval(const std::vector<blop::var> &args,
                             const std::vector<blop::var> &def_args,
                             const std::vector<blop::var> &pars,
                             std::vector<blop::var> &res,
                             int *ind) const
	{
	    int nout_x1   = x1_->n_out();
	    int nout_x2   = x2_->n_out();
	    int nout_epsilon = epsilon_->n_out();

	    std::vector<blop::var>
		x1(nout_x1),
		x2(nout_x2),
		epsilon(nout_epsilon);

	    {
		int ind_tmp = 0;
		x1_->eval(args, def_args, pars, x1, &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		x2_->eval(args, def_args, pars, x2, &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		epsilon_->eval(args, def_args, pars, epsilon, &ind_tmp);
	    }

            function fff(func_);
            fff.params(args);
            fff.print_param_value(true);

            const double tmp = blop::find_root(fff,x1[0],x2[0],epsilon[0]);
            res[(*ind)++] = tmp;
	}

	void find_root::eval_dbl(const std::vector<blop::var> &args,
                                 const std::vector<blop::var> &def_args,
                                 const std::vector<blop::var> &pars,
                                 std::vector<blop::var> &res,
                                 int *ind) const
	{
	    int nout_x1   = x1_->n_out();
	    int nout_x2   = x2_->n_out();
	    int nout_epsilon = epsilon_->n_out();

	    std::vector<blop::var>
		x1(nout_x1),
		x2(nout_x2),
		epsilon(nout_epsilon);

	    {
		int ind_tmp = 0;
		x1_->eval_dbl(args, def_args, pars, x1, &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		x2_->eval_dbl(args, def_args, pars, x2, &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		epsilon_->eval_dbl(args, def_args, pars, epsilon, &ind_tmp);
	    }

            function fff(func_);
            fff.params(args);

            std::vector<blop::var> result(1);
            res[(*ind)++].dbl() = blop::find_root(fff,x1[0],x2[0],epsilon[0]);
	}

        

	// ------------------------- value_in_interval --------------------------
	value_in_interval::value_in_interval(const function::core *func,
					     const function::core *from,
					     const function::core *to,
					     const function::core *step,
					     const std::string &name)
	{
	    func_ = func->clone();
	    from_ = from->clone();
	    to_   = to->clone();
	    step_ = step->clone();
	    name_ = name;
	}
	value_in_interval::value_in_interval(const value_in_interval &rhs)
	{
	    func_ = rhs.func_->clone();
	    from_ = rhs.from_->clone();
	    to_   = rhs.to_  ->clone();
	    step_ = rhs.step_->clone();
	    name_ = rhs.name_;
	}
	int value_in_interval::nargs() const
	{
	    if(nargs_>=0)
	    {
		cerr<<"nargs set by user?"<<endl;
		return nargs_;
	    }
	    return std::max(std::max(from_->nargs(),to_->nargs()),std::max(func_->nargs(),step_->nargs()));
	}
	int value_in_interval::npars() const
	{
	    if(npars_>=0) return npars_;
	    return std::max(std::max(from_->npars(),to_->npars()),step_->npars());
	}
	bool value_in_interval::uses_arg(int argno) const
	{
	    if(from_->uses_arg(argno)) return true;
	    if(to_->uses_arg(argno)) return true;
	    if(step_->uses_arg(argno)) return true;
	    return false;
	}
	bool value_in_interval::uses_par(int parno) const
	{
	    if(from_->uses_par(parno)) return true;
	    if(to_->uses_par(parno)) return true;
	    if(step_->uses_par(parno)) return true;
	    return false;
	}
	int value_in_interval::n_out() const
	{
	    return func_->n_out();
	}
	var value_in_interval::sprint(const std::vector<blop::var> &p, bool v, std::map<int,blop::var> variable_names, std::map<int,blop::var> param_names) const
	{
	    var result = var(name_) & "(";
	    result &= (func_->n_out()>1?"[":"") & func_->sprint(p,v,variable_names,param_names) & (func_->n_out()>1?"]":"");
	    result &= ",";
	    result &= (from_->n_out()>1?"[":"") & from_->sprint(p,v,variable_names,param_names) & (from_->n_out()>1?"]":"");
	    result &= ",";
	    result &= (to_->n_out()>1?"[":"") & to_->sprint(p,v,variable_names,param_names) & (to_->n_out()>1?"]":"");
	    result &= ",";
	    result &= (step_->n_out()>1?"[":"") & step_->sprint(p,v,variable_names,param_names) & (step_->n_out()>1?"]":"");
	    result &= ")";
	    return result;
	}
	var value_in_interval::sprint_latex(const std::vector<blop::var> &p, bool v,
					    const var &x, const var &y, const var &z) const
	{
	    var result = var("\\mathrm{") & var(name_) & "}\\left(";

	    if(func_->n_out()>1) result &= "\\left[";
	    result &= func_->sprint_latex(p,v,x,y,z);
	    if(func_->n_out()>1) result &= "\\right]";

	    result &= ",";

	    if(from_->n_out()>1) result &= "\\left[";
	    result &= from_->sprint_latex(p,v,x,y,z);
	    if(from_->n_out()>1) result &= "\\right]";

	    result &= ",";

	    if(to_->n_out()>1) result &= "\\left[";
	    result &= to_->sprint_latex(p,v,x,y,z);
	    if(to_->n_out()>1) result &= "\\right]";

	    result &= ",";

	    if(step_->n_out()>1) result &= "\\left[";
	    result &= step_->sprint_latex(p,v,x,y,z);
	    if(step_->n_out()>1) result &= "\\right]";

	    result &= "\\right)";

	    return result;
	}
	void value_in_interval::eval(const std::vector<blop::var> &args,
				     const std::vector<blop::var> &def_args,
				     const std::vector<blop::var> &pars,
				     std::vector<blop::var> &res,
				     int *ind) const
	{
	    //int nout_func = func_->n_out();
	    int nout_from = from_->n_out();
	    int nout_to   = to_  ->n_out();
	    int nout_step = step_->n_out();

	    std::vector<blop::var>
		from(nout_from),
		to  (nout_to  ),
		step(nout_step);

	    {
		int ind_tmp = 0;
		from_->eval(args, def_args, pars, from, &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		to_  ->eval(args, def_args, pars, to  , &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		step_->eval(args, def_args, pars, step, &ind_tmp);
	    }
		
	    from.resize(std::min(nout_from,nout_to));
	    to  .resize(std::min(nout_from,nout_to));
	    step.resize(std::min(nout_from,nout_to),0.0);

            for(unsigned int i=0; i<from.size(); ++i)
            {
                if(to[i].dbl() < from[i].dbl())
                {
                    for(int i=0; i<n_out(); ++i)
                    {
                        res[(*ind)++] = 0;
                    }
                    return;
                }
            }

	    for(unsigned int i=0; i<step.size(); ++i)
	    {
		if(step[i].dbl() == 0.0) step[i] = std::abs(to[i].dbl()-from[i].dbl())/100.0;
	    }
		
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
		func_->eval(func_args, def_args, pars, current_result, &ind_tmp);

		accumulate_result(count,func_args, def_args, pars, step, current_result, final_result);

		could_increment = false;
		for(unsigned int i=0; i<func_args.size(); ++i)
		{
		    // If this argument is not to be incremented, skip it.
		    if(from[i].dbl() == to[i].dbl()) continue;

		    // otherwise increment the given argument by the
		    // corresponding step
		    func_args[i] += step[i];

		    // If 'to' (upper limit of the loop of this argument)
		    // has not been reached, everything is good, stop here
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
	    //int nout_func = func_->n_out();
	    int nout_from = from_->n_out();
	    int nout_to   = to_  ->n_out();
	    int nout_step = step_->n_out();

	    std::vector<blop::var>
		from(nout_from),
		to  (nout_to  ),
		step(nout_step);

	    {
		int ind_tmp = 0;
		from_->eval_dbl(args, def_args, pars, from, &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		to_  ->eval_dbl(args, def_args, pars, to  , &ind_tmp);
	    }
	    {
		int ind_tmp = 0;
		step_->eval_dbl(args, def_args, pars, step, &ind_tmp);
	    }
		
	    from.resize(std::min(nout_from,nout_to));
	    to  .resize(std::min(nout_from,nout_to));
	    step.resize(std::min(nout_from,nout_to),0.0);

	    for(unsigned int i=0; i<step.size(); ++i)
	    {
		if(step[i].dbl() == 0.0) step[i].dbl((to[i].dbl()-from[i].dbl())/100.0);
	    }
		
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
		func_->eval_dbl(func_args, def_args, pars, current_result, &ind_tmp);

		accumulate_result_dbl(count,func_args, def_args, pars, step, current_result, final_result);

		could_increment = false;
		for(unsigned int i=0; i<func_args.size(); ++i)
		{
		    // If this argument is not to be incremented, skip it.
		    if(from[i].dbl() == to[i].dbl()) continue;

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
