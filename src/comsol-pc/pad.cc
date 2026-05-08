#include "frame.h"
#include "pad.h"
#include "canvas.h"
#include <algorithm>
#include "exc.hh"

#include <iostream>

using namespace std;

namespace blop
{

    // --------------  pad --------------------------------------

    color pad::default_bordercolor_(0,0,0);
    bool pad::default_draw_border_  = false;
    length pad::default_borderwidth_(new length::base_id_t(terminal::LW));
    bool pad::default_border_3D_ = false;
    color pad::default_fillcolor_(1,1,1);
    bool pad::default_fill_ = false;

    pad &pad::fillcolor(const color &c)
    {
	fillcolor_ = c;
	fill_ = true;
	return *this;
    }
    color pad::fillcolor() const
    {
	return fillcolor_;
    }
    void pad::default_fillcolor(const color &c)
    {
	default_fillcolor_ = c;
    }

    pad &pad::fill(bool i)
    {
	fill_ = i;
	return *this;
    }
    bool pad::fill() const
    {
	return fill_;
    }
    void pad::default_fill(bool i)
    {
	default_fill_ = i;
    }

    void pad::draw_border_bg(terminal *t)
    {
	vector<terminal::coord> cc;

	if(fill_)
	{
	    cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));
	    cc.push_back(terminal::coord(terminal::id(1.0,1),terminal::id(0.0,2)));
	    cc.push_back(terminal::coord(terminal::id(1.0,1),terminal::id(1.0,2)));
	    cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(1.0,2)));
	    cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));

	    t->set_color(fillcolor_);
	    t->fill_polygon(cc);

	    cc.clear();
	}

	if(draw_border_)
	{
	    if(!border_3D_)
	    {
		cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));
		cc.push_back(terminal::coord(terminal::id(1.0,1),terminal::id(0.0,2)));
		cc.push_back(terminal::coord(terminal::id(1.0,1),terminal::id(1.0,2)));
		cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(1.0,2)));
		cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));

		t->set_linewidth(borderwidth_.termspecific_id());
		t->set_color(bordercolor_);
		t->set_linestyle(sym::solid);
		t->draw_lines(cc);
	    }
	    else
	    {
		length l1 = -1 * !borderwidth_;
		length l2 = !cwidth() + !borderwidth_;
		length l3 = !cheight() + !borderwidth_;
		l1.specialize(t);
		l2.specialize(t);
		l3.specialize(t);

		color c1(::min(fillcolor_.red()*0.5 + 0.5 , 1.35*fillcolor_.red()),
			 ::min(fillcolor_.green()*0.5 + 0.5, 1.35*fillcolor_.green()),
			 ::min(fillcolor_.blue()*0.5 + 0.5, 1.35*fillcolor_.blue()));

		color c2(fillcolor_.red() * 0.8,
			 fillcolor_.green() * 0.8,
			 fillcolor_.blue() * 0.8);

		t->set_color(c1);

		cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));
		cc.push_back(terminal::coord(l1.termspecific_id(),l1.termspecific_id()));
		cc.push_back(terminal::coord(l1.termspecific_id(),l3.termspecific_id()));
		cc.push_back(terminal::coord(l2.termspecific_id(),l3.termspecific_id()));
		cc.push_back(terminal::coord(terminal::id(1.0,1),terminal::id(1.0,2)));
		cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(1.0,2)));
		cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));
		t->fill_polygon(cc);
		cc.clear();

		t->set_color(c2);

		cc.push_back(terminal::coord(terminal::id(1.0,1),terminal::id(1.0,2)));
		cc.push_back(terminal::coord(l2.termspecific_id(),l3.termspecific_id()));
		cc.push_back(terminal::coord(l2.termspecific_id(),l1.termspecific_id()));
		cc.push_back(terminal::coord(terminal::id(1.0,1),terminal::id(0.0,2)));
		cc.push_back(terminal::coord(terminal::id(1.0,1),terminal::id(1.0,2)));
		t->fill_polygon(cc);
		cc.clear();


		cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));
		cc.push_back(terminal::coord(l1.termspecific_id(),l1.termspecific_id()));
		cc.push_back(terminal::coord(l2.termspecific_id(),l1.termspecific_id()));
		cc.push_back(terminal::coord(terminal::id(1.0,1),terminal::id(0.0,2)));
		cc.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));
		t->fill_polygon(cc);
		cc.clear();
	    }
	}
    }

    void pad::print(terminal *t)
    {
	if(print_me_ < 2) return;
	t->open_layer(layer_);
	t->subpicture_begin(terminal::coord(cleft().termspecific_id(),
					    cbottom().termspecific_id()),
			    terminal::coord(cright().termspecific_id(),
					    ctop().termspecific_id()));
	
	draw_border_bg(t);
	for(vector<grob *>::size_type i=0; i<content_.size(); ++i)
	{
	    content_[i]->print(t);
	}
	t->subpicture_end();
	t->close_layer(layer_);
    }

    void pad::prepare_for_draw()
    {
	if(print_me_ < 1) return;
	container::prepare_for_draw();
	if(draw_border_) borderwidth_.register_me();
    }
    
    pad *pad::current_pad_ = 0;

    void pad::cd_specific()
    {
	current_pad_ = this;
	frame::current_ = 0;
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    if(frame *f = dynamic_cast<frame *>(content_[i])) frame::current_ = f;
	}
    }

    pad &pad::borderwidth (const length &l)
    {
	borderwidth_ = l;
	return *this;
    }
    const length &pad::borderwidth() const
    {
	return borderwidth_;
    }
    void pad::default_borderwidth(const length &l)
    {
	default_borderwidth_ = l;
    }

    pad &pad::bordercolor (const color &c)
    {
	draw_border_ = true;
	bordercolor_ = c;
	return *this;
    }
    color pad::bordercolor() const
    {
	return bordercolor_;
    }
    void pad::default_bordercolor(const color &c)
    {
	default_bordercolor_ = c;
    }

    pad &pad::draw_border(bool i)
    {
	draw_border_ = i;
	return *this;
    }
    bool pad::draw_border() const
    {
	return draw_border_;
    }
    void pad::default_draw_border(bool i)
    {
	default_draw_border_ = i;
    }

    pad &pad::border_3D(bool i)
    {
	border_3D_ = i;
	return *this;
    }
    bool pad::border_3D() const
    {
	return border_3D_;
    }
    void pad::default_border_3D(bool i)
    {
	default_border_3D_ = i;
    }

    pad::pad(const length &x1,const length &y1,
	     const length &x2,const length &y2,
	     bool is_canvas)
    {
	name("pad");

	draw_border_ = default_draw_border_;
	bordercolor_ = default_bordercolor_;
	fill_ = default_fill_;
	borderwidth_ = default_borderwidth_;
	border_3D_ = default_border_3D_;

	right(x2);
	left(x1);
	top(y2);
	bottom(y1);

	lmargin(!borderwidth_);
	rmargin(!borderwidth_);
	bmargin(!borderwidth_);
	tmargin(!borderwidth_);
    }

    pad::~pad()
    {
	if(current_pad_ == this)
	{
	    current_pad_ = 0;
	    frame::current_ = 0;
	}
    }

    void pad::clear()
    {
        // Set the title to the empty string. This automatically removes it from the container
        title("");
        container::clear();
    }

    pad &pad::title(const var &t)
    {
        if(t.str() != "")
        {
            if(!find(&title_)) add(&title_);
            title_.text(t);
            title_.x(0.5).y(1.0+EX).align(sym::center,sym::bottom);
            tmargin(blop::height(t)+2*EX);
        }
        else
        {
            remove(&title_);
            tmargin(!borderwidth_);
        }
        return *this;
    }
	
    pad &pad::mknew(length x1, length y1,
		    length x2, length y2)

    {
	return mksub(x1,y1,x2,y2,canvas::current());
    }

    pad &pad::mksub(length x1, length y1,
		    length x2, length y2,
		    container &parent)
    {
	pad *p = new pad(x1,y1,x2,y2);
	p->autodel(true);
	parent.add(p);
	p->cd();
	return *p;
    }

    pad &pad::current()
    {
	if(current_pad_ == 0) return canvas::current();
	return *current_pad_;
    }


}
