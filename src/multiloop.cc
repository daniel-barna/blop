#include "multiloop.h"
#include <iostream>


multiloop::multiloop(unsigned int size, int to, int from, bool permutations) : current_(size), from_(size), to_(size), permutations_(permutations) 
{
    for(unsigned int i=0; i<size; ++i)
    {
        from_[i] = from;
        to_[i] = to;
    }
    // Initialize the state to 'false' in case init_() fails (which can happen if no permutations are allowed,
    // and the limits are too low to allow all indices to be different)
    reset();
}


bool multiloop::init_(int j)
{
    if(permutations_)
    {
        for(unsigned int i=j; i<current_.size(); ++i) current_[i] = from_[i];
    }
    else
    {
        for(unsigned int i=j; i<current_.size(); ++i) 
        {
            current_[i] = (i==0?from_[i]:current_[i-1]+1);
            if(current_[i]>=to_[i]) return false;
        }
    }
    return true;
}


int &multiloop::operator[](int i) 
{
    return current_[i];
}
const int &multiloop::operator[](int i)  const
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
    return true;
}

void multiloop::set(unsigned int i)
{
    reset();
    for(unsigned int j=0; j<i; ++j) ++(*this);
}
