#include "array.h"
#include <map>
#include <algorithm>
#include <functional>
#include "warning.h"


namespace blop
{
    var array::join(const var &separator, unsigned int from, int to) const
    {
        var result = "";
        unsigned int TO = size();
        if(to>=0 && to<=(int)size()) TO = to;
        for(unsigned int i=from; i<TO; ++i)
        {
            if(i>from) result &= separator;
            result &= (*this)[i];
        }
        return result;
    }

    array &array::replace(const var &from, const var &to)
    {
	for(unsigned int i=0; i<size(); ++i)
	{
	    (*this)[i].replace(from,to);
	}
        return *this;
    }
    array &array::replace_re(const var &pattern, const var &replacement)
    {
	for(unsigned int i=0; i<size(); ++i)
	{
	    (*this)[i].replace_re(pattern,replacement);
	}
        return *this;
    }

    class var_less_s : public std::binary_function<var,var,bool>
    {
    public:
	bool operator()(const var &x, const var &y) const { return x.str()<y.str(); }
    };
    class var_less_n : public std::binary_function<var,var,bool>
    {
    public:
	bool operator()(const var &x, const var &y) const { return x.dbl()<y.dbl(); }
    };
    

    void array::sort_s()
    {
	std::sort(begin(), end(), var_less_s());
    }
    void array::sort_n()
    {
	std::sort(begin(), end(), var_less_n());
    }

    var array::outfs_ = " ";
    var array::infs_  = " ";

    const var &array::outfs() { return outfs_; }
    void array::outfs(const var &f) { outfs_ = f; }
    const var &array::infs() { return infs_; }
    void array::infs(const var &f) { infs_ = f; }

    array::array() : iterator_(0) {}
    array::array(const array &o) : std::vector<blop::var>(o), iterator_(o.iterator_) {}
    array::array(unsigned int size) : std::vector<blop::var>(size), iterator_(0) {}

    array::array(const std::initializer_list<double> &l) : iterator_(0)
    {
        reserve(l.size());
        for(auto &e : l) push_back(e);
    }
    array::array(const std::initializer_list<int> &l) : iterator_(0)
    {
        reserve(l.size());
        for(auto &e : l) push_back(e);
    }
    array::array(const std::initializer_list<std::string> &l) : iterator_(0)
    {
        reserve(l.size());
        for(auto &e : l) push_back(e);
    }
    array::array(const std::initializer_list<const char *> &l) : iterator_(0)
    {
        reserve(l.size());
        for(auto &e : l) push_back(e);
    }
    array::array(const std::initializer_list<blop::var> &l) : iterator_(0)
    {
        reserve(l.size());
        for(auto &e : l) push_back(e);
    }

    void array::setfirst() { iterator_ = 0; }
    void array::setlast() { iterator_ = size()-1; }
    void array::set(int i) { iterator_ = i; }
    int array::pos() const { return iterator_ ; }
    array::operator bool () const {return 0<=iterator_ && iterator_<size();}
    const array &array::operator++() {++iterator_; return *this;}
    const array &array::operator--() {--iterator_; return *this;}
    var &array::operator()()
    {
	if(0<=iterator_ && iterator_<size()) return (*this)[iterator_];
	warning::print("Iterator is out of range","array::operator()");
	static var dummy;
	return dummy;
    }
    const var &array::operator()() const { return (*this)[iterator_]; }

    const array &array::operator=(const array &rhs)
    {
	std::vector<blop::var>::operator=(rhs);
	iterator_ = rhs.iterator_;
	return rhs;
    }

    var array::pop_back()
    {
	var result = "";
	if(!empty())
	{
	    result = back();
	    resize(size()-1);
	}
	return result;
    }


    array &array::erase(unsigned int pos)
    {
	std::vector<blop::var>::erase(begin()+pos);
	return *this;
    }

    array &array::push_back(const array &other)
    {
	int off = size();
	resize(size() + other.size());
	for(unsigned int i=0; i<other.size(); ++i)
	{
	    (*this)[off+i] = other[i];
	}
	return *this;
    }



    

    array &array::operator() (const var &v)
    {
	vector<var>::push_back(v);
	return *this;
    }


    void array::push_front(const var &v)
    {
	resize(size() + 1);
	for(int i=size()-1; i>=1; --i)
	{
	    (*this)[i] = (*this)[i-1];
	}
	(*this)[0] = v;
    }

    void array::push_front(const array &o)
    {
	int oldsize = size();
	int newsize =   oldsize + o.size();
	resize(newsize);
	for(int i=oldsize-1; i>=0; --i)
	{
	    (*this)[i+o.size()] = (*this)[i];
	}
	for(unsigned int i=0; i<o.size(); ++i)
	{
	    (*this)[i] = o[i];
	}
    }

    array array::tmp_;

    array &array::sequence(const var &step, const var &from, const var &to)
    {
	tmp_.clear();
	for(var v = from; v.dbl() <= to.dbl(); v += step)
	{
	    tmp_.push_back(v);
	}
	return tmp_;
    }

    array &array::make(const var &v)
    {
	tmp_.clear();
	tmp_.push_back(v);
	return tmp_;
    }

    array array_unique(const array &a)
    {
	std::map<std::string, bool> m;
	for(unsigned int i=0; i<a.size(); ++i) m[a[i]] = true;
	array result;
	for(std::map<std::string,bool>::iterator i=m.begin();
	    i != m.end(); ++i)
	{
	    result.push_back((*i).first);
	}
	return result;
    }

}



std::ostream& operator<< (std::ostream &out, const blop::array &a)
{
    for(unsigned int i=0; i<a.size(); ++i)
    {
	out<<a[i];
	if(i < a.size()-1) out<<blop::array::outfs();
    }
    return out;
}
