#include "plot.h"
#include "frame.h"
#include "warning.h"
#include "pstream.h"
#include <algorithm>
#include <cmath>

namespace blop
{
    plotcmd_interpreter* &plotcmd_interpreter::get()
    {
	static plotcmd_interpreter *c = 0;
	return c;
    }
    void plotcmd_interpreter::set(plotcmd_interpreter *c)
    {
	get() = c;
    }

    plottable &plotcmd(const var &cmd)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplotcmd(cmd);
    }

    plottable &mplotcmd(const var &cmd)
    {
	plotcmd_interpreter *c = plotcmd_interpreter::get();
	plottable *p = 0;
	if(c == 0 || (p = c->run(cmd)) == 0)
	{
	    if(c == 0) warning::print("No plotcmd_interpreter was set",
				      "mplot_custom(const var &cmd)");
	    static dgraph g;
	    return g;
	}
	frame::current().add(p);
	return *p;
    }

    dgraph &plot_rectangle(const var &x1, const var &y1, const var &x2, const var &y2)
    {
	double x[5], y[5];
	x[0] = x1.dbl();
	y[0] = y1.dbl();
	x[1] = x2.dbl();
	y[1] = y1.dbl();
	x[2] = x2.dbl();
	y[2] = y2.dbl();
	x[3] = x1.dbl();
	y[3] = y2.dbl();
	x[4] = x1.dbl();
	y[4] = y1.dbl();
	return plot(5,x,y);
    }
    dgraph &mplot_rectangle(const var &x1, const var &y1, const var &x2, const var &y2)
    {
	double x[5], y[5];
	x[0] = x1.dbl();
	y[0] = y1.dbl();
	x[1] = x2.dbl();
	y[1] = y1.dbl();
	x[2] = x2.dbl();
	y[2] = y2.dbl();
	x[3] = x1.dbl();
	y[3] = y2.dbl();
	x[4] = x1.dbl();
	y[4] = y1.dbl();
	return mplot(5,x,y);
    }

    fgraph &plot(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(f1,f2,f3,f4,f5);
    }

    fgraph &mplot(const function &f1, const function &f2, const function &f3, const function &f4, const function &f5)
    {
        function f(f1,f2,f3,f4,f5);

        if(f.components()==1) f = function(_1,f1,f2,f3,f4,f5);
	fgraph *g = new fgraph(f);
	g->autodel(true);
	g->set_default_legend();
	frame::current().add(g);
	return *g;
    }

    plottable &mplot(plottable &g)
    {
	frame::current().add(&g);
	return g;
    }

    plottable &plot(plottable &g)
    {
	frame::current().clear_graphs_and_autosettings();
	frame::current().add(&g);
	return g;
    }


    dgraph &mplot(dgraph &g,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	if(!f1.initialized())
	{
	    frame::current().add(&g);
	    return g;
	}
	function f(f1,f2,f3,f4);
	int cols = f.components();

	dgraph *gg = new dgraph(g.size(), cols);
	for(unsigned int i=0; i<g.size(); ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    f.meval(g[i],(*gg)[i]);
	}
        function::clear_named_params();
	gg->autodel(true);
	gg->copy_style(g);
	frame::current().add(gg);
	return *gg;
    }

    // ------------------- plot_if/mplot_if for vectors ---------------------

    dgraph &plot(const std::vector<double> &y)
    {
        std::vector<double> x(y.size());
        for(unsigned int i=0; i<x.size(); ++i) x[i] = i;
        return plot(x,y);
    }
    dgraph &mplot(const std::vector<double> &y)
    {
        std::vector<double> x(y.size());
        for(unsigned int i=0; i<x.size(); ++i) x[i] = i;
        return mplot(x,y);
    }

    dgraph &plot(const std::vector<std::complex<double>> &z)
    {
        std::vector<double> x(z.size()), y(z.size());
        for(unsigned int i=0; i<z.size(); ++i)
        {
            x[i] = z[i].real();
            y[i] = z[i].imag();
        }
        return plot(x,y);
    }
    dgraph &mplot(const std::vector<std::complex<double>> &z)
    {
        std::vector<double> x(z.size()), y(z.size());
        for(unsigned int i=0; i<z.size(); ++i)
        {
            x[i] = z[i].real();
            y[i] = z[i].imag();
        }
        return mplot(x,y);
    }

    dgraph &mplot_if(const std::vector<double> &x,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	frame::current().add(g);
	if(condition.nargs() > 1)
	{
	    warning::print("Data arrays have insufficient columns to execute condition",
			   "mplot_if(vector<double>&,function& condition, ....)");
	    return *g;
	}

	function f(f1,f2,f3,f4);
	const int cols = f.components();
	if(f.initialized() && cols<2)
	{
	    warning::print("Filter functions produce less than 2 columns",
			   "mplot_if(vector<double>&,function &condition,function &f1..)");
	}

	datapoint p(cols);
	vector<var> args(1);
	for(unsigned int i=0; i<x.size(); ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
            
	    args[0] = x[i];
	    if(condition.eval(args).dbl() == 0.0) continue;
	    if(f.initialized())
	    {
		f.meval(args,p);
		g->add(p);
	    }
	    else
	    {
		g->add(args);
	    }
	}
        function::clear_named_params();
	return *g;
    }

    dgraph &mplot_if(const std::vector<double> &x,
		     const std::vector<double> &y,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	frame::current().add(g);
	if(condition.nargs() > 2)
	{
	    warning::print("Data arrays have insufficient columns to execute condition",
			   "mplot_if(vector<double>&,function& condition, ....)");
	    return *g;
	}

	function f(f1,f2,f3,f4);
	const int cols = f.components();
	if(f.initialized() && cols<2)
	{
	    warning::print("Filter functions produce less than 2 columns",
			   "mplot_if(vector<double>&,function &condition,function &f1..)");
	}

	datapoint p(cols);
	vector<var> args(2);
	const unsigned int npoints = std::min(x.size(), y.size());
	for(unsigned int i=0; i<npoints; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    args[0] = x[i];
	    args[1] = y[i];
	    if(condition.eval(args).dbl() == 0.0) continue;
	    if(f.initialized())
	    {
		f.meval(args,p);
		g->add(p);
	    }
	    else
	    {
		g->add(args);
	    }
	}
        function::clear_named_params();
	return *g;
    }

    dgraph &mplot_if(const std::vector<double> &x,
		     const std::vector<double> &y,
		     const std::vector<double> &z,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	frame::current().add(g);
	if(condition.nargs() > 3)
	{
	    warning::print("Data arrays have insufficient columns to execute condition",
			   "mplot_if(vector<double>&,function& condition, ....)");
	    return *g;
	}

	function f(f1,f2,f3,f4);
	const int cols = f.components();
	if(f.initialized() && cols<2)
	{
	    warning::print("Filter functions produce less than 2 columns",
			   "mplot_if(vector<double>&,function &condition,function &f1..)");
	}

	datapoint p(cols);
	vector<var> args(3);
	const unsigned int npoints = std::min(std::min(x.size(), y.size()),z.size());
	for(unsigned int i=0; i<npoints; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    args[0] = x[i];
	    args[1] = y[i];
	    args[2] = z[i];
	    if(condition.eval(args).dbl() == 0.0) continue;
	    if(f.initialized())
	    {
		f.meval(args,p);
		g->add(p);
	    }
	    else
	    {
		g->add(args);
	    }
	}
        function::clear_named_params();
	return *g;
    }
    dgraph &mplot_if(const std::vector<double> &x,
		     const std::vector<double> &y,
		     const std::vector<double> &z,
		     const std::vector<double> &w,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	frame::current().add(g);
	if(condition.nargs() > 4)
	{
	    warning::print("Data arrays have insufficient columns to execute condition",
			   "mplot_if(vector<double>&,function& condition, ....)");
	    return *g;
	}

	function f(f1,f2,f3,f4);
	const int cols = f.components();
	if(f.initialized() && cols<2)
	{
	    warning::print("Filter functions produce less than 2 columns",
			   "mplot_if(vector<double>&,function &condition,function &f1..)");
	}

	datapoint p(cols);
	vector<var> args(4);
	const unsigned int npoints = std::min(std::min(x.size(), y.size()),z.size());
	for(unsigned int i=0; i<npoints; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    args[0] = x[i];
	    args[1] = y[i];
	    args[2] = z[i];
	    args[3] = w[i];
	    if(condition.eval(args).dbl() == 0.0) continue;
	    if(f.initialized())
	    {
		f.meval(args,p);
		g->add(p);
	    }
	    else
	    {
		g->add(args);
	    }
	}
        function::clear_named_params();
	return *g;
    }

    dgraph &plot_if(const std::vector<double> &x,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(x,condition,f1,f2,f3,f4);
    }
    dgraph &plot_if(const std::vector<double> &x,
		    const std::vector<double> &y,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(x,y,condition,f1,f2,f3,f4);
    }
    dgraph &plot_if(const std::vector<double> &x,
		    const std::vector<double> &y,
		    const std::vector<double> &z,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(x,y,z,condition,f1,f2,f3,f4);
    }
    dgraph &plot_if(const std::vector<double> &x,
		    const std::vector<double> &y,
		    const std::vector<double> &z,
		    const std::vector<double> &w,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(x,y,z,w,condition,f1,f2,f3,f4);
    }



    dgraph &mplot_if(const std::vector<var> &x,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	frame::current().add(g);
	if(condition.nargs() > 1)
	{
	    warning::print("Data arrays have insufficient columns to execute condition",
			   "mplot_if(vector<var>&,function& condition, ....)");
	    return *g;
	}

	function f(f1,f2,f3,f4);
	const int cols = f.components();
	if(f.initialized() && cols<2)
	{
	    warning::print("Filter functions produce less than 2 columns",
			   "mplot_if(vector<var>&,function &condition,function &f1..)");
	}

	datapoint p(cols);
	vector<var> args(1);
	for(unsigned int i=0; i<x.size(); ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    args[0] = x[i];
	    if(condition.eval(args).dbl() == 0.0) continue;
	    if(f.initialized())
	    {
		f.meval(args,p);
		g->add(p);
	    }
	    else
	    {
		g->add(args);
	    }
	}
        function::clear_named_params();
	return *g;
    }

    dgraph &mplot_if(const std::vector<var> &x,
		     const std::vector<var> &y,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	frame::current().add(g);
	if(condition.nargs() > 2)
	{
	    warning::print("Data arrays have insufficient columns to execute condition",
			   "mplot_if(vector<var>&,function& condition, ....)");
	    return *g;
	}

	function f(f1,f2,f3,f4);
	const int cols = f.components();
	if(f.initialized() && cols<2)
	{
	    warning::print("Filter functions produce less than 2 columns",
			   "mplot_if(vector<var>&,function &condition,function &f1..)");
	}

	datapoint p(cols);
	vector<var> args(2);
	const unsigned int npoints = std::min(x.size(), y.size());
	for(unsigned int i=0; i<npoints; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    args[0] = x[i];
	    args[1] = y[i];
	    if(condition.eval(args).dbl() == 0.0) continue;
	    if(f.initialized())
	    {
		f.meval(args,p);
		g->add(p);
	    }
	    else
	    {
		g->add(args);
	    }
	}
        function::clear_named_params();
	return *g;
    }

    dgraph &mplot_if(const std::vector<var> &x,
		     const std::vector<var> &y,
		     const std::vector<var> &z,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	frame::current().add(g);
	if(condition.nargs() > 3)
	{
	    warning::print("Data arrays have insufficient columns to execute condition",
			   "mplot_if(vector<var>&,function& condition, ....)");
	    return *g;
	}

	function f(f1,f2,f3,f4);
	const int cols = f.components();
	if(f.initialized() && cols<2)
	{
	    warning::print("Filter functions produce less than 2 columns",
			   "mplot_if(vector<var>&,function &condition,function &f1..)");
	}

	datapoint p(cols);
	vector<var> args(3);
	const unsigned int npoints = std::min(std::min(x.size(), y.size()),z.size());
	for(unsigned int i=0; i<npoints; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    args[0] = x[i];
	    args[1] = y[i];
	    args[2] = z[i];
	    if(condition.eval(args).dbl() == 0.0) continue;
	    if(f.initialized())
	    {
		f.meval(args,p);
		g->add(p);
	    }
	    else
	    {
		g->add(args);
	    }
	}
        function::clear_named_params();
	return *g;
    }
    dgraph &mplot_if(const std::vector<var> &x,
		     const std::vector<var> &y,
		     const std::vector<var> &z,
		     const std::vector<var> &w,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	frame::current().add(g);
	if(condition.nargs() > 4)
	{
	    warning::print("Data arrays have insufficient columns to execute condition",
			   "mplot_if(vector<var>&,function& condition, ....)");
	    return *g;
	}

	function f(f1,f2,f3,f4);
	const int cols = f.components();
	if(f.initialized() && cols<2)
	{
	    warning::print("Filter functions produce less than 2 columns",
			   "mplot_if(vector<var>&,function &condition,function &f1..)");
	}

	datapoint p(cols);
	vector<var> args(4);
	const unsigned int npoints = std::min(std::min(x.size(), y.size()),z.size());
	for(unsigned int i=0; i<npoints; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    args[0] = x[i];
	    args[1] = y[i];
	    args[2] = z[i];
	    args[3] = w[i];
	    if(condition.eval(args).dbl() == 0.0) continue;
	    if(f.initialized())
	    {
		f.meval(args,p);
		g->add(p);
	    }
	    else
	    {
		g->add(args);
	    }
	}
        function::clear_named_params();
	return *g;
    }

    dgraph &plot_if(const std::vector<var> &x,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(x,condition,f1,f2,f3,f4);
    }
    dgraph &plot_if(const std::vector<var> &x,
		    const std::vector<var> &y,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(x,y,condition,f1,f2,f3,f4);
    }
    dgraph &plot_if(const std::vector<var> &x,
		    const std::vector<var> &y,
		    const std::vector<var> &z,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(x,y,z,condition,f1,f2,f3,f4);
    }
    dgraph &plot_if(const std::vector<var> &x,
		    const std::vector<var> &y,
		    const std::vector<var> &z,
		    const std::vector<var> &w,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(x,y,z,w,condition,f1,f2,f3,f4);
    }


    // ----------------------------------------------------------------------

    dgraph &mplot_if(dgraph &g,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	dgraph *gg = new dgraph;
	gg->autodel(true);
	gg->copy_style(g);
	frame::current().add(gg);

	if(condition.nargs() > g.columns())
	{
	    warning::print("Graph has insufficient columns to execute condition",
			   "mplot_if(dgraph&,function& condition, ....)");
	    return *gg;
	}

	function f(f1,f2,f3,f4);
	int cols = f.components();

	datapoint p(cols);
	for(unsigned int i=0; i<g.size(); ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    if(condition.eval(g[i]).dbl() == 0.0) continue;
	    if(f.initialized())
	    {
		f.meval(g[i],p);
		gg->add(p);
	    }
	    else
	    {
		gg->add(g[i]);
	    }
	}
        function::clear_named_params();
	return *gg;
    }

    
    fgraph &plot_if(const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(condition,f1,f2,f3,f4);
    }

    fgraph &mplot_if(const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4)
    {
	fgraph *g = new fgraph(function(f1,f2,f3,f4));
	g->filter(condition);
	g->autodel(true);
	g->set_default_legend();
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(dgraph &g,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(g,f1,f2,f3,f4);
    }

    dgraph &plot_if(dgraph &g,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(g,condition,f1,f2,f3,f4);
    }

    hist &hisplot(const var &filename, const function &f1, const function &f2, const histopt &hopt)
    {
        frame::current().clear_graphs_and_autosettings();
        return mhisplot(filename,f1,f2,hopt);
    }

    hist &mhisplot(const var &filename, const function &f1, const function &f2, const histopt &hopt)
    {
        hist *result = 0;
        function filter;
        if(f1.initialized()) filter.append(f1);
        if(f2.initialized()) filter.append(f2);

        bool needs_buffering = (hopt.buffer_size()>0);
        if(filter.components() == 2)
        {
            if(hopt.min(1)==unset || hopt.max(1)==unset || hopt.min(2)==unset || hopt.max(2)==unset) needs_buffering = true;
            if(hopt.bins(1)==0 || hopt.bins(2)==0) needs_buffering = true;
        }
        else
        {
            if(hopt.min(1)==unset || hopt.max(1)==unset) needs_buffering = true;
            if(hopt.bins(1)==0) needs_buffering = true;
        }

        istream *file = openin(filename);
        int lineno = 0;
        string line;
        vector<var> input;
        vector<var> filtered;
        if(needs_buffering)
        {
            warning::print("Buffering in mhisplot should use the built-in buffering of 'hist'","mhisplot");

            const unsigned int nbuffer = 2000;
            
            vector<double> buffer1, buffer2;
            buffer1.reserve(nbuffer+10);
            if(filter.components()>=2) buffer2.reserve(nbuffer+10);
            
            
            double min1=0, max1=0, min2=0, max2=0;
            
            // First, process max 1000 data lines
            for(; getline(*file,line) && buffer1.size()<nbuffer; ++lineno)
            {
                function::clear_named_params();
                function::named_param("linenumber",lineno);
                split_quoted(line,input);
                
                // skip empty or comment lines
                if(input.empty()) continue;
                if(input[0][0] == '#') continue;
                
                if(filter.initialized() && filter.components()>=1)
                {
                    filter.meval(input,filtered);
                    
                    if(buffer1.empty() || filtered[0].dbl()<min1) min1 = filtered[0].dbl();
                    if(buffer1.empty() || filtered[0].dbl()>max1) max1 = filtered[0].dbl();
                    buffer1.push_back(filtered[0].dbl());
                    
                    if(filter.components()>=2)
                    {
                        if(buffer2.empty() || filtered[1].dbl()<min2) min2 = filtered[1].dbl();
                        if(buffer2.empty() || filtered[1].dbl()>max2) max2 = filtered[1].dbl();
                        buffer2.push_back(filtered[1].dbl());
                    }
                }
                else
                {
                    for(unsigned int i=0; i<input.size(); ++i)
                    {
                        if(buffer1.empty() || input[i].dbl()<min1) min1=input[i].dbl();
                        if(buffer1.empty() || input[i].dbl()>max1) max1=input[i].dbl();
                        buffer1.push_back(input[i].dbl());
                    }
                }
            }
            function::clear_named_params();
            // We have processed the first 2000 lines. Now we have enough statistics (hopefully) to estimate
            // the histogra ranges
            
            double range1 = max1-min1;
            min1 -= range1/10;
            max1 += range1/10;
            int nbins = std::min(std::max(20,(int)(buffer1.size())/10),80);

            // If the range is nearly symmetric around 0, make it symmetric
            if(min1*max1<0 && (::fabs(min1)-::fabs(max1))/(::fabs(min1)+::fabs(max1))<0.05)
            {
                max1 = std::max(::fabs(min1),::fabs(max1));
                min1 = -max1;
            }

            if(filter.components()>=2)
            {
                double range2 = max2-min2;
                min2 -= range2/10;
                max2 += range2/10;


                // If the range is nearly symmetric around 0, make it symmetric
                if(min2*max2<0 && (::fabs(min2)-::fabs(max2))/(::fabs(min2)+::fabs(max2))<0.05)
                {
                    max2 = std::max(::fabs(min2),::fabs(max2));
                    min2 = -max2;
                }

                //cerr<<"Creating 2D histogram: "<<endl;
                //cerr<<"  "<<nbins<<" bins: "<<min1<<" .. "<<max1<<endl;
                //cerr<<"  "<<nbins<<" bins: "<<min2<<" .. "<<max2<<endl;

                result = new hist(hopt.min(1)==unset?min1:hopt.min(1),
                                  hopt.max(1)==unset?max1:hopt.max(1),
                                  hopt.bins(1)==0?nbins:hopt.bins(1),
                                  hopt.min(2)==unset?min2:hopt.min(2),
                                  hopt.max(2)==unset?max2:hopt.max(2),
                                  hopt.bins(2)==0?nbins:hopt.bins(2));
            }
            else
            {
                //cerr<<"Creating 1D histogram: "<<endl;
                //cerr<<"  "<<nbins<<" bins: "<<min1<<" .. "<<max1<<endl;
                result = new hist(hopt.min(1)==unset?min1:hopt.min(1),
                                  hopt.max(1)==unset?max1:hopt.max(1),
                                  hopt.bins(1)==0?nbins:hopt.bins(1));
            }
            
            for(unsigned int i=0; i<buffer1.size(); ++i)
            {
                if(filter.components()>=2) result->fill(buffer1[i],buffer2[i]);
                else                       result->fill(buffer1[i]);
            }
        }
        else
        {
            if(filter.components()>=2) result = new hist(hopt.min(1),hopt.max(1),hopt.bins(1),
                                                         hopt.min(2),hopt.max(2),hopt.bins(2));
            else                       result = new hist(hopt.min(1),hopt.max(1),hopt.bins(1));
        }

        // read the rest of the file

        for(; getline(*file,line); ++lineno)
        {
            function::clear_named_params();
            function::named_param("linenumber",lineno);
            split_quoted(line,input);

            // skip empty or comment lines
            if(input.empty()) continue;
            if(input[0][0] == '#') continue;

            if(filter.initialized() && filter.components()>=1)
            {
                filter.meval(input,filtered);
                if(filter.components()>=2) result->fill(filtered[0].dbl(),filtered[1].dbl());
                else                       result->fill(filtered[0].dbl());
            }
            else
            {
                for(unsigned int i=0; i<input.size(); ++i) result->fill(input[i].dbl());
            }
        }
        function::clear_named_params();

        delete file;

        result->autodel(true);
        frame::current().add(result);
        return *result;
    }

    dgraph &plot(const var &filename,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4,
		 const function &f5,
		 const function &f6)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(filename, f1, f2, f3, f4, f5, f6);
    }

    dgraph &mplot(const var &filename,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4,
		  const function &f5,
		  const function &f6)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	function filter;
	filter.append(f1);
	filter.append(f2);
	filter.append(f3);
	filter.append(f4);
	filter.append(f5);
	filter.append(f6);
	g->read(filename, filter);
	frame::current().add(g);
	return *g;
    }

    // -------------------

    dgraph &plot(int n,
		 double *x,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(n,x,f1,f2,f3,f4);
    }
    dgraph &mplot(int n,
		  double *x,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	function transform(f1,f2,f3,f4);
	dgraph *g = new dgraph(n,transform.components());
	vector<var> in(1),out(transform.components());
	for(int i=0; i<n; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    in[0] = x[i];
	    transform.meval(in,out);
	    for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	}
        function::clear_named_params();
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(int n,
		 double *x,
		 double *y,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(n,x,y,f1,f2,f3,f4);
    }
    dgraph &mplot(int n,
		  double *x,
		  double *y,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(2),out(transform.components());
	    for(int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(n,x,y);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(int n,
		 double *x,
		 double *y,
		 double *z,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(n,x,y,z,f1,f2,f3,f4);
    }
    dgraph &mplot(int n,
		  double *x,
		  double *y,
		  double *z,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(3),out(transform.components());
	    for(int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		in[2] = z[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(n,x,y,z);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(int n,
		 double *x,
		 double *y,
		 double *z,
		 double *w,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(n,x,y,z,w,f1,f2,f3,f4);
    }
    dgraph &mplot(int n,
		  double *x,
		  double *y,
		  double *z,
		  double *w,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(4),out(transform.components());
	    for(int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		in[2] = z[i];
		in[3] = w[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(n,x,y,z,w);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    // ------------

    dgraph &plot(int n,
		 var *x,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(n,x,f1,f2,f3,f4);
    }
    dgraph &mplot(int n,
		  var *x,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	function transform(f1,f2,f3,f4);
	dgraph *g = new dgraph(n,transform.components());
	vector<var> in(1),out(transform.components());
	for(int i=0; i<n; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    in[0] = x[i];
	    transform.meval(in,out);
	    for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	}
        function::clear_named_params();
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(int n,
		 var *x,
		 var *y,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(n,x,y,f1,f2,f3,f4);
    }
    dgraph &mplot(int n,
		  var *x,
		  var *y,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(2),out(transform.components());
	    for(int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(n,x,y);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(int n,
		 var *x,
		 var *y,
		 var *z,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(n,x,y,z,f1,f2,f3,f4);
    }
    dgraph &mplot(int n,
		  var *x,
		  var *y,
		  var *z,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(3),out(transform.components());
	    for(int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		in[2] = z[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(n,x,y,z);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(int n,
		 var *x,
		 var *y,
		 var *z,
		 var *w,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(n,x,y,z,w,f1,f2,f3,f4);
    }
    dgraph &mplot(int n,
		  var *x,
		  var *y,
		  var *z,
		  var *w,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(4),out(transform.components());
	    for(int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		in[2] = z[i];
		in[3] = w[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(n,x,y,z,w);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }




    // ------------

    dgraph &plot(double x, double y)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y);
    }
    dgraph &mplot(double x, double y)
    {
	dgraph *g = new dgraph(1,&x,&y);
	g->autodel(true);
	g->drawstyle(points());
	frame::current().add(g);
	return *g;
    }
    dgraph &plot(double x, var y)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y);
    }
    dgraph &mplot(double x, var y)
    {
	dgraph *g = new dgraph;
        g->add(x,y);
	g->autodel(true);
	g->drawstyle(points());
	frame::current().add(g);
	return *g;
    }
    dgraph &plot(var x, double y)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y);
    }
    dgraph &mplot(var x, double y)
    {
	dgraph *g = new dgraph;
        g->add(x,y);
	g->autodel(true);
	g->drawstyle(points());
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(double x, double y, double z)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y,z);
    }
    dgraph &mplot(double x, double y, double z)
    {
	dgraph *g = new dgraph(1,&x,&y,&z);
	g->autodel(true);
	g->drawstyle(points());
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(double x, double y, var z)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y,z);
    }
    dgraph &mplot(double x, double y, var z)
    {
	dgraph *g = new dgraph;
        g->add(x,y,z);
	g->autodel(true);
	g->drawstyle(points());
	frame::current().add(g);
	return *g;
    }


    dgraph &plot(const var &x, const var &y, const var &z)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y,z);
    }
    dgraph &mplot(const var &x, const var &y, const var &z)
    {
	dgraph *g = 0;
	if(z.dbl() != unset) g = new dgraph(1,&x,&y,&z);
	else                 g = new dgraph(1,&x,&y);
	g->autodel(true);
	g->drawstyle(points());
	frame::current().add(g);
	return *g;
    }


    dgraph &plot(const vector<double> &x,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,f1,f2,f3,f4);
    }
    dgraph &mplot(const vector<double> &x,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	unsigned int n=x.size();
	function transform(f1,f2,f3,f4);
	dgraph *g = new dgraph(n,transform.components());
	vector<var> in(1),out(transform.components());
	for(unsigned int i=0; i<n; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    in[0] = x[i];
	    transform.meval(in,out);
	    for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	}
        function::clear_named_params();
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(const vector<double> &x,
		 const vector<double> &y,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y,f1,f2,f3,f4);
    }
    dgraph &mplot(const vector<double> &x,
		  const vector<double> &y,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	unsigned int n=std::min(x.size(),y.size());
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(2),out(transform.components());
	    for(unsigned int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(x,y);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(const vector<double> &x,
		 const vector<double> &y,
		 const vector<double> &z,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y,z,f1,f2,f3,f4);
    }
    dgraph &mplot(const vector<double> &x,
		  const vector<double> &y,
		  const vector<double> &z,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	unsigned int n=std::min(std::min(x.size(),y.size()),z.size());
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(3),out(transform.components());
	    for(unsigned int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		in[2] = z[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(x,y,z);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(const vector<double> &x,
		 const vector<double> &y,
		 const vector<double> &z,
		 const vector<double> &w,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y,z,w,f1,f2,f3,f4);
    }
    dgraph &mplot(const vector<double> &x,
		  const vector<double> &y,
		  const vector<double> &z,
		  const vector<double> &w,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	unsigned int n=std::min(std::min(std::min(x.size(),y.size()),z.size()),w.size());
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(4),out(transform.components());
	    for(unsigned int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		in[2] = z[i];
		in[3] = w[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(x,y,z,w);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }


    // ------------


    dgraph &plot(const vector<var> &x,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,f1,f2,f3,f4);
    }
    dgraph &mplot(const vector<var> &x,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	unsigned int n=x.size();
	function transform(f1,f2,f3,f4);
	dgraph *g = new dgraph(n,transform.components());
	vector<var> in(1),out(transform.components());
	for(unsigned int i=0; i<n; ++i)
	{
            function::clear_named_params();
	    function::named_param("linenumber",i);
	    in[0] = x[i];
	    transform.meval(in,out);
	    for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	}
        function::clear_named_params();
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(const vector<var> &x,
		 const vector<var> &y,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y,f1,f2,f3,f4);
    }
    dgraph &mplot(const vector<var> &x,
		  const vector<var> &y,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	unsigned int n=std::min(x.size(),y.size());
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(2),out(transform.components());
	    for(unsigned int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(x,y);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(const vector<var> &x,
		 const vector<var> &y,
		 const vector<var> &z,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y,z,f1,f2,f3,f4);
    }
    dgraph &mplot(const vector<var> &x,
		  const vector<var> &y,
		  const vector<var> &z,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	unsigned int n=std::min(std::min(x.size(),y.size()),z.size());
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(3),out(transform.components());
	    for(unsigned int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		in[2] = z[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(x,y,z);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }

    dgraph &plot(const vector<var> &x,
		 const vector<var> &y,
		 const vector<var> &z,
		 const vector<var> &w,
		 const function &f1,
		 const function &f2,
		 const function &f3,
		 const function &f4)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot(x,y,z,w,f1,f2,f3,f4);
    }
    dgraph &mplot(const vector<var> &x,
		  const vector<var> &y,
		  const vector<var> &z,
		  const vector<var> &w,
		  const function &f1,
		  const function &f2,
		  const function &f3,
		  const function &f4)
    {
	dgraph *g = 0;
	unsigned int n=std::min(std::min(std::min(x.size(),y.size()),z.size()),w.size());
	if(f1.initialized())
	{
	    function transform(f1,f2,f3,f4);
	    g = new dgraph(n,transform.components());
	    vector<var> in(4),out(transform.components());
	    for(unsigned int i=0; i<n; ++i)
	    {
                function::clear_named_params();
		function::named_param("linenumber",i);
		in[0] = x[i];
		in[1] = y[i];
		in[2] = z[i];
		in[3] = w[i];
		transform.meval(in,out);
		for(unsigned int j=0; j<out.size(); ++j) (*g)[i][j] = out[j];
	    }
            function::clear_named_params();
	}
	else
	{
	    g = new dgraph(x,y,z,w);
	}
	g->autodel(true);
	frame::current().add(g);
	return *g;
    }


    // ------------


     plottables plot_groups(const var &filename,
                            const function &grouping_value,
                            const function &f1,
                            const function &f2,
                            const function &f3,
                            const function &f4,
                            const function &f5,
                            const function &f6)
     {
	frame::current().clear_graphs_and_autosettings();
	return mplot_groups(filename, grouping_value, f1, f2, f3, f4, f5, f6);
    }

    plottables plot_groups_if(const var &filename,
			       const function &grouping_value,
			       const function &condition,
			       const function &f1,
			       const function &f2,
			       const function &f3,
			       const function &f4,
			       const function &f5,
			       const function &f6)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_groups_if(filename, grouping_value, condition, f1, f2, f3, f4, f5, f6);
    }

    plottables mplot_groups(const var &filename,
			   const function &grouping_value,
			   const function &f1,
			   const function &f2,
			   const function &f3,
			   const function &f4,
			   const function &f5,
			   const function &f6)
    {
	return mplot_groups_if(filename, grouping_value, unset, f1, f2, f3, f4, f5, f6);
    }


    plottables mplot_groups_if(const var &filename,
				const function &grouping_value,
				const function &condition,
				const function &f1,
				const function &f2,
				const function &f3,
				const function &f4,
				const function &f5,
				const function &f6)
    {
	plottables graphs;
	function filter;
	filter.append(f1);
	filter.append(f2);
	filter.append(f3);
	filter.append(f4);
	filter.append(f5);
	filter.append(f6);
	dgraph::read_groups_if(filename, grouping_value, condition, filter, &graphs);
	for(unsigned int i=0; i<graphs.size(); ++i)
	{
	    graphs[i]->autodel(true);
	    frame::current().add(graphs[i]);
	}
	return graphs;
    }

    // ------------------ plot many files (filename pattern) ------------------

    plottables plot_many(const var &filename_pattern,
			  const function &f1,
			  const function &f2,
			  const function &f3,
			  const function &f4,
			  const function &f5,
			  const function &f6)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_many(filename_pattern,f1,f2,f3,f4,f5,f6);
    }

    plottables mplot_many(const var &filename_pattern,
			   const function &f1,
			   const function &f2,
			   const function &f3,
			   const function &f4,
			   const function &f5,
			   const function &f6)
    {
	string cmd = filename_pattern.str();
	plottables graphs;
	if(cmd.find(" ") == string::npos) cmd = "echo " + cmd;
	ipstream filelist(cmd);
	string fn;
	while(filelist>>fn)
	{
	    graphs.add(&mplot(fn,f1,f2,f3,f4,f5,f6).ac(autocolor).pt(autopoint()));
	}
	return graphs;
    }

    // --------------- plot with condition ------------------------------------

    dgraph &plot_if(const var &filename,
		    const function &condition,
		    const function &f1,
		    const function &f2,
		    const function &f3,
		    const function &f4,
                    const function &f5)
    {
	frame::current().clear_graphs_and_autosettings();
	return mplot_if(filename, condition, f1, f2, f3, f4, f5);
    }
    
    
    dgraph &mplot_if(const var &filename,
		     const function &condition,
		     const function &f1,
		     const function &f2,
		     const function &f3,
		     const function &f4,
                     const function &f5)
    {
	dgraph *g = new dgraph;
	g->autodel(true);
	//function filter(f1,f2,f3,f4);
	//g->read_if(filename,condition,filter);
        g->read_if(filename,condition,f1,f2,f3,f4,f5);
	frame::current().add(g);
	return *g;
    }

}
