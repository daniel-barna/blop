#include "box.h"
#include "exc.hh"
#include "warning.h"
#include <cstdio>

namespace blop
{
    using namespace std;

    box &box::fix_width(bool f)
    {
	if(f && (width_.depends_on(left_) || width_.depends_on(right_) || width_.depends_on(xcenter_)))
	{
	    warning::print("Please set the width of the box to an independent value before fixing it","box::fix_width(true)");
	    return *this;
	}
	width_fixed_ = f;
	return *this;
    }
    box &box::fix_height(bool f)
    {
	if(f && (height_.depends_on(bottom_) || height_.depends_on(top_) || height_.depends_on(ycenter_)))
	{
	    warning::print("Please set the height of the box to an independent value before fixing it","box::fix_height(true)");
	    return *this;
	}
	height_fixed_ = f;
	return *this;
    }

    // This function is using variables like 'left', 'right', 'width'. This is only to
    // simplify the understanding. This function is in fact also used for setting the
    // vertical position/size, but this is a completely analogous algorithm, substituting
    // left->bottom, right->top, width->height, etc
    void box::set_(const length &l,
		   length &left,
		   length &right,
		   length &width,
		   length &center,
		   bool width_fixed,
		   history_values history[2],
		   history_values todo,
		   bool &left_modified,
		   bool &right_modified)
    {
	left_modified = false;
	right_modified = false;

	// First of all, set the corresponding length, which is to be set. Other values
	// will be updated later, if needed (for example if the last action was to set left,
	// and now we are setting right, then width and center will be updated later to
	// keep consistency)
	switch(todo)
	{
	case LeftBottom: left = l; left_modified = true;  break;
	case RightTop: right = l; right_modified = true; break;
	case WidthHeight:
	    if(width_fixed)
	    {
		warning::print("This should never happen: trying to set width/height of box although it's fixed");
		return;
	    }
	    width = l; break;
	case Center: center = l; break;
	default: err("This should not happen...");
	}

	// This is the same as the last action before, all the dependent
	// lengths are set up such that it is sufficient to update the
	// corresponding length
	if(history[1] == todo)
	{
	    return;
	}

	// This is the same as the butlast action: also no need to change
	// the dependent lengths. Only update the history
	if(history[0] == todo)
	{
	    history[0] = history[1];
	    history[1] = todo;
	    return;
	}

	// now update the other values. If width is fixed, then ignore the history, and
	// update all other values based on only the current operation.
	if(width_fixed)
	{
	    // make a quick check to make sure that width is not dependent on left/right/xcenter
	    if(width.depends_on(left) || width.depends_on(right) || width.depends_on(center))
	    {
		warning::print("Width/height depends on other values, although it is fixed. This should never happen...");
		return;
	    }
	    
	    switch(todo)
	    {
	    case LeftBottom: right = !left + !width;  center=0.5*(!left+!right); right_modified=true; break;
	    case RightTop:   left  = !right - !width; center=0.5*(!left+!right); left_modified =true; break;
	    case Center:     left  = !center - 0.5*!width; right = !center + 0.5*!width; right_modified=true; left_modified=true; break;
	    default: warning::print("This should never happen","box::set_"); break;
	    }
	}

	// otherwise take into account the history, i.e. if the last action was for example
	// setting left, and now we are setting right, then center and width must be
	// updated accordingly. 
	else
	{
	    history_values h1=history[1], h2=todo;
	    if(h1>h2) swap(h1,h2);
	    
	    if(h1 == LeftBottom && h2 == RightTop)
	    {
		width = !right - !left;
		center = 0.5*(!left + !right);
	    }
	    else if(h1 == LeftBottom && h2 == WidthHeight)
	    {
		right = !left + !width;
		center = !left + 0.5*!width;
		right_modified = true;
	    }
	    else if(h1 == LeftBottom && h2 == Center)
	    {
		width = 2*(!center - !left);
		right = 2*!center - !left;
		right_modified = true;
	    }
	    else if(h1 == RightTop && h2 == WidthHeight)
	    {
		left = !right - !width;
		center = !right - 0.5*!width;
		left_modified = true;
	    }
	    else if(h1 == RightTop && h2 == Center)
	    {
		width = 2*(!right - !center);
		left = 2*!center - !right;
		left_modified = true;
	    }
	    else if(h1 == WidthHeight && h2 == Center)
	    {
		left = !center - 0.5*!width;
		right = !center + 0.5*!width;
		left_modified = right_modified = true;
	    }
	    else
	    {
		err("h1 and h2 has impossible values");
	    }
	}

	history[0] = history[1];
	history[1] = todo;
    }

    void box::prepare_for_draw()
    {
//	left_.set_scale(x_unit());
//	right_.set_scale(x_unit());
//	width_.set_scale(x_unit());
//	xcenter_.set_scale(x_unit());
//	bottom_.set_scale(y_unit());
//	top_.set_scale(y_unit());
//	height_.set_scale(y_unit());
//	ycenter_.set_scale(y_unit());
	left_.register_me();
	right_.register_me();
	bottom_.register_me();
	top_.register_me();
    }

    box::box() : left_(0.0), right_(CM), bottom_(0.0), top_(1.0)
    {
	name("box");
	width_fixed_ = false;
	height_fixed_ = false;
	width_ = !right_ - !left_;
	xcenter_ = 0.5*(!left_ + !right_);
	height_ = !top_ - !bottom_;
	ycenter_ = 0.5*(!bottom_ + !top_);
	xhistory_[0] = RightTop;
	xhistory_[1] = LeftBottom;
	yhistory_[0] = RightTop;
	yhistory_[1] = LeftBottom;

	left_.set_scale(x_unit());
	right_.set_scale(x_unit());
	width_.set_scale(x_unit());
	xcenter_.set_scale(x_unit());
	bottom_.set_scale(y_unit());
	top_.set_scale(y_unit());
	height_.set_scale(y_unit());
	ycenter_.set_scale(y_unit());
    }

    void box::set_left_(const length &l, bool &left_modified, bool &right_modified)
    {
	modified_ = true;
	set_(l,left_,right_,width_,xcenter_,width_fixed_,xhistory_,LeftBottom,
	     left_modified,right_modified);
	left_.set_scale(x_unit());
    }
    box &box::left(const length &l)
    {
	bool b;
	set_left_(l,b,b);
	return *this;
    }

    void box::set_right_(const length &l, bool &left_modified, bool &right_modified)
    {
	modified_ = true;
	set_(l,left_,right_,width_,xcenter_,width_fixed_,xhistory_,RightTop,
	     left_modified,right_modified);
	right_.set_scale(x_unit());
    }
    box &box::right(const length &l)
    {
	bool b;
	set_right_(l,b,b);
	return *this;
    }


    void box::set_width_(const length &l, bool &left_modified, bool &right_modified)
    {
	modified_ = true;
	set_(l,left_,right_,width_,xcenter_,width_fixed_,xhistory_,WidthHeight,
	     left_modified,right_modified);
	width_.set_scale(x_unit());
    }
    box &box::width(const length &l)
    {
	if(width_fixed_)
	{
	    warning::print("The width of this box is fixed","box::width(const length &)");
	    return *this;
	}
	bool b;
	set_width_(l,b,b);
	return *this;
    }


    void box::set_xcenter_(const length &l, bool &left_modified, bool &right_modified)
    {
	modified_ = true;
	set_(l,left_,right_,width_,xcenter_,width_fixed_,xhistory_,Center,
	     left_modified,right_modified);
	xcenter_.set_scale(x_unit());
    }
    box &box::xcenter(const length &l)
    {
	bool b;
	set_xcenter_(l,b,b);
	return *this;
    }

    void box::set_bottom_(const length &l, bool &bottom_modified, bool &top_modified)
    {
	modified_ = true;
	set_(l,bottom_,top_,height_,ycenter_,height_fixed_,yhistory_,LeftBottom,
	     bottom_modified,top_modified);
	bottom_.set_scale(y_unit());
    }
    box &box::bottom(const length &l)
    {
	bool b;
	set_bottom_(l,b,b);
	return *this;
    }

    void box::set_top_(const length &l, bool &bottom_modified, bool &top_modified)
    {
	modified_ = true;
	set_(l,bottom_,top_,height_,ycenter_,height_fixed_,yhistory_,RightTop,
	     bottom_modified,top_modified);
	top_.set_scale(y_unit());
    }
    box &box::top(const length &l)
    {
	bool b;
	set_top_(l,b,b);
	return *this;
    }

    void box::set_height_(const length &l, bool &bottom_modified, bool &top_modified)
    {
	modified_ = true;
	set_(l,bottom_,top_,height_,ycenter_,height_fixed_,yhistory_,WidthHeight,
	     bottom_modified,top_modified);
	height_.set_scale(y_unit());
    }
    box &box::height(const length &l)
    {
	if(height_fixed_)
	{
	    warning::print("The height of this box is fixed","box::height(const length &)");
	    return *this;
	}
	bool b;
	set_height_(l,b,b);
	return *this;
    }

    void box::set_ycenter_(const length &l, bool &bottom_modified, bool &top_modified)
    {
	modified_ = true;
	set_(l,bottom_,top_,height_,ycenter_,height_fixed_,yhistory_,Center,
	     bottom_modified,top_modified);
	ycenter_.set_scale(y_unit());
    }
    box &box::ycenter(const length &l)
    {
	bool b;
	set_ycenter_(l,b,b);
	return *this;
    }

    const length &box::left() const {return left_;}
    const length &box::right() const {return right_;}
    const length &box::width() const {return width_;}
    const length &box::xcenter() const {return xcenter_;}
    const length &box::bottom() const {return bottom_;}
    const length &box::top() const {return top_;}
    const length &box::height() const {return height_;}
    const length &box::ycenter() const {return ycenter_;}



}






