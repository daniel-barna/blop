#ifndef __BLOP_PLOTTABLE_H__
#define __BLOP_PLOTTABLE_H__

//#include "graph_drawer.h"
#include "point_drawer.h"
#include "constants.h"
#include "function.h"
#include "factory.h"

#include <vector>
#ifndef __MAKECINT__
#include <functional>
#endif

namespace blop
{
    typedef std::vector<blop::var> datapoint;
    class frame;
    class graph_drawer;

    class plottable : public factory_base<plottable>
    {
        FACTORY(plottable);
        
        protected:
            int level_;

	    bool permanent_;

//	    bool autodel_;

	    int ordered_;

	    frame *parent_;
	    
	    double xmin_,xmax_,ymin_,ymax_;
	    
	    // point properties
	    length        pointsize_;
        smartptr<point_drawer> point_drawer_;
	    color         pointcolor_;
	
	    // line properties
	    color         linecolor_;
	    length        linewidth_;
	    sym::linestyle linestyle_;

	    // fill properties
	    color         fillcolor_;
	    bool          fill_;

	    // legend properties
	    color         legendcolor_;

            mutable smartptr<graph_drawer> graph_drawer_;

	    int xaxis_,yaxis_;

	    mutable var legend_;

	    bool modified_;

	    void copy(const plottable &);

	    var layer_;
    
            std::function<void()> plot_setup_function_;

    public:

	    plottable();
	    plottable(const plottable &);
	    virtual ~plottable();
	    const plottable &operator=(const plottable &);

	    virtual plottable &layer(const var & i) { layer_ = i; return *this; }
	    virtual const var &layer() const { return layer_; }

	    void copy_style(const plottable &);

	    void parent(frame *f) { parent_ = f; }

            //html <a name="autodel"> </a>
//	    plottable &autodel(bool b) { autodel_ = b; return *this; }
//	    bool autodel() const { return autodel_; }
	    plottable &permanent(bool b) { permanent_ = b; return *this; }
	    bool permanent() const { return permanent_; }
	    plottable &level(int l) { level_ = l; modified_ = true; return *this; }
	    int level() const { return level_; }

	    plottable &ordered(int f) { ordered_ = f; return *this; }
	    int        ordered() const { return ordered_; }

	    plottable &xmin(double d) {xmin_ = d; return *this;}
	    plottable &xmax(double d) {xmax_ = d; return *this;}
	    plottable &xrange(double d1,double d2)
		{xmin_ = d1; xmax_ = d2; return *this;}
	    plottable &ymin(double d) {ymin_ = d; return *this;}
	    plottable &ymax(double d) {ymax_ = d; return *this;}
	    plottable &yrange(double d1,double d2) {ymin_ = d1; ymax_ = d2; return *this;}


	    double xmin()  const {return xmin_;}
	    double xmax()  const {return xmax_;}
	    double ymin()  const {return ymin_;}
	    double ymax()  const {return ymax_;}


	    plottable     &legend(const var &s) { legend_ = s; modified_ = true; return *this;}
	    const var  &legend() const      { return legend_; }

	    plottable   &legendcolor(const color &c)
		{legendcolor_ = c; modified_ = true; return *this;}
	    plottable   &legendcolor(double r, double g, double b)
		{ return legendcolor(color(r,g,b)); }
	    const color  &legendcolor() const    {return legendcolor_;}

	    // -------------  fill properties  --------------------

	    plottable        &fillcolor(const color &c)
		{
		    fillcolor_ = c; legendcolor_ = c; fill_ = true;
		    modified_ = true; 
		    return *this;
		}
	    plottable        &fc(const color &c)        {return fillcolor(c);}
	    plottable        &fillcolor(double red, double green, double blue) { return fillcolor(color(red,green,blue)); }
	    plottable        &fc(double red, double green, double blue) { return fillcolor(color(red,green,blue)); }
	    const color  &fillcolor() const         {return fillcolor_;}

	    plottable        &fill(bool b) {fill_ = b; modified_ = true; return *this;}
	    bool          fill() const              {return fill_;}

	    // -------------  line properties   -------------------

	    plottable   &linecolor(const color &c)
		{
		    linecolor_ = c;
		    legendcolor_ = c;
		    modified_ = true; 
		    return *this;
		}
	    plottable   &lc       (const color &c) {return linecolor(c);}
	    plottable   &linecolor(double r, double g, double b) { return linecolor(color(r,g,b)); }
	    plottable   &lc       (double r, double g, double b) { return linecolor(color(r,g,b)); }
	    const color  &linecolor() const    {return linecolor_;}

	    plottable    &linewidth(const length &d)
		{linewidth_ = d; modified_=true;return *this;}
	    plottable    &lw       (const length &d) {return linewidth(d);}
	    const length  &linewidth() const     {return linewidth_;}

	    plottable   &linestyle(sym::linestyle s) {linestyle_ = s; modified_=true; return *this;}
	    plottable   &ls       (sym::linestyle s) {return linestyle(s);}
	    sym::linestyle           linestyle() const {return linestyle_; }

	    plottable &linetype(sym::linestyle s) { return linestyle(s); }  // alias for linestyle, for confused users
	    plottable &lt      (sym::linestyle s) { return linestyle(s); }  // shorthand for linetype

	    // -------------   point properties  -------------------
            plottable &pointtype(smartptr<point_drawer> d);
            plottable &pt       (smartptr<point_drawer> d) { return pointtype(d); }
	    plottable   &pointtype(const point_drawer &d); 
	    plottable   &pt       (const point_drawer &d) {return pointtype(d);}
            smartptr<point_drawer> pointtype() const           {return point_drawer_;}
	    
	    plottable   &pointsize(const length &d) {pointsize_ = d; modified_=true; return *this;}
	    plottable   &ps       (const length &d) {return pointsize(d);}
	    const length &pointsize() const     {return pointsize_;}

	    plottable   &pointcolor(const color &c) {pointcolor_ = c; legendcolor_ = c; modified_ = true; return *this;}
	    plottable   &pc        (const color &c) {return pointcolor(c);}
	    plottable   &pointcolor(double r, double g, double b) { return pointcolor(color(r,g,b)); }
	    plottable   &pc        (double r, double g, double b) { return pointcolor(color(r,g,b)); }
	    const color  &pointcolor() const    {return pointcolor_;}

	    plottable   &allcolor(const color &c)
		{
		    modified_ = true;
		    pointcolor_ = c;
		    linecolor_ = c;
		    legendcolor_ = c;
		    fillcolor_ = c;
		    return *this;
		}
	    plottable   &ac(const color &c) {return allcolor(c);}
	    plottable   &allcolor(double r, double g, double b) { return allcolor(color(r,g,b)); }
	    plottable   &ac(double r, double g, double b) { return allcolor(color(r,g,b)); }

            plottable   &drawstyle(smartptr<graph_drawer> d);
            plottable   &dw(smartptr<graph_drawer> d) { return drawstyle(d); }
	    plottable   &drawstyle(const graph_drawer &d);
	    plottable   &ds       (const graph_drawer &d) {return drawstyle(d);}
            smartptr<graph_drawer> drawstyle() const { return graph_drawer_;}

	    plottable   &xaxis(int i) {xaxis_ = i; return *this;}
	    int           xaxis() const {return xaxis_;}
	    plottable   &yaxis(int i) {yaxis_ = i; return *this;}
	    int           yaxis() const {return yaxis_;}


	    // duplicate the thing
	    virtual plottable &dup() = 0;

	    // virtual dummy functions for parameter setting. overwritten in fgraph
	    virtual plottable &p1min(double d) { return *this;}
	    virtual plottable &p1max(double d) { return *this;}
	    virtual plottable &p2min(double d) { return *this;}
	    virtual plottable &p2max(double d) { return *this;}
	    virtual plottable &p1range(double a,double b,int n=0) { return *this;}
	    virtual plottable &p2range(double a,double b,int n=0) { return *this;}
	    virtual plottable &p1log(bool) { return *this; }
	    virtual plottable &p2log(bool) { return *this; }
	    virtual plottable &nsamples1(int n) { return *this; }
	    virtual plottable &nsamples2(int n) { return *this; }
	    virtual plottable &nsamples(int n) { return *this; }
            virtual plottable &dsamples1(double d) { return *this; }
            virtual plottable &dsamples2(double d) { return *this; }
            virtual plottable &dsamples(double d) { return *this; }

	    // virtual functions, overwritten in fgraph
	    virtual double p1min() const {return 0; }
	    virtual double p1max() const {return 0; }
	    virtual double p2min() const {return 0; }
	    virtual double p2max() const {return 0; }

	    // -------------------------------------------------------------------
	    // pick a block (continous set of data in a file/stream,
	    // between empty lines)
	    // i=1 corresponds to the first block

	    virtual plottable &block(int i) {return *this;}


	    // ------------------------  hints  ---------------------------------------
	    virtual function x_hint() const { return unset; }
	    virtual function y_hint() const { return unset; }
	    virtual function z_hint() const { return unset; }
	    virtual function dx_hint() const { return unset; }
	    virtual function x1_hint() const { return unset; }
	    virtual function x2_hint() const { return unset; }
	    virtual function dy_hint() const { return unset; }
	    virtual function y1_hint() const { return unset; }
	    virtual function y2_hint() const { return unset; }
	    virtual function dz_hint() const { return unset; }
	    virtual function z1_hint() const { return unset; }
	    virtual function z2_hint() const { return unset; }
	    
	    typedef unsigned int size_type;
	    virtual const datapoint* get(int index) const = 0;
	    virtual size_type size() const = 0;
	    bool empty() const { return size() <= 0; }
	    virtual var max(const function &fmax, function fret=unset) const = 0;
	    virtual var min(const function &fmin, function fret=unset) const = 0;
	    virtual int columns() const = 0;

	    virtual void prepare_for_draw(smartptr<axis>, smartptr<axis>, smartptr<frame>, int count) = 0;

	    virtual bool modified() const { return modified_; }
	    virtual void modified(bool f) { modified_ = f; }

            virtual plottable &esample(double) { return *this; }

            virtual plottable &sort(const blop::function &key) {return *this;};

            const std::function<void()> &plot_setup_function() const { return plot_setup_function_; }
            plottable &plot_setup_function(const std::function<void()> &f) { plot_setup_function_ = f; return *this; }

    };
}


#endif
