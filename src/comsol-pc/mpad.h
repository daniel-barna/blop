#ifndef __BLOP_MPAD_H__
#define __BLOP_MPAD_H__

#include "pad.h"
#include "mcontainer.h"

namespace blop
{

class mpad : public container, public mcontainer
{
 private:
    length gap_;
    static length &default_gap_();

    length *widths_,*heights_;
    double *rwidths_,*rheights_;
    length x_avail_, y_avail_;

    static int default_direction_;

    pad **pads_;

    void set_widths();
    void set_heights();
    void cd_to_sub_(int i) { pads_[i]->cd(); }

 public:
    mpad(int i, int j);

    // estimate the best grid i*j such that i*j<=n
    mpad(int n);

    ~mpad();


    // ---------------------------  creator function -----------------
    // create a new mpad with the specified dimensions, and cd to one of its
    // subpads (which is derived from the default looping direction, see below)

    static mpad &mknew(int i, int j);
    static mpad &mknew(int n);


    // --------------------------  set gap ---------------------------
    // Set the gap between the subpads, and set the default value of it

    mpad &gap(const length &l);
    static void default_gap(const length &l);


    mpad &width(int i, const length &l);
    mpad &rwidth(int i, double d);
    mpad &height(int i, const length &l);
    mpad &rheight(int i, double d);

    mpad &draw_border(bool b);
    mpad &bordercolor(const color &);
    mpad &borderwidth(const length &l);
    mpad &border_3D(bool);
    mpad &fill(bool i);
    mpad &fillcolor(const color &);

    mpad &cd(int i, int j) { mcontainer::cd(i,j); return *this; }
    mpad &cd(int i) { mcontainer::cd(i); return *this; }
    mpad &cd_first() { mcontainer::cd_first(); return *this; }

    // ----------------------------------------------------------
    // specify the direction for looping. if jump is also given
    // (like direction(mcontainer::right_down | mcontainer::jump) ), then it will
    // automatically jump to the beginning (and begin looping), if the
    // last subpad is reached

    mpad &direction(int d) { mcontainer::direction(d); return *this; }
    static void default_direction(int);

    // ---------  override container:: and box:: dimension functions --------------
    // so that they return a pad& instead of container& or box&
    
    mpad &cleft(const length &l) { container::cleft(l); return *this; }
    mpad &cright(const length &l) { container::cright(l); return *this; }
    mpad &cbottom(const length &l) { container::cbottom(l); return *this; }
    mpad &ctop(const length &l) { container::ctop(l); return *this; }
    
    const length &cleft() const { return container::cleft(); }
    const length &cright() const { return container::cright(); }
    const length &cbottom() const { return container::cbottom(); }
    const length &ctop() const { return container::ctop(); }
    
    mpad &lmargin(const length &l) { container::lmargin(l); return *this; }
    mpad &rmargin(const length &l) { container::rmargin(l); return *this; }
    mpad &bmargin(const length &l) { container::bmargin(l); return *this; }
    mpad &tmargin(const length &l) { container::tmargin(l); return *this; }
    const length &lmargin() const { return container::lmargin(); }
    const length &rmargin() const { return container::rmargin(); }
    const length &bmargin() const { return container::bmargin(); }
    const length &tmargin() const { return container::tmargin(); }
    
    mpad &left(const length &l) { box::left(l); return *this; }
    mpad &right(const length &l) { box::right(l); return *this; }
    mpad &width(const length &l) { box::width(l); return *this; }
    mpad &bottom(const length &l) { box::bottom(l); return *this; }
    mpad &top(const length &l) { box::top(l); return *this; }
    mpad &height(const length &l) { box::height(l); return *this; }
    const length &left() const { return box::left(); }
    const length &right() const { return box::right(); }
    const length &width() const { return box::width(); }
    const length &bottom() const { return box::bottom(); }
    const length &top() const { return box::top(); }
    const length &height() const { return box::height(); }
    
    bool modified() const;
    void modified(bool f);

    void cd_specific() {}
    void prepare_for_draw();
    void print(terminal *);

    pad *operator() (int i,int j);
};

}

#endif
