#include "bloputils.h"
#include "blop_bookkeeper.h"
#include "exc.hh"
#include "graph_drawer.h"
#include "pstream.h"
#include "warning.h"
#include "canvas.h"

#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <map>
#include <cmath>
#include <unistd.h>

namespace blop
{
    using namespace std;

    bool isfinite(double v) { return std::isfinite(v); }
    bool isnan(double v)    { return std::isnan(v); }
    bool isinf(double v)    { return std::isinf(v); }
    bool isnormal(double v) { return std::isnormal(v); }

    length axislength(axis *a, double from, double to)
    {
	return length::base_axis_t(a,from,to);
    }

    length x1len(double a,double b)
    {
	return length::base_axis_t(frame::current().x1axis(),a,b);
    }
    length x2len(double a,double b)
    {
	return length::base_axis_t(frame::current().x2axis(),a,b);
    }
    length y1len(double a,double b)
    {
	return length::base_axis_t(frame::current().y1axis(),a,b);
    }
    length y2len(double a,double b)
    {
	return length::base_axis_t(frame::current().y2axis(),a,b);
    }

    std::istream *openin(const var &filename)
    {
	if(filename.length() == 0)
	{
	    warning::print("empty string","openin(const var &filename)");
	    return 0;
	}
	istream *file = 0;

	// filename beginning with << is interpreted as here-document (istringstream)
	if(filename.size()>2 && filename[0] == '<' && filename[1] == '<')
	{
	    file = new istringstream(filename.c_str()+2);
	}
	else if(filename[filename.length()-1]=='|')
	{
	    file = new ipstream(filename.str().substr(0,filename.length()-1).c_str());
	}
	else if(filename.find("scp://") == 0)
	{
	    file = new iscpstream(filename.str().substr(6,string::npos).c_str());
	}
	else if(filename.find("http://") == 0)
	{
	    file = new ihttpstream(filename.str().c_str());
	}
	// everything else is a normal file
	else
	{
	    file = new ifstream(filename.str().c_str());
	}

	if(file->fail())
	{
	    warning::print("Can't open file: " & filename);
	    delete file;
	    return 0;
	}
	
	return file;
    }

    std::ostream *openout(const var &filename)
    {
	if(filename.length() == 0)
	{
	    warning::print("empty string","openout(const var &filename)");
	    return 0;
	}
	ostream *file = 0;

	// filename beginning with << is interpreted as here-document (istringstream)
	if(filename[0] == '>' && filename[1] == '>')
	{
	    file = new ofstream(filename.str().substr(2).c_str(),ios::app);
	}
	// ignore > at the beginning, open as normal file
	else if(filename[0] == '>')
	{
	    file = new ofstream(filename.str().substr(1).c_str());
	}
	// filename beginning with | is an output pipe
	else if(filename[0] == '|')
	{
	    file = new opstream(filename.str().substr(1).c_str());
	}
	else if(filename.find("scp://") == 0)
	{
	    file = new oscpstream(filename.str().substr(6,string::npos).c_str());
	}
	// everything else is a normal file
	else
	{
	    file = new ofstream(filename.str().c_str());
	}

	if(file->fail())
	{
	    warning::print("Can't open file: " & filename);
	    delete file;
	    return 0;
	}
	
	return file;
    }



    void print(terminal *t)
	TRY
    {
	canvas::current().print(t);
    }
    CATCH("print(terminal *)")

    int system(const var &cmd)
    {
	return ::system(cmd.str().c_str());
    }

    int system_bg(const var &cmd)
    {
	int pid = fork();
	if(pid == 0)
	{
	    const char *shell = "/bin/bash";
	    struct stat buf;
	    if(stat(shell,&buf) != 0) shell = "/bin/sh";
	    execl(shell, "sh", "-c", cmd.c_str(), NULL);
	}
	else
	{
	    blop_bookkeeper::register_pid(pid);
	}
	return pid;
    }

    void shell(const var &cmd)
    {
	static opstream shellstr("/bin/sh");
	shellstr<<cmd.str()<<endl;
    }


    var readcmd(const var &cmd_orig,int *rc)
    {
	string resultstr;
	var cmd = cmd_orig;
	if(cmd[0] == '<')
	{
	    cmd = cmd.str().substr(1);
	}
	ipstream cmdstr(cmd);
	int lineno = 0;
	char c;
	while(cmdstr.get(c))
	{
	    resultstr += c;
	    if(c == '\n') ++lineno;
	}
	if(lineno == 1 && resultstr[resultstr.size()-1] == '\n')
	{
	    resultstr.erase(resultstr.size()-1);
	}
	if(rc) *rc = cmdstr.close();
	return resultstr;
    }

    int readcmd(const var &cmd, array &out, const var &separator, int *rc)
    {
	var f = readcmd(cmd, rc);
	out = split(f,separator);
	return out.size();
    }
    int readcmd(const var &cmd, std::vector<std::string> &out, const var &separator, int *rc)
    {
        var f = readcmd(cmd, rc);
        split(f, separator, out);
        return out.size();
    }
     
    var readfile(const var &filename)
    {
	var result;
	char c;
	ifstream file(filename.str().c_str());
	while(file.get(c)) result &= c;
	return result;
    }

    text &fdraw_file_content(const var &filename)
    {
        return text::fdraw(tolatex(readfile(filename))).verbatim(true);
    }
    text &pdraw_file_content(const var &filename)
    {
        return text::pdraw(tolatex(readfile(filename))).verbatim(true);
    }
    text &cdraw_file_content(const var &filename)
    {
        return text::cdraw(tolatex(readfile(filename))).verbatim(true);
    }

    int readfile(const var &filename, array &out, const var &separator)
    {
	var f = readfile(filename);
	out = split(f,separator);
	return out.size();
    }


    var join(const array &a, const var &sep)
    {
	if(a.empty()) return "";
	var result = a[0];
	for(unsigned int i=1; i<a.size(); ++i)
	{
	    result &= sep;
	    result &= a[i];
	}
	return result;
    }

    var join(const std::vector<double> &a, const var &sep)
    {
	if(a.empty()) return "";
	var result = a[0];
	for(unsigned int i=1; i<a.size(); ++i)
	{
	    result &= sep;
	    result &= a[i];
	}
	return result;
    }
    var join(const std::vector<std::string> &a, const var &sep)
    {
	if(a.empty()) return "";
	var result = a[0];
	for(unsigned int i=1; i<a.size(); ++i)
	{
	    result &= sep;
	    result &= a[i];
	}
	return result;
    }

    array sequence(var start, var stop, var step)
    {
	array out;
	for(double v = start.dbl(); v<=stop.dbl(); v+=step.dbl())
	{
	    out.push_back(var(v));
	}
	return out;
    }

    string read_word_from_string(const string &line,std::string::size_type &index,const string &delimiter)
    {
	string result;
	for(; index < line.size() && delimiter.find(line[index]) == string::npos; ++index)
	{
	    result += line[index];
	}
	return result;
    }

    void split_quoted(string line, vector<var> &fields)
    {
	line += ' ';
	fields.clear();
	string s;
	for(string::size_type i=0; i<line.size(); ++i)
	{
	    // skip whitespaces
	    for(; i<line.size() && ( line[i] == ' ' || line[i] == '\t') ; ++i);
	    if(i >= line.size()) break;

	    if(line[i] == '\'')
	    {
		s = read_word_from_string(line,++i,"'");
		//if(s != "")
		fields.push_back(s);
	    }
	    else if(line[i] == '"')
	    {
		s = read_word_from_string(line,++i,"\"");
		//if(s != "")
		fields.push_back(s);
	    }
	    else
	    {
		s = read_word_from_string(line,i," \t");
		if(s != "") fields.push_back(s);
	    }
	}
    }

    bool split_contains(const var &haystack, const var &separator, const var &needle)
    {
        array a = split(haystack, separator);
        for(unsigned int i=0; i<a.size(); ++i) if(a[i].str()==needle.str()) return true;
        return false;
    }

    array split(const var &ss,const var &sep)
    {
	array out;

	string separator = sep.str();
	string s = ss.str();

	string w;
	for(unsigned int i=0; i<s.size(); ++i)
	{
	    if(separator.find(s[i]) != string::npos)
	    {
		if(w != "") out.push_back(w);
		w = "";
	    }
	    else
	    {
		w += s[i];
	    }
	}
	if(w != "") out.push_back(w);
	return out;
    }

    int split(const string &s,const string &separator, std::vector<std::string> &out)
    {
	out.clear();

	string w;
	for(unsigned int i=0; i<s.size(); ++i)
	{
	    if(separator.find(s[i]) != string::npos)
	    {
		if(w != "") out.push_back(w);
		w = "";
	    }
	    else
	    {
		w += s[i];
	    }
	}
	if(w != "") out.push_back(w);
	return out.size();
    }

    istream &getline(istream &in, var &line, const var &line_separator)
    {
	string s;
	std::getline(in,s,line_separator[0]);
	line = s;
	return in;
    }

    istream &getline(istream &in, array &line, const var &line_separator, const var &field_separator)
    {
	line.clear();
	string s;
	std::getline(in,s,line_separator[0]);
	line = split(s,field_separator);
	return in;
    }

    istream &getline(istream &in, std::vector<std::string> &line, const std::string &line_separator, const std::string &field_separator)
    {
	line.clear();
	string s;
	std::getline(in,s,line_separator[0]);
	split(s,field_separator,line);
	return in;
    }


    var getfield(const var &s, unsigned int i, const var &separator)
    {
	vector<var> out = split(s,separator);
        
	if(i >= out.size()) return "";
	if(i < 0) return "";
	return out[i];
    }

    var tolatex(const var &input)
    {
	string fn = input.str();
	string result;
	for(unsigned int i=0; i<fn.size(); ++i)
	{
	    switch(fn[i])
	    {
	    case '<': result += "$<$"; break;
	    case '>': result += "$>$"; break;
	    case '_': result += "\\_"; break;
            case '^': result += "\\^"; break;
	    case '%': result += "\\%"; break;
	    case '@': result += "\\@"; break;
	    case '{': result += "$\\{$"; break;
	    case '}': result += "$\\}$"; break;
	    case '$': result += "\\$"; break;
	    case '&': result += "\\&"; break;
	    default: result += fn[i];
	    }
	}
	return result;
    }

    // ---------------- parallel_sort ----------------------------------------------------
    void parallel_sort(std::vector<double> &x, std::vector<double> &y,bool unique)
    {
	parallel_sort<std::vector<double>,std::vector<double> >(x,y,unique);
    }
    void parallel_sort(std::vector<blop::var> &x, std::vector<blop::var> &y, bool unique)
    {
	parallel_sort<std::vector<blop::var>,std::vector<blop::var> >(x,y,unique);
    }
    void parallel_sort(array &x, array &y,bool unique)
    {
	parallel_sort<array,array>(x,y,unique);
    }

    
    int find_crossings(const dgraph &g, const std::vector<double> &values, std::vector<double> &crosspoints, const function &x, const function &y)
    {
        crosspoints.clear();
        const int n = g.size();
        double X_prev = 0;
        double Y_prev = 0;
        for(int i=0; i<n; ++i)
        {
            const datapoint &d = (*g.get(i));
            double X = x.eval(d).dbl();
            double Y = y.eval(d).dbl();
            if(i>0)
            {
                for(int j=0; j<(int)values.size(); ++j)
                {
                    if((values[j]-Y)*(values[j]-Y_prev)<0)
                    {
                        const double C = X_prev + (values[j]-Y_prev)/(Y-Y_prev)*(X-X_prev);
                        crosspoints.push_back(C);
                    }
                }
            }
            X_prev = X;
            Y_prev = Y;
        }
        return (int)crosspoints.size(); 
    }

}


