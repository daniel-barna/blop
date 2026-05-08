#ifndef __BLOP_MFRAME_H__
#define __BLOP_MFRAME_H__

#include "frame.h"
#include "mcontainer.h"
#include "label.h"

namespace blop
{

class epad;

class mframe : public container, public mcontainer
{
 private:
    typedef container base;

    static int default_direction_;

    frame **frames_;
    bool   *show_subframe_;
    axis  **x1axes_, **y1axes_, **x2axes_, **y2axes_;
    epad   **lmarginboxes_, **rmarginboxes_, **bmarginboxes_, **tmarginboxes_;

    length xgap_, ygap_;
    length *widths_,*heights_;
    double *rwidths_,*rheights_;
    length x_avail_, y_avail_;
    void set_widths();
    void set_heights();

    void cd_to_sub_(int i) { frames_[i]->cd(); }
    
    label *title_;

    // in a 2x2 frame, for example, the highest y-tic of the lower row,
    // and the highest x-tic of the left column can collide with the first tic
    // of the top row or the first tic of the right column. This flag (true by default)
    // removes therefore these highest tics
    bool remove_conflicting_tic_;
    static bool default_remove_conflicting_tic_;

    unsigned int equal_ranges_;

 public:
    mframe(int i, int j);
    mframe(int n);
    ~mframe();

    static mframe &mknew(int i, int j);
    static mframe &mknew(int n);

    mframe &cd(int i, int j) { mcontainer::cd(i,j); return *this; }
    mframe &cd(int i)        { mcontainer::cd(i); return *this; }
    mframe &cd_first()       { mcontainer::cd_first(); return *this; }

    // -------------  Direction for cycling -------------------------
    // specify the direction for looping. if jump is also given
    // (like direction(mcontainer::right_down | mcontainer::jump) ), then it will
    // automatically jump to the beginning (and begin looping), if the
    // last subpad is reached

    mframe &direction(int dir) { mcontainer::direction(dir); return *this; }
    static void default_direction(int);  // default value for all new mframe


    // ---------  override container:: and box:: dimension functions --------------
    // so that they return a mframe& instead of container& or box&
    
    mframe &cleft(const length &l) { container::cleft(l); return *this; }
    mframe &cright(const length &l) { container::cright(l); return *this; }
    mframe &cbottom(const length &l) { container::cbottom(l); return *this; }
    mframe &ctop(const length &l) { container::ctop(l); return *this; }
    
    const length &cleft() const { return container::cleft(); }
    const length &cright() const { return container::cright(); }
    const length &cbottom() const { return container::cbottom(); }
    const length &ctop() const { return container::ctop(); }
    
    mframe &lmargin(const length &l) { container::lmargin(l); return *this; }
    mframe &rmargin(const length &l) { container::rmargin(l); return *this; }
    mframe &bmargin(const length &l) { container::bmargin(l); return *this; }
    mframe &tmargin(const length &l) { container::tmargin(l); return *this; }
    const length &lmargin() const { return container::lmargin(); }
    const length &rmargin() const { return container::rmargin(); }
    const length &bmargin() const { return container::bmargin(); }
    const length &tmargin() const { return container::tmargin(); }
    
    mframe &left(const length &l) { box::left(l); return *this; }
    mframe &right(const length &l) { box::right(l); return *this; }
    mframe &width(const length &l) { box::width(l); return *this; }
    mframe &bottom(const length &l) { box::bottom(l); return *this; }
    mframe &top(const length &l) { box::top(l); return *this; }
    mframe &height(const length &l) { box::height(l); return *this; }
    const length &left() const { return box::left(); }
    const length &right() const { return box::right(); }
    const length &width() const { return box::width(); }
    const length &bottom() const { return box::bottom(); }
    const length &top() const { return box::top(); }
    const length &height() const { return box::height(); }

    // --------  Return the row/column number of a subframe -------
    // return 0 if this frame is not a subframe of me

    int row(const frame *f) const;
    int col(const frame *f) const;

    // --------  Return the given subframe ------------------------

    frame       *operator()(int i, int j);
    const frame *operator()(int i, int j) const;

    // -------- show/hide a given subframe  -----------------------

    mframe &show_subframe(int i, int j, bool );
    bool    show_subframe(int i, int j) const; 

    // --------  Specify some geometrical parameters --------------

    mframe &gap(const length &l);                        // Specify gap between subframes (both x and y direction)
    mframe &gap(const length &xgap, const length &ygap); // Specify gaps separately in the two directions
    mframe &xgap(const length &g);
    mframe &ygap(const length &g);
    mframe &width(int i, const length &l); // specify width of the ith column
    mframe &rwidth(int i, double d);       // specify relative width of the ith col
    mframe &height(int i, const length &l);// specify height of the ith row
    mframe &rheight(int i, double d);      // specify relative height of the ith row

    // --------------  enforce equal ranges on the axes  -----------

    mframe &equal_ranges(unsigned int axes) { equal_ranges_ = axes; return *this; }
    mframe &equal_ranges(bool f) { if(f) equal_ranges_ = axis::x1|axis::y1; else equal_ranges_ = 0; return *this; }


    // -----------  Set title  -------------------------------------

    mframe &title(const var &t);

    void print_hierarchy(int level);
    
    // ----------------  remove conflicting tics --------------------

    mframe &remove_conflicting_tics(bool f) { remove_conflicting_tic_ = f; return *this; }

    // --------   not to be called by user ------------------------

    bool parent(container *c);  // specify the parent of this mframe

    bool modified() const;
    void modified(bool f);
    void cd_specific() {}
    void prepare_for_draw();
    void print(terminal *);

    

};

}

#endif
