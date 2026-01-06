#ifndef __BLOP_MULTILOOP_H__
#define __BLOP_MULTILOOP_H__

#include <vector>

class multiloop
{
private:
    std::vector<int> current_, from_, to_;
    bool permutations_; // Allow permutations
    bool init_(int j=0);
    bool state_;
    unsigned int pos_;

public:
    multiloop(unsigned int size, int to, int from=0, bool permutations=true);
    unsigned int size() const { return current_.size(); }
    bool operator++();
    int &operator[](int);
    const int &operator[](int) const;
    operator bool() const { return state_; }
    void reset();
    void set(unsigned int i);
    unsigned int get() const { return pos_; }
};

#endif
