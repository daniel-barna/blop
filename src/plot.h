#ifndef __BLOP_PLOT_H__
#define __BLOP_PLOT_H__

#include <vector>
#include <tuple>
#include "dgraph.h"
#include "fgraph.h"
#include "var.h"
#include "length.h"
#include "function.h"
#include "constants.h"
#include "hist.h"

namespace blop
{
    // ----------------------------  plot  --------------------------------------
    // The following functions plot data provided as arrays

    class plotcmd_interpreter
    {
    public:
	virtual ~plotcmd_interpreter() {}
	virtual plottable *run(const var &cmd) = 0;
	static void set(plotcmd_interpreter *);
	static plotcmd_interpreter *&get();
    };

    plottable &plotcmd(const var &cmd);
    plottable &mplotcmd(const var &cmd);

    //html <a name='plot_rectangle'>
    dgraph &plot_rectangle(const var &x1, const var &y1,
			   const var &x2, const var &y2);
    dgraph &mplot_rectangle(const var &x1, const var &y1,
			   const var &x2, const var &y2);
    //html </a>

    hist &hisplot (const var &filename, const function &f1=unset, const function &f2=unset, const histopt &hopt = histopt());
    hist &mhisplot(const var &filename, const function &f1=unset, const function &f2=unset, const histopt &hopt = histopt());

    dgraph &plot(int n,
		 double *x,
		 const function &f1,
		 const function &f2=unset,
		 const function &f3=unset,
		 const function &f4=unset);
    dgraph &mplot(int n,
		  double *x,
		  const function &f1,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &plot(int n,
		 double *x,
		 double *y,
		 const function &f1=unset,
		 const function &f2=unset,
		 const function &f3=unset,
		 const function &f4=unset);
    dgraph &mplot(int n,
		  double *x,
		  double *y,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &plot(int n,
		 double *x,
		 double *y,
		 double *z,
		 const function &f1=unset,
		 const function &f2=unset,
		 const function &f3=unset,
		 const function &f4=unset);
    dgraph &mplot(int n,
		  double *x,
		  double *y,
		  double *z,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &plot(int n,
		 double *x,
		 double *y,
		 double *z,
		 double *w,
		 const function &f1=unset,
		 const function &f2=unset,
		 const function &f3=unset,
		 const function &f4=unset);
    dgraph &mplot(int n,
		  double *x,
		  double *y,
		  double *z,
		  double *w,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);


    dgraph &plot (int n,
		  var *x,
		  const function &f1,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot(int n,
		  var *x,
		  const function &f1,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &plot (int n,
		  var *x,
		  var *y,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot(int n,
		  var *x,
		  var *y,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &plot (int n,
		  var *x,
		  var *y,
		  var *z,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot(int n,
		  var *x,
		  var *y,
		  var *z,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);

    dgraph &plot (int n,
		  var *x,
		  var *y,
		  var *z,
		  var *w,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot(int n,
		  var *x,
		  var *y,
		  var *z,
		  var *w,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);

    dgraph & plot(double x, double y); // just draw a point
    dgraph &mplot(double x, double y);
    dgraph & plot(double x, double y, double z); 
    dgraph &mplot(double x, double y, double z); 
    dgraph & plot(double x, double y, var z); 
    dgraph &mplot(double x, double y, var z); 
    dgraph & plot(const var &x, const var &y, const var &z=unset);
    dgraph &mplot(const var &x, const var &y, const var &z=unset);
    dgraph & plot(double x, var y);
    dgraph &mplot(double x, var y);
    dgraph & plot(var x, double y);
    dgraph &mplot(var x, double y);



    // --------------

    dgraph &plot(const std::vector<double> &y);
    dgraph &mplot(const std::vector<double> &y);

    dgraph &plot (const std::vector<double> &x,
		  const function &f1,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot (const std::vector<double> &x,
		   const function &f1,
		   const function &f2=unset,
		   const function &f3=unset,
		   const function &f4=unset);
    dgraph &plot (const std::vector<double> &x,
		  const std::vector<double> &y,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot (const std::vector<double> &x,
		   const std::vector<double> &y,
		   const function &f1=unset,
		   const function &f2=unset,
		   const function &f3=unset,
		   const function &f4=unset);
    dgraph &plot (const std::vector<double> &x,
		  const std::vector<double> &y,
		  const std::vector<double> &z,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot (const std::vector<double> &x,
		   const std::vector<double> &y,
		   const std::vector<double> &z,
		   const function &f1=unset,
		   const function &f2=unset,
		   const function &f3=unset,
		   const function &f4=unset);

    dgraph &plot (const std::vector<double> &x,
		  const std::vector<double> &y,
		  const std::vector<double> &z,
		  const std::vector<double> &w,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot (const std::vector<double> &x,
		   const std::vector<double> &y,
		   const std::vector<double> &z,
		   const std::vector<double> &w,
		   const function &f1=unset,
		   const function &f2=unset,
		   const function &f3=unset,
		   const function &f4=unset);


    dgraph &plot(const std::vector<std::complex<double>> &z);
    dgraph &mplot(const std::vector<std::complex<double>> &z);

    // -----------


    dgraph &plot (const std::vector<blop::var> &x,
		  const function &f1,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot (const std::vector<blop::var> &x,
		   const function &f1,
		   const function &f2=unset,
		   const function &f3=unset,
		   const function &f4=unset);
    dgraph &plot (const std::vector<blop::var> &x,
		  const std::vector<blop::var> &y,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot (const std::vector<blop::var> &x,
		   const std::vector<blop::var> &y,
		   const function &f1=unset,
		   const function &f2=unset,
		   const function &f3=unset,
		   const function &f4=unset);
    dgraph &plot (const std::vector<blop::var> &x,
		  const std::vector<blop::var> &y,
		  const std::vector<blop::var> &z,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot (const std::vector<blop::var> &x,
		   const std::vector<blop::var> &y,
		   const std::vector<blop::var> &z,
		   const function &f1=unset,
		   const function &f2=unset,
		   const function &f3=unset,
		   const function &f4=unset);
    dgraph &plot (const std::vector<blop::var> &x,
		  const std::vector<blop::var> &y,
		  const std::vector<blop::var> &z,
		  const std::vector<blop::var> &w,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    dgraph &mplot (const std::vector<blop::var> &x,
		   const std::vector<blop::var> &y,
		   const std::vector<blop::var> &z,
		   const std::vector<blop::var> &w,
		   const function &f1=unset,
		   const function &f2=unset,
		   const function &f3=unset,
		   const function &f4=unset);


    // ------------------  plotting data from a file ------------------------------------
    // The following functions plot data from a file (specified by the first argument)
    // and the columns are selected by the remaining arguments

    dgraph &plot(const var &filename,
		const function &f1 = unset,
		const function &f2 = unset,
		const function &f3 = unset,
		const function &f4 = unset,
		const function &f5 = unset,
		const function &f6 = unset);
    inline dgraph &plot(const char *filename,
			const function &f1 = unset,
			const function &f2 = unset,
			const function &f3 = unset,
			const function &f4 = unset,
			const function &f5 = unset,
			const function &f6 = unset) { return plot(var(filename), f1, f2, f3, f4, f5, f6); }
    inline dgraph &plot(const string &filename,
			const function &f1 = unset,
			const function &f2 = unset,
			const function &f3 = unset,
			const function &f4 = unset,
			const function &f5 = unset,
			const function &f6 = unset) { return plot(var(filename), f1, f2, f3, f4, f5, f6); }
    


    dgraph &mplot(const var &filename,
		 const function &f1 = unset,
		 const function &f2 = unset,
		 const function &f3 = unset,
		 const function &f4 = unset,
		 const function &f5 = unset,
		 const function &f6 = unset);
    inline dgraph &mplot(const char *filename,
			 const function &f1 = unset,
			 const function &f2 = unset,
			 const function &f3 = unset,
			 const function &f4 = unset,
			 const function &f5 = unset,
			 const function &f6 = unset) { return mplot(var(filename), f1, f2, f3, f4, f5, f6); }
    inline dgraph &mplot(const string &filename,
			 const function &f1 = unset,
			 const function &f2 = unset,
			 const function &f3 = unset,
			 const function &f4 = unset,
			 const function &f5 = unset,
			 const function &f6 = unset) { return mplot(var(filename), f1, f2, f3, f4, f5, f6); }
		 

    plottables plot_groups(const var &filename,
                           const function &grouping_value,
                           const function &f1 = unset,
                           const function &f2 = unset,
                           const function &f3 = unset,
                           const function &f4 = unset,
                           const function &f5 = unset,
                           const function &f6 = unset);

    plottables mplot_groups(const var &filename,
                            const function &grouping_value,
                            const function &f1 = unset,
                            const function &f2 = unset,
                            const function &f3 = unset,
                            const function &f4 = unset,
                            const function &f5 = unset,
                            const function &f6 = unset);

    plottables plot_groups_if(const var &filename,
                              const function &grouping_value,
                              const function &condition,
                              const function &f1 = unset,
                              const function &f2 = unset,
                              const function &f3 = unset,
                              const function &f4 = unset,
                              const function &f5 = unset,
                              const function &f6 = unset);

    /*
    plottables plot_groups_if(const var &filename,
                              const std::tuple<const function &, double> &grouping_value_and_diff,
                              const function &condition,
                              const function &f1 = unset,
                              const function &f2 = unset,
                              const function &f3 = unset,
                              const function &f4 = unset,
                              const function &f5 = unset,
                              const function &f6 = unset);
    */

    plottables mplot_groups_if(const var &filename,
				const function &grouping_value,
				const function &condition, 
				const function &f1 = unset,
				const function &f2 = unset,
				const function &f3 = unset,
				const function &f4 = unset,
				const function &f5 = unset,
				const function &f6 = unset);
    

    // ------------------------------------------------------------------------
    // plot blocks (separated by empty lines) of a file
/*
    plottables plot_blocks(const var &filename,
                           const function &f1 = unset,
                           const function &f2 = unset,
                           const function &f3 = unset,
                           const function &f4 = unset,
                           const function &f5 = unset,
                           const function &f6 = unset);
    inline plottables plot_blocks(const std::string &filename,
                                  const function &f1 = unset,
                                  const function &f2 = unset,
                                  const function &f3 = unset,
                                  const function &f4 = unset,
                                  const function &f5 = unset,
                                  const function &f6 = unset) { return plot_blocks(var(filename), f1, f2, f3, f4, f5, f6); }
    inline plottables plot_blocks(const char *filename,
                                  const function &f1 = unset,
                                  const function &f2 = unset,
                                  const function &f3 = unset,
                                  const function &f4 = unset,
                                  const function &f5 = unset,
                                  const function &f6 = unset) { return plot_blocks(var(filename), f1, f2, f3, f4, f5, f6); }
    plottables mplot_blocks(const var &filename,
                           const function &f1 = unset,
                           const function &f2 = unset,
                           const function &f3 = unset,
                           const function &f4 = unset,
                           const function &f5 = unset,
                           const function &f6 = unset);
    inline plottables mplot_blocks(const std::string &filename,
                                  const function &f1 = unset,
                                  const function &f2 = unset,
                                  const function &f3 = unset,
                                  const function &f4 = unset,
                                  const function &f5 = unset,
                                  const function &f6 = unset) { return mplot_blocks(var(filename), f1, f2, f3, f4, f5, f6); }
    inline plottables mplot_blocks(const char *filename,
                                  const function &f1 = unset,
                                  const function &f2 = unset,
                                  const function &f3 = unset,
                                  const function &f4 = unset,
                                  const function &f5 = unset,
                                  const function &f6 = unset) { return mplot_blocks(var(filename), f1, f2, f3, f4, f5, f6); }
*/  

    // ------------------------------------------------------------------------
    // plot several files (use a filename pattern)

    plottables plot_many(const var &filename_pattern,
			  const function &f1 = unset,
			  const function &f2 = unset,
			  const function &f3 = unset,
			  const function &f4 = unset,
			  const function &f5 = unset,
			  const function &f6 = unset);
    plottables mplot_many(const var &filename_pattern,
			   const function &f1 = unset,
			   const function &f2 = unset,
			   const function &f3 = unset,
			   const function &f4 = unset,
			   const function &f5 = unset,
			   const function &f6 = unset);

    // ----------------------- plot functions ---------------------------------
    // The following functions plot functions

    fgraph &plot(const function &f1,
		 const function &f2 = unset,
		 const function &f3 = unset,
		 const function &f4 = unset,
		 const function &f5 = unset);


    fgraph &mplot(const function &f1,
		  const function &f2 = unset,
		  const function &f3 = unset,
		  const function &f4 = unset,
		  const function &f5 = unset);


    // these function are defined in blop_cint.cc
    fgraph &plot(void *f1,
		 void *f2=0,
		 void *f3=0,
		 void *f4=0,
		 void *f5=0);

    fgraph &plot(const function &f1,
		 void *f2,
		 void *f3=0,
		 void *f4=0,
		 void *f5=0);

    fgraph &plot(void *f1,
		 const function &f2,
		 void *f3=0,
		 void *f4=0,
		 void *f5=0);
    
    fgraph &mplot(void *f1,
		  void *f2=0,
		  void *f3=0,
		  void *f4=0,
		  void *f5=0);

    // ------------- plot with a condition -----------------------------

    dgraph &plot_if(const var &filename,
		    const function &condition,
		    const function &f1 = unset,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset,
                    const function &f5 = unset);
    inline dgraph &plot_if(const char *filename,
			   const function &condition,
			   const function &f1 = unset,
			   const function &f2 = unset,
			   const function &f3 = unset,
			   const function &f4 = unset,
                           const function &f5 = unset) { return plot_if(var(filename), condition, f1, f2, f3, f4, f5); }
    inline dgraph &plot_if(const string &filename,
			   const function &condition,
			   const function &f1 = unset,
			   const function &f2 = unset,
			   const function &f3 = unset,
			   const function &f4 = unset,
                           const function &f5 = unset) { return plot_if(var(filename), condition, f1, f2, f3, f4, f5); }


    dgraph &mplot_if(const var &filename,
		     const function &condition,
		     const function &f1 = unset,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset,
                     const function &f5 = unset);
    inline dgraph &mplot_if(const char *filename,
			    const function &condition,
			    const function &f1 = unset,
			    const function &f2 = unset,
			    const function &f3 = unset,
			    const function &f4 = unset,
                            const function &f5 = unset) { return mplot_if(var(filename), condition, f1, f2, f3, f4, f5); }
    inline dgraph &mplot_if(const string &filename,
			    const function &condition,
			    const function &f1 = unset,
			    const function &f2 = unset,
			    const function &f3 = unset,
			    const function &f4 = unset,
                            const function &f5 = unset) { return mplot_if(var(filename), condition, f1, f2, f3, f4, f5); }

    // --------------------- plot_if/mplot_if for vectors, arrays, etc --------------------

    dgraph &plot_if(const std::vector<double> &x,
		    const function &condition,
		    const function &f1 = unset,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset);
    dgraph &mplot_if(const std::vector<double> &x,
		     const function &condition,
		     const function &f1 = unset,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset);
    dgraph &plot_if(const std::vector<double> &x,
		    const std::vector<double> &y,
		    const function &condition,
		    const function &f1 = unset,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset);
    dgraph &mplot_if(const std::vector<double> &x,
		     const std::vector<double> &y,
		     const function &condition,
		     const function &f1 = unset,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset);
    dgraph &plot_if(const std::vector<double> &x,
		    const std::vector<double> &y,
		    const std::vector<double> &z,
		    const function &condition,
		    const function &f1 = unset,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset);
    dgraph &mplot_if(const std::vector<double> &x,
		     const std::vector<double> &y,
		     const std::vector<double> &z,
		     const function &condition,
		     const function &f1 = unset,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset);
    dgraph &plot_if(const std::vector<double> &x,
		    const std::vector<double> &y,
		    const std::vector<double> &z,
		    const std::vector<double> &w,
		    const function &condition,
		    const function &f1 = unset,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset);
    dgraph &mplot_if(const std::vector<double> &x,
		     const std::vector<double> &y,
		     const std::vector<double> &z,
		     const std::vector<double> &w,
		     const function &condition,
		     const function &f1 = unset,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset);

    dgraph &plot_if(const std::vector<var> &x,
		    const function &condition,
		    const function &f1 = unset,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset);
    dgraph &mplot_if(const std::vector<var> &x,
		     const function &condition,
		     const function &f1 = unset,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset);
    dgraph &plot_if(const std::vector<var> &x,
		    const std::vector<var> &y,
		    const function &condition,
		    const function &f1 = unset,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset);
    dgraph &mplot_if(const std::vector<var> &x,
		     const std::vector<var> &y,
		     const function &condition,
		     const function &f1 = unset,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset);
    dgraph &plot_if(const std::vector<var> &x,
		    const std::vector<var> &y,
		    const std::vector<var> &z,
		    const function &condition,
		    const function &f1 = unset,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset);
    dgraph &mplot_if(const std::vector<var> &x,
		     const std::vector<var> &y,
		     const std::vector<var> &z,
		     const function &condition,
		     const function &f1 = unset,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset);
    dgraph &plot_if(const std::vector<var> &x,
		    const std::vector<var> &y,
		    const std::vector<var> &z,
		    const std::vector<var> &w,
		    const function &condition,
		    const function &f1 = unset,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset);
    dgraph &mplot_if(const std::vector<var> &x,
		     const std::vector<var> &y,
		     const std::vector<var> &z,
		     const std::vector<var> &w,
		     const function &condition,
		     const function &f1 = unset,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset);


    // ---------------- plot_if/mplot_if for functions -----------------------------


    fgraph &plot_if(const function &condition,
		    const function &f1,
		    const function &f2 = unset,
		    const function &f3 = unset,
		    const function &f4 = unset);

    fgraph &mplot_if(const function &condition,
		     const function &f1,
		     const function &f2 = unset,
		     const function &f3 = unset,
		     const function &f4 = unset);


    
    // ---------------------------------------------------------------------------------
    // These functions plot already existing graphs (add it to the current frame, etc)

    plottable &plot(plottable &g);
    inline plottable &plot(plottable *g) { return plot(*g); }
    plottable &mplot(plottable &g);
    inline plottable &mplot(plottable *g) { return mplot(*g); }


    // plot already existing graphs transformed. the _0 function can be also
    // used to define f1, f2 etc, which evaluates to the index of the current
    // datapoint of the graph
    // The graphs are NOT copied!
    dgraph &plot(dgraph &g,
		 const function &f1=unset,
		 const function &f2=unset,
		 const function &f3=unset,
		 const function &f4=unset);
    inline dgraph &plot(dgraph *g,
		 const function &f1=unset,
		 const function &f2=unset,
		 const function &f3=unset,
		 const function &f4=unset)
    {
        return plot(*g,f1,f2,f3,f4);
    }



    dgraph &mplot(dgraph &g,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset);
    inline dgraph &mplot(dgraph *g,
		  const function &f1=unset,
		  const function &f2=unset,
		  const function &f3=unset,
		  const function &f4=unset)
    {
        return mplot(*g,f1,f2,f3,f4);
    }

    

    dgraph &plot_if(dgraph &g,
		    const function &condition,
		    const function &f1=unset,
		    const function &f2=unset,
		    const function &f3=unset,
		    const function &f4=unset);

    dgraph &mplot_if(dgraph &g,
		     const function &condition,
		     const function &f1=unset,
		     const function &f2=unset,
		     const function &f3=unset,
		     const function &f4=unset);
    inline dgraph &plot_if(dgraph *g,
		    const function &condition,
		    const function &f1=unset,
		    const function &f2=unset,
		    const function &f3=unset,
		    const function &f4=unset)
    {
        return plot_if(*g,condition,f1,f2,f3,f4);
    }

    inline dgraph &mplot_if(dgraph *g,
		     const function &condition,
		     const function &f1=unset,
		     const function &f2=unset,
		     const function &f3=unset,
		     const function &f4=unset)
    {
        return mplot_if(*g,condition,f1,f2,f3,f4);
    }

    
    
}

#endif
