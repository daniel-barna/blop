#include "container.h"
#include "warning.h"
#include "exc.hh"
#include <stack>
#include <cstdio>
#include <algorithm>

namespace blop
{

    grob *container::find(const var &name)
    {
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    if(content_[i]->name() == name.str()) return content_[i];
	    if(container *c = dynamic_cast<container*>(content_[i]))
	    {
		grob *r = c->find(name);
		if(r) return r;
	    }
	}
	return 0;
    }
    bool container::find(const grob *p)
    {
        return( std::find(content_.begin(), content_.end(), p) != content_.end() );
    }

    void container::update_c_x_(bool left_modified, bool right_modified)
    {
	// if the left is modified, we need to update the lmargin or cleft.
	// if any of the last 2 operations on the lmargin was a change of left_,
	// then lmargin_ and cleft_ are set up such that they depend on left_,
	// so there is no need to do anything
	if(left_modified)
	{
	    if(lmargin_history_[0] != Left && lmargin_history_[1] != Left)
	    {
		if(lmargin_history_[1] == Cleft)
		{
		    lmargin_ = !cleft_ - !left_;
		}
		else
		{
		    bool cleft_modified, cright_modified;
		    set_cleft_(!left_ + !lmargin_, cleft_modified, cright_modified);
		}
	    }
	    if(lmargin_history_[1] != Left)
	    {
		lmargin_history_[0] = lmargin_history_[1];
		lmargin_history_[1] = Left;
	    }
	}
	if(right_modified)
	{
	    if(rmargin_history_[0] != Right && rmargin_history_[1] != Right)
	    {
		if(rmargin_history_[1] == Cright)
		{
		    rmargin_ = !right_ - !cright_;
		}
		else
		{
		    bool cleft_modified, cright_modified;
		    set_cright_(!right_ - !rmargin_, cleft_modified, cright_modified);
		}
	    }
	    if(rmargin_history_[1] != Right)
	    {
		rmargin_history_[0] = rmargin_history_[1];
		rmargin_history_[1] = Right;
	    }
	}

    }

    void container::update_c_y_(bool bottom_modified, bool top_modified)
    {
	if(bottom_modified)
	{
	    if(bmargin_history_[0] != Bottom && bmargin_history_[1] != Bottom)
	    {
		if(bmargin_history_[1] == Cbottom)
		{
		    bmargin_ = !cbottom_ - !bottom_;
		}
		else
		{
		    bool cbottom_modified, ctop_modified;
		    set_cbottom_(!bottom_ + !bmargin_, cbottom_modified, ctop_modified);
		}
	    }
	    if(bmargin_history_[1] != Bottom)
	    {
		bmargin_history_[0] = bmargin_history_[1];
		bmargin_history_[1] = Bottom;
	    }
	}
	if(top_modified)
	{
	    if(tmargin_history_[0] != Top && tmargin_history_[1] != Top)
	    {
		if(tmargin_history_[1] == Ctop)
		{
		    tmargin_ = !top_ - !ctop_;
		}
		else
		{
		    bool cbottom_modified, ctop_modified;
		    set_ctop_(!top_ - !tmargin_, cbottom_modified, ctop_modified);
		}
	    }
	    if(tmargin_history_[1] != Top)
	    {
		tmargin_history_[0] = tmargin_history_[1];
		tmargin_history_[1] = Top;
	    }
	}
    }


    void container::update_x_(bool cleft_modified, bool cright_modified)
    {
	// if the cleft is modified, we need to update the lmargin and left.
	// if any of the last 2 operations on the lmargin was a change of cleft_,
	// then lmargin_ and left_ are set up such that they depend on cleft_,
	// so there is no need to do anything
	if(cleft_modified)
	{
	    if(lmargin_history_[0] != Cleft && lmargin_history_[1] != Cleft)
	    {
		if(lmargin_history_[1] == Left)
		{
		    lmargin_ = !cleft_ - !left_;
		}
		else
		{
		    bool left_modified, right_modified;
		    set_left_(!cleft_ - !lmargin_, left_modified, right_modified);
		}
	    }
	    if(lmargin_history_[1] != Cleft)
	    {
		lmargin_history_[0] = lmargin_history_[1];
		lmargin_history_[1] = Cleft;
	    }
	}
	if(cright_modified)
	{
	    if(rmargin_history_[0] != Cright && rmargin_history_[1] != Cright)
	    {
		if(rmargin_history_[1] == Right)
		{
		    rmargin_ = !right_ - !cright_;
		}
		else
		{
		    bool left_modified, right_modified;
		    set_right_(!cright_ + !rmargin_, left_modified, right_modified);
		}
	    }
	    if(rmargin_history_[1] != Cright)
	    {
		rmargin_history_[0] = rmargin_history_[1];
		rmargin_history_[1] = Cright;
	    }
	}
	
    }

    void container::update_y_(bool cbottom_modified, bool ctop_modified)
    {
	if(cbottom_modified)
	{
	    if(bmargin_history_[0] != Cbottom && bmargin_history_[1] != Cbottom)
	    {
		if(bmargin_history_[1] == Bottom)
		{
		    bmargin_ = !cbottom_ - !bottom_;
		}
		else
		{
		    bool bottom_modified, top_modified;
		    set_bottom_(!cbottom_ - !bmargin_, bottom_modified, top_modified);
		}
	    }
	    if(bmargin_history_[1] != Cbottom)
	    {
		bmargin_history_[0] = bmargin_history_[1];
		bmargin_history_[1] = Cbottom;
	    }
	}
	if(ctop_modified)
	{
	    if(tmargin_history_[0] != Ctop && tmargin_history_[1] != Ctop)
	    {
		if(tmargin_history_[1] == Top)
		{
		    tmargin_ = !top_ - !ctop_;
		}
		else
		{
		    bool bottom_modified, top_modified;
		    set_top_(!ctop_ + !tmargin_, bottom_modified, top_modified);
		}
	    }
	    if(tmargin_history_[1] != Ctop)
	    {
		tmargin_history_[0] = tmargin_history_[1];
		tmargin_history_[1] = Ctop;
	    }
	}
    }

    void container::set_cleft_(const length &l, bool &cleft_modified, bool &cright_modified)
    {
	modified_ = true;
	box::set_(l,cleft_,cright_,cwidth_,cxcenter_,false,cxhistory_,LeftBottom,
		  cleft_modified,cright_modified);
    }
    void container::set_cright_(const length &l, bool &cleft_modified, bool &cright_modified)
    {
	modified_ = true;
	box::set_(l,cleft_,cright_,cwidth_,cxcenter_,false,cxhistory_,RightTop,
		  cleft_modified,cright_modified);
    }
    void container::set_cwidth_(const length &l, bool &cleft_modified, bool &cright_modified)
    {
	modified_ = true;
	box::set_(l,cleft_,cright_,cwidth_,cxcenter_,false,cxhistory_,WidthHeight,
		  cleft_modified,cright_modified);
    }
    void container::set_cxcenter_(const length &l, bool &cleft_modified, bool &cright_modified)
    {
	modified_ = true;
	box::set_(l,cleft_,cright_,cwidth_,cxcenter_,false,cxhistory_,Center,
		  cleft_modified,cright_modified);
    }


    void container::set_cbottom_(const length &l, bool &cbottom_modified, bool &ctop_modified)
    {
	modified_ = true;
	box::set_(l,cbottom_,ctop_,cheight_,cycenter_,false,cyhistory_,LeftBottom,
		  cbottom_modified,ctop_modified);
    }
    void container::set_ctop_(const length &l, bool &cbottom_modified, bool &ctop_modified)
    {
	modified_ = true;
	box::set_(l,cbottom_,ctop_,cheight_,cycenter_,false,cyhistory_,RightTop,
		  cbottom_modified,ctop_modified);
    }
    void container::set_cheight_(const length &l,bool &cbottom_modified, bool &ctop_modified)
    {
	modified_ = true;
	box::set_(l,cbottom_,ctop_,cheight_,cycenter_,false,cyhistory_,WidthHeight,
		  cbottom_modified,ctop_modified);
    }
    void container::set_cycenter_(const length &l,bool &cbottom_modified,bool &ctop_modified)
    {
	modified_ = true;
	box::set_(l,cbottom_,ctop_,cheight_,cycenter_,false,cyhistory_,Center,
		  cbottom_modified,ctop_modified);
    }


    container &container::left(const length &l)
    {
	bool left_modified, right_modified;
	set_left_(l,left_modified, right_modified);
	update_c_x_(left_modified, right_modified);
	return *this;
    }

    container &container::right(const length &l)
    {
	bool left_modified, right_modified;
	set_right_(l,left_modified, right_modified);
	update_c_x_(left_modified, right_modified);
	return *this;
    }

    container &container::width(const length &l)
    {
	bool left_modified, right_modified;
	set_width_(l,left_modified, right_modified);
	update_c_x_(left_modified, right_modified);
	return *this;
    }

    container &container::xcenter(const length &l)
    {
	bool left_modified, right_modified;
	set_xcenter_(l,left_modified, right_modified);
	update_c_x_(left_modified, right_modified);
	return *this;
    }



    container &container::bottom(const length &l)
    {
	bool bottom_modified, top_modified;
	set_bottom_(l,bottom_modified, top_modified);
	update_c_y_(bottom_modified, top_modified);
	return *this;
    }

    container &container::top(const length &l)
    {
	bool bottom_modified, top_modified;
	set_top_(l,bottom_modified, top_modified);
	update_c_y_(bottom_modified, top_modified);
	return *this;
    }

    container &container::height(const length &l)
    {
	bool bottom_modified, top_modified;
	set_height_(l,bottom_modified, top_modified);
	update_c_y_(bottom_modified, top_modified);
	return *this;
    }

    container &container::ycenter(const length &l)
    {
	bool bottom_modified, top_modified;
	set_ycenter_(l,bottom_modified, top_modified);
	update_c_y_(bottom_modified, top_modified);
	return *this;
    }

    // Change the container (inner) sizes

    container &container::cleft(const length &l)
    {
	if(caspect_ > 0)
	{
	    warning::print("Can not set cleft if caspect is fixed");
	    return *this;
	}
	bool cleft_modified, cright_modified;
	set_cleft_(l,cleft_modified, cright_modified);
	update_x_(cleft_modified, cright_modified);
	return *this;
    }
    container &container::cright(const length &l)
    {
	if(caspect_ > 0)
	{
	    warning::print("Can not set cright if caspect is fixed");
	    return *this;
	}
	bool cleft_modified, cright_modified;
	set_cright_(l,cleft_modified, cright_modified);
	update_x_(cleft_modified, cright_modified);
	return *this;
    }
    container &container::cwidth(const length &l)
    {
	if(caspect_ > 0)
	{
	    warning::print("Can not set cwidth if caspect is fixed");
	    return *this;
	}
	bool cleft_modified, cright_modified;
	set_cwidth_(l,cleft_modified, cright_modified);
	update_x_(cleft_modified, cright_modified);
	return *this;
    }
    container &container::cxcenter(const length &l)
    {
	if(caspect_ > 0)
	{
	    warning::print("Can not set cxcenter if caspect is fixed");
	    return *this;
	}
	bool cleft_modified, cright_modified;
	set_cxcenter_(l,cleft_modified, cright_modified);
	update_x_(cleft_modified, cright_modified);
	return *this;
    }


    container &container::cbottom(const length &l)
    {
	if(caspect_ > 0)
	{
	    warning::print("Can not set cbottom if caspect is fixed");
	    return *this;
	}
	bool cbottom_modified, ctop_modified;
	set_cbottom_(l,cbottom_modified, ctop_modified);
	update_y_(cbottom_modified, ctop_modified);
	return *this;
    }
    container &container::ctop(const length &l)
    {
	if(caspect_ > 0)
	{
	    warning::print("Can not set ctop if caspect is fixed");
	    return *this;
	}
	bool cbottom_modified, ctop_modified;
	set_ctop_(l,cbottom_modified, ctop_modified);
	update_y_(cbottom_modified, ctop_modified);
	return *this;
    }
    container &container::cheight(const length &l)
    {
	if(caspect_ > 0)
	{
	    warning::print("Can not set cheight if caspect is fixed");
	    return *this;
	}
	bool cbottom_modified, ctop_modified;
	set_cheight_(l,cbottom_modified, ctop_modified);
	update_y_(cbottom_modified, ctop_modified);
	return *this;
    }
    container &container::cycenter(const length &l)
    {
	if(caspect_ > 0)
	{
	    warning::print("Can not set cycenter if caspect is fixed");
	    return *this;
	}
	bool cbottom_modified, ctop_modified;
	set_cycenter_(l,cbottom_modified, ctop_modified);
	update_y_(cbottom_modified, ctop_modified);
	return *this;
    }

    static bool length_depends_on_any(const length &l, length* d[], int n)
    {
	for(int i=0; i<n; ++i)
	{
	    if(l.depends_on(*(d[i]))) return true;
	}
	return false;
    }

    container &container::caspect(double r)
    {
	// relax the ratio lock, if r is negative
	if(r<=0)
	{
	    caspect_ = r;
	    return *this;
	}
	

	length *d[] = {&cleft_, &cright_, &cwidth_, &cxcenter_,
		       &cbottom_, &ctop_, &cheight_, &cycenter_};
	if(length_depends_on_any(lmargin_,d,8) ||
	   length_depends_on_any(rmargin_,d,8) ||
	   length_depends_on_any(bmargin_,d,8) ||
	   length_depends_on_any(tmargin_,d,8))
	{
	    warning::print("To fix the caspect, the margins should not depends on "
			   "any of cwidth, cleft, etc...");
	    return *this;
	}

	caspect_ = r;

	/*
	length csize_for_caspect = length::min(
	    !width_ - (cx_lock_==sym::right?rmargin_:lmargin_),
	    (!height_ - (cy_lock_==sym::top?tmargin_:bmargin_))/caspect_);
	*/
	length csize_for_caspect = length::min(
	    !width_ - lmargin_ - rmargin_,
	    (!height_ - bmargin_ - tmargin_)/caspect_);

	if(cx_lock_==sym::right)
	{
	    lmargin_ = !width_ - rmargin_ - csize_for_caspect;
	    lmargin_history_[0] = Left;
	    lmargin_history_[1] = Lmargin;

	    cleft_  = !left_  + !lmargin_;
	    cright_ = !right_ - !rmargin_;
	    cwidth_ = !cright_ - !cleft_;
	    cxhistory_[0] = LeftBottom;
	    cxhistory_[1] = RightTop;

	    rmargin_history_[0] = Rmargin;
	    rmargin_history_[1] = Rmargin;
	}
	else
	{
	    rmargin_ = !width_ - lmargin_ - csize_for_caspect;
	    rmargin_history_[0] = Right;
	    rmargin_history_[1] = Rmargin;

	    cleft_  = !left_  + !lmargin_;
	    cright_ = !right_ - !rmargin_;
	    cwidth_ = !cright_ - !cleft_;
	    cxhistory_[0] = RightTop;
	    cxhistory_[1] = LeftBottom;

	    lmargin_history_[0] = Lmargin;
	    lmargin_history_[1] = Lmargin;

	}
	cxcenter_ = 0.5*(!cleft_+!cright_);

	if(cy_lock_==sym::top)
	{
	    bmargin_ = !height_ - tmargin_ - csize_for_caspect*caspect_;
	    bmargin_history_[0] = Bottom;
	    bmargin_history_[1] = Bmargin;

	    cbottom_ = !bottom_ + !bmargin_;
	    ctop_    = !top_    - !tmargin_;
	    cheight_ = !ctop_ - !cbottom_;
	    cyhistory_[0] = LeftBottom;
	    cyhistory_[1] = RightTop;

	    tmargin_history_[0] = Tmargin;
	    tmargin_history_[1] = Tmargin;
	}
	else
	{
	    tmargin_ = !height_ - bmargin_ - csize_for_caspect*caspect_;
	    tmargin_history_[0] = Top;
	    tmargin_history_[1] = Tmargin;

	    cbottom_  = !bottom_ + !bmargin_;
	    ctop_     = !top_    - !tmargin_;
	    cheight_  = !ctop_ - !cbottom_;
	    cyhistory_[0] = RightTop;
	    cyhistory_[1] = LeftBottom;

	    bmargin_history_[0] = Bmargin;
	    bmargin_history_[1] = Bmargin;

	}
	cycenter_ = 0.5*(!cbottom_ + !ctop_);

	return *this;
    }

    container &container::cxlock(sym::position a)
    {
	cerr<<"Do not call container::cxlock yet"<<endl;
	//cx_lock_ = a;
	return *this;
    }
    container &container::cylock(sym::position a)
    {
	cerr<<"Do not call container::cylock yet"<<endl;
	//cy_lock_ = a;
	return *this;
    }
    
    container &container::margins(const length &l)
    {
	lmargin(l);
	rmargin(l);
	bmargin(l);
	tmargin(l);
	return *this;
    }

    void container::set_lmargin_(const length &l)
    {
	modified_ = true;
	lmargin_ = l;
	if(lmargin_history_[0] != Lmargin && lmargin_history_[1] != Lmargin)
	{
	    bool dummy1, dummy2;
	    if(lmargin_history_[1] == container::Left)
	    {
		set_cleft_(!left_ + !lmargin_, dummy1, dummy2);
	    }
	    else if(lmargin_history_[1] == container::Cleft)
	    {
		set_left_(!cleft_ - !lmargin_, dummy1, dummy2);
	    }
	}
	if(lmargin_history_[1] != container::Lmargin)
	{
	    lmargin_history_[0] = lmargin_history_[1];
	    lmargin_history_[1] = Lmargin;
	}
    }

    container &container::lmargin(const length &l)
    {
	// if no caspect is defined, simply set the lmargin
	if(caspect_<=0)
	{
	    set_lmargin_(l);
	    return *this;
	}

	if(l.depends_on(cwidth()) ||
	   l.depends_on(cxcenter()) ||
	   l.depends_on(cleft()) ||
	   l.depends_on(cright()))
	{
	    warning::print("With a fixed caspect, lmargin can not depend on "
			   "cwidth, cxcenter, cleft or cright");
	    return *this;
	}

	length csize_for_caspect;
	if(cx_lock_==sym::right)
	{
	    csize_for_caspect = length::min(
		!width_ - rmargin_ - l,
		(!height_ - (cy_lock_==sym::top?tmargin_:bmargin_))/caspect_);
	    lmargin_ = !width_ - rmargin_ - csize_for_caspect;
	}
	else
	{
	    lmargin_ = l;
	    csize_for_caspect = length::min(
		!width_ - lmargin_ - rmargin_,
		(!height_ - (cy_lock_==sym::top?tmargin_:bmargin_))/caspect_);
	    rmargin_ = !width_ - lmargin_ - csize_for_caspect;
	}
	if(cy_lock_ == sym::top) bmargin_ = !height_ - tmargin_ - csize_for_caspect*caspect_;
	else                     tmargin_ = !height_ - bmargin_ - csize_for_caspect*caspect_;
	return *this;
    }

    void container::set_rmargin_(const length &l)
    {
	modified_ = true;
	rmargin_ = l;
	
	if(rmargin_history_[0] != Rmargin && rmargin_history_[1] != Rmargin)
	{
	    bool dummy1, dummy2;
	    if(rmargin_history_[1] == container::Right)
	    {
		set_cright_(!right_ - !rmargin_, dummy1, dummy2);
	    }
	    else if(rmargin_history_[1] == container::Cright)
	    {
		set_right_(!cright_ + !rmargin_, dummy1, dummy2);
	    }
	}
	if(rmargin_history_[1] != container::Rmargin)
	{
	    rmargin_history_[0] = rmargin_history_[1];
	    rmargin_history_[1] = Rmargin;
	}
    }

    container &container::rmargin(const length &l)
    {
	if(caspect_<=0)
	{
	    set_rmargin_(l);
	    return *this;
	}

	if(l.depends_on(cwidth()) ||
	   l.depends_on(cxcenter()) ||
	   l.depends_on(cleft()) ||
	   l.depends_on(cright()))
	{
	    warning::print("With a fixed caspect, rmargin can not depend on cwidth, cxcenter, cleft or cright");
	    return *this;
	}

	length csize_for_caspect;
	if(cx_lock_==sym::right)
	{
	    rmargin_ = l;
	    csize_for_caspect = length::min(
		!width_ - lmargin_ - rmargin_,
		(!height_ - (cy_lock_==sym::top?tmargin_:bmargin_))/caspect_);
	    lmargin_ = !width_ - rmargin_ - csize_for_caspect;
	}
	else
	{
	    csize_for_caspect = length::min(
		!width_ - lmargin_ - l,
		(!height_ - (cy_lock_==sym::top?tmargin_:bmargin_))/caspect_);
	    rmargin_ = !width_ - lmargin_ - csize_for_caspect;
	}
	if(cy_lock_ == sym::top) bmargin_ = !height_ - tmargin_ - csize_for_caspect*caspect_;
	else                     tmargin_ = !height_ - bmargin_ - csize_for_caspect*caspect_;

	return *this;
    }

    void container::set_bmargin_(const length &l)
    {
	modified_ = true;
	bmargin_ = l;
	if(bmargin_history_[0] != Bmargin && bmargin_history_[1] != Bmargin)
	{
	    bool dummy1, dummy2;
	    if(bmargin_history_[1] == container::Bottom)
	    {
		set_cbottom_(!bottom_ + !bmargin_, dummy1, dummy2);
	    }
	    else if(bmargin_history_[1] == container::Cbottom)
	    {
		set_bottom_(!cbottom_ - !bmargin_, dummy1, dummy2);
	    }
	}
	if(bmargin_history_[1] != container::Bmargin)
	{
	    bmargin_history_[0] = bmargin_history_[1];
	    bmargin_history_[1] = Bmargin;
	}
    }
    container &container::bmargin(const length &l)
    {
	if(caspect_<=0)
	{
	    set_bmargin_(l);
	    return *this;
	}

	if(l.depends_on(cheight()) ||
	   l.depends_on(cycenter()) ||
	   l.depends_on(cbottom()) ||
	   l.depends_on(ctop()))
	{
	    warning::print("With a fixed caspect, bmargin can not depend on cwidth, cxcenter, cleft or cright");
	    return *this;
	}

	length csize_for_caspect;
	if(cx_lock_==sym::top)
	{
	    csize_for_caspect = length::min(
		!width_ - (cx_lock_==sym::right?rmargin_:lmargin_),
		(!height_ - tmargin_ - l)/caspect_);
	    bmargin_ = !height_ - tmargin_ - csize_for_caspect*caspect_;
	}
	else
	{
	    bmargin_ = l;
	    csize_for_caspect = length::min(
		!width_ - (cx_lock_==sym::right?rmargin_:lmargin_),
		(!height_ - bmargin_ - tmargin_)/caspect_);
	    tmargin_ = !height_ - bmargin_ - csize_for_caspect*caspect_;
	}
	if(cx_lock_ == sym::right) lmargin_ = !width_ - rmargin_ - csize_for_caspect;
	else                       rmargin_ = !width_ - lmargin_ - csize_for_caspect;

	return *this;

    }

    void container::set_tmargin_(const length &l)
    {
	modified_ = true;
	tmargin_ = l;
	if(tmargin_history_[0] != Tmargin && tmargin_history_[1] != Tmargin)
	{
	    bool dummy1, dummy2;
	    if(tmargin_history_[1] == container::Top)
	    {
		set_ctop_(!top_ - !tmargin_, dummy1, dummy2);
	    }
	    else if(tmargin_history_[1] == container::Ctop)
	    {
		set_top_(!ctop_ + !tmargin_, dummy1, dummy2);
	    }
	}
	if(tmargin_history_[1] != container::Tmargin)
	{
	    tmargin_history_[0] = tmargin_history_[1];
	    tmargin_history_[1] = Tmargin;
	}
    }
    container &container::tmargin(const length &l)
    {
	if(caspect_<=0)
	{
	    set_tmargin_(l);
	    return *this;
	}

	if(l.depends_on(cheight()) ||
	   l.depends_on(cycenter()) ||
	   l.depends_on(cbottom()) ||
	   l.depends_on(ctop()))
	{
	    warning::print("With a fixed caspect, tmargin can not depend on cwidth, cxcenter, cleft or cright");
	    return *this;
	}

	length csize_for_caspect;
	if(cx_lock_==sym::top)
	{
	    tmargin_ = l;
	    csize_for_caspect = length::min(
		!width_ - (cx_lock_==sym::right?rmargin_:lmargin_),
		(!height_ - bmargin_ - tmargin_)/caspect_);
	    bmargin_ = !height_ - tmargin_ - csize_for_caspect*caspect_;
	}
	else
	{
	    csize_for_caspect = length::min(
		!width_ - (cx_lock_==sym::right?rmargin_:lmargin_),
		(!height_ - bmargin_ - l)/caspect_);
	    tmargin_ = !height_ - bmargin_ - csize_for_caspect*caspect_;
	}
	if(cx_lock_ == sym::right) lmargin_ = !width_ - rmargin_ - csize_for_caspect;
	else                       rmargin_ = !width_ - lmargin_ - csize_for_caspect;

	return *this;
    }





    bool container::modified() const
    {
	if(modified_) return true;
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    if(content_[i]->modified()) return true;
	}
	return false;
    }

    void container::modified(bool f)
    {
	modified_ = f;
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    content_[i]->modified(f);
	}
    }

    void container::cd()
    {
	std::stack<container *> s;
	for(container *p = this; p; p = p->parent())
	{
	    s.push(p);
	}
	for(; !s.empty(); s.pop())
	{
	    s.top()->cd_specific();
	}
    }

    const length &container::cleft() const {return cleft_;}
    const length &container::cright() const {return cright_;}
    const length &container::cwidth() const {return cwidth_;}
    const length &container::cxcenter() const { return cxcenter_; }
    const length &container::cbottom() const {return cbottom_;}
    const length &container::ctop() const {return ctop_;}
    const length &container::cheight() const {return cheight_;}
    const length &container::cycenter() const { return cycenter_; }

    void container::prepare_for_draw()
    {
	if(print_me_ < 1) return;
	box::prepare_for_draw();

	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    content_[i]->prepare_for_draw();
	}

	cleft_.set_scale(x_unit());
	cright_.set_scale(x_unit());
	cwidth_.set_scale(x_unit());
	cbottom_.set_scale(y_unit());
	ctop_.set_scale(y_unit());
	cheight_.set_scale(y_unit());

	cleft_.register_me();
	cright_.register_me();
	cbottom_.register_me();
	ctop_.register_me();
    }

    void container::print(terminal *term)
    {
	if(print_me_<2) return;
	term->subpicture_begin(terminal::coord(cleft_.termspecific_id(),
					       cbottom_.termspecific_id()),
			       terminal::coord(cright_.termspecific_id(),
					       ctop_.termspecific_id()));
	term->set_color(black);
	for(unsigned int i=0; i<content_.size(); ++i) content_[i]->print(term);
	term->subpicture_end();
    }

    std::vector<blop::container *> &container::all()
    {
	static std::vector<blop::container*> *f= new std::vector<blop::container*>;
	return *f;
    }

    container::container()
    {
	name("container");
	all().push_back(this);

	caspect_ = -1;
	cx_lock_ = sym::left;
	cy_lock_ = sym::bottom;

	// set up box dimensions
	box::right(1.0);
	box::left(0.0);
	box::top(1.0);
	box::bottom(0.0);

	lmargin_ = 0.0;
	cleft_ = !left_ + !lmargin_;
	lmargin_history_[0] = container::Lmargin;
	lmargin_history_[1] = container::Left;

	rmargin_ = 0;
	cright_ = !right_ - !rmargin_;
	rmargin_history_[0] = container::Rmargin;
	rmargin_history_[1] = container::Right;

	bmargin_ = 0;
	cbottom_ = !bottom_ + !bmargin_;
	bmargin_history_[0] = container::Bmargin;
	bmargin_history_[1] = container::Bottom;

	tmargin_ = 0;
	ctop_ = !top_ - !tmargin_;
	bmargin_history_[0] = container::Bmargin;
	bmargin_history_[1] = container::Bottom;

	cwidth_  = !cright_ - !cleft_;
	cxcenter_ = 0.5*(!cleft_+!cright_);
	cheight_ = !ctop_ - !cbottom_;
	cycenter_ = 0.5*(!cbottom_+!ctop_);

	cxhistory_[0] = RightTop;
	cxhistory_[1] = LeftBottom;
	cyhistory_[0] = RightTop;
	cyhistory_[1] = LeftBottom;

    }

    container::~container()
    {
	vector<container *>::iterator i =
	    std::find(all().begin(), all().end(), this);

	if(i == all().end())
	    err("Unregistered container (programming error)");

	all().erase(i);

	clear();
    }

    void container::remove_from_all(grob *g)
    {
	for(unsigned int i=0; i<all().size(); ++i)
	{
	    all()[i] -> remove(g);
	}
    }

    bool container::remove(grob *g)
    {
	vector<grob *>::iterator i =
	    std::find(content_.begin(), content_.end(), g);
	if(i == content_.end()) return false;
	g->parent(0);
	content_.erase(i);
	modified_ = true;
	return true;
    }

    void container::clear()
    {
	// first copy the pointers to a temporary vector
	vector<grob*> content = content_;

	// then clear the content_ vector
	content_.clear();

	// then delete the autodel=true objects (they will try to
	// remove themselves from the content_ vector, but this has
	// already been cleared, so no problem)
	for(unsigned int i=0; i<content.size(); ++i)
	{
	    if(content[i]->autodel()) delete content[i];
	}

	modified(true);
    }

    void container::add(grob *g)
    {
	if(g == 0) return;
	if(g->parent(this)) content_.push_back(g);
	modified_ = true;
    }

    void container::print_hierarchy(int level)
    {
	for(int i=0; i<level*4; ++i) cout<<" ";
	cout<<name()<<endl;
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    content_[i]->print_hierarchy(level+1);
	}
    }


    bool container::empty() const
    {
	return content_.empty();
    }
}
