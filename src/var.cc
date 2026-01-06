#include "warning.h"
#include "var.h"
#include "constants.h"
#include "config.h"
#include <ctype.h>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <cstdarg>
#ifdef HAVE_REGEX_H
#include <regex.h>
#endif
#include "array.h"

using namespace std;

namespace blop
{
    var var::sprintf(const var &format, ...)
    {
        const int N = 1000;
        char buffer[N];
        va_list args;
        va_start (args, format);
        int n = vsnprintf (buffer,N,format.c_str(), args);
        var result;
        if(n>=N)
        {
            char *buffer2 = new char[n+1];
            vsnprintf(buffer2,n,format.c_str(),args);
            result = buffer2;
            delete[] buffer2;
        }
        else
        {
            result = buffer;
        }
        va_end (args);
        return result;
    }

    var &var::replace_re(const var &pattern, const var &replacement_var)
    {
#ifdef HAVE_REGEX_H
	string replacement = replacement_var.str();

	regex_t preg;
	int result = regcomp(&preg,pattern.c_str(),REG_EXTENDED);
	if(result != 0)
	{
	    char errbuf[300];
	    regerror(result,&preg,errbuf,299);
	    regfree(&preg);
	    warning::print(var("Failed to compile regex pattern: ") & errbuf,
			   "var::matches(const var &pattern)");
	    return *this;
	}
	regmatch_t regmatches[100];
	result = regexec(&preg,c_str(),100,regmatches,0);

	// if no match, return 
	if(result != 0) return *this;

	int nmatches = 0;
	for(; nmatches<100 && regmatches[nmatches].rm_so>=0; ++nmatches);

	// first replace the \0, \1, etc references in the 'replacement'
	// string to the corresponding matching sub-expressions
	for(string::size_type i=0; i<replacement.size(); ++i)
	{
	    if(replacement[i] != '\\') continue;
	    if(i>0 && replacement[i-1] == '\\') continue;
	    string::size_type end = i+1;
	    for(; end<replacement.size() && isdigit(replacement[end]); ++end);
	    if(end != i+1)
	    {
		int ref = atoi(replacement.substr(i+1,end-(i+1)).c_str());
		if(ref<nmatches)
		{
		    string submatch =
			string_.substr(regmatches[ref].rm_so,
				       regmatches[ref].rm_eo-regmatches[ref].rm_so);
		    replacement.replace(i,end-i,submatch);
		    i += submatch.size()-1;
		}
	    }
	}

	// then replace in this string
	string_.replace(regmatches[0].rm_so,
			regmatches[0].rm_eo-regmatches[0].rm_so,
			replacement);
	double_ = atof(string_.c_str());
	regfree(&preg);
#else
	warning::print("<regex.h> missing, therefore this function is not supported",
		       "var::replace_re(const var &pattern, const var &replacement)");
#endif
	return *this;
    }

    bool var::matches(const var &pattern)
    {
#ifdef HAVE_REGEX_H
	regex_t preg;
	int result = regcomp(&preg,pattern.c_str(),REG_EXTENDED|REG_NOSUB);
	if(result != 0)
	{
	    char errbuf[300];
	    regerror(result,&preg,errbuf,299);
	    regfree(&preg);
	    warning::print(var("Failed to compile regex pattern: ") & errbuf,
			   "var::matches(const var &pattern)");
	    return false;
	}
	regmatch_t regmatches;
	result = regexec(&preg,c_str(),1,&regmatches,0);
	regfree(&preg);
	return result==0;
#else
	warning::print("<regex.h> missing, therefore this function is not supported",
		       "var::matches(const var &pattern, array &matches)");
	return false;
#endif
    }

    int var::matches(const var &pattern,array &the_matches)
    {
#ifdef HAVE_REGEX_H
	the_matches.clear();
	regex_t preg;
	int result = regcomp(&preg,pattern.c_str(),REG_EXTENDED);
	if(result != 0)
	{
	    char errbuf[300];
	    regerror(result,&preg,errbuf,299);
	    regfree(&preg);
	    warning::print(var("Failed to compile regex pattern: ") & errbuf,
			   "var::matches(const var &pattern)");
	    return false;
	}
	regmatch_t regmatches[100];
	result = regexec(&preg,c_str(),100,regmatches,0);

	// if no match, return 0
	if(result != 0) return 0;

	// loop over the matches, and copy them to the array 'the_matches'
	for(int i=0; i<100 && regmatches[i].rm_so>=0; ++i)
	{
	    the_matches.push_back(str().substr(regmatches[i].rm_so,
					       regmatches[i].rm_eo-regmatches[i].rm_so));
	}
	regfree(&preg);
	return the_matches.size();
#else
	warning::print("<regex.h> missing, therefore this function is not supported",
		       "var::matches(const var &pattern, array &matches)");
	return 0;
#endif	
    }
	

    string &var::default_format_()
    {
	static string f = "%g";
	return f;
    }

    char var::back() const
    {
	if(string_.size() == 0) return '\0';
	return string_[string_.size()-1];
    }

    char var::pop_back()
    {
	if(string_.size() == 0) return '\0';
	char c = string_[string_.size()-1];
	string_.resize(string_.size()-1);
	return c;
    }

    char var::operator[] (int i) const
    {
	return string_[i];
    }

    var::var() {double_ = 0; string_ = ""; format_ = default_format_();}

    var::var(const string &s) {format_ = default_format_(); *this = s; }

    var::var(double d) { format_ = default_format_(); *this = d; }
    var::var(long double d) { format_ = default_format_(); *this = (double)d; }
    var::var(double d, const var &fmt)
    { 
        format_ = default_format_();
        double_ = d;
        if(d==unset) string_ = "unset";
        else
        {
	    char s[1000];
            std::sprintf(s,fmt.c_str(),d);
	    string_ = s;
        }
    }

    var::var(int i) {format_ = default_format_(); *this = i; }
    var::var(long int i) {format_ = default_format_(); *this = (int)i; }

    var::var(unsigned int i) {format_ = default_format_(); *this = i; }

    var::var(const var &other) { format_ = other.format_; *this = other;  }

    var::var(const char *s) { format_ = default_format_(); if(s == 0) *this = ""; else *this = s;  }

    var::var(char c) { format_ = default_format_(); *this = c; }

    var &var::format(const var &f) {format_ = f.str(); return *this; }
    void var::default_format(const var &f) {default_format_() = f.str(); }

    var &var::reformat(const var &format, bool set_format)
    {
        if(set_format) format_ = format.str();
        char s[1000];
        std::sprintf(s,format.c_str(),double_);
        string_ = s;
        return *this;
    }

    bool var::is_int() const
    {
	if(string_.empty()) return false;
	if(string_[0] == '-' && string_.size() == 1) return false;
	if(string_[0] != '-' && !isdigit(string_[0])) return false;
	for(unsigned int i=1; i<string_.size(); ++i) if(!isdigit(string_[i])) return false;
	return true;
    }

    bool var::is_dbl() const
    {
	if(string_.empty()) return false;
	const char *c = string_.c_str();
	char *endptr;
	(void)strtod(c,&endptr);
	if(endptr == c + string_.size()) return true;
	return false;
    }

    const var &var::operator= (const string &s)
    {
	if(s == "nan") double_ = 0;
	else if(s == "inf") double_ = 0;
	else double_ = atof(s.c_str());
	string_ = s;
	return *this;
    }
    
    const var &var::operator= (const char *s)
    {
	if(s == 0)
	{
	    double_ = 0;
	    string_ = "";
	    return *this;
	}
	     
	if(!strcmp(s,"nan")) double_ = 0;
	else if(!strcmp(s,"inf")) double_ = 0;
	else double_ = atof(s);
	string_ = s;
	return *this;	
    }
    
    const var &var::operator= (char c)
    {
	string_ = c;
	double_ = atof(string_.c_str());
	return *this;
    }

    const var &var::operator= (double d)
    {
	double_ = d;
	if(d == unset) string_ = "unset";
	else
	{
	    char s[1000];
            std::sprintf(s,format_.c_str(),d);
	    string_ = s;
	}
	return *this;
    }

    const var &var::operator= (int i)
    {
	double_ = i;
	char s[1000];
        std::sprintf(s,"%i",i);
	string_ = s;
	return *this;
    }

    const var &var::operator= (long int i)
    {
	double_ = i;
	char s[1000];
        std::sprintf(s,"%li",i);
	string_ = s;
	return *this;
    }
    const var &var::operator= (long long i)
    {
	double_ = i;
	char s[1000];
        std::sprintf(s,"%lli",i);
	string_ = s;
	return *this;
    }

    const var &var::operator= (unsigned int i)
    {
	double_ = i;
	char s[1000];
        std::sprintf(s,"%i",i);
	string_ = s;
	return *this;
    }

    const var &var::operator= (const var &o)
    {
	double_ = o.double_;
	string_ = o.string_;
	return *this;
    }

/*
    const var &var::incr(const var &o)
    {
	bool is_dbl_1 = true;
	int point_1 = -1;
	int before_1 = 0;
	int after_1 = 0;
	for(int i=0; i<size(); ++i)
	{
	    if((*this)[i] == '-')
	    {
		if(i != 0) { is_dbl_1 = false; break; }
	    }
	    else if((*this)[i] == '.')
	    {
		if(point_1 >= 0) { is_dbl_1 = false; break; }
		point_1 = i;
	    }
	    else if(isdigit((*this)[i]))
	    {
		if(point_1 >= 0) ++after_1;
		else ++before_1;
	    }
	    else
	    {
		is_dbl_1 = false; break;
	    }
	}

	bool is_dbl_2 = true;
	int point_2 = -1;
	int before_1 = 0;
	int after_1 = 0;
	for(int i=0; i<size(); ++i)
	{
	    if(o[i] == '-')
	    {
		if(i != 0) { is_dbl_2 = false; break; }
	    }
	    else if(o[i] == '.')
	    {
		if(point_2 >= 0) { is_dbl_2 = false; break; }
		point_2 = i;
	    }
	    else if(isdigit(o[i]))
	    {
		if(point_2 >= 0) ++after_2;
		else ++before_2;
	    }
	    else
	    {
		is_dbl_2 = false; break;
	    }
	}

	if(!is_dbl_1 || !is_dbl_2) { operator+= (o); return *this; }

	
	
    }
	
*/

    const var &var::operator+= (double d)
    {
	double_ += d;
	char s[1000];
        std::sprintf(s,format_.c_str(),double_);
	string_ = s;
	return *this;
    }

    const var &var::operator+= (const string &s)
    {
	double ddd = atof(s.c_str());
	operator+=(ddd);
	return *this;
    }


    const var &var::operator+= (const var &o)
    {
	operator+=(o.double_);
	return *this;
    }

    const var &var::operator+= (int i)
    {
	if(!is_int())
	{
	    operator += ((double)i);
	    return *this;
	}
	int old_value = atoi(string_.c_str());
	int new_value = old_value + i;
	double_ = new_value;
	char s[1000];
	if(string_[0] == '0') // zero-padded
	{
	    int old_size  = string_.size();
	    char format[100];
	    std::sprintf(format,"%%0%ii",old_size);
	    std::sprintf(s,format,new_value);
	    string_ = s;
	}
	else
	{
	    std::sprintf(s,"%i",new_value);
	    string_ = s;
	}
	return *this;
    }

    const var &var::operator-= (double d)
    {
	double_ -= d;
	char s[1000];
	std::sprintf(s,format_.c_str(),double_);
	string_ = s;
	return *this;
    }

    const var &var::operator-= (int i)
    {
	operator += (-i);
	return *this;
    }

    const var &var::operator-= (const string &s)
    {
	double ddd = atof(s.c_str());
	operator-=(ddd);
	return *this;
    }

    const var &var::operator-= (const var &o)
    {
	operator-=(o.double_);
	return *this;
    }

    const var &var::operator*= (double d)
    {
	double_ *= d;
	char s[1000];
	std::sprintf(s,format_.c_str(),double_);
	string_ = s;
	return *this;
    }
    const var &var::operator*= (int i)
    {
	double_ *= i;
	char s[1000];
	std::sprintf(s,format_.c_str(),double_);
	string_ = s;
	return *this;
    }

    const var &var::operator*= (const string &s)
    {
	double ddd = atof(s.c_str());
	operator*=(ddd);
	return *this;
    }

    const var &var::operator*= (const var &v)
    {
	operator*=(v.double_);
	return *this;
    }

    const var &var::operator/= (double d)
    {
	double_ /= d;
	char s[1000];
	std::sprintf(s,format_.c_str(),double_);
	string_ = s;
	return *this;
    }
    const var &var::operator/= (int i)
    {
	double_ /= i;
	char s[1000];
	std::sprintf(s,format_.c_str(),double_);
	string_ = s;
	return *this;
    }

    const var &var::operator/= (const string &s)
    {
	double ddd = atof(s.c_str());
	operator/=(ddd);
	return *this;
    }

    const var &var::operator/= (const var &v)
    {
	operator/=(v.double_);
	return *this;
    }

    const var &var::operator++ ()
    {
	*this += 1;
	return *this;
    }

    var var::operator++ (int)
    {
	var tmp = *this;
	*this += 1;
	return tmp;
    }

    const var &var::operator-- ()
    {
	*this -= 1;
	return *this;
    }

    var var::operator-- (int)
    {
	var tmp = *this;
	*this -= 1;
	return tmp;
    }

    const var &var::operator&= (const string &s)
    {
	string_ += s;
	double_ = atof(string_.c_str());
	return *this;
    }

    const var &var::operator&= (const char *s)
    {
	string_ += s;
	double_ = atof(string_.c_str());
	return *this;
    }

    const var &var::operator&= (char c)
    {
	string_ += c;
	double_ = atof(string_.c_str());
	return *this;
    }

    const var &var::operator&= (double d)
    {
	char s[100];
	std::sprintf(s,default_format_().c_str(),d);
	string_ += s;
	double_ = atof(string_.c_str());
	return *this;
    }

    const var &var::operator&= (int i)
    {
	char s[100];
	std::sprintf(s,"%i",i);
	string_ += s;
	double_ = atof(string_.c_str());
	return *this;
    }

    const var &var::operator&= (const var &v)
    {
	operator&=(v.string_);
	return *this;
    }

    var::operator bool () const
    {
	if(string_ == "" ||
	   string_ == "0") return false;
	return true;
    }

    int var::integer() const
    {
	return (int)(double_);
    }


    const char *var::c_str() const
    {
	return string_.c_str();
    }

    var &var::round(int n) 
    {
        int scale = 1;
        for(int i=0; i<n; ++i) scale *= 10;
        operator=(std::round(double_*scale)/scale);
        return *this;
    }

    var replace_re(const var &pattern, const var &replacement, const var &in)
    {
	var tmp = in;
	tmp.replace_re(pattern, replacement);
	return tmp;
    }

    var replace(const var &from, const var &to, const var &in)
    {
	var tmp = in;
	tmp.replace(from,to);
	return tmp;
    }

    var replace1(const var &from, const var &to, const var &in)
    {
	var tmp = in;
	tmp.replace1(from,to);
	return tmp;
    }

    var abs(var v)
    {
	if(v.string_[0] != '-' && v.double_ >= 0) return v;
	if(v.string_[0] == '-' && v.double_ < 0)
	{
	    v.double_ = -v.double_;
	    v.string_.erase(0,1);
	    return v;
	}
	warning::print("Inconsistency between string/double representation ",
		       "abs(const var &)");
	return v;
    }

    bool operator< (const blop::var &v,double d)
    {
	return v.dbl() < d;
    }
    bool operator< (const blop::var &v,int i)
    {
	return v.dbl() < i;
    }
    bool operator< (double d,const blop::var &v)
    {
	return d < v.dbl();
    }
    bool operator< (int i,const blop::var &v)
    {
	return i < v.dbl();
    }
    bool operator< (const blop::var &v1,const blop::var &v2)
    {
	return v1.dbl() < v2.dbl();
    }

    bool operator<= (const blop::var &v,double d)
    {
	return v.dbl() <= d;
    }
    bool operator<= (const blop::var &v,int i)
    {
	return v.dbl() <= i;
    }
    bool operator<= (double d,const blop::var &v)
    {
	return d <= v.dbl();
    }
    bool operator<= (int i,const blop::var &v)
    {
	return i <= v.dbl();
    }
    bool operator<= (const blop::var &v1,const blop::var &v2)
    {
	return v1.dbl() <= v2.dbl();
    }


    bool operator> (const blop::var &v,double d)
    {
	return v.dbl() > d;
    }
    bool operator> (const blop::var &v,int i)
    {
	return v.dbl() > i;
    }
    bool operator> (double d,const blop::var &v)
    {
	return d > v.dbl();
    }
    bool operator> (int i,const blop::var &v)
    {
	return i > v.dbl();
    }
    bool operator> (const blop::var &v1,const blop::var &v2)
    {
	return v1.dbl() > v2.dbl();
    }

    bool operator>= (const blop::var &v,double d)
    {
	return v.dbl() >= d;
    }
    bool operator>= (const blop::var &v,int i)
    {
	return v.dbl() >= i;
    }
    bool operator>= (double d,const blop::var &v)
    {
	return d >= v.dbl();
    }
    bool operator>= (int i,const blop::var &v)
    {
	return i >= v.dbl();
    }
    bool operator>= (const blop::var &v1,const blop::var &v2)
    {
	return v1.dbl() >= v2.dbl();
    }

    bool operator == (const blop::var &v1,const blop::var &v2)
    {
	return v1.str() == v2.str();
    }
    bool operator == (const blop::var &v1,const char *v2)
    {
	return v1.str() == v2;
    }
    bool operator == (const char *v1,const blop::var &v2)
    {
	return v1 == v2.str();
    }
    bool operator == (const blop::var &v1,const string &v2)
    {
	return v1.str() == v2;
    }
    bool operator == (const string &v1,const blop::var &v2)
    {
	return v1 == v2.str();
    }
    bool operator == (const blop::var &v1, double d)
    {
	return v1.dbl() == d;
    }
    bool operator == (double d, const blop::var &v2)
    {
	return d == v2.dbl();
    }
    bool operator == (const blop::var &v1, int i)
    {
	return v1.dbl() == i;
    }
    bool operator == (int d, const blop::var &v2)
    {
	return d == v2.dbl();
    }


    bool operator != (const blop::var &v1,const blop::var &v2)
    {
	return !operator==((var)v1,(var)v2);
    }
    bool operator != (const blop::var &v1,const char *v2)
    {
	return !operator==(v1,v2);
    }
    bool operator != (const char *v1,const blop::var &v2)
    {
	return !operator==(v1,v2);
    }
    bool operator != (const blop::var &v1,const string &v2)
    {
	return !operator==(v1,v2);
    }
    bool operator != (const string &v1,const blop::var &v2)
    {
	return !operator==(v1,v2);
    }
    bool operator != (const blop::var &v1, double d)
    {
	return !operator==(v1,d);
    }
    bool operator != (double d, const blop::var &v2)
    {
	return !operator==(d,v2);
    }
    bool operator != (const blop::var &v1, int d)
    {
	return !operator==(v1,d);
    }
    bool operator != (int d, const blop::var &v2)
    {
	return !operator==(d,v2);
    }


    var &var::replace(size_type pos, size_type n, const var &s)
    {
	string_.replace(pos,n,s.str());
	double_ = atof(string_.c_str());
	return *this;
    }

    var &var::replace(const var &from,const var &to)
    {
	string::size_type s1 = from.length();
	string::size_type s2 = to.length();

	for(string::size_type ind = 0;
	    (ind = string_.find(from.str(), ind)) != std::string::npos;
	    ind += s2)
	{
	    string_.replace(ind, s1, to.str());
	}
	
	double_ = atof(string_.c_str());
	return *this;
    }

    var &var::replace1(const var &from,const var &to)
    {
	string::size_type ind = 0;
	if( (ind = string_.find(from.str(), ind)) != std::string::npos )
	{
	    string_.replace(ind, from.length(), to.str());
	}
	
	double_ = atof(string_.c_str());
	return *this;
    }

    const var::size_type var::npos = std::string::npos;

    var::size_type var::size() const
    {
	return string_.size(); 
    }

    var::size_type var::length() const
    {
	return string_.length();
    }

    var::size_type var::find(const var &o, var::size_type i) const
    {
	return string_.find(o.str(),i);
    }

    var::size_type var::rfind(const var &o, var::size_type i) const
    {
	return string_.rfind(o.str(),i);
    }

    var::size_type var::find_first_of(const var &o, size_type i) const
    {
	return string_.find_first_of(o.str(), i);
    }

    var::size_type var::find_last_of(const var &o, size_type i) const
    {
	return string_.find_last_of(o.str(), i);
    }

    var::size_type var::find_first_not_of(const var &o, size_type i) const
    {
	return string_.find_first_not_of(o.str(), i);
    }

    var::size_type var::find_last_not_of(const var &o, size_type i) const
    {
	return string_.find_last_not_of(o.str(), i);
    }

    void var::subvar::update_var()
    {
	var result;
	if(from_ > 0) result &= var_->string_.substr(0,from_);
	result &= *this;
	if(to_ != std::string::npos) result &= var_->string_.substr(to_+1,std::string::npos);
	*var_ = result;
    }

    var::subvar::subvar(var *v,std::string::size_type from,std::string::size_type to)
    {
	var_ = v;
	from_ = from;
	to_   = to;
	var::operator=( v->string_.substr(from,to-from+1) );
    }

    var::subvar::subvar(const subvar &o) : var(o)
    {
	var_ = o.var_;
	from_ = o.from_;
	to_   = o.to_;
    }

    var::subvar var::operator() (std::string::size_type from, std::string::size_type to)
    {
	return subvar(this,from,to);
    }

    ostream &operator<<(ostream &out,const blop::var &v)
    {
	out<<v.str();
	return out;
    }

    istream &operator>>(istream &in,blop::var &v)
    {
	string s;
	in>>s;
	v = s;
	return in;
    }


    var operator& (const var &v1,const var &v2)
    {
	var result = v1;
	result &= v2;
	return result;
    }
    var operator& (const var &v1,const string &s    )
    {
	var result = v1;
	result &= s;
	return result;
    }
    var operator& (const string &s    ,const var &v2)
    {
	var result = s;
	result += v2;
	return result;
    }
    var operator& (const var &v1,const char *s      )
    {
	var result = v1;
	result &= s;
	return result;
    }
    var operator& (const char *s      ,const var &v2)
    {
	var result = s;
	result &= v2;
	return result;
    }
    var operator& (const var &v1,double d           )
    {
	var result = v1;
	result &= d;
	return result;
    }
    var operator& (double d           ,const var &v2)
    {
	var result = d;
	result &= v2;
	return result;
    }
    var operator& (const string &s1,const string &s2)
    {
	var result = s1;
	result &= s2;
	return result;
    }
    var operator& (const string &s1,const char   *s2)
    {
	var result = s1;
	result &= s2;
	return result;
    }
    var operator& (const char   *s1,const string &s2)
    {
	var result = s1;
	result &= s2;
	return result;
    }

    var operator& (double d,const string &s)
    {
	var result = d;
	result &= s;
	return result;
    }

    var operator& (const string &s,double d)
    {
	var result = s;
	result &= d;
	return result;
    }

    var operator& (const var &v, int i)
    {
	var result = v;
	result &= i;
	return result;
    }

    var operator& (int i, const var &v)
    {
	var result = i;
	result &= v;
	return result;
    }

    var operator& (const var &v, char c)
    {
	var result = v;
	result &= c;
	return result;
    }

    var operator& (char c, const var &v)
    {
	var result = c;
	result &= v;
	return result;
    }
}


ostream &operator<<(ostream &out,const blop::var &v)
{
    blop::operator<<(out,v);
    return out;
}

istream &operator>>(istream &in,blop::var &v)
{
    blop::operator>>(in,v);
    return in;
}





// ------------------ mathematical functions for vars ------------------

blop::var pow  (const blop::var &a,const blop::var &b)
{
    return pow(a.dbl(),b.dbl());
}
blop::var exp  (const blop::var &a)
{
    return exp(a.dbl());
}
blop::var log  (const blop::var &a)
{
    return log(a.dbl());
}
blop::var log10(const blop::var &a)
{
    return log10(a.dbl());
}
blop::var sqrt (const blop::var &a)
{
    return sqrt(a.dbl());
}
blop::var sin  (const blop::var &a)
{
    return sin(a.dbl());
}
blop::var cos  (const blop::var &a)
{
    return cos(a.dbl());
}
blop::var asin (const blop::var &a)
{
    return asin(a.dbl());
}
blop::var acos (const blop::var &a)
{
    return acos(a.dbl());
}
blop::var tan  (const blop::var &a)
{
    return tan(a.dbl());
}
blop::var atan (const blop::var &a)
{
    return atan(a.dbl());
}
blop::var sinh (const blop::var &a)
{
    return sinh(a.dbl());
}
blop::var asinh(const blop::var &a)
{
    return asinh(a.dbl());
}
blop::var cosh (const blop::var &a)
{
    return cosh(a.dbl());
}
blop::var acosh(const blop::var &a)
{
    return acosh(a.dbl());
}

#ifdef _BLOP_VAR_ARITHMETIC_OPERATORS_IN_NAMESPACE
namespace blop
{
#endif    
    blop::var operator+ (const blop::var &v1,const blop::var &v2) {return v1.dbl() + v2.dbl();}
    blop::var operator+ (const blop::var &v1,double d) {return v1.dbl() + d;}
    blop::var operator+ (double d, const blop::var &v2) {return d + v2.dbl();}
    blop::var operator+ (const blop::var &v1, int d) { return v1.dbl() + d; }
    blop::var operator+ (int d, const blop::var &v2) { return d + v2.dbl(); }
    blop::var operator+ (const blop::var &v1,const string &s)
    {
        blop::var result = v1;
        result += s;
        return result;
    }
    blop::var operator+ (const string &s,const blop::var &v2)
    {
        blop::var result = s;
        result += v2;
        return result;
    }
    blop::var operator+ (const char *s,const blop::var &v2)
    {
        return string(s) + v2;
    }
    blop::var operator+ (const blop::var &v2,const char *s)
    {
        return v2 + string(s);
    }
    
    blop::var operator- (const blop::var &v)
    {
        return - v.dbl();
    }
    
    blop::var operator- (const blop::var &v1,const blop::var &v2) {return v1.dbl() - v2.dbl();}
    blop::var operator- (const blop::var &v1,double d) {return v1.dbl() - d;}
    blop::var operator- (double d, const blop::var &v2) {return d - v2.dbl();}
    blop::var operator- (const blop::var &v1, int d) { return v1.dbl() - d; }
    blop::var operator- (int d, const blop::var &v2) { return d - v2.dbl(); }
    blop::var operator- (const blop::var &v1,const string &s)
    {
        blop::var result = v1;
        result -= s;
        return result;
    }
    blop::var operator- (const string &s,const blop::var &v2)
    {
        blop::var result = s;
        result -= v2;
        return result;
    }
    blop::var operator- (const char *s,const blop::var &v2)
    {
        return string(s) - v2;
    }
    blop::var operator- (const blop::var &v2,const char *s)
    {
        return v2 - string(s);
    }

    blop::var operator* (const blop::var &v1,const blop::var &v2) {return v1.dbl() * v2.dbl();}
    blop::var operator* (const blop::var &v1,double d) {return v1.dbl() * d;}
    blop::var operator* (double d, const blop::var &v2) {return d * v2.dbl();}
    blop::var operator* (const blop::var &v1, int d) { return v1.dbl() * d; }
    blop::var operator* (int d, const blop::var &v2) { return d * v2.dbl(); }
    blop::var operator* (const blop::var &v1,const string &s)
    {
        blop::var result = v1;
        result *= s;
        return result;
    }
    blop::var operator* (const string &s,const blop::var &v2)
    {
        blop::var result = s;
        result *= v2;
        return result;
    }
    blop::var operator* (const char *s,const blop::var &v2)
    {
        return string(s) + v2;
    }
    blop::var operator* (const blop::var &v2,const char *s)
    {
        return v2 * string(s);
    }
    
    blop::var operator/ (const blop::var &v1,const blop::var &v2) {return v1.dbl() / v2.dbl();}
    blop::var operator/ (const blop::var &v1,double d)  {return v1.dbl() / d;}
    blop::var operator/ (double d, const blop::var &v2) {return d / v2.dbl();}
    blop::var operator/ (const blop::var &v1, int d)    { return v1.dbl() / d; }
    blop::var operator/ (int d, const blop::var &v2)    { return d / v2.dbl(); }
    blop::var operator/ (const blop::var &v1,const string &s)
    {
        blop::var result = v1;
        result /= s;
        return result;
    }
    blop::var operator/ (const string &s,const blop::var &v2)
    {
        blop::var result = s;
        result /= v2;
        return result;
    }
    blop::var operator/ (const char *s,const blop::var &v2)
    {
        return string(s) / v2;
    }
    blop::var operator/ (const blop::var &v2,const char *s)
    {
        return v2 / string(s);
    }
#ifdef _BLOP_VAR_ARITHMETIC_OPERATORS_IN_NAMESPACE
}
#endif


