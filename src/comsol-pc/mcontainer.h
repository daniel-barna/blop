#ifndef __BLOP_MCONTAINER_H__
#define __BLOP_MCONTAINER_H__

#include "container.h"
#include <vector>

namespace blop
{
class mcontainer
{
protected:
    int direction_;
    int nx_, ny_;
    int current_ix_, current_iy_;
    virtual int ind(int i,int j) const;
    virtual void cd_to_sub_(int i) = 0;

    void init_(int i, int j, int dir=right_down);

    static void get_gridsize_(int n, int &xdim, int &ydim);

public:
    enum {right_down = 1,  right_up = 2 , down_right = 4 , up_right = 8,
	  left_down  = 16, left_up  = 32, down_left  = 64, up_left  = 128,
	  jump = 256 };

    mcontainer(int i, int j,int dir=right_down);
    mcontainer();
    virtual ~mcontainer() {}

    int ncols() const { return nx_; }
    int nrows() const { return ny_; }

    virtual mcontainer &direction(int d);
    virtual mcontainer &cd(int i, int j);
    virtual mcontainer &cd(int i);

    // ----------------------------------------------------------
    // loop over all subpads in a specified direction (see below)
    // if the 'jump' bit is not set in the direction (see below), and the
    // looping has already reached the last subpad, no action is taken,
    // and false is returned

    virtual bool         cd_next();
    virtual mcontainer  &cd_first(); // change to the first subcontainer, according to direction
    
    // ----------------------------------------------------------
    // Get the current indices. Indices are 1-based,
    // i.e. ix=1 means the leftmost column, iy=1 means the bottom row.

    virtual int current_ix() const { return current_ix_; }
    virtual int current_iy() const { return current_iy_; }

};
}

#endif
