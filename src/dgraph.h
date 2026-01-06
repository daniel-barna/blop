#ifndef __BLOP_DGRAPH_H__
#define __BLOP_DGRAPH_H__

#include "graph.h"
#include "function.h"
#include "array.h"
#include "sym.h"
#include <iostream>

namespace blop
{

    class dgraph : public graph
	{
	private:
	    var filename_;

	    void init_defaults();

	    static length default_pointsize_;
	    static point_drawer  *default_point_drawer_;
	    static color  default_pointcolor_;
	    static color default_linecolor_;
	    static length default_linewidth_;
	    static sym::linestyle    default_linestyle_;
	    static color default_fillcolor_;
	    static bool  default_fill_;
	    static color default_legendcolor_;
	    static graph_drawer* &default_graph_drawer_();
	    static var default_legend_;
	    static bool  falsecondition_break_;
            static std::string default_comment_chars_;

	    int columns_;

	    std::istream *open_file(const var &filename);

	    vector<var> title_hints_;
	    bool title_hints_latex_;
	    bool autotitles_;
	    static bool default_autotitles_;
            
            std::string comment_chars_;

	public:
	    // --------------------  default properties --------------------------------
	    static void default_legend(const var &);  // %f in this string is replaced by the filename
	    static void default_pointsize(length l);
	    static void default_pointtype(const point_drawer &);
	    static void default_pointcolor(const color &);
	    static void default_linecolor(const color &);
	    static void default_linewidth(length l);
	    static void default_linestyle(sym::linestyle);
	    static void default_fillcolor(const color &);
	    static void default_fill(bool);
	    static void default_legendcolor(const color &);
	    static void default_drawstyle(const graph_drawer &);
	    static void falsecondition_break(bool b) { falsecondition_break_ = b; }
            static void default_comment_chars(const std::string &s);
            static std::string default_comment_chars() { return default_comment_chars_; }

	    int columns() const {return columns_;}
	    dgraph &columns(int ncol);

	    dgraph(const dgraph &);
	    const dgraph &operator=(const dgraph &);

	    dgraph();

	    dgraph(int npoints, int cols);

	    dgraph(int n, const double x[], const double y[]);
	    dgraph(int n, const double x[], const double y[], const double z[]);
	    dgraph(int n, const double x[], const double y[], const double z[], const double w[]);

	    dgraph(int n,const var x[],const var y[]);
	    dgraph(int n,const var x[],const var y[],const var z[]);
	    dgraph(int n,const var x[],const var y[],const var z[],const var w[]);

	    dgraph(const std::vector<blop::var> &x,
		   const std::vector<blop::var> &y);
	    dgraph(const std::vector<blop::var> &x,
		   const std::vector<blop::var> &y,
		   const std::vector<blop::var> &z);
	    dgraph(const std::vector<blop::var> &x,
		   const std::vector<blop::var> &y,
		   const std::vector<blop::var> &z,
		   const std::vector<blop::var> &w);

	    dgraph(const std::vector<double> &x,
		   const std::vector<double> &y);
	    dgraph(const std::vector<double> &x,
		   const std::vector<double> &y,
		   const std::vector<double> &z);
	    dgraph(const std::vector<double> &x,
		   const std::vector<double> &y,
		   const std::vector<double> &z,
		   const std::vector<double> &w);

            dgraph(const var &filename,
                   function f1 = unset,
                   function f2 = unset,
                   function f3 = unset,
                   function f4 = unset,
                   function f5 = unset);


	    ~dgraph() {}

	    dgraph        &read    (istream &input,
				    function f1 = unset,
				    function f2 = unset,
				    function f3 = unset,
				    function f4 = unset,
                                    function f5 = unset);
	    static dgraph &read_static    (istream &input,
				    function f1 = unset,
				    function f2 = unset,
				    function f3 = unset,
				    function f4 = unset,
                                    function f5 = unset);

            // The 'condition' function must return at least 1 value, can return 2 values.
            // Those points for which the first returned value is zero, will be skipped
            // Whenver the second returnerd value is zero, the processing of the file will be interrupted.

	    dgraph        &read_if (istream &input,
				    const function &condition,
				    function f1 = unset,
				    function f2 = unset,
				    function f3 = unset,
				    function f4 = unset,
                                    function f5 = unset);
	    static dgraph &read_if_static (istream &input,
				    const function &condition,
				    function f1 = unset,
				    function f2 = unset,
				    function f3 = unset,
				    function f4 = unset,
                                    function f5 = unset);

	    dgraph        &read    (const var &filename,
				    function f1 = unset,
				    function f2 = unset,
				    function f3 = unset,
				    function f4 = unset,
                                    function f5 = unset);
	    static dgraph   &read_static  (const var &filename,
				    function f1 = unset,
				    function f2 = unset,
				    function f3 = unset,
				    function f4 = unset,
                                    function f5 = unset);

	    dgraph        &read_if (const var &filename,
				    const function &condition,
				    function f1 = unset,
				    function f2 = unset,
				    function f3 = unset,
				    function f4 = unset,
                                    function f5 = unset);
            static dgraph &read_if_static (const var &filename,
				    const function &condition,
				    function f1 = unset,
				    function f2 = unset,
				    function f3 = unset,
				    function f4 = unset,
                                    function f5 = unset);


	    static void read_groups_if(istream &file, const function &grouping_value,
				     const function &condition,
				     const function &filter, plottables *out);
	    static void read_groups_if(const var &filename, const function &grouping_value,
				       const function &condition,
				       const function &filter, plottables *out);

            dgraph &remove_unset(); // remove unset, inf, nan

            dgraph &comment_chars(const std::string &s);

	    graph &smooth(int col_index, double weight1=1, double weight2=2, double weight3=1);
	    graph &smooth_range(int col_index_x, double from, double to,
				int col_index,
				double weight1=1, double weight2=2, double weight3=1);

	    dgraph &add(const datapoint &p);
	    dgraph &add(const var &x1, const var &x2);
	    dgraph &add(const var &x1, const var &x2, const var &x3);
	    dgraph &add(const var &x1, const var &x2, const var &x3, const var &x4);
	    dgraph &add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5);
	    dgraph &add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6);
	    dgraph &add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6, const var &x7);
	    dgraph &add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6, const var &x7, const var &x8);
	    dgraph &add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6, const var &x7, const var &x8, const var &x9);
	    dgraph &add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6, const var &x7, const var &x8, const var &x9, const var &x10);
            dgraph &add(int n, double val[]);

            // --------------  Find the min/max of an expression evaluated over the whole graph --------
            // (just redefine graph::min(const function &) and graph::max(const function &)
            double min(const function &fmin, function fret=unset) { return graph::min(fmin,fret); }
            double max(const function &fmax, function fret=unset) { return graph::max(fmax,fret); }


	    // --------------  max/min value in a column  -----------------------------
	    // Find the max/min of column 'n', if column 'm' is
	    // in the range [low..high]

	    double min(int n, int m, double low, double high);
	    double max(int n, int m, double low, double high);

            // -------------- calculate the average of a column -----------------------

            // Return the average of an expression over the graph
            double average(blop::function &f);

            // Returns the average of column 'n' if column 'm' is in the range [low..high]
            double average(int n, int m, double low, double high);

	    // --------------  block --------------------------------------
	    // pick a block (continous set of data in a file/stream,
	    // between empty lines)
	    // i=1 corresponds to the first block
	    // This function alters the graph itself: it ERASES every
	    // other datapoint outside of the ith block.

	    graph &block(int i); 

            //html <a name="transform">
	    // -------------------  transform a column -----------------------------
	    // Make a transformation on a column specified by 'col' (1-based)
	    // The arguments given to the function 'f' are the values
	    // in all columns (that is, if you want to multiply column 2
	    // by column 1, then say: transform_column(2,_1*_2);

	    dgraph &transform_column(int col, function f);

            // -------------------  transform a whole dgraph ------------------------
            // Loop over the graph and transform each point. The function 'f' will be
            // called on the original point, and its return value will overwrite
            // the original datapoint. _0 evaluates to the index of the data-point (0-based)

            dgraph &transform(const function &f);

            //html </a>

	    // ------------  obtain a point, and resize if necessary -------------

	    datapoint &operator[] (int i);
	    const datapoint &operator[] (int i) const;
	    dgraph &resize(int newsize);


	    // --------------  autotitles ----------------------------------------

	    plottable &autotitles(bool f) { autotitles_ = f; return *this; }
	    static void default_autotitles(bool f) { default_autotitles_ = f; }

	    // ---------------  duplicate ----------------------------------------

	    dgraph &dup();

	    void prepare_for_draw(axis *,axis *, frame *, int count);

	    dgraph &layer(const var & l) { plottable::layer(l); return *this; }

            dgraph &sort(const blop::function &key);

            // -------------- filter ----------------------------------------------
            // Filter the graph: create a new one (using 'new') containing only those
            // datapoints for which the function returns true
            dgraph *filter(const blop::function &filt);
            dgraph *filter(const blop::function &filt, const blop::function &trans);
	};


}

std::ostream &operator<<(std::ostream &out, const blop::dgraph &g);


#endif
