#include "mpad.h"
#include <vector>
#include "warning.h"

namespace blop
{
    bool mpad::modified() const
    {
	if(container::modified()) return true;
	for(int i=1; i<=nx_; ++i)
	{
	    for(int j=1; j<=ny_; ++j)
	    {
		if(pads_[ind(i,j)]->modified()) return true;
	    }
	}
	return false;
    }
    void mpad::modified(bool f)
    {
	container::modified(f);
	for(int i=1; i<=nx_; ++i)
	{
	    for(int j=1; j<=ny_; ++j)
	    {
		pads_[ind(i,j)]->modified(f);
	    }
	}
    }


    int mpad::default_direction_ = mpad::right_down | mpad::jump;
    length &mpad::default_gap_()
    {
	static length g = ZERO;
	return g;
    }
    void mpad::default_gap(const length &l)
    {
	default_gap_() = l;
    }

    void mpad::default_direction(int i) { default_direction_ = i; }

    mpad::~mpad()
    {
	for(int i=0; i<nx_*ny_; ++i) delete pads_[i];
	delete [] pads_;
	delete [] widths_;
	delete [] heights_;
	delete [] rwidths_;
	delete [] rheights_;
    }

    mpad::mpad(int xdim, int ydim) : mcontainer(xdim,ydim,default_direction_)
    {
	name("mpad");

	widths_ = new length[nx_];
	heights_ = new length[ny_];
	rwidths_ = new double[nx_];
	rheights_ = new double[ny_];

	for(int i=0; i<nx_; ++i) rwidths_[i] = 1;
	for(int i=0; i<ny_; ++i) rheights_[i] = 1;

	x_avail_ = !cwidth();
	y_avail_ = !cheight();
	gap_     = default_gap_();

	pads_ = new pad*[nx_ * ny_];

	left(ZERO);
	right(!parent_cwidth_);
	bottom(ZERO);
	top(!parent_cheight_);

	lmargin(ZERO);
	rmargin(ZERO);
	bmargin(ZERO);
	tmargin(ZERO);

	for(int i=1; i <= nx_; ++i)
	{
	    for(int j=1; j<=ny_; ++j)
	    {
		pads_[ind(i,j)] = new pad(0.0,1.0,0.0,1.0);
	    }
	}
	set_widths();
	set_heights();
    }

    mpad::mpad(int n)
    {
	int xdim=0, ydim=0;
	mcontainer::get_gridsize_(n,xdim,ydim);
	mcontainer::init_(xdim,ydim,default_direction_);

	name("mpad");

	widths_ = new length[nx_];
	heights_ = new length[ny_];
	rwidths_ = new double[nx_];
	rheights_ = new double[ny_];

	for(int i=0; i<nx_; ++i) rwidths_[i] = 1;
	for(int i=0; i<ny_; ++i) rheights_[i] = 1;

	x_avail_ = !cwidth();
	y_avail_ = !cheight();
	gap_     = default_gap_();

	pads_ = new pad*[nx_ * ny_];

	left(ZERO);
	right(!parent_cwidth_);
	bottom(ZERO);
	top(!parent_cheight_);

	lmargin(ZERO);
	rmargin(ZERO);
	bmargin(ZERO);
	tmargin(ZERO);

	for(int i=1; i <= nx_; ++i)
	{
	    for(int j=1; j<=ny_; ++j)
	    {
		pads_[ind(i,j)] = new pad(0.0,1.0,0.0,1.0);
	    }
	}
	set_widths();
	set_heights();
    }


    void mpad::set_widths()
    {
	x_avail_ = !cwidth() - (nx_-1)*!gap_;
	double sum = 0;
	for(int i=0; i<nx_; ++i)
	{
	    if(rwidths_[i] > 0)
	    {
		sum += rwidths_[i];
	    }
	    else // width[i] is fixed
	    {
		x_avail_ -= !widths_[i];
	    }
	}

	length l = ZERO;
	for(int i=0; i<nx_; ++i)
	{
	    if(rwidths_[i] > 0) widths_[i] = rwidths_[i]/sum * !x_avail_;
	    for(int j=0; j<ny_; ++j)
	    {
		int ii = ind(i+1,j+1);
		pads_[ii]->left(l);
		pads_[ii]->right(l+!widths_[i]);
	    }
	    l += !widths_[i] + !gap_;
	}
    }

    void mpad::set_heights()
    {
	y_avail_ = !cheight() - (ny_-1)*!gap_;
	double sum = 0;
	for(int i=0; i<ny_; ++i)
	{
	    if(rheights_[i] > 0)
	    {
		sum += rheights_[i];
	    }
	    else // height[i] is fixed
	    {
		y_avail_ -= !heights_[i];
	    }
	}

	length l = ZERO;
	for(int i=0; i<ny_; ++i)
	{
	    if(rheights_[i] > 0) heights_[i] = rheights_[i]/sum * !y_avail_;
	    for(int j=0; j<nx_; ++j)
	    {
		int ii = ind(j+1,i+1);
		pads_[ii]->bottom(l);
		pads_[ii]->top(l+!heights_[i]);
	    }
	    l += !heights_[i] + !gap_;
	}
    }

    mpad &mpad::width(int i, const length &l)
    {
	if(i <= 0 || nx_ < i)
	{
	    warning::print(var("Index [") & i & var("] is out of range"),
			   "mpad::width");
	    return *this;
	}
	--i;
	rwidths_[i] = -1;
	widths_[i] = l;
	set_widths();
	return *this;
    }
    mpad &mpad::rwidth(int i, double d)
    {
	if(i <= 0 || nx_ < i)
	{
	    warning::print(var("Index [") & i & var("] is out of range"),
			   "mpad::rwidth");
	    return *this;
	}
	--i;
	rwidths_[i] = d;
	widths_[i] = ZERO;
	set_widths();
	return *this;
    }
    mpad &mpad::height(int i, const length &l)
    {
	if(i <= 0 || ny_ < i)
	{
	    warning::print(var("Index [") & i & var("] is out of range"),
			   "mpad::height");
	    return *this;
	}
	--i;
	rheights_[i] = -1;
	heights_[i] = l;
	set_heights();
	return *this;
    }
    mpad &mpad::rheight(int i, double d)
    {
	if(i <= 0 || ny_ < i)
	{
	    warning::print(var("Index [") & i & var("] is out of range"),
			   "mpad::rheight");
	    return *this;
	}
	--i;
	rheights_[i] = d;
	heights_[i] = ZERO;
	set_heights();
	return *this;
    }



    mpad &mpad::gap(const length &l)
    {
	gap_ = l;
	set_widths();
	set_heights();
	return *this;
    }

    mpad &mpad::draw_border(bool b)
    {
	for(int i=0; i<nx_*ny_; ++i) pads_[i]->draw_border(b);
	return *this;
    }

    mpad &mpad::bordercolor(const color &c)
    {
	for(int i=0; i<nx_*ny_; ++i) pads_[i]->bordercolor(c);
	return *this;	
    }

    mpad &mpad::borderwidth(const length &l)
    {
	for(int i=0; i<nx_*ny_; ++i) pads_[i]->borderwidth(l);
	return *this;
    }

    mpad &mpad::border_3D(bool b)
    {
	for(int i=0; i<nx_*ny_; ++i) pads_[i]->border_3D(b);
	return *this;
    }

    mpad &mpad::fill(bool b)
    {
	for(int i=0; i<nx_*ny_; ++i) pads_[i]->fill(b);
	return *this;
    }

    mpad &mpad::fillcolor(const color &c)
    {
	for(int i=0; i<nx_*ny_; ++i) pads_[i]->fillcolor(c);
	return *this;
    }

    pad *mpad::operator() (int i,int j)
    {
	return pads_[ind(i,j)];
    }
	    
    mpad &mpad::mknew(int i, int j)
    {
	mpad *p = new mpad(i,j);
	p->autodel(true);
	pad::current().add(p);

	int ix, iy;

	if( (p->direction_&right_up) || (p->direction_&right_down) || (p->direction_&down_right) || (p->direction_&up_right) ) ix = 1;
	else ix = p->nx_;

	if( (p->direction_&right_up) || (p->direction_&left_up) || (p->direction_&up_right) || (p->direction_&up_left) ) iy = 1;
	else iy = p->ny_;
	
	p->cd(ix,iy);
	return *p;
    }
    mpad &mpad::mknew(int n)
    {
	int i=0,j=0;
	mcontainer::get_gridsize_(n,i,j);
	mpad *p = new mpad(i,j);
	p->autodel(true);
	pad::current().add(p);

	int ix, iy;

	if( (p->direction_&right_up) || (p->direction_&right_down) || (p->direction_&down_right) || (p->direction_&up_right) ) ix = 1;
	else ix = p->nx_;

	if( (p->direction_&right_up) || (p->direction_&left_up) || (p->direction_&up_right) || (p->direction_&up_left) ) iy = 1;
	else iy = p->ny_;
	
	p->cd(ix,iy);
	return *p;
    }



    void mpad::prepare_for_draw()
    {
	container::prepare_for_draw();
	for(int i=0; i<nx_*ny_; ++i) pads_[i]->prepare_for_draw();
    }

    void mpad::print(terminal *term)
    {
	if(print_me_ < 2) return;
	term->set_linewidth(LW.termspecific_id());
	term->set_color(black);
	term->set_linestyle(sym::solid);

	term->subpicture_begin(terminal::coord(cleft().termspecific_id(),
					       cbottom().termspecific_id()),
			       terminal::coord(cright().termspecific_id(),
					       ctop().termspecific_id()));

	for(int i=0; i<nx_*ny_; ++i) pads_[i]->print(term);
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    content_[i]->print(term);
	}

	term->subpicture_end();
    }

}
