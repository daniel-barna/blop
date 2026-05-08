#include "graph.h"
#include "axis.h"
#include "warning.h"
#include "global.h"

#include <fstream>
#include <algorithm>

namespace blop
{
    graph::graph(const graph &o) : plottable(o), data_(o.data_)
    {
    }

    const graph &graph::operator=(const graph &o)
    {
	plottable::operator=(o);
	data_ = o.data_;
	return o;
    }

    void graph::prepare_for_draw(axis *,axis *, frame *f, int count)
    {
	if(global::debug>0) cout<<"[blop] [graph] prepare_for_draw begins..."<<endl;
	if(graph_drawer_) graph_drawer_->prepare_for_draw(this, f, count);
	if(global::debug>0) cout<<"[blop] [graph] prepare_for_draw finished."<<endl;
    }	

    graph::~graph()
    {
    }

    graph::graph()
    {
    }

    var graph::min(const function &fmin, function fret) const
    {
	if(empty())
	{
	    warning::print("Empty graph","dgraph::min(function)");
	    return "";
	}
        if(!fret.initialized()) fret=fmin;
        double min_val=0;
        var    min_pos="";
	for(size_type i=0; i<size(); ++i)
	{
	    var val = fmin.eval(data_[i]);
	    if(val.dbl() < min_val || i==0)
            {
                min_val = val;
                min_pos = fret.eval(data_[i]);
            }
	}
	return min_pos;
    }
    var graph::max(const function &fmin, function fret) const
    {
	if(empty())
	{
	    warning::print("Empty graph","dgraph::max(function)");
	    return "";
	}
        if(!fret.initialized()) fret=fmin;
        double min_val=0;
        var    min_pos="";
	for(size_type i=0; i<size(); ++i)
	{
	    var val = fmin.eval(data_[i]);
	    if(val.dbl() > min_val || i==0)
            {
                min_val = val;
                min_pos = fret.eval(data_[i]);
            }
	}
	return min_pos;
    }

    var graph::min_if(const function &f, const function &condition) const
    {
	if(empty())
	{
	    warning::print("Empty graph","dgraph::min_if(function,function)");
	    return "";
	}

	var result = "";
	for(size_type i=0; i<size(); ++i)
	{
	    var v = f.eval(data_[i]);
	    var cond = condition.eval(data_[i]);
	    if(cond.dbl() != 0.0 && (v.dbl() < result.dbl() || result.str() == "")) result = v;
	}
	return result;
    }
    
    var graph::max_if(const function &f, const function &condition) const
    {
	if(empty())
	{
	    warning::print("Empty graph","dgraph::max_if(function,function)");
	    return "";
	}

	var result = "";
	for(size_type i=0; i<size(); ++i)
	{
	    var v = f.eval(data_[i]);
	    var cond = condition.eval(data_[i]);
	    if(cond.dbl() != 0.0 && (v.dbl() > result.dbl() || result.str() == "")) result = v;
	}
	return result;
    }



    // convert the string 'fn' to a latex-compatible string
    void graph::make_latex_legend(string fn)
    {
	string leg;
	if(fn.find("<<") == 0) leg = "STDIN";
	else
	{
	    for(unsigned int i=0; i<fn.size(); ++i)
	    {
		switch(fn[i])
		{
		case '<': leg += "$<$"; break;
		case '>': leg += "$>$"; break;
		case '_': leg += "\\_"; break;
		case '%': leg += "\\%"; break;
		case '@': leg += "\\@"; break;
		case '{': leg += "$\\{$"; break;
		case '}': leg += "$\\}$"; break;
		case '$': leg += "\\$"; break;
		case '&': leg += "\\&"; break;
		default: leg += fn[i];
		}
	    }
	}
	legend(leg);
    }

    graph &graph::sort(int col)
    {
        std::sort(data_.begin(), data_.end(), [col](const blop::datapoint &p1, const blop::datapoint &p2) -> bool { return p1[col-1] < p2[col-1]; });
        return *this;
    }

}
