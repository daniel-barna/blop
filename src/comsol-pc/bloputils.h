#ifndef __BLOP_BLOPUTILS_H__
#define __BLOP_BLOPUTILS_H__

#include "array.h"
#include "frame.h"
#include "pad.h"
#include "var.h"
#include "length.h"
#include "function.h"
#include "constants.h"
#include "text.h"
#include "dgraph.h"
#include <algorithm>

namespace blop
{

    inline bool file_exists(const string &filename)
    {
	ifstream f(filename.c_str());
	return f.good();
    }

    // ---------------------------------------------------------------------------------

    bool isfinite(double v);
    bool isnan(double v);
    bool isinf(double v);
    bool isnormal(double v);

    // ---------------------------------------------------------------------------------

    std::istream *openin(const var &filename);
    std::ostream *openout(const var &filename);

    // ---------------------------------------------------------------------------------

    //html <a name="axislength"></a>
    length axislength(axis *a, double from, double to=unset);
    length x1len(double a,double b=unset);
    length x2len(double a,double b=unset);
    length y1len(double a,double b=unset);
    length y2len(double a,double b=unset);

#ifndef __MAKECINT__
    template <int ax>
        length xlen(double a, double b=unset) 
    { 
        if(ax==axis::x1) return length::base_axis_t(frame::current().x1axis(),a,b);
        if(ax==axis::x2) return length::base_axis_t(frame::current().x2axis(),a,b);
        return 0.0;
    }
    template <int ax>
        length ylen(double a, double b=unset) 
    { 
        if(ax==axis::y1) return length::base_axis_t(frame::current().y1axis(),a,b);
        if(ax==axis::y2) return length::base_axis_t(frame::current().y2axis(),a,b);
        return 0.0;
    }
    template <int ax>
        length alen(double a, double b=unset) 
    { 
        if(ax==axis::x1) return length::base_axis_t(frame::current().x1axis(),a,b);
        if(ax==axis::x2) return length::base_axis_t(frame::current().x2axis(),a,b);
        if(ax==axis::y1) return length::base_axis_t(frame::current().y1axis(),a,b);
        if(ax==axis::y2) return length::base_axis_t(frame::current().y2axis(),a,b);
        return 0.0;
    }
#endif    

    // return the length along the x1/x2/y1/y2 axes:
    // - from the origin to 'a', if 'b'==unset,
    // - from 'a' to 'b', if 'b' is not unset


    // -------------------------------------------------------------------------------
    // Print the current canvas to a terminal

    void print(terminal *term);


    //html <a name=system></a>
    // --------------------------  system ----------------------------------------------
    // Like the usual 'system(const char *cmd)' function of C,
    // but for 'var' argument

    int system(const var &);

    // Run a command in the background, return it's process-id. This process-id is also
    // automatically registered, and will be waited for before quitting the program.
    int system_bg(const var &cmd);

    //html <a name=shell></a>
    // -----------------------  shell --------------------------------------------------
    // The same as the 'system' function, but the same shell process
    // is used for all subsequent calls. For example a 'cd /tmp' command
    // processed by this function will have an effect for all later calls
    // of this function. These two commands list the /tmp directory:
    // shell("cd /tmp");
    // shell("ls");

    void shell(const var &command);


    //html <a name=readcmd></a>
    // -----------------------  readcmd -----------------------------------------------
    // Read the output of the provided command (which is executed
    // in /bin/sh). The first character MAY be a '<', in which
    // case it is removed from it (this is introduced in order to
    // match the 'plot' command, where the initial '<' in the filename
    // to be plotted indicates that it is to be treated as a pipe)
    // If 'rc' is provided, the return code of the command
    // is returned in this variable
    
    var readcmd(const var &cmd,int *rc = 0);
    int readcmd(const var &cmd, array &out, const var &separator = "\n", int *rc = 0);
    int readcmd(const var &cmd, std::vector<std::string> &out, const var &separator = "\n", int *rc = 0);

    //html <a name=readfile></a>
    // -------------------------  readfile --------------------------------------------
    // Reads and returns the content of the provided file. The second version reads
    // the file, and splits its contents (by default at newlines) into the array 'out',
    // and returns the number of entries in out

    var readfile(const var &filename);
    int readfile(const var &filename, array &out, const var &separator = "\n");

    //html <a name='draw_file_content'></a>
    // -------------------------  draw the content of a file --------------------------

    text &fdraw_file_content(const var &filename);
    text &pdraw_file_content(const var &filename);
    text &cdraw_file_content(const var &filename);

    //html <a name=sequence></a>
    // Create a sequence of numbers
    array sequence(var start, var stop, var step=1);

    //html <a name=splitjoin></a>
    // ------------------------- split and join ---------------------------------------
    // Splits the string 's' into the vector 'out', and returns the number of entries

    array split(const var &s, const var &separator = " \t\n");
    int   split(const string &s,const string &separator, std::vector<std::string> &out);
    var   join(const array &a, const var &separator = array::outfs());
    var   join(const std::vector<double> &a, const var &separator = array::outfs());
    var   join(const std::vector<std::string> &a, const var &separator = array::outfs());

    //html <a name=split_data_line>
    // --------------------  split a data line (interpret quoation marks, etc) --------
    void split_quoted(std::string line, std::vector<blop::var> &fields);
    //html </a>

    //html <a name=split_contains>
    bool split_contains(const var &haystack, const var &separator, const var &needle);
    //html </a>
    
    //html <a name=getfield></a>
    // --------------------------  getfield  ------------------------------------------
    // Split the provided string 's' at characters specified by 'separator', and return
    // the 'i'th element

    var getfield(const var &s, unsigned int i, const var &separator = " \t\n");


    //html <a name=getline></a>
    // -------------------  getline  --------------------------------------------------
    // Read a line from the input stream and return it in 'v' (the first version)
    // or immediately split it as well (at whitespaces) into the array 'a'
    // line_separator specifies the character to be interpreted as end-of-line (only
    // the first character of this string is used)
    // field_separator contains the characters at which the line will be split
    // (it will be the second argument to the 'split' function described above)

    istream &getline(istream &, var &v, const var &line_separator = "\n");
    istream &getline(istream &, array &a, const var &line_separator = "\n", const var &field_separator = "\t ");
    istream &getline(istream &, std::vector<std::string> &a, const std::string &line_separator = "\n", const std::string &field_separator = "\t ");

    //html <a name=tolatex></a>
    // -------------------  tolatex  --------------------------------------------------
    // Transform a string (possibly containing _, $, and other latex-special chars)
    // to latex, where these characters are escaped

    var tolatex(const var &);

    //html <a name='parallel_sort'></a>
    // ----------------------  parallel_sort ------------------------------------------
    // Sort two arrays based on the values of the first array. The elements of the second
    // array will correspond to the same elements of the first array.

    template <class A, class B>
    class LessThanPairFirst
    {
    public:
	// compare by the 'first' element of a pair (if they are equal,
	// compare by the 'second' element of the pair)
	bool operator()(const std::pair<A,B> &a, const std::pair<A,B> &b)
	{
	    if(a.first<b.first) return true;
	    if(a.first<=b.first && a.second<b.second) return true;
	    return false;
	}
    };

    template <class Container1, class Container2>
    void parallel_sort(Container1 &v1,Container2 &v2,bool unique=false)
    {
	std::vector<std::pair<typename Container1::value_type,typename Container2::value_type> > a;
	const unsigned int n = std::min(v1.size(),v2.size());
	for(unsigned int i=0; i<n; ++i)
	{
	    a.push_back(std::pair<typename Container1::value_type,typename Container2::value_type>(v1[i],v2[i]));
	}
        auto comparator = LessThanPairFirst<typename Container1::value_type, typename Container2::value_type>();
	sort(a.begin(), a.end(), comparator);
        unsigned int i_out=0;
	for(unsigned int i=0; i<n; ++i)
	{
            // If we require unique sorting, then skip those which are equal to the last one
            if(unique && i>0)
            {
                if(!comparator(a[i],a[i-1]) && !comparator(a[i-1],a[i])) continue;
            }
	    v1[i_out] = a[i].first;
	    v2[i_out] = a[i].second;
            ++i_out;
	}
        v1.resize(i_out);
        v2.resize(i_out);
    }

    void parallel_sort(std::vector<double> &x, std::vector<double> &y,bool unique=false);
    void parallel_sort(std::vector<blop::var> &x, std::vector<blop::var> &y,bool unique=false);
    void parallel_sort(array &x, array &y,bool unique=false);

    

    int find_crossings(const dgraph &g, const std::vector<double> &values, std::vector<double> &crosspoints, const function &x = _1, const function &y = _2);




    // A utility class to in-line initialize a std::vector. Usage:
    // vector<int>     i = initvector(1)(2)(3)(4)(5);
    // vector<double>  d = initvector(1.1)(2.2)(3.3);
    // vector<double>  d2 = initvector<double>(1)(2)(3);  
    template <typename T> class vector_initializer
    {
    private:
        std::vector<T> vector_;
    public:
        vector_initializer(const T &t) { vector_.push_back(t); }
        vector_initializer<T> &operator()(const T &t) { vector_.push_back(t); return *this; }
        operator std::vector<T>() const { return vector_; }
    };
    template <typename T> vector_initializer<T> initvector(const T &t) { return vector_initializer<T>(t); }
}


#endif
