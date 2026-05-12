#ifndef __BLOP_MULTILOOP_H__
#define __BLOP_MULTILOOP_H__

#include <vector>

namespace blop
{

class multiloop
{
private:
    // Current values of the variables
    std::vector<int> current_;

    // Starting values of the variables
    std::vector<int> from_;

    // Upper limits of the variables
    std::vector<int> to_;

    // Initialize all variables starting at index 'j' to their starting values
    bool init_(int j=0);

    // A state flag to indicate whether the loop is in a valid state (all variables
    // within their ranges)
    bool state_;
    
    unsigned int pos_;

    void create_(unsigned int nvars, const std::vector<int> &from, const std::vector<int> &to);
    

public:
    // Specify the number of variables and a common upper limit for all of them. They will all start from zero.
    multiloop(unsigned int nvars, int to) : current_(nvars), from_(nvars), to_(nvars) { create_(nvars, {0}, {to}); }

    // Specify the number of variables and individual upper limits. They all start from zero
    // 'to' must have to.size()==nvars. If less values are given, the last one is replicated. If more values
    // are given the superfluous one are ignored
    multiloop(unsigned int nvars, const std::vector<int> &to) : current_(nvars), from_(nvars), to_(nvars) { create_(nvars, {0}, to); }

    // Specify number of variables and a common starting value and upper limit for them
    multiloop(unsigned int nvars, int from, int to) : current_(nvars), from_(nvars), to_(nvars) { create_(nvars, {from}, {to}); }

    // Specify number of variables, individual starting and common upper limits
    multiloop(unsigned int nvars, const std::vector<int> &from, int to) : current_(nvars), from_(nvars), to_(nvars) { create_(nvars, from, {to}); }

    // Specify number of variables, common starting value, individual upper limits
    multiloop(unsigned int nvars, int from, const std::vector<int> &to) : current_(nvars), from_(nvars), to_(nvars) { create_(nvars, {from}, to); }

    // Specify number of variables, indivicual starting values and upper limits
    multiloop(unsigned int nvars, const std::vector<int> &from, const std::vector<int> &to) : current_(nvars), from_(nvars), to_(nvars) { create_(nvars, from, to); }

    // Return the number of variables
    unsigned int n_vars() const { return current_.size(); }

    // Increment the multiloop. Return true if the multiloop is valid after the increment
    // (i.e. all variables are within their range)
    bool operator++();

    // Return the ith variable's current value
    int operator[](int) const;

    // Return all current values in the form of a vector using the empty parenthesis operator
    const std::vector<int> &operator()() const { return current_; }

    // Return true if the multiloop is a valid state (has not exceeded its upper limt)
    operator bool() const { return state_; }

    // Reset the multiloop to its initial state
    void reset();

    // Set the multiloop to the ith combination of values (equivalent to executing the operator++
    // i times from the initial state
    void set(unsigned int i);

    // get the global index of the current value combination (in the range [0:n_combinations()-1]
    unsigned int get() const { return pos_; }

    // Returns the total number of value combinations
    unsigned int n_values() const;
    
};

}

#endif
