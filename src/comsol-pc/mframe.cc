#include "mframe.h"
#include "warning.h"
#include "epad.h"
#include <vector>

namespace blop
{
    frame *mframe::operator()(int i, int j) { return frames_[ind(i,j)]; }
    const frame *mframe::operator()(int i, int j) const { return frames_[ind(i,j)]; }
    bool mframe::default_remove_conflicting_tic_ = true;

    int mframe::row(const frame *f) const
    {
        for(int nx=1; nx<=nx_; ++nx)
        {
            for(int ny=1; ny<=ny_; ++ny)
            {
                if((*this)(nx,ny) == f) return ny;
            }
        }
        return 0;
    }
    int mframe::col(const frame *f) const
    {
        for(int nx=1; nx<=nx_; ++nx)
        {
            for(int ny=1; ny<=ny_; ++ny)
            {
                if((*this)(nx,ny) == f) return nx;
            }
        }
        return 0;
    }

    bool mframe::modified() const
    {
	if(container::modified()) return true;
	for(int i=1; i<=nx_; ++i)
	{
	    for(int j=1; j<=ny_; ++j)
	    {
		if(frames_[ind(i,j)]->modified()) return true;
	    }
	}
	return false;
    }
    void mframe::modified(bool f)
    {
	container::modified(f);
	for(int i=1; i<=nx_; ++i)
	{
	    for(int j=1; j<=ny_; ++j)
	    {
		frames_[ind(i,j)]->modified(f);
	    }
	}
    }

    int mframe::default_direction_ = mframe::right_down | mframe::jump;

    void mframe::default_direction(int i) { default_direction_ = i; }


    mframe::~mframe()
    {
	for(int i=0; i<nx_*ny_; ++i) delete frames_[i];
	delete [] frames_;

        delete [] show_subframe_;

	delete [] widths_;
	delete [] heights_;
	delete [] rwidths_;
	delete [] rheights_;

	for(int i=0; i<nx_; ++i)
	{
	    delete x1axes_[i];
	    delete x2axes_[i];
	    delete bmarginboxes_[i];
	    delete tmarginboxes_[i];
	}
	for(int i=0; i<ny_; ++i)
	{
	    delete y1axes_[i];
	    delete y2axes_[i];
	    delete lmarginboxes_[i];
	    delete rmarginboxes_[i];
	}
	delete [] x1axes_;
	delete [] x2axes_;
	delete [] y1axes_;
	delete [] y2axes_;
	delete [] lmarginboxes_;
	delete [] rmarginboxes_;
	delete [] bmarginboxes_;
	delete [] tmarginboxes_;
        delete title_;
    }


    bool mframe::parent(container *c)
    {
	if(c == 0) return false;
	base::parent(c);
	box::right(!(c->cwidth()));
	box::top  (!(c->cheight()));
	return true;
    }

    mframe::mframe(int xdim, int ydim) : mcontainer(xdim,ydim,default_direction_), equal_ranges_(0)
    {
	name("mframe");
        title_ = 0;
	frames_ = new frame*[nx_ * ny_];

        show_subframe_ = new bool[nx_*ny_];
        for(int i=0; i<nx_*ny_; ++i) show_subframe_[i] = true;
        
	x1axes_ = new axis*[nx_];
	x2axes_ = new axis*[nx_];
	y1axes_ = new axis*[ny_];
	y2axes_ = new axis*[ny_];

	lmarginboxes_ = new epad*[ny_];
	rmarginboxes_ = new epad*[ny_];
	bmarginboxes_ = new epad*[nx_];
	tmarginboxes_ = new epad*[nx_];

	widths_ = new length[nx_];
	heights_ = new length[ny_];
	rwidths_ = new double[nx_];
	rheights_ = new double[ny_];
	for(int i=0; i<nx_; ++i) rwidths_[i] = 1;
	for(int i=0; i<ny_; ++i) rheights_[i] = 1;
	x_avail_ = !cwidth();
	y_avail_ = !cheight();
	xgap_ = ZERO;
        ygap_ = ZERO;

	vector<length> x1m,x2m,y1m,y2m, lmb, rmb, bmb, tmb;
	//x1m.push_back(blop::height("X"));
	//x2m.push_back(blop::height("X"));
	//y1m.push_back(0.5*EX);
	//y2m.push_back(0.5*EX);

	// create the subframes
	for(int a=1; a<=nx_; ++a)
	{
	    for(int b=1; b<=ny_; ++b)
	    {
		frames_[ind(a,b)] = new frame;
		var name=var("frame(") & a & var(",") & b & var(")");
		frames_[ind(a,b)]->name(name);
	    }
	}

	for(int a=0; a<nx_; ++a)
	{
	    x1axes_[a] = new axis(axis::x1);
	    x1m.push_back(!x1axes_[a]->scriptsize());
	    x2axes_[a] = new axis(axis::x2);
	    x2m.push_back(!x2axes_[a]->scriptsize());

	    bmarginboxes_[a] = new epad;
	    bmarginboxes_[a]->name(var("bmarginbox(") & (a+1) & var(")"));
	    bmarginboxes_[a]->borderwidth(0.0);
	    add(bmarginboxes_[a]);
	    tmarginboxes_[a] = new epad;
	    tmarginboxes_[a]->name(var("tmarginbox(") & (a+1) & var(")"));
	    tmarginboxes_[a]->borderwidth(0.0);
	    add(tmarginboxes_[a]);

	    bmb.push_back(!bmarginboxes_[a]->height());
	    tmb.push_back(!tmarginboxes_[a]->height());
	}
	for(int a=0; a<ny_; ++a)
	{
	    y1axes_[a] = new axis(axis::y1);
	    y1m.push_back(!y1axes_[a]->scriptsize());
	    y2axes_[a] = new axis(axis::y2);
	    y2m.push_back(!y2axes_[a]->scriptsize());
	    lmarginboxes_[a] = new epad;
	    lmarginboxes_[a]->name(var("lmarginbox(") & (a+1) & var(")"));
	    lmarginboxes_[a]->borderwidth(0.0);
	    add(lmarginboxes_[a]);
	    rmarginboxes_[a] = new epad;
	    rmarginboxes_[a]->name(var("rmarginbox(") & (a+1) & var(")"));
	    rmarginboxes_[a]->borderwidth(0.0);
	    add(rmarginboxes_[a]);
	    lmb.push_back(!lmarginboxes_[a]->width());
	    rmb.push_back(!rmarginboxes_[a]->width());
	}

	left(0.0);
	right(10*CM);
	bottom(0.0);
	top(10*CM);

	bmargin(length::max(x1m) + blop::height("X") + length::max(bmb));
	tmargin(length::max(x2m) + blop::height("X") + length::max(tmb));
	lmargin(length::max(y1m) + 0.5*EX + length::max(lmb));
	rmargin(length::max(y2m) + 0.5*EX + length::max(rmb));

	for(int a=1; a<=nx_; ++a)
	{
	    for(int b=1; b<=ny_; ++b)
	    {
		frames_[ind(a,b)]->x1axis(x1axes_[a-1]);
		frames_[ind(a,b)]->x2axis(x2axes_[a-1]);
		frames_[ind(a,b)]->y1axis(y1axes_[b-1]);
		frames_[ind(a,b)]->y2axis(y2axes_[b-1]);

		frames_[ind(a,b)]->lmargin((a-1.0)/nx_ * !cwidth());
		frames_[ind(a,b)]->rmargin((nx_-(double)a)/nx_ * !cwidth());
		frames_[ind(a,b)]->bmargin((b-1.0)/ny_ * !cheight());
		frames_[ind(a,b)]->tmargin((ny_-(double)b)/ny_ * !cheight());

		frames_[ind(a,b)]->lmarginbox(lmarginboxes_[b-1]);
		frames_[ind(a,b)]->rmarginbox(rmarginboxes_[b-1]);
		frames_[ind(a,b)]->bmarginbox(bmarginboxes_[a-1]);
		frames_[ind(a,b)]->tmarginbox(tmarginboxes_[a-1]);
		
		frames_[ind(a,b)]->parent(this);
	    }
	}
	set_widths();
	set_heights();
        remove_conflicting_tic_ = default_remove_conflicting_tic_;
    }

    mframe::mframe(int n) : equal_ranges_(0)
    {
	int xdim=0, ydim=0;
	mcontainer::get_gridsize_(n,xdim,ydim);
	mcontainer::init_(xdim,ydim,default_direction_);

	name("mframe");
        title_ = 0;
	frames_ = new frame*[nx_ * ny_];

        show_subframe_ = new bool[nx_*ny_];
        for(int i=0; i<nx_*ny_; ++i) show_subframe_[i] = true;

	x1axes_ = new axis*[nx_];
	x2axes_ = new axis*[nx_];
	y1axes_ = new axis*[ny_];
	y2axes_ = new axis*[ny_];

	lmarginboxes_ = new epad*[ny_];
	rmarginboxes_ = new epad*[ny_];
	bmarginboxes_ = new epad*[nx_];
	tmarginboxes_ = new epad*[nx_];

	widths_ = new length[nx_];
	heights_ = new length[ny_];
	rwidths_ = new double[nx_];
	rheights_ = new double[ny_];
	for(int i=0; i<nx_; ++i) rwidths_[i] = 1;
	for(int i=0; i<ny_; ++i) rheights_[i] = 1;
	x_avail_ = !cwidth();
	y_avail_ = !cheight();
	xgap_ = ZERO;
	ygap_ = ZERO;

	vector<length> x1m,x2m,y1m,y2m, lmb, rmb, bmb, tmb;
	//x1m.push_back(blop::height("X"));
	//x2m.push_back(blop::height("X"));
	//y1m.push_back(0.5*EX);
	//y2m.push_back(0.5*EX);

	// create the subframes
	for(int a=1; a<=nx_; ++a)
	{
	    for(int b=1; b<=ny_; ++b)
	    {
		frames_[ind(a,b)] = new frame;
		var name=var("frame(") & a & var(",") & b & var(")");
		frames_[ind(a,b)]->name(name);
	    }
	}

	for(int a=0; a<nx_; ++a)
	{
	    x1axes_[a] = new axis(axis::x1);
	    x1m.push_back(!x1axes_[a]->scriptsize());
	    x2axes_[a] = new axis(axis::x2);
	    x2m.push_back(!x2axes_[a]->scriptsize());

	    bmarginboxes_[a] = new epad;
	    bmarginboxes_[a]->name(var("bmarginbox(") & (a+1) & var(")"));
	    bmarginboxes_[a]->borderwidth(0.0);
	    add(bmarginboxes_[a]);
	    tmarginboxes_[a] = new epad;
	    tmarginboxes_[a]->name(var("tmarginbox(") & (a+1) & var(")"));
	    tmarginboxes_[a]->borderwidth(0.0);
	    add(tmarginboxes_[a]);

	    bmb.push_back(!bmarginboxes_[a]->height());
	    tmb.push_back(!tmarginboxes_[a]->height());
	}
	for(int a=0; a<ny_; ++a)
	{
	    y1axes_[a] = new axis(axis::y1);
	    y1m.push_back(!y1axes_[a]->scriptsize());
	    y2axes_[a] = new axis(axis::y2);
	    y2m.push_back(!y2axes_[a]->scriptsize());
	    lmarginboxes_[a] = new epad;
	    lmarginboxes_[a]->name(var("lmarginbox(") & (a+1) & var(")"));
	    lmarginboxes_[a]->borderwidth(0.0);
	    add(lmarginboxes_[a]);
	    rmarginboxes_[a] = new epad;
	    rmarginboxes_[a]->name(var("rmarginbox(") & (a+1) & var(")"));
	    rmarginboxes_[a]->borderwidth(0.0);
	    add(rmarginboxes_[a]);
	    lmb.push_back(!lmarginboxes_[a]->width());
	    rmb.push_back(!rmarginboxes_[a]->width());
	}

	left(0.0);
	right(10*CM);
	bottom(0.0);
	top(10*CM);

	bmargin(length::max(x1m) + blop::height("X") + length::max(bmb));
	tmargin(length::max(x2m) + blop::height("X") + length::max(tmb));
	lmargin(length::max(y1m) + 0.5*EX + length::max(lmb));
	rmargin(length::max(y2m) + 0.5*EX + length::max(rmb));

	for(int a=1; a<=nx_; ++a)
	{
	    for(int b=1; b<=ny_; ++b)
	    {
		frames_[ind(a,b)]->x1axis(x1axes_[a-1]);
		frames_[ind(a,b)]->x2axis(x2axes_[a-1]);
		frames_[ind(a,b)]->y1axis(y1axes_[b-1]);
		frames_[ind(a,b)]->y2axis(y2axes_[b-1]);

		frames_[ind(a,b)]->lmargin((a-1.0)/nx_ * !cwidth());
		frames_[ind(a,b)]->rmargin((nx_-(double)a)/nx_ * !cwidth());
		frames_[ind(a,b)]->bmargin((b-1.0)/ny_ * !cheight());
		frames_[ind(a,b)]->tmargin((ny_-(double)b)/ny_ * !cheight());

		frames_[ind(a,b)]->lmarginbox(lmarginboxes_[b-1]);
		frames_[ind(a,b)]->rmarginbox(rmarginboxes_[b-1]);
		frames_[ind(a,b)]->bmarginbox(bmarginboxes_[a-1]);
		frames_[ind(a,b)]->tmarginbox(tmarginboxes_[a-1]);
		
		frames_[ind(a,b)]->parent(this);
	    }
	}
	set_widths();
	set_heights();
    }

    mframe &mframe::title(const var &t)
    {
        // if an empty string, clear the title
        if(t.str() == "")
        {
            if(title_) delete title_;
            title_ = 0;
            return *this;
        }

        if(!title_)
        {
            title_ = new label(t);
            add(title_);
        }
        else
        {
            title_->text(t);
        }
        title_->x(0.5,sym::center).y(1.0+0.5*EM,sym::bottom);
        tmargin(!title_->height()+EM);

        return *this;
    }
    

    mframe &mframe::show_subframe(int i, int j, bool f)
    {
        show_subframe_[ind(i,j)] = f;
        return *this;
    }
    bool mframe::show_subframe(int i, int j) const
    {
        return show_subframe_[ind(i,j)];
    }

    mframe &mframe::mknew(int i, int j)
    {
	mframe *f = new mframe(i,j);
	f->autodel(true);
	pad::current().add(f);

	int ix, iy;

	if( (f->direction_&right_up) || (f->direction_&right_down) || (f->direction_&down_right) || (f->direction_&up_right) ) ix = 1;
	else ix = f->nx_;

	if( (f->direction_&right_up) || (f->direction_&left_up) || (f->direction_&up_right) || (f->direction_&up_left) ) iy = 1;
	else iy = f->ny_;

	f->cd(ix,iy);
	//f->frames_[0]->cd();
	return *f;
    }

    mframe &mframe::mknew(int n)
    {
	int i=0,j=0;
	mcontainer::get_gridsize_(n,i,j);
	mframe *f = new mframe(i,j);
	f->autodel(true);
	pad::current().add(f);

	int ix, iy;

	if( (f->direction_&right_up) || (f->direction_&right_down) || (f->direction_&down_right) || (f->direction_&up_right) ) ix = 1;
	else ix = f->nx_;

	if( (f->direction_&right_up) || (f->direction_&left_up) || (f->direction_&up_right) || (f->direction_&up_left) ) iy = 1;
	else iy = f->ny_;

	f->cd(ix,iy);
	//f->frames_[0]->cd();
	return *f;
    }


    void mframe::prepare_for_draw()
    {
	container::prepare_for_draw();

        double x1min=unset, x1max=unset, y1min=unset, y1max=unset, x2min=unset, x2max=unset, y2min=unset, y2max=unset;
	for(int i=0; i<nx_*ny_; ++i)
        {
            frames_[i]->prepare_for_draw();
            if(frames_[i]->x1axis() && frames_[i]->x1axis()->min()!=unset && (i==0 || frames_[i]->x1axis()->min()<x1min)) x1min = frames_[i]->x1axis()->min();
            if(frames_[i]->x1axis() && frames_[i]->x1axis()->max()!=unset && (i==0 || frames_[i]->x1axis()->max()>x1max)) x1max = frames_[i]->x1axis()->max();
            if(frames_[i]->y1axis() && frames_[i]->y1axis()->min()!=unset && (i==0 || frames_[i]->y1axis()->min()<y1min)) y1min = frames_[i]->y1axis()->min();
            if(frames_[i]->y1axis() && frames_[i]->y1axis()->max()!=unset && (i==0 || frames_[i]->y1axis()->max()>y1max)) y1max = frames_[i]->y1axis()->max();
            if(frames_[i]->x2axis() && frames_[i]->x2axis()->min()!=unset && (i==0 || frames_[i]->x2axis()->min()<x2min)) x2min = frames_[i]->x2axis()->min();
            if(frames_[i]->x2axis() && frames_[i]->x2axis()->max()!=unset && (i==0 || frames_[i]->x2axis()->max()>x2max)) x2max = frames_[i]->x2axis()->max();
            if(frames_[i]->y2axis() && frames_[i]->y2axis()->min()!=unset && (i==0 || frames_[i]->y2axis()->min()<y2min)) y2min = frames_[i]->y2axis()->min();
            if(frames_[i]->y2axis() && frames_[i]->y2axis()->max()!=unset && (i==0 || frames_[i]->y2axis()->max()>y2max)) y2max = frames_[i]->y2axis()->max();
        }

        if(equal_ranges_)
        {
            for(int i=0; i<nx_*ny_; ++i)
            {
                if(equal_ranges_&axis::x1 && frames_[i]->x1axis())
                {
                    if(x1min!=unset) frames_[i]->x1axis()->extend_range(x1min);
                    if(x1max!=unset) frames_[i]->x1axis()->extend_range(x1max);
                }
                if(equal_ranges_&axis::y1 && frames_[i]->y1axis())
                {
                    if(y1min!=unset) frames_[i]->y1axis()->extend_range(y1min);
                    if(y1max!=unset) frames_[i]->y1axis()->extend_range(y1max);
                }
                if(equal_ranges_&axis::x2 && frames_[i]->x2axis())
                {
                    if(x2min!=unset) frames_[i]->x2axis()->extend_range(x2min);
                    if(x2max!=unset) frames_[i]->x2axis()->extend_range(x2max);
                }
                if(equal_ranges_&axis::y2 && frames_[i]->y2axis())
                {
                    if(y2min!=unset) frames_[i]->y2axis()->extend_range(y2min);
                    if(y2max!=unset) frames_[i]->y2axis()->extend_range(y2max);
                }
            }
        }
        
	for(int i=0; i<nx_; ++i)
	{
	    x1axes_[i]->calculate_tics();
	    x1axes_[i]->prepare_for_draw();
            if(remove_conflicting_tic_)
            {
                for(unsigned int t=0; t<x1axes_[i]->tics_.size(); ++t)
                {
                    const double m = x1axes_[i]->map_point(x1axes_[i]->tics_[t].value());
                    if(i<nx_-1 && ::fabs(m-1) < 0.01)
                    {
                        x1axes_[i]->tics_.erase(x1axes_[i]->tics_.begin()+t);
                        --t;
                    }
                }
            }
	    x2axes_[i]->calculate_tics();
	    x2axes_[i]->prepare_for_draw();
            if(remove_conflicting_tic_)
            {
                for(unsigned int t=0; t<x2axes_[i]->tics_.size(); ++t)
                {
                    const double m = x2axes_[i]->map_point(x2axes_[i]->tics_[t].value());
                    if(i<nx_-1 && ::fabs(m-1) < 0.01)
                    {
                        x2axes_[i]->tics_.erase(x2axes_[i]->tics_.begin()+t);
                        --t;
                    }
                }
            }
	}
	for(int i=0; i<ny_; ++i)
	{
	    y1axes_[i]->calculate_tics();
	    y1axes_[i]->prepare_for_draw();
            if(remove_conflicting_tic_)
            {
                for(unsigned int t=0; t<y1axes_[i]->tics_.size(); ++t)
                {
                    const double m = y1axes_[i]->map_point(y1axes_[i]->tics_[t].value());
                    if(i<ny_-1 && ::fabs(m-1) < 0.01)
                    {
                        y1axes_[i]->tics_.erase(y1axes_[i]->tics_.begin()+t);
                        --t;
                    }
                }
	    }
	    y2axes_[i]->calculate_tics();
	    y2axes_[i]->prepare_for_draw();
            if(remove_conflicting_tic_)
            {
                for(unsigned int t=0; t<y2axes_[i]->tics_.size(); ++t)
                {
                    const double m = y2axes_[i]->map_point(y2axes_[i]->tics_[t].value());
                    if(i<ny_-1 && ::fabs(m-1) < 0.01)
                    {
                        y2axes_[i]->tics_.erase(y2axes_[i]->tics_.begin()+t);
                        --t;
                    }
                }
	    }
	}
    }

    void mframe::print(terminal *term)
    {
	if(print_me_ < 2) return;
	term->set_linewidth(LW.termspecific_id());
	term->set_color(black);
	term->set_linestyle(sym::solid);

	term->subpicture_begin(terminal::coord(cleft().termspecific_id(),
					       cbottom().termspecific_id()),
			       terminal::coord(cright().termspecific_id(),
					       ctop().termspecific_id()));

	for(int i=0; i<nx_; ++i)
	{
	    x1axes_[i]->print(term,false);
	    x2axes_[i]->print(term,false);
	}
	for(int i=0; i<ny_; ++i)
	{
	    y1axes_[i]->print(term,false);
	    y2axes_[i]->print(term,false);
	}

	for(int i=0; i<nx_*ny_; ++i) if(show_subframe_[i]) frames_[i]->print(term);
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    content_[i]->print(term);
	}

	term->subpicture_end();
    }

    mframe &mframe::width(int i, const length &l)
    {
	if(i <= 0 || nx_ < i)
	{
	    warning::print(var("Index [") & i & "] is out of range","mframe::width(int, const length &)");
	    return *this;
	}
	--i;
	rwidths_[i] = -1;
	widths_[i] = l;
	set_widths();
	return *this;
    }
    mframe &mframe::rwidth(int i, double d)
    {
	if(i <= 0 || nx_ < i)
	{
	    warning::print(var("Index [") & i & "] is out of range", "mframe::rwidth(int,double)");
	    return *this;
	}
	--i;
	rwidths_[i] = d;
	widths_[i] = ZERO;
	set_widths();
	return *this;
    }
    mframe &mframe::height(int i, const length &l)
    {
	if(i <= 0 || ny_ < i)
	{
	    warning::print(var("Index [") & i & "] is out of range","mframe::height(int,const length &)");
	    return *this;
	}
	--i;
	rheights_[i] = -1;
	heights_[i] = l;
	set_heights();
	return *this;
    }
    mframe &mframe::rheight(int i, double d)
    {
	if(i <= 0 || ny_ < i)
	{
	    warning::print(var("Index [") & i & "] is out of range","mframe::rheight(int,double)");
	    return *this;
	}
	--i;
	rheights_[i] = d;
	heights_[i] = ZERO;
	set_heights();
	return *this;
    }



    mframe &mframe::gap(const length &l)
    {
	xgap_ = l;
        ygap_ = l;
	set_widths();
	set_heights();
	return *this;
    }
    mframe &mframe::gap(const length &xgap, const length &ygap)
    {
	xgap_ = xgap;
        ygap_ = ygap;
	set_widths();
	set_heights();
	return *this;
    }
    mframe &mframe::xgap(const length &xgap)
    {
	xgap_ = xgap;
	set_widths();
	return *this;
    }
    mframe &mframe::ygap(const length &ygap)
    {
        ygap_ = ygap;
	set_heights();
	return *this;
    }

    void mframe::set_widths()
    {
	x_avail_ = !cwidth() - (nx_-1)*!xgap_;
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
	    bmarginboxes_[i]->left(l);
	    bmarginboxes_[i]->right(l+!widths_[i]);
	    bmarginboxes_[i]->fix_width(true);
	    bmarginboxes_[i]->top(-!x1axes_[i]->scriptsize() - blop::height("X"));
	    tmarginboxes_[i]->left(l);
	    tmarginboxes_[i]->right(l+!widths_[i]);
	    tmarginboxes_[i]->fix_width(true);
	    tmarginboxes_[i]->bottom(1.0+!x2axes_[i]->scriptsize() + blop::height("X"));
	    for(int j=0; j<ny_; ++j)
	    {
		int ii = ind(i+1,j+1);
		frames_[ii]->cleft(l);
		frames_[ii]->cright(l+!widths_[i]);
	    }
	    x1axes_[i]->pos(ZERO      , l, l+!widths_[i]);
	    x2axes_[i]->pos(!cheight() , l, l+!widths_[i]);
	    l += !widths_[i] + !xgap_;
	}
    }

    void mframe::set_heights()
    {
	y_avail_ = !cheight() - (ny_-1)*!ygap_;
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

	    lmarginboxes_[i]->bottom(l);
	    lmarginboxes_[i]->top(l+!heights_[i]);
	    lmarginboxes_[i]->fix_height(true);
	    lmarginboxes_[i]->right(-!y1axes_[i]->scriptsize() - 0.5*EX);

	    rmarginboxes_[i]->bottom(l);
	    rmarginboxes_[i]->top(l+!heights_[i]);
	    rmarginboxes_[i]->fix_height(true);
	    rmarginboxes_[i]->left(1.0+!y2axes_[i]->scriptsize() + 0.5*EX);

	    for(int j=0; j<nx_; ++j)
	    {
		int ii = ind(j+1,i+1);
		frames_[ii]->cbottom(l);
		frames_[ii]->ctop(l+!heights_[i]);
		y1axes_[i]->pos(ZERO    , l, l+!heights_[i]);
		y2axes_[i]->pos(!cwidth(), l, l+!heights_[i]);
	    }
	    l += !heights_[i] + !ygap_;
	}
    }

    void mframe::print_hierarchy(int level)
    {
	for(int i=0; i<level*4; ++i) cout<<" ";
	cout<<name()<<endl;
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    content_[i]->print_hierarchy(level+1);
	}
	for(int a=1; a<=nx_; ++a)
	{
	    for(int b=1; b<=ny_; ++b)
	    {
		frames_[ind(a,b)]->print_hierarchy(level+1);
	    }
	}
    }

}
