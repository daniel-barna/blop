#include "grob.h"
#include "pad.h"
#include "exc.hh"
#include "frame.h"

namespace blop
{
    using namespace std;

    length &grob::default_x_unit_() { static length l = CM; return l; }
    length &grob::default_y_unit_() { static length l = CM; return l; }
    bool grob::default_use_parent_x_unit_ = true;
    bool grob::default_use_parent_y_unit_ = true;
    
    void grob::default_x_unit(length l) {default_x_unit_() = l; default_use_parent_x_unit_ = false; }
    void grob::default_y_unit(length l) {default_y_unit_() = l; default_use_parent_y_unit_ = false; }
    void grob::default_unit(length l)
    {
	default_x_unit(l);
	default_y_unit(l);
    }
    void grob::default_x_unit(int) { default_use_parent_x_unit_ = true; }
    void grob::default_y_unit(int) { default_use_parent_y_unit_ = true; }
    void grob::default_unit(int)
    {
	default_x_unit(use_parent);
	default_y_unit(use_parent);
    }

    length grob::x_unit()
    {
	return !x_unit_;
    }
    grob &grob::x_unit(length x)
    {
	x_unit_ = x;
	return *this;
    }
    grob &grob::x_unit(int)
    {
	x_unit_ = !parent_cwidth_;
	return *this;
    }


    length grob::y_unit()
    {
	return !y_unit_;
    }
    grob &grob::y_unit(length x)
    {
	y_unit_ = x;
	return *this;
    }
    grob &grob::y_unit(int)
    {
	y_unit_ = !parent_cheight_;
	return *this;
    }

    grob &grob::unit(length x)
    {
	x_unit(x);
	y_unit(x);
	return *this;
    }
    grob &grob::unit(int)
    {
	x_unit(use_parent);
	y_unit(use_parent);
	return *this;
    }

    grob::grob(): parent_(0), autodel_(false), print_me_(2), layer_("default")
    {
	modified_ = true;
	parent_cwidth_ = ZERO;
	parent_cheight_ = ZERO;
	if(default_use_parent_x_unit_) x_unit_ = !parent_cwidth_;
	else x_unit_ = default_x_unit_();
	if(default_use_parent_y_unit_) y_unit_ = !parent_cheight_;
	else y_unit_ = default_y_unit_();
    }

    void grob::print_hierarchy(int level)
    {
	for(int i=0; i<level*4; ++i) cout<<" ";
	cout<<name()<<endl;
    }

    bool grob::parent(container *p)
    {
	if(p == 0)
	{
	    parent_cwidth_ = ZERO;
	    parent_cheight_ = ZERO;
	    return false;
	}
	parent_ = p;
	parent_cwidth_ = !parent_->cwidth();
	parent_cheight_ = !parent_->cheight();
	return true;
    }


    grob::~grob()
    {
	container::remove_from_all(this);
    }

    grobs &grobs::linecolor(const color &c)
    {
        for(auto g: grobs_) g->linecolor(c);
        return *this;
    }

    grobs &grobs::fillcolor(const color &c)
    {
        for(auto g: grobs_) g->linecolor(c);
        return *this;
    }

    grobs &grobs::linestyle(sym::linestyle s)
    {
        for(auto g: grobs_) g->linestyle(s);
        return *this;
    }

    grobs &grobs::linewidth(const length &l)
    {
        for(auto g: grobs_) g->linewidth(l);
        return *this;
    }

}
