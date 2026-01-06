#ifndef __BLOP_GRAPH_H__
#define __BLOP_GRAPH_H__

#include "plottable.h"
#include <vector>

namespace blop
{
    class axis;

    class graph : public plottable
	{
	protected:
	    std::vector<blop::datapoint> data_;
	    void make_latex_legend(string fn);

	public:

	    graph();
	    graph (const graph &);
	    virtual ~graph();
	    const graph &operator=(const graph &);

	    size_type size() const   { return data_.size(); }
	    void clear() { data_.clear(); }
	    void resize(int n) { data_.resize(n); }
	    const datapoint* get(int index) const { return &data_[index]; }

	    const datapoint &operator[](int i) const { return data_[i]; }
	    datapoint &operator[]      (int i)       { return data_[i]; }

	    virtual void prepare_for_draw(axis *,axis *, frame *, int count);

	    // -------------------------------------------------------------------
	    // find the max/min values of an expression over the whole graph, and return the value of 
            // the expression 'fret' on that point. By default it is the min/max value itself,
            // but it can be set for example to another column to return the 'position' of the min/max.

	    var max(const function &fmax, function fret=unset) const;
	    var min(const function &fmin, function fret=unset) const;

	    var max_if(const function &f, const function &condition) const;
	    var min_if(const function &f, const function &condition) const;

            graph &sort(int col);


	    // -------------------------------------------------------------------
	    // pick a block (continous set of data in a file/stream,
	    // between empty lines)
	    // i=1 corresponds to the first block

	    virtual graph &block(int i) {return *this;}


	    // -------------------------------------------------------------------
	    // Smooth a column

	    virtual graph &smooth(int col_index, double weight1=1, double weight2=2, double weight3=1)
		{return *this;}
	    virtual graph &smooth_range(int col_index_x, double from, double to,
					int col_index,
					double weight1=1, double weight2=2, double weight3=1)
		{
		    return *this;
		}

	};

}

#endif
