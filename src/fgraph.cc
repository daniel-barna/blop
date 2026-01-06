#include "fgraph.h"
#include "function.h"
#include "constants.h"
#include "axis.h"
#include "exc.hh"
#include "warning.h"
#include "frame.h"
#include "ignore.h"
#include "function_core.hh"
#include "global.h"
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <limits>

namespace blop
{
    int fgraph::default_nsamples1_ = 100;
    int fgraph::default_nsamples2_ = 100;
    void fgraph::default_nsamples(int i) { default_nsamples1_ = default_nsamples2_ = i; }
    void fgraph::default_nsamples1(int i) { default_nsamples1_ = i; }
    void fgraph::default_nsamples2(int i) { default_nsamples2_ = i; }

    bool   fgraph::default_p1log_ = false;
    bool   fgraph::default_p2log_ = false;
    double fgraph::default_p1min_ = unset;
    double fgraph::default_p1max_ = unset;
    double fgraph::default_p2min_ = unset;
    double fgraph::default_p2max_ = unset;
    void fgraph::default_p1log(bool f) { default_p1log_ = f; }
    void fgraph::default_p2log(bool f) { default_p2log_ = f; }
    void fgraph::default_p1min(double v) { default_p1min_ = v; }
    void fgraph::default_p1max(double v) { default_p1max_ = v; }
    void fgraph::default_p1range(double v1,double v2) { default_p1min_ = v1; default_p1max_ = v2; }
    void fgraph::default_p2min(double v) { default_p2min_ = v; }
    void fgraph::default_p2max(double v) { default_p2max_ = v; }
    void fgraph::default_p2range(double v1,double v2) { default_p2min_ = v1; default_p2max_ = v2; }

    var fgraph::default_legend_ = "%f";
    void fgraph::default_legend(const var &f) { default_legend_ = f; }

    length fgraph::default_linewidth_(new length::base_id_t(terminal::LW));
    void fgraph::default_linewidth(const length &l) {default_linewidth_ = l;}

    color  fgraph::default_linecolor_(0,0,0);
    void fgraph::default_linecolor(const color &c) { default_linecolor_ = c; }
    
    sym::linestyle    fgraph::default_linestyle_ = sym::solid;
    void fgraph::default_linestyle(sym::linestyle i) { default_linestyle_ = i; }

    length fgraph::default_pointsize_(new length::base_id_t(terminal::PS));
    void fgraph::default_pointsize(const length &l) { default_pointsize_ = l; }

    point_drawer *fgraph::default_point_drawer_ = new autopoint();
    void fgraph::default_pointtype(const point_drawer &p)
    {
	delete default_point_drawer_;
	if(dynamic_cast<const autopoint *>(&p)) default_point_drawer_ = new autopoint();
	else default_point_drawer_ = p.clone();
    }

    color fgraph::default_pointcolor_(0,0,0);
    void fgraph::default_pointcolor(const color &c) { default_pointcolor_ = c; }

    color fgraph::default_fillcolor_(1,1,1);
    void fgraph::default_fillcolor(const color &c) { default_fillcolor_ = c; }

    bool fgraph::default_fill_ = false;
    void fgraph::default_fill(bool i) { default_fill_ = i; }

    color fgraph::default_legendcolor_(0,0,0);
    void fgraph::default_legendcolor(const color &c) { default_legendcolor_ = c; }

    graph_drawer *fgraph::default_graph_drawer_ = new lines;
    void fgraph::default_drawstyle(const graph_drawer &g)
    {
	delete default_graph_drawer_;
	default_graph_drawer_ = g.clone();
    }

    const fgraph &fgraph::operator=(const fgraph &o)
    {
	graph::operator=(o);
	nsamples_p1_ = o.nsamples_p1_;
	nsamples_p2_ = o.nsamples_p2_;
	p1log_ = o.p1log_;
	p2log_ = o.p2log_;
	p1min_ = o.p1min_;
	p1max_ = o.p1max_;
	p2min_ = o.p2min_;
	p2max_ = o.p2max_;
	function_ = o.function_;
	return o;
    }

    const function &fgraph::operator=(const function &f)
    {
	function_ = f;
	modified_ = true;
	return f;
    }

    fgraph &fgraph::filter(const function &f)
    {
	filter_ = f;
	return *this;
    }

    fgraph &fgraph::param(int n, const var &value)
    {
	function_.param(n,value);
	modified_ = true;
	return *this;
    }

    const function &fgraph::func() const
    {
	return function_;
    }

    fgraph &fgraph::set(function  f1, function f2, function f3, function f4)
    {
	function_ = function(f1,f2,f3,f4);
	modified_ = true;
	return *this;
    }

    fgraph &fgraph::dup()
    {
	fgraph *result = new fgraph(*this);
	if(parent_) parent_->add(result);
	return *result;
    }

    void fgraph::init_()
    {
        esample_ = -1;

	nsamples_p1_ = default_nsamples1_;
	nsamples_p2_ = default_nsamples2_;

	p1log_ = default_p1log_;
	p2log_ = default_p2log_;
	p1min_ = default_p1min_;
	p1max_ = default_p1max_;
	p2min_ = default_p2min_;
	p2max_ = default_p2max_;

	linecolor(default_linecolor_);
	linestyle(default_linestyle_);
	linewidth(default_linewidth_);

	pointsize(default_pointsize_);
	pointtype(*default_point_drawer_);
	pointcolor(default_pointcolor_);

	fillcolor(default_fillcolor_);
	fill(default_fill_);

	legendcolor(default_legendcolor_);

	drawstyle(*default_graph_drawer_);

	filter_ = unset;
    }

    fgraph::fgraph(const function &f) : function_(f)
    {
	init_();
    }

    fgraph::fgraph(const fgraph &o) : graph(o)
    {
	nsamples_p1_ = o.nsamples_p1_;
	nsamples_p2_ = o.nsamples_p2_;
	p1log_ = o.p1log_;
	p2log_ = o.p2log_;
	p1min_ = o.p1min_;
	p1max_ = o.p1max_;
	p2min_ = o.p2min_;
	p2max_ = o.p2max_;
	function_ = o.function_;
	dx_hint_ = o.dx_hint_;
	dy_hint_ = o.dy_hint_;
	filter_ = o.filter_;
        esample_ = o.esample_;
    }

    fgraph::fgraph()
    {
	init_();
    }

/*
    fgraph &fgraph::component(int i,const function &f)
    {
	--i;
	if(i >= (int)function_components_.size()) function_components_.resize(i+1,0.0);
	if(i<0)
	{
	    warning::print("Component index is less than 1, ignoring...","fgraph::component(int,const function&)");
	    return *this;
	}
	function_components_[i] = f;
	return *this;
    }
*/

    int fgraph::nargs()
    {
	int result = function_.nargs();
	return result;
    }

    void fgraph::prepare_for_draw(axis *x,axis *y, frame *f, int count)
    {
	if(global::debug>0) cout<<"[blop] [fgraph] prepare_for_draw starts... pass="<<count<<endl;

	// At the first go, clear and resample the function
	if(count == 1)
	{
	    if(global::debug>0) cout<<"[blop] [fgraph] clearing/resampling the fgraph"<<endl;
	    clear();
	    
	    int n = nargs();
	    
	    if(n > 2)
	    {
		warning::print(var("Function to be plotted requires too many (") & n & ") arguments",
			       "fgraph::prepare_for_draw(...)");
		return;
	    }

	    double p1_min = p1min_;
	    if(p1_min == unset) p1_min = function_.arg_min(1)+20*numeric_limits<double>::epsilon();
	    double p1_max = p1max_;
	    if(p1_max == unset) p1_max = function_.arg_max(1)-20*numeric_limits<double>::epsilon();
	    double p2_min = p2min_;
	    if(p2_min == unset) p2_min = function_.arg_min(2)+20*numeric_limits<double>::epsilon();
	    double p2_max = p2max_;
	    if(p2_max == unset) p2_max = function_.arg_max(2)-20*numeric_limits<double>::epsilon();

	    const bool needs_p1 = function_.uses_arg(1);
	    const bool needs_p2 = function_.uses_arg(2);
	    
	    double p1_from_xaxis = 0.0, p2_from_yaxis = 0.0;
	    
	    // Now check for the following 3 cases: the first (0th) component of the function
	    // to be plotted is either
	    // - the scanned parameter p1 (i.e. _1)
	    // - _1 multiplied by a constant number
	    // - _1 divided by a constant number (typically a unit, MHz or similar)
	    // In this case the user most probably wanted to plot some function vs. x,
	    // where x might be scaled by a unit. In this case the user also most probably
	    // wants x to run along the whole range of the x-axis. So if the p1range was
	    // not set, set it from the range of the xaxis, scaled by the 'unit'
	    if(function_[0].equals(_1))
	    {
		p1_from_xaxis = 1;
	    }
	    else if(blop::function_core::Mul *m = dynamic_cast<blop::function_core::Mul *>(function_.base_))
	    {
		function_core::arg *a = dynamic_cast<function_core::arg*>(m->left_);
	        function_core::constant *c = dynamic_cast<function_core::constant*>(m->right_);
		if(!a || !c)
		{
		    a = dynamic_cast<function_core::arg*>(m->right_);
		    c = dynamic_cast<function_core::constant*>(m->left_);
		}
		if(a && c)
		{
		    if(a->arg_index()==1) p1_from_xaxis = c->value().dbl();
		}
	    }
	    else if(blop::function_core::Div *d = dynamic_cast<blop::function_core::Div *>(function_.base_))
	    {
		function_core::arg      *a = dynamic_cast<function_core::arg*>(d->left_);
		function_core::constant *c = dynamic_cast<function_core::constant*>(d->right_);
		if(a && c)
		{
		    if(a->arg_index()==1) p1_from_xaxis = 1.0/c->value().dbl();
		}
	    }

	    //function y_func = graph_drawer_->get_y(this);
	    //if(y_func.equals(_2)) p2_from_yaxis = true;
	    if(function_[1].equals(_2)) p2_from_yaxis = 1;
	    
	    // if parameter range has not been specified explicitely,
	    // take these ranges from the x and y axes for p1 and p2,
	    // respectively, if p1==_1 or p2==_2
	    // otherwise cleverly try 0..10 range

	    double dp1=-1,rp1=-1,dp2=-1,rp2=-1;
	    
	    if(needs_p1)
	    {
		if(p1_min == unset)
		{
		    //if(p1_from_xaxis) p1_min = x->min();
		    if(p1_from_xaxis != 0.0) p1_min = x->min()/p1_from_xaxis;
		    if(p1_min == unset)
		    {
			if(p1_max != unset) p1_min = p1_max-10;
			else p1_min = 0;
		    }
		}
		if(p1_max == unset)
		{
		    //if(p1_from_xaxis) p1_max = x->max();
		    if(p1_from_xaxis != 0.0) p1_max = x->max()/p1_from_xaxis;
		    if(p1_max == unset)
		    {
			if(p1_min != unset) p1_max = p1_min+10;
			else p1_min = 10;
		    }
		}
		
		if(p1_max < p1_min) err("p1_max < p1_min");
		
		if((p1_from_xaxis!=0.0 && x->logscale()) || p1log_)
		{
                    if(nsamples_p1_ > 0)
                    {
                        rp1 = ::pow(p1_max/p1_min,1.0/(nsamples_p1_-1));
                    }
                    else
                    {
                        rp1 = dsamples_p1_;
                    }
		}
		else
		{
                    if(nsamples_p1_ > 0)
                    {
                        dp1 = (p1_max - p1_min)/(nsamples_p1_-1);
                    }
                    else
                    {
                        dp1 = dsamples_p1_;
                    }
		}
	    }
	    else
	    {
		p1_min = p1_max = 0;
		dp1 = 1;
	    }

    
	    if(needs_p2)
	    {
		if(p2_min == unset) 
		{
		    //if(p2_from_yaxis) p2_min = y->min();
		    if(p2_from_yaxis!=0.0) p2_min = y->min()/p2_from_yaxis;
		    if(p2_min == unset)
		    {
			if(p2_max != unset) p2_min = p2_max-10;
			else p2_min = 0;
		    }
		}
		
		if(p2_max == unset)
		{
		    //if(p2_from_yaxis) p2_max = y->max();
		    if(p2_from_yaxis!=0.0) p2_max = y->max()/p2_from_yaxis;
		    if(p2_max == unset)
		    {
			if(p2_min != unset) p2_max = p2_min+10;
			else p2_max = 0;
		    }
		}
		
		if(p2_max < p2_min) err("p2_max < p2_min");

		if((p2_from_yaxis!=0.0 && y->logscale()) || p2log_)
		{
                    if(nsamples_p2_ > 0)
                    {
                        rp2 = ::pow(p2_max/p2_min,1.0/(nsamples_p2_-1));
                    }
                    else
                    {
                        rp2 = dsamples_p2_;
                    }
		}
		else
		{
                    if(nsamples_p2_ > 0)
                    {
                        dp2 = (p2_max - p2_min)/(nsamples_p2_-1);
                    }
                    else
                    {
                        dp2 = dsamples_p2_;
                    }
		}
	    }
	    else
	    {
		p2_min = p2_max = 0;
		dp2 = 1;
	    }

	    if(dp1 > 0 && function_[0].equals(_1)) dx_hint_ = dp1;
	    else dx_hint_ = unset;
	    if(dp2 > 0 && function_[1].equals(_2)) dy_hint_ = dp2;
	    else dy_hint_ = unset;

	    vector<var> fresult1(function_.components());
	    vector<var> fresult2(function_.components());
	    
	    vector<var> *fresult=&fresult1, *fresult_prev=0;
	    
	    ordered_ = function_.components();
	    vector<var> args((needs_p2?2:1));

            // in case equidistant (path-length) sampling is required... we need the derivative
            function deriv = 0;
            if(needs_p1 && !needs_p2 && esample_>0 && function_.components()==2) deriv = function_.derivative(1);

            for(double p1 = p1_min; p1 < (dp1>0.0 ? p1_max+0.5*dp1 : p1_max*::sqrt(rp1)); )
            {
                args[0] = p1;
		
                for(double p2 = p2_min;
                    p2 <= (dp2>0.0 ? p2_max+0.5*dp2 : p2_max*::sqrt(rp2));
                    (dp2>0 ? p2+=dp2 : p2*=rp2))
                {
                    if(needs_p2) args[1] = p2;

                    function_.meval(args,*fresult);
		    
                    bool skip_it = false;
		    
                    // first, apply the filter (if any), and check if it returns non-0
                    if(filter_.initialized())
                    {
                        if(filter_.nargs() > (int)args.size())
                        {
                            warning::print("insufficient columns to execute condition",
                                           "fgraph::prepare_for_draw(...)");
                        }
                        else
                        {
                            if(filter_.eval(args).dbl() == 0.0) skip_it = true;
                        }
                    }
		    
                    // check, if any of the components of the result are void (to be ignored)
                    if(!skip_it)
                    {
                        for(unsigned int i=0; i<fresult->size(); ++i)
                        {
                            if(ignore::it((*fresult)[i]))
                            {
                                skip_it = true;
                                break;
                            }
                        }
                    }
		    
		    
                    // if it was all fine, just store it
                    if(!skip_it)
                    {
                        data_.push_back(*fresult);
                    }
                    else
                    {
                        // otherwise store a set of 'unset' values, as a 'separator'
                        // in the set of data, if there was valid data before
                        if(!data_.empty() && data_.back()[0].dbl() != unset)
                        {
                            for(unsigned int i=0; i<fresult->size(); ++i)
                            {
                                (*fresult)[i] = unset;
                            }
                            data_.push_back(*fresult);
                        }
                    }
		    
                    if(fresult_prev)
                    {
                        for(int i=0; i<(int)(fresult->size()); ++i)
                        {
                            // any element of this array can only be smaller than the
                            // previous one, if other elements with smaller index
                            // are larger
                            if((*fresult)[i].dbl() < (*fresult_prev)[i].dbl())
                            {
                                bool ok = false;
                                for(int j=0; j<i; ++j)
                                {
                                    if((*fresult)[j].dbl() > (*fresult_prev)[j].dbl())
                                    {
                                        ok = true;
                                        break;
                                    }
                                }
                                if(!ok) ordered_ = std::min(ordered_, i);
                            }
                        }
                    }
                    if(fresult_prev == 0) fresult_prev = &fresult2;
                    swap(fresult,fresult_prev);
                }

                // Increment p1...
                if(needs_p1 && !needs_p2 && esample_>0 && function_.components()==2)
                {
                    deriv.meval(args,*fresult);
                    p1 += esample_/std::sqrt((*fresult)[0].dbl()*(*fresult)[0].dbl() + (*fresult)[1].dbl()*(*fresult)[1].dbl());
                }
                else
                {
                    (dp1>0 ? p1+=dp1 : p1*=rp1);
                }
            }
	    if(global::debug>0) cout<<"[blop] [fgraph] clearing/resampling finished"<<endl;
	} // end if(count==1)

	graph::prepare_for_draw(x,y,f,count);

	if(global::debug>0) cout<<"[blop] [fgraph] prepare_for_draw finished."<<endl;
    }
	
    double fgraph::min(int i)
    {
	double val = unset;

	int n = nargs();

	if(p1min_ == unset || p1max_ == unset ||
	   (n > 1 && (p2min_ == unset || p2max_ == unset)) )
	{
	    warning::print("fgraph: parameter range not specified",
			   "fgraph::min(int)");
	    return 0;
	}

	double dp1 = (p1max_ - p1min_)/100;
	double dp2 = (p2max_ - p2min_)/100;
	if(n == 1) dp2 = 2*(p2max_ - p2min_);

	vector<var> args(2), fresult(function_.components());
	for(double p1=p1min_; p1<=p1max_; p1 += dp1)
	{
	    for(double p2=p2min_; p2<=p2max_; p2 += dp2)
	    {
		args[0] = p1;
		args[1] = p2;
		function_.meval(args,fresult);
		double d = fresult[i].dbl();
		if(val == unset || d<val) val = d;
	    }
	}
	return val;
    }

    double fgraph::max(int i)
    {
	double val = unset;

	int n = nargs();

	if(p1min_ == unset || p1max_ == unset ||
	   (n > 1 && (p2min_ == unset || p2max_ == unset)) )
	{
	    warning::print("fgraph: parameter range not specified",
			   "fgraph::max(int)");
	    return 0;
	}

	double dp1 = (p1max_ - p1min_)/100;
	double dp2 = (p2max_ - p2min_)/100;
	if(n == 1) dp2 = 2*(p2max_ - p2min_);
	vector<var> args(2), fresult(function_.components());
	for(double p1=p1min_; p1<=p1max_; p1 += dp1)
	{
	    for(double p2=p2min_; p2<=p2max_; p2 += dp2)
	    {
		args[0] = p1;
		args[2] = p2;
		function_.meval(args,fresult);
		double d = fresult[i].dbl();
		if(val == unset || d>val) val = d;
	    }
	}
	return val;
    }

    void fgraph::set_default_legend()
    {
	var flist = function_.sprint_latex();
	var leg = default_legend_;
	leg.replace("%f",flist);

	legend(leg);
	//make_latex_legend(leg);
    }
}
