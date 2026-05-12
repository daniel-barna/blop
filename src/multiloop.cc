#include "multiloop.h"
#include "warning.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace blop
{
    
void multiloop::create_(unsigned int nvars, const std::vector<int> &from, const std::vector<int> &to)
{
    for(unsigned int i=0; i<nvars; ++i)
    {
        const double from_value = (i<from.size() ? from[i] : from.back());
        const double to_value   = (i<to.size()   ? to[i]   : to.back());
        if(from_value > to_value)
        {
            ostringstream s;
            s<<"multiloop::multiloop("<<nvars<<",vector<int>("<<from.size()<<"),vector<int>("<<to.size()<<"))";
            warning::print("'to' must be larger than 'from'",s.str());
        }
        from_[i] = from_value;
        to_[i] = to_value;
    }
    reset();
}


bool multiloop::init_(int j)
{
    for(unsigned int i=j; i<current_.size(); ++i) current_[i] = from_[i];
    return true;
}

int multiloop::operator[](int i)  const
{
    return current_[i];
}

void multiloop::reset()
{
    state_ = init_();
    pos_ = 0;
}

bool multiloop::operator++()
{
    // Do nothing if not in a correct state
    if(state_==false) return false;

    int i=current_.size()-1; 

    for(; i>=0; --i)
    {
        // Try to increase the ith index.
        if(++current_[i]<to_[i]) 
        {
            // If the increased index is within limits, reinitialize subsequent indices. If we succeed, we are done
            if(init_(i+1))
            {
                ++pos_;
                return true;
            }
            // Otherwise go backwards to previous index, and try to increase that one.
        }
    }

    // WE could not increment, set to false
    if(i<0)
    {
        init_();
        state_ = false;
        return false;
    }

    std::cerr<<"This should never happen (?) in multiloop.cc"<<std::endl;
    return false;
}

void multiloop::set(unsigned int i)
{
    reset();
    for(unsigned int j=0; j<i; ++j) ++(*this);
}

}
