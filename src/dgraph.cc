#include "dgraph.h"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include "exc.hh"
#include "pstream.h"
#include "warning.h"
#include "bloputils.h"
#include "ignore.h"

namespace blop
{
/*
    the indexing of data points should be changed everywyere. 
    also in the graph_drawer at a few places
*/

    dgraph &dgraph::dup()
    {
	dgraph *result = new dgraph(*this);
	if(parent_) parent_->add(result);
	return *result;
    }

    dgraph &dgraph::columns(int ncols)
    {
	columns_ = ncols;
	for(unsigned int i=0; i<data_.size(); ++i) data_[i].resize(ncols);
	return *this;
    }
    dgraph &dgraph::resize(int newsize)
    {
	if(columns_ <= 0)
	{
	    warning::print("Do not know the number of columns yet. It is set now to 2",
			   "dgraph::resize(int newsize)");
	    columns_ = 2;
	    if(!data_.empty())
	    {
		warning::print("Strange, this should not happen...",
			       "dgraph::resize(int)");
		for(unsigned int i=0; i<data_.size(); ++i) data_[i].resize(2);
	    }
	}
	data_.resize(newsize,datapoint(columns_));
	return *this;
    }

    datapoint &dgraph::operator[](int i)
    {
	if(i<0)
	{
	    static datapoint dummy;
	    warning::print("Negative index","graph::operator[](int)");
	    return dummy;
	}
	if(i>=(int)data_.size())
	{
	    data_.resize(i+1,datapoint(columns()));
	}
	return data_[i];
    }
    const datapoint &dgraph::operator[](int i) const
    {
	if(i<0 || (int)data_.size()<=i)
	{
	    static datapoint dummy;
	    warning::print("Index out of range","graph::operator[](int) const");
	    return dummy;
	}
	return data_[i];
    }


    dgraph::dgraph(int npoints, int cols)
    {
	data_.resize(npoints, datapoint(columns_ = cols));
	init_defaults();
    }

    dgraph::dgraph(const dgraph &o) : graph(o)
    {
	columns_ = o.columns_;
    }
    const dgraph &dgraph::operator=(const dgraph &o)
    {
	graph::operator=(o);
	columns_ = o.columns_;
	return o;
    }

    dgraph::dgraph(const var &filename,
                   function f1,
                   function f2,
                   function f3,
                   function f4,
                   function f5)
    {
        read(filename,f1,f2,f3,f4,f5);
    }

    std::string dgraph::default_comment_chars_ = "#%";
    bool dgraph::default_autotitles_ = true;
    length dgraph::default_pointsize_ = PS; //(new length::base_id_t(terminal::PS));
    point_drawer *dgraph::default_point_drawer_ = new autopoint;
    color dgraph::default_pointcolor_(0,0,0);
    color dgraph::default_linecolor_(0,0,0);
    length dgraph::default_linewidth_ = LW; //(new length::base_id_t(terminal::LW));
    sym::linestyle dgraph::default_linestyle_ = sym::solid;
    color dgraph::default_fillcolor_(0,0,0);
    bool dgraph::default_fill_ = false;
    color dgraph::default_legendcolor_(0,0,0);
    bool dgraph::falsecondition_break_ = true;

    graph_drawer* &dgraph::default_graph_drawer_()
    {
	static graph_drawer *d = new points;
	return d;
    }
    var dgraph::default_legend_ = "%f %C";

    void dgraph::default_comment_chars(const std::string &s) { default_comment_chars_ = s; }
    void dgraph::default_pointsize(length l) { default_pointsize_ = l; }
    void dgraph::default_pointtype(const point_drawer &p)
    {
	delete default_point_drawer_;
	if(dynamic_cast<const autopoint *>(&p)) default_point_drawer_ = new autopoint();
	else default_point_drawer_ = p.clone();
    }
    void dgraph::default_pointcolor(const color &c) { default_pointcolor_ = c; }
    void dgraph::default_linecolor(const color &c) { default_linecolor_ = c; }
    void dgraph::default_linewidth(length l) { default_linewidth_ = l; }
    void dgraph::default_linestyle(sym::linestyle i) { default_linestyle_ = i; }
    void dgraph::default_fillcolor(const color &c) { default_fillcolor_ = c; }
    void dgraph::default_fill(bool f) { default_fill_ = f; }
    void dgraph::default_legendcolor(const color &c) { default_legendcolor_ = c; }
    void dgraph::default_drawstyle(const graph_drawer &d)
    {
	delete default_graph_drawer_();
	default_graph_drawer_() = d.clone();
    }
    void dgraph::default_legend(const var &l) { default_legend_ = l.str(); }

    graph &dgraph::smooth(int col_index, double weight1, double weight2, double weight3)
    {
	--col_index;
	double *v  = new double[size()];
	for(size_type i=0; i<size(); ++i)
	{
	    double n=0,d=0;
	    if(i-1 >= 0)
	    {
		n += data_[i-1][col_index].dbl();
		d += weight1;
	    }
	    n += data_[i][col_index].dbl();
	    d += weight2;
	    if(i+1 < size())
	    {
		n += data_[i+1][col_index].dbl();
		d += weight3;
	    }
	    v[i] = n/d;
	}
	for(size_type i=0; i<size(); ++i) data_[i][col_index] = v[i];
	delete [] v;
	return *this;
    }
    graph &dgraph::smooth_range(int col_index_x, double from, double to,
				int col_index, double weight1, double weight2, double weight3)
    {
	--col_index;
	--col_index_x;
	double *v = new double[size()];
	for(size_type i=0; i<size(); ++i)
	{
	    if(from <= data_[i][col_index_x].dbl() && data_[i][col_index_x].dbl() <= to)
	    {
		double n=0,d=0;
		if(i-1 >= 0)
		{
		    n += data_[i-1][col_index].dbl();
		    d += weight1;
		}
		n += data_[i][col_index].dbl();
		d += weight2;
		if(i+1 < size())
		{
		    n += data_[i+1][col_index].dbl();
		    d += weight3;
		}
		v[i] = n/d;
	    }
	    else
	    {
		v[i] = data_[i][col_index];
	    }
	}
	for(size_type i=0; i<size(); ++i) data_[i][col_index] = v[i];
	delete [] v;
	return *this;
    }

   
    dgraph &dgraph::comment_chars(const std::string &s) { comment_chars_ = s; return *this; }

    void dgraph::init_defaults()
    {
	autotitles_ = default_autotitles_;
	title_hints_latex_ = false;
	pointsize(default_pointsize_);
	pointtype(*default_point_drawer_);
	pointcolor(default_pointcolor_);
	linecolor(default_linecolor_);
	linewidth(default_linewidth_);
	linestyle(default_linestyle_);
	fillcolor(default_fillcolor_);
	fill(default_fill_);
	legendcolor(default_legendcolor_);
	drawstyle(*default_graph_drawer_());
        comment_chars_ = default_comment_chars_;
    }

    dgraph::dgraph()
	: columns_(0)
    {
	init_defaults();
    }

    dgraph::dgraph(int n,const double x[],const double y[]) : columns_(2)
    {
	init_defaults();
	legend("");
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    data_.push_back(p);
	}
    }
    dgraph::dgraph(int n,const double x[],const double y[],const double z[]) : columns_(3)
    {
	init_defaults();
	legend("");
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    p[2] = z[i];
	    data_.push_back(p);
	}
    }
    dgraph::dgraph(int n,const double x[],const double y[],const double z[],const double w[]) : columns_(4)
    {
	init_defaults();
	legend("");
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    p[2] = z[i];
	    p[3] = w[i];
	    data_.push_back(p);
	}
    }


    dgraph::dgraph(int n,const var x[],const var y[]) : columns_(2)
    {
	init_defaults();
	legend("");
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    data_.push_back(p);
	}
    }

    dgraph::dgraph(int n,const var x[],const var y[],const var z[]) : columns_(3)
    {
	init_defaults();
	legend("");
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    p[2] = z[i];
	    data_.push_back(p);
	}
    }

    dgraph::dgraph(int n,const var x[],const var y[],const var z[],const var w[]) : columns_(4)
    {
	init_defaults();
	legend("");
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    p[2] = z[i];
	    p[3] = w[i];
	    data_.push_back(p);
	}
    }


    dgraph::dgraph(const vector<var> &x, const vector<var> &y) : columns_(2)
    {
	init_defaults();
	legend("");
	int n = std::min(x.size(),y.size());
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    data_.push_back(p);
	}
    }


    dgraph::dgraph(const vector<var> &x, const vector<var> &y, const vector<var> &z) : columns_(3)
    {
	init_defaults();
	legend("");
	int n = std::min(x.size(),y.size());
	n = std::min((int)n,(int)z.size());
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    p[2] = z[i];
	    data_.push_back(p);
	}
    }


    dgraph::dgraph(const vector<var> &x, const vector<var> &y, const vector<var> &z, const vector<var> &w) : columns_(4)
    {
	init_defaults();
	legend("");
	int n = std::min(x.size(),y.size());
	n = std::min((int)n,(int)z.size());
	n = std::min((int)n,(int)w.size());
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    p[2] = z[i];
	    p[3] = w[i];
	    data_.push_back(p);
	}
    }

    // --------------
    dgraph::dgraph(const vector<double> &x, const vector<double> &y) : columns_(2)
    {
	init_defaults();
	legend("");
	int n = std::min(x.size(),y.size());
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    data_.push_back(p);
	}
    }

    dgraph::dgraph(const vector<double> &x, const vector<double> &y, const vector<double> &z) : columns_(3)
    {
	init_defaults();
	legend("");
	int n = std::min(x.size(),y.size());
	n = std::min((int)n,(int)z.size());
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    p[2] = z[i];
	    data_.push_back(p);
	}
    }

    dgraph::dgraph(const vector<double> &x, const vector<double> &y,
		   const vector<double> &z, const vector<double> &w) : columns_(4)
    {
	init_defaults();
	legend("");
	int n = std::min(x.size(),y.size());
	n = std::min((int)n,(int)z.size());
	n = std::min((int)n,(int)w.size());
	data_.reserve(n);
	for(int i=0; i<n; ++i)
	{
	    datapoint p(columns_);
	    p[0] = x[i];
	    p[1] = y[i];
	    p[2] = z[i];
	    p[3] = w[i];
	    data_.push_back(p);
	}
    }


   
    /*
      The real read function, which all other read functions call.
      
     */

    dgraph &dgraph::read_if(istream &file, const function &condition,
			    function f1, function f2, function f3, function f4, function f5)
    {
	// Collect all the four filter function f1, f2, etc into one single object
	function filter = unset;
	if(f1.initialized()) filter.append(f1);
	if(f2.initialized()) filter.append(f2);
	if(f3.initialized()) filter.append(f3);
	if(f4.initialized()) filter.append(f4);
	if(f5.initialized()) filter.append(f5);

	clear();

        // We have created the filter function here, so it will have no 'history' of previous calls. We start safely...

	datapoint point1, point2;
	datapoint *point_cur=&point1, *point_prev=0;

	// If the filter function is provided, this will define the number
	// of data columns in the dgraph. Otherwise set it to 0 at the moment,
	// and determine at the first valid line read.
	if(filter.initialized())
	{
	    columns_ = filter.components();
	    point1.resize(columns_);
	    point2.resize(columns_);
	}
	else columns_ = 0;

	string line;
	vector<var> col;

	title_hints_.clear();

	string blockname = "";
        int blockno = 0, Blockno = 0;
        bool last_line_empty = false;
        bool last2_line_empty = false;


	// Loop over all lines of the data file
	for(int lineno=0; getline(file,line); ++lineno)
	{
            // Remove trailing \r for dos-files (\n has already been removed by getline itself)
            if(!line.empty() && *line.rbegin() == '\r') line.erase( line.length()-1, 1);

	    if(data_.size() == data_.capacity()) data_.reserve(size() + 100);
	    
	    split_quoted(line,col);

            if(last_line_empty && !col.empty()) ++blockno;
            if(last_line_empty && last2_line_empty && !col.empty()) ++Blockno;
            last2_line_empty = last_line_empty;
            last_line_empty = col.empty();

            function::clear_named_params();
	    function::named_param("linenumber",lineno);
	    function::named_param("blocknumber",blockno);
	    function::named_param("Blocknumber",Blockno);

	    // First: check for special instructions in the datafile
	    if(line.find("##LEGEND:") == 0) make_latex_legend(line.substr(9));
	    else if(line.find("##TITLES:") == 0 || line.find("##LTITLES:") == 0)
	    {
		vector<var> titles;
		if(line.find("##TITLES:") == 0) split_quoted(line.substr(10),titles);
		else                            split_quoted(line.substr(11),titles);

		if(!filter.initialized()) title_hints_ = titles;
		else
		{
		    for(int i=0; i<filter.components(); ++i)
		    {
			string t = filter[i].sprint().str();
			for(unsigned int c=0; c<t.size(); ++c)
			{
			    // only do something if underscore found
			    if(t[c] != '_') continue;

			    // find the end of a _xxx sequence, where x is a digit
			    unsigned int end = c+1;
			    for(; end<t.size() && isdigit(t[end]); ++end);

			    // if no digits found after the underscore, do nothing
			    if(end<=c+1) continue;

			    int argindex = atoi(t.substr(c+1,end-c-1).c_str());

			    if(argindex == 0)
			    {
				t.replace(c,2,"LINENO");
				c += 5;
			    }
			    else if(argindex <= (int)titles.size())
			    {
				t.replace(c,end-c,titles[argindex-1].str());
				c += titles[argindex-1].size()-1;
			    }
			}
			title_hints_.push_back(t);
		    }
		}
		if(line.find("##TITLES:")== 0) title_hints_latex_ = false;
		else title_hints_latex_ = true;
	    }
	    else if(line.find("##XRANGE:") == 0)
	    {
		if(col.size()>=3) xrange(col[1],col[2]);
	    }
	    else if(line.find("##YRANGE:") == 0)
	    {
		if(col.size()>=3) yrange(col[1],col[2]);
	    }
	    else if(line.find("##BLOCK:") == 0)
	    {
		string::size_type i = 8;
		for(;line[i] == ' '; ++i);
		blockname = line.substr(i);
	    }

	    bool skip_this_point = false;

            // Check the condition first. Some conditions (such as 'between_lines') must be called also on
            // comment-lines, etc
	    if(!skip_this_point)
	    {
		// if a condition was given, 
		if(condition.initialized())
		{
		    // insufficient data to execute condition. skip this line
		    if(condition.nargs() > (int)col.size()) skip_this_point = true;
		    
		    // if condition not fulfilled, skip this line
		    else
                    {
                        std::vector<blop::var> condition_result;
                        condition.meval(col,condition_result);

                        // If the first returned condition value is zero (false), skip this point
                        if(condition_result.size()>0 && condition_result[0].dbl()==0) skip_this_point = true;
                        
                        // In case the condition function returns two values, and the second value is false, stop reading the file
                        if(condition_result.size()>1 && condition_result[1].dbl()==0) return *this;
                    }
		}
	    }

            
	    // skip lines where the first non-ws char is #
	    // and also skip empty lines
	    if((!col.empty() && comment_chars_.find(col[0][0])!=string::npos) || col.empty()) skip_this_point = true;

	    // skip also those lines, where there is insufficient data (number of cols)
	    // to evaluate the filter function (if given)
	    if(filter.initialized() && filter.nargs()>(int)col.size()) skip_this_point = true;

	    // If so far this line is not marked for skipping, make a further check:
	    // skip those lines which contain data in the ignore pattern.
	    // If a filter function was used, only check those columns, which will
	    // be used by the filter function
	    if(!skip_this_point)
	    {
		for(unsigned int j=0; j<col.size(); ++j)
		{
		    if(filter.initialized())
		    {
			if(filter.uses_arg(j+1) && ignore::it(col[j]))
			{
			    skip_this_point=true;
			    break;
			}
		    }
		    else
		    {
			if(ignore::it(col[j]))
			{
			    skip_this_point=true;
			    break;
			}
		    }
		}
	    }

            // This was the position where the condition was checked before


            // By here we have evaluated if the current line should be skipped based on the
            // original data from the file.
            // Now evaluate the filter function (even if the point should be skipped), so that it stores
            // the previous value internally.

            // if filter functions were given, evaluate them, check result.
            // No need to set columns_, it has been set earlier already,
            // in this case
            if(filter.initialized())
            {
                filter.meval(col, *point_cur);
                for(unsigned int i=0; i<point_cur->size(); ++i)
                {
                    if(ignore::it((*point_cur)[i]))
                    {
                        skip_this_point = true;
                        break;
                    }
                }
            }


	    // If this line is still not marked for skipping, then
            // If we use this point, and no filter was given, simply copy it to point_cur
	    if(!skip_this_point && !filter.initialized())
	    {

                // This seems to be the first usable line, since columns_ is 0
                // so set it now, resize point_cur, etc
                if(columns_ == 0)
                {
                    columns_ = col.size();
                    ordered_ = columns_;
                    point1.resize(columns_);
                    point2.resize(columns_);
                }
                for(int i=0; i<columns_; ++i) (*point_cur)[i] = col[i];
	    }

	    // if this line is to be skipped, break the data stream with
	    // an unset point (if the flag is set), and continue to next line
	    if(skip_this_point)
	    {
		// break the data with an unset point, if the flag is set
		if(!empty() && falsecondition_break_)
		{
		    if(columns_ <= 0)
		    {
			warning::print("This can not happen 2",
				       "dgraph::read_if(...)");
			exit(1);
		    }
		    datapoint point(columns_);
		    for(int i=0; i<columns_; ++i) point[i] = unset;
		    data_.push_back(point);
		}
		continue;
	    }
	    
	    data_.push_back(*point_cur);

	    if(point_prev)
	    {
		for(int i=0; i<(int)(point_cur->size()); ++i)
		{
		    // any element of this array can only be smaller than the
		    // previous one, if other elements with smaller index
		    // are larger
		    if((*point_cur)[i].dbl() < (*point_prev)[i].dbl())
		    {
			bool ok = false;
			for(int j=0; j<i; ++j)
			{
			    if((*point_cur)[j].dbl() > (*point_prev)[j].dbl())
			    {
				ok = true;
				break;
			    }
			}
			if(!ok) ordered_ = std::min(ordered_, i);
		    }
		}
	    }
	    if(point_prev == 0) point_prev = &point2;
	    swap(point_cur,point_prev);
	}
        function::clear_named_params();

	// remove the unset points from the end (resulting from empty lines
	// at the end of the file)
	while(!empty() && data_.back()[0].dbl() == unset) data_.erase(data_.end()-1);

	if(empty()) warning::print("File '" & filename_ & "' did not contain any data");


	return *this;
    }
    dgraph &dgraph::read_if_static(istream &file, const function &condition,
                                   function f1, function f2, function f3, function f4, function f5)
    {
        dgraph *result = new dgraph;
        result->read_if(file,condition,f1,f2,f3,f4,f5);
        return *result;
    }


    void dgraph::read_groups_if(const var &filename, const function &grouping_value,
				const function &condition,
				const function &filter, plottables *out)
    {
	istream *file = openin(filename);
	if(!file) return;
	read_groups_if(*file, grouping_value, condition, filter, out);
	delete file;
    }

    color sequential_color(var i)
    {
	if(i.str() == "red") return red;
	if(i.str() == "black") return black;
	if(i.str() == "blue") return blue;
	if(i.str() == "green") return green;
	if(i.str() == "magenta") return magenta;
	if(i.str() == "yellow") return yellow;
	if(i.str() == "cyan") return cyan;
	if(i.str() == "orange") return orange;

	int index = (int)(i.dbl()+0.1);

	color colors[7] = {black, red, blue, green, magenta, cyan, orange};

	return colors[index%7];
    }

    point_drawer *sequential_point(var p, bool all = true)
    {
	const char *names_all[14] = {"circle","fcircle",
				     "square","fsquare",
				     "triangle","ftriangle",
				     "triangledown","ftriangledown",
				     "diamond","fdiamond",
				     "star4","fstar4",
				     "cross","plus"};
	const char *names_empty[8] = {"circle",
				      "square",
				      "triangle",
				      "triangledown",
				      "diamond",
				      "star4",
				      "cross",
				      "plus"};
	if(p.is_int() || p.is_dbl())
	{
	    int index = (int)(p.dbl()+1.0);
	    p = (all?names_all[index%14]:names_empty[index%8]);
	}
	if(p.str() == "circle") return new circle;
	if(p.str() == "fcircle") return new fcircle;
	if(p.str() == "square") return new square;
	if(p.str() == "fsquare") return new fsquare;
	if(p.str() == "triangle") return new triangle;
	if(p.str() == "ftriangle") return new ftriangle;
	if(p.str() == "triangledown") return new triangledown;
	if(p.str() == "ftriangledown") return new ftriangledown;
	if(p.str() == "diamond") return new diamond;
	if(p.str() == "fdiamond") return new fdiamond;
	if(p.str() == "star4") return new star4;
	if(p.str() == "fstar4") return new fstar4;
	if(p.str() == "cross") return new cross;
	if(p.str() == "plus") return new blop::plus;
	cerr<<"This should never happen!"<<endl;
	return new circle;
    }

    void dgraph::prepare_for_draw(axis *xaxis,axis *yaxis, frame *f, int count)
    {
	graph::prepare_for_draw(xaxis,yaxis,f, count);

	if(count != 1) return;

	if(!autotitles_ || title_hints_.empty())
	{
	    return;
	}

	const function x = graph_drawer_->get_x(this);
	const function y = graph_drawer_->get_y(this);
	var xtitle = x.sprint();
	var ytitle = y.sprint();
	for(unsigned int i=std::min(20,(int)title_hints_.size()); i>0; --i)
	{
	    xtitle.replace(var("_") & var(i), title_hints_[i-1]);
	    ytitle.replace(var("_") & var(i), title_hints_[i-1]);
	}
	if(!title_hints_latex_)
	{
	    xtitle = tolatex(xtitle);
	    ytitle = tolatex(ytitle);
	}
	if(xaxis->title().text().str() == "" || xaxis->title_autoset()==true) xaxis->title(xtitle,true);
	if(yaxis->title().text().str() == "" || yaxis->title_autoset()==true) yaxis->title(ytitle,true);
    }



    void dgraph::read_groups_if(istream &file, const function &grouping_value,
				const function &condition,
				const function &filter,
				plottables *out)
    {
	out->clear();
	map<string,dgraph*> the_map;
        map<string,map<string,var>> named_params_map;

	string line;
	vector<var> col;

	int index = 0;

        int blockno = 0, Blockno = 0;
        bool last_line_empty = false;
        bool last2_line_empty = false;

	for(int lineno=1; getline(file,line); ++lineno)
	{
            // Remove trailing \r for dos-files (\n has already been removed by getline itself)
            if(!line.empty() && *line.rbegin() == '\r') line.erase(line.length()-1, 1);

	    split_quoted(line,col);

            if(last_line_empty && !col.empty()) ++blockno;
            if(last_line_empty && last2_line_empty && !col.empty()) ++Blockno;
            last2_line_empty = last_line_empty;
            last_line_empty = col.empty();
            
            function::clear_named_params();
	    function::named_param("linenumber",lineno);
	    function::named_param("blocknumber",blockno);
	    function::named_param("Blocknumber",Blockno);

	    if(col.empty() || default_comment_chars_.find(col[0][0])!=string::npos) continue;

	    // skip lines where the first non-ws char is #
	    if(!col.empty() && col[0][0] == '#') continue;

	    // check if any of the fields is a pattern contained in the ignore_
	    // array. in this case skip this line
	    for(unsigned int j=0; j<col.size(); ++j)
	    {
		if(ignore::it(col[j])) { col.clear(); break; }
	    }

	    var the_grouping_value = grouping_value.eval(col);
            function::named_param("groupingvalue",the_grouping_value);

	    if(condition.initialized() && condition.eval(col).dbl() == 0.0) continue;

	    dgraph *the_graph = the_map[the_grouping_value.str()];
            named_params_map[the_grouping_value.str()] = {{"groupingvalue",the_grouping_value}};
	    if(the_graph == 0)
	    {
		the_graph = the_map[the_grouping_value.str()] = new dgraph;
		the_graph->legend(the_grouping_value);
		point_drawer *pt = sequential_point(index);
		the_graph->pointtype(*pt);
		delete pt;
		the_graph->allcolor(sequential_color(index));
		++index;
	    }

	    if(!col.empty())
	    {
		// if the graph has not been filled yet
		if(the_graph->columns() == 0)
		{
		    if(filter.initialized()) the_graph->columns(filter.components());
		    else the_graph->columns(col.size());
		}
		datapoint point(the_graph->columns());

		// if filter functions were given, call them, otherwise simply copy all
		// fields from the line
		if(filter.initialized()) filter.meval(col, point);
		else for(int i=0; i<the_graph->columns(); ++i) point[i] = col[i];

		the_graph->add(point);
	    }
	    else
	    {
		// Do not store unset lines at the beginning. 'unset' lines are used
		// to separate data blocks in the file, at the beginning they make no sense
		if(the_graph->empty()) continue;

		if(the_graph->columns() <= 0)
		{
		    warning::print("This can not happen 3",
				   "dgraph::read_if(...)");
		    exit(1);
		}

		datapoint point(the_graph->columns());

		// an empty line separating data blocks. store a data point consisting of
		// 'unset' values. The drawstyles should interpret these lines correctly
		for(int i=0; i<the_graph->columns(); ++i) point[i] = unset;

		the_graph->add(point);
	    }

	}
        function::clear_named_params();

	// remove the unset points from the end (resulting from empty lines
	// at the end of the file)
	for(map<string,dgraph*>::iterator i=the_map.begin();
	    i != the_map.end(); ++i)
	{
	    dgraph *g = (*i).second;
	    while(!g->empty() && g->data_.back()[0].dbl() == unset) g->data_.erase(g->data_.end()-1);
	    if(g->empty()) warning::print("Empty graph in graph::read_many");
	    out->add(g,named_params_map[(*i).first]);
	}
    }

    dgraph &dgraph::read    (istream &input, function f1, function f2, function f3, function f4, function f5)
    {
	function condition = unset;
	read_if(input, condition, f1, f2, f3, f4, f5);
	return *this;
    }
    dgraph &dgraph::read_static    (istream &input, function f1, function f2, function f3, function f4, function f5)
    {
        dgraph *result = new dgraph;
        result->read(input,f1,f2,f3,f4,f5);
        return *result;
    }

    dgraph &dgraph::read(const var &filename,function f1, function f2, function f3, function f4, function f5)
    TRY
    {
	filename_ = filename;
	istream *file = openin(filename);
	if(!file) return *this;
	var legend = default_legend_;
	legend.replace("%f",filename);

	// there was no condition set, so replaces %C with an
	// empty string
	legend.replace(" %C","");
	legend.replace("%C","");
	make_latex_legend(legend);
	read(*file, f1, f2, f3, f4, f5);
	delete file;
	return *this;
    }
    CATCH("dgraph::read(const var &,function, function, function, function)")

    dgraph &dgraph::read_static(const var &filename,function f1, function f2, function f3, function f4, function f5)
    {
        dgraph *result = new dgraph;
        result->read(filename,f1,f2,f3,f4,f5);
        return *result;
    }
    

    dgraph &dgraph::read_if_static(const var &filename, const function &condition,
			    function f1, function f2, function f3, function f4, function f5)
    {
        dgraph *result = new dgraph;
        result->read_if(filename,condition,f1,f2,f3,f4,f5);
        return *result;
    }
    
    dgraph &dgraph::read_if(const var &filename, const function &condition,
			    function f1, function f2, function f3, function f4, function f5)
    {
	filename_ = filename;
	istream *file = openin(filename);
	if(!file)
	{
	    warning::print(var("Could not open file ") & filename,
			   "dgraph::read_if(const var &filename, ...)");
	    return *this;
	}
	var legend = default_legend_;
	legend.replace("%f",filename);
	if(condition.initialized())
	{
	    legend.replace("%C", var("{") & condition.sprint() & var("}"));
	}
	else
	{
	    legend.replace(" %C","");
	    legend.replace("%C","");
	}
	make_latex_legend(legend);
	read_if(*file, condition, f1, f2, f3, f4, f5);
	delete file;
	return *this;
    }

    dgraph &dgraph::remove_unset()
    {
        for(unsigned int i=0; i<size(); ++i)
        {
            for(unsigned int j=0; j<data_[i].size(); ++j)
            {
                if(data_[i][j].dbl() == unset || isnan(data_[i][j].dbl()) || isinf(data_[i][j].dbl()))
                {
                    data_.erase(data_.begin()+i);
                    --i;
                    break;
                }
            }
        }
        return *this; 
    }

    dgraph &dgraph::add(const datapoint &p)
    {
	if(columns_ != 0 && columns_ != (int)p.size())
	{
	    warning::print("Could not add point, size mismatch","dgraph::add(datapoint)");
	    return *this;
	}
	if(columns_ == 0) columns_ = (int)p.size();
	data_.push_back(p);
	return *this;
    }

    dgraph &dgraph::add(const var &x1, const var &x2)
    {
	if(columns_ != 0 && columns_ != 2)
	{
	    warning::print("Can not add points (" & x1 & "," & x2
			   & ") to dgraph, this graph has " & columns_ & " columns",
			   "dgraph::add(var,var)");
	    return *this;
	}
	if(columns_ == 0) columns_ = 2;
	datapoint p(columns_);
	p[0] = x1;
	p[1] = x2;
	data_.push_back(p);
	return *this;
    }
    dgraph &dgraph::add(const var &x1, const var &x2, const var &x3)
    {
	if(columns_ != 0 && columns_ != 3)
	{
	    warning::print("Can't add points (" & x1 & "," & x2 & "," & x3 &
			   ") to dgraph, this graph has " & columns_ & " columns",
			   "dgraph::add(var,var,var)");
	    return *this;
	}
	if(columns_ == 0) columns_ = 3;
	datapoint p(columns_);
	p[0] = x1;
	p[1] = x2;
	p[2] = x3;
	data_.push_back(p);
	return *this;
    }
    dgraph &dgraph::add(const var &x1, const var &x2, const var &x3, const var &x4)
    {
	if(columns_ != 0 && columns_ != 4)
	{
	    warning::print(var("Can not add points (") 
			   & x1 & var(",") & x2 & var(",")
			   & x3 & var(",") & x4
			   & var(") to dgraph, this graph has ") & var(columns_) &
			   var(" columns"),
			   "dgraph::add(var,var,var,var)");
	    return *this;
	}
	if(columns_ == 0) columns_ = 4;
	datapoint p(columns_);
	p[0] = x1;
	p[1] = x2;
	p[2] = x3;
	p[3] = x4;
	data_.push_back(p);
	return *this;
    }
    dgraph &dgraph::add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5)
    {
	if(columns_ != 0 && columns_ != 5)
	{
	    warning::print(var("Can not add points (") 
			   & x1 & var(",") & x2 & var(",")
			   & x3 & var(",") & x4 & var(",") & x5
			   & var(") to dgraph, this graph has ") & columns_ & var(" columns"),
			   "dgraph::add(var,var,var,var,var)");
	    return *this;
	}
	if(columns_ == 0) columns_ = 5;
	datapoint p(columns_);
	p[0] = x1;
	p[1] = x2;
	p[2] = x3;
	p[3] = x4;
	p[4] = x5;
	data_.push_back(p);
	return *this;
    }
    dgraph &dgraph::add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6)
    {
	if(columns_ != 0 && columns_ != 6)
	{
	    warning::print(var("Can not add points (") 
			   & x1 & var(",") & x2 & var(",")
			   & x3 & var(",") & x4 & var(",") & x5 &
			   var(",") & x6 &
			   var(") to dgraph, this graph has ") & columns_ & var(" columns"),
			   "dgraph::add(var,var,var,var,var,var)");
	    return *this;
	}
	if(columns_ == 0) columns_ = 6;
	datapoint p(columns_);
	p[0] = x1;
	p[1] = x2;
	p[2] = x3;
	p[3] = x4;
	p[4] = x5;
	p[5] = x6;
	data_.push_back(p);
	return *this;
    }

    dgraph &dgraph::add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6, const var &x7)
    {
	if(columns_ != 0 && columns_ != 7)
	{
	    warning::print(var("Can not add points (") 
			   & x1 & var(",")
			   & x2 & var(",")
			   & x3 & var(",")
			   & x4 & var(",")
			   & x5 & var(",")
			   & x6 & var(",")
			   & x7 & var(") to dgraph, this graph has ")
			   & columns_ & var(" columns"),
			   "dgraph::add(var,var,var,var,var,var)");
	    return *this;
	}
	if(columns_ == 0) columns_ = 7;
	datapoint p(columns_);
	p[0] = x1;
	p[1] = x2;
	p[2] = x3;
	p[3] = x4;
	p[4] = x5;
	p[5] = x6;
	p[6] = x7;
	data_.push_back(p);
	return *this;
    }
    dgraph &dgraph::add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6, const var &x7, const var &x8)
    {
	if(columns_ != 0 && columns_ != 8)
	{
	    warning::print(var("Can not add points (") 
			   & x1 & var(",")
			   & x2 & var(",")
			   & x3 & var(",")
			   & x4 & var(",")
			   & x5 & var(",")
			   & x6 & var(",")
			   & x7 & var(",")
			   & x8 & var(") to dgraph, this graph has ")
			   & columns_ & var(" columns"),
			   "dgraph::add(var,var,var,var,var,var)");
	    return *this;
	}
	if(columns_ == 0) columns_ = 8;
	datapoint p(columns_);
	p[0] = x1;
	p[1] = x2;
	p[2] = x3;
	p[3] = x4;
	p[4] = x5;
	p[5] = x6;
	p[6] = x7;
	p[7] = x8;
	data_.push_back(p);
	return *this;
    }
    dgraph &dgraph::add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6, const var &x7, const var &x8, const var &x9)
    {
	if(columns_ != 0 && columns_ != 9)
	{
	    warning::print(var("Can not add points (") 
			   & x1 & var(",")
			   & x2 & var(",")
			   & x3 & var(",")
			   & x4 & var(",")
			   & x5 & var(",")
			   & x6 & var(",")
			   & x7 & var(",")
			   & x8 & var(",")
			   & x9 & var(") to dgraph, this graph has ")
			   & columns_ & var(" columns"),
			   "dgraph::add(var,...)");
	    return *this;
	}
	if(columns_ == 0) columns_ = 9;
	datapoint p(columns_);
	p[0] = x1;
	p[1] = x2;
	p[2] = x3;
	p[3] = x4;
	p[4] = x5;
	p[5] = x6;
	p[6] = x7;
	p[7] = x8;
	p[8] = x9;
	data_.push_back(p);
	return *this;
    }
    dgraph &dgraph::add(const var &x1, const var &x2, const var &x3, const var &x4, const var &x5, const var &x6, const var &x7, const var &x8, const var &x9, const var &x10)
    {
	if(columns_ != 0 && columns_ != 10)
	{
	    warning::print(var("Can not add points (") 
			   & x1 & var(",")
			   & x2 & var(",")
			   & x3 & var(",")
			   & x4 & var(",")
			   & x5 & var(",")
			   & x6 & var(",")
			   & x7 & var(",")
			   & x8 & var(",")
			   & x9 & var(",")
			   & x10 & var(") to dgraph, this graph has ")
			   & columns_ & var(" columns"),
			   "dgraph::add(var,...)");
	    return *this;
	}
	if(columns_ == 0) columns_ = 10;
	datapoint p(columns_);
	p[0] = x1;
	p[1] = x2;
	p[2] = x3;
	p[3] = x4;
	p[4] = x5;
	p[5] = x6;
	p[6] = x7;
	p[7] = x8;
	p[8] = x9;
	p[9] = x10;
	data_.push_back(p);
	return *this;
    }

    dgraph &dgraph::add(int n, double val[])
    {
        if(columns_!=0  && columns_!=n)
        {
            warning::print(var("Can not add point, number of components do not agree"));
            return *this;
        }
        columns_ = n;
        datapoint p(n);
        for(int i=0; i<n; ++i) p[i] = val[i];
        data_.push_back(p);
        return *this;
    }


    double dgraph::min(int n, int m, double low, double high)
    {
        if(n<1 || columns_<n) return unset;
        if(m<1 || columns_<m) return unset;
	--n;
	--m;
        if(empty()) return unset;
        double result = unset;
        for(size_type i=0; i<size(); ++i)
        {
            if(data_[i][n].dbl() == unset || data_[i][m].dbl() == unset) continue;
            if(low<=data_[i][m].dbl() && data_[i][m].dbl() <= high)
            {
                if(data_[i][n].dbl() < result || result == unset) result = data_[i][n].dbl();
            }
        }
        return result;
    }


    double dgraph::max(int n, int m, double low, double high)
    {
	if(n<1 || columns_<n) return unset;
	if(m<1 || columns_<m) return unset;
	--n;
	--m;
	if(empty()) return unset;
	double result = unset;
	for(size_type i=0; i<size(); ++i)
	{
	    if(data_[i][n].dbl() == unset || data_[i][m].dbl() == unset) continue;
	    if(low<=data_[i][m].dbl() && data_[i][m].dbl() <= high)
	    {
		if(data_[i][n].dbl() > result || result == unset) result = data_[i][n].dbl();
	    }
	}
	return result;
    }

    double dgraph::average(blop::function &f)
    {
        if(empty()) return unset;
        double sum = 0;
        int num = 0;
        for(size_type i=0; i<size(); ++i)
        {
            sum += f.eval_dbl(data_[i]);
            ++num;
        }
        return sum/num;
    }

    double dgraph::average(int n, int m, double low, double high)
    {
        if(n<1 || columns_<n) return unset;
        if(m<1 || columns_<m) return unset;
	--n;
	--m;
        if(empty()) return unset;

        double sum = 0;
        int num = 0;
        for(size_type i=0; i<size(); ++i)
        {
            if(data_[i][n].dbl() == unset || data_[i][m].dbl() == unset) continue;
            if(low<=data_[i][m].dbl() && data_[i][m].dbl() <= high)
            {
                sum += data_[i][n].dbl();
                ++num;
            }
        }
        return sum/num;
    }

    
    dgraph &dgraph::transform(const function &f)
    {
        std::vector<blop::var> transformed(f.components());
        for(unsigned int i=0; i<size(); ++i)
        {
            function::clear_named_params();
            function::named_param("linenumber",i);
            f.meval(data_[i],transformed);
            data_[i] = transformed;
        }
        function::clear_named_params();
        return *this;
    }

    dgraph &dgraph::transform_column(int col, function f)
    {
	if(col<1 || columns_<col)
	{
	    warning::print("Column index out of range",
			   var("dgraph::transform_column(") & col & var(",function"));
	    return *this;
	}
	--col;
	for(size_type i=0; i<size(); ++i)
	{
	    array v;
	    for(int a=0; a<columns_; ++a) v.push_back(data_[i][a]);
	    data_[i][col] = f.eval(v);
	}
	return *this;
    }


    graph &dgraph::block(int index)
    {
	if(index<=0)
	{
	    warning::print(var("Bad block index: ") & index,"dgraph::block(int i)");
	    return *this;
	}
	int first=-1, last=-1;
	int block_index = 0;
	for(size_type i=0; i<size(); ++i)
	{
	    if(data_[i][0].dbl() != unset)
	    {
		if(block_index == 0) block_index = 1;
		else if(block_index < 0) block_index = -block_index+1;

		if(block_index == index && first<0) first = i;
	    }
	    else
	    {
		if(block_index == index) last = i;
		block_index = -::abs(block_index);
	    }
	}

	if(last >=0 && last  < (int)size()) data_.erase(data_.begin()+last, data_.end());
	if(first > 0) data_.erase(data_.begin(), data_.begin()+first);

	return *this;
    }

    dgraph &dgraph::sort(const blop::function &key)
    {
        std::vector<double> keys;
        for(unsigned int i=0; i<data_.size(); ++i) keys.push_back(key.eval(data_[i]).dbl());
        parallel_sort(keys,data_);
        return *this;
    }

    dgraph *dgraph::filter(const blop::function &filt)
    {
        dgraph *result = new dgraph;
        for(unsigned int i=0; i<size(); ++i)
        {
            if(filt.eval((*this)[i])) result->add((*this)[i]);
        }
        return result;
    }
    dgraph *dgraph::filter(const blop::function &filt, const blop::function &trans)
    {
        dgraph *result = new dgraph;
        std::vector<var> out;
        for(unsigned int i=0; i<size(); ++i)
        {
            if(filt.eval((*this)[i]))
            {
                trans.meval((*this)[i],out);
                result->add(out);
            }
        }
        return result;
    }

}


std::ostream &operator<<(std::ostream &out, const blop::dgraph &g)
{
    for(unsigned int i=0; i<g.size(); ++i)
    {
        for(int c=0; c<g.columns(); ++c)
        {
            out<<(*(g.get(i)))[c]<<" ";
        }
        out<<std::endl;
    }
    return out;
}
