#ifndef __BLOP_FGRAPH_H__
#define __BLOP_FGRAPH_H__

#include <vector>

#include "constants.h"
#include "graph.h"
#include "function.h"
#include "sym.h"

namespace blop
{
    class fgraph : public graph
	{
	private:
	    function function_;
	    function filter_;
            double esample_; // equidistant sample distance

	    static var    default_legend_;

	    static length default_linewidth_;
	    static color  default_linecolor_;
	    static sym::linestyle    default_linestyle_;

	    static length default_pointsize_;
	    static point_drawer *default_point_drawer_;
	    static color  default_pointcolor_;

	    static color default_fillcolor_;
	    static bool  default_fill_;

	    static color default_legendcolor_;

	    static graph_drawer *default_graph_drawer_;

	    bool   p1log_, p2log_;
	    static bool default_p1log_, default_p2log_;
	    double p1min_, p1max_, p2min_, p2max_;
	    static double default_p1min_, default_p1max_, default_p2min_, default_p2max_;

	    int nsamples_p1_, nsamples_p2_;
	    static int default_nsamples1_, default_nsamples2_;

            // A complementary method to specifying the number of samples is to define
            // the sampling distance. If nsamples_p1_ is set to <= 0, then dsamples_p1_ will be used,
            // similarly for p2.
            double dsamples_p1_, dsamples_p2_;

	    void init_();

	    function dx_hint_, dy_hint_;
	public:
	    int nargs();


	    // ---------  Constructor, destructor, assignment ----------

	    const fgraph &operator= (const fgraph &);
	    fgraph();
	    fgraph(const fgraph &);
	    fgraph(const function &f);

	    const function &operator=(const function &);
	    fgraph &set(function f1, function f2=unset, function f3=unset, function f4=unset);
	    fgraph &param(int n, const var &value);
	    const function &func() const;    // access the function generating this fgraph
	    
	    // --------- Components -------------------------------------

	    //fgraph &component(int i,const function &);                    // specify the 'ith' component function
	    int columns() const {return function_.components();}     // return number of components

	    // ----------- Filter ---------------------------------------

	    fgraph &filter(const function &f);

	    // --------  Default values ---------------------------------
	    static void default_legend(const var &);
	    static void default_linewidth(const length &);
	    static void default_linecolor(const color &);
	    static void default_linestyle(sym::linestyle);
	    static void default_pointsize(const length &);
	    static void default_pointtype(const point_drawer &);
	    static void default_pointcolor(const color &);
	    static void default_fillcolor(const color &);
	    static void default_fill(bool);
	    static void default_legendcolor(const color &);
	    static void default_drawstyle(const graph_drawer &);
//html <a name="nsamples">
	    static void default_nsamples(int);
	    static void default_nsamples1(int);
	    static void default_nsamples2(int);
//html </a>

	    static void default_p1min(double);
	    static void default_p1max(double);
	    static void default_p1range(double,double);
	    static void default_p2min(double);
	    static void default_p2max(double);
	    static void default_p2range(double,double);

	    // ------------  set logarithmic scanning in the parameters ----------
	    // If this flag is set to true, the corresponding parameter will be
	    // scanned (for the plotting) in a logarithmic way, i.e. it will be
	    // incremented in each scanning step by the same *multiplicative factor* 
	    // and not by the same step

	    fgraph &p1log(bool f) { p1log_ = f; return *this; }
	    fgraph &p2log(bool f) { p2log_ = f; return *this; }

	    // set default values 
	    static void default_p1log(bool f);
	    static void default_p2log(bool f);

	    // ----------  Set/get the parameter ranges  -------------------------

	    fgraph &p1min(double v) { p1min_ = v; return *this; }
	    fgraph &p1max(double v) { p1max_ = v; return *this; }
	    fgraph &p1range(double min, double max, int n=0)
		{ p1min_=min; p1max_=max; if(n!=0) nsamples_p1_=n; return *this; }
	    fgraph &p2min(double v) { p2min_ = v; return *this; }
	    fgraph &p2max(double v) { p2max_ = v; return *this; }
	    fgraph &p2range(double min, double max, int n=0)
		{ p2min_=min; p2max_=max; if(n!=0) nsamples_p2_=n; return *this; }

	    double p1min() const { return p1min_; }
	    double p1max() const { return p1max_; }
	    double p2min() const { return p2min_; }
	    double p2max() const { return p2max_; }



	    // ----------  Set number of samples ----------------------------

	    fgraph &nsamples1(int n) { nsamples_p1_ = n; return *this; }
	    fgraph &nsamples2(int n) { nsamples_p2_ = n; return *this; }
	    fgraph &nsamples (int n) { nsamples_p1_ = nsamples_p2_ = n; return *this; }

            // ------------  Equidistant sampling of a curve in 2D  --------------

            fgraph &esample(double d) { esample_ = d; return *this; }
            fgraph &esample(bool t) { if(t) esample_ = std::abs(esample_); else esample_ = -std::abs(esample_); return *this; }

            // ----------  set the sampling distance -----------------------
            fgraph &dsamples1(double d) { dsamples_p1_ = d; nsamples_p1_=0; return *this; }
            fgraph &dsamples2(double d) { dsamples_p2_ = d; nsamples_p2_=0; return *this; }
            fgraph &dsamples(double d)
            {
                dsamples1(d);
                dsamples2(d);
                return *this;
            }


	    // ------ Get max/min values of column 'i' -----------------------

	    double max(int i);
	    double min(int i);
	    

	    function dx_hint() const { return dx_hint_; }
	    function dy_hint() const { return dy_hint_; }

	    // ---------------  duplicate -----------------------------------

	    fgraph &dup();

	    // -------  These are not relevant for the user  ----------------

	    void prepare_for_draw(axis *,axis *, frame *, int count);
	    bool operator== (const fgraph &) {return true;}
	    void set_default_legend();

	    fgraph &layer(const var & l) { plottable::layer(l); return *this; }
	};

}

#ifdef __MAKECINT__
#pragma link off function operator==(vector<blop::function>&,vector<blop::function>&);
#pragma link off function operator!=(vector<blop::function>&,vector<blop::function>&);
#pragma link off function operator<(vector<blop::function>&,vector<blop::function>&);
#pragma link off function operator>(vector<blop::function>&,vector<blop::function>&);
#pragma link off function operator<=(vector<blop::function>&,vector<blop::function>&);
#pragma link off function operator>=(vector<blop::function>&,vector<blop::function>&);
#endif

#endif
