#include "warning.h"
#include "line.h"
#include "length.h"
#include "frame.h"
#include "canvas.h"


namespace blop
{
    bool line::default_arrow_fore_ = false;
    bool line::default_arrow_back_ = false;
    bool line::default_clip_       = true;

    length &line::default_arrowlength_()
    {
	static length l = 3*MM;
	return l;
    }
    double line::default_arrowangle_  = 30;
    sym::linestyle    line::default_linestyle_ = sym::solid;
    color  line::default_linecolor_(0,0,0);
    bool   line::default_draw_line_ = true;
    color  line::default_fillcolor_(0,0,0);
    bool   line::default_fill_ = false;
    length &line::default_linewidth_()
    {
	static length l = LW;
	return l;
    }

    line::line()
    {
	name("line");
	linecolor_         = default_linecolor_;
	draw_line_         = default_draw_line_;
	fillcolor_         = default_fillcolor_;
	fill_              = default_fill_;
	linewidth_         = default_linewidth_();
	linestyle_         = default_linestyle_;
	arrow_length_fore_ = default_arrowlength_();
	arrow_length_back_ = default_arrowlength_();
	arrow_angle_fore_  = default_arrowangle_;
	arrow_angle_back_  = default_arrowangle_;

	arrow_width_fore_ = ::tan(arrow_angle_fore_/2*3.1415/180) * !arrow_length_fore_;
	arrow_width_back_ = ::tan(arrow_angle_back_/2*3.1415/180) * !arrow_length_back_;

	arrow_fore_ = default_arrow_fore_;
	arrow_back_ = default_arrow_back_;

        clip_       = default_clip_;
    }

    line &line::arrowangle(double d)
    {
	arrow_angle_fore_ = d;
	arrow_width_fore_ = ::tan(d/2*3.1415/180) * !arrow_length_fore_;
	arrow_fore_ = true;
	return *this;
    }

    line &line::arrowangle_back(double d)
    {
	arrow_angle_back_ = d;
	arrow_width_back_ = ::tan(d/2*3.1415/180) * !arrow_length_back_;
	arrow_back_ = true;
	return *this;
    }

    int line::npoints() const
    {
	return x_.size();
    }

    length &line::x(int i)
    {
	if(x_.empty())
	{
	    static length dummy;
	    return dummy;
	}
	if(i < 0 || i >= (int)x_.size()) i = x_.size()-1;
	return x_[i];
    }

    length &line::y(int i)
    {
	if(y_.empty())
	{
	    static length dummy;
	    return dummy;
	}
	if(i < 0 || i >= (int)y_.size()) i = y_.size()-1;
	return y_[i];
    }

    line &line::operator()(const length &x, const length &y)
    {
	to(x,y);
	return *this;
    }

    line &line::to (length x, length y)
    {
	x_.push_back(x);
	y_.push_back(y);
	return *this;
    }

    line &line::rto(length x, length y)
    {
	if(x_.empty())
	{
	    x_.push_back(x);
	    y_.push_back(y);
	    return *this;
	}
	x_.push_back(x_.back() + x);
	y_.push_back(y_.back() + y);
	return *this;
    }

    line &line::hto(length x)
    {
	if(x_.empty())
	{
	    x_.push_back(x);
	    y_.push_back(terminal::ZERO);
	    return *this;
	}
	x_.push_back(x);
	y_.push_back(y_.back());
	return *this;
    }

    line &line::vto(length y)
    {
	if(x_.empty())
	{
	    x_.push_back(terminal::ZERO);
	    y_.push_back(y);
	    return *this;
	}
	x_.push_back(x_.back());
	y_.push_back(y);
	return *this;
    }

    line &line::close()
    {
	if(x_.empty())
	{
	    warning::print("Line has no points, can't close it",
			   "line::close()");
	    return *this;
	}
	x_.push_back(x_[0]);
	y_.push_back(y_[0]);
	return *this;
    }

    line &line::draw(container *parent)
    {
	line *l = new line;
	l->autodel(true);
	parent->add(l);
	return *l;
    }

    line &line::fdraw(const length &x, const length &y)
    {
	return line::draw(&frame::current())(x,y);
    }
    line &line::pdraw(const length &x, const length &y)
    {
	return line::draw(&pad::current())(x,y);
    }
    line &line::cdraw(const length &x, const length &y)
    {
	return line::draw(&canvas::current())(x,y);
    }

    void line::prepare_for_draw()
    {
	if(print_me_ == 0) return;
	if(parent() == 0)
	{
	    warning::print("Strange: no parent of line");
	    return;
	}
	for(vector<length>::size_type i=0; i<x_.size(); ++i)
	{
	    x_[i].set_scale(x_unit());
	    y_[i].set_scale(y_unit());
	    x_[i].register_me();
	    y_[i].register_me();
	}

	if(!fill_) linewidth_.register_me();

	if(arrow_fore_)
	{
	    arrow_length_fore_.register_me();
	    arrow_width_fore_.register_me();
	}

	if(arrow_back_)
	{
	    arrow_length_back_.register_me();
	    arrow_width_back_.register_me();
	}
    }

    void line::print(terminal *t)
    {
	if(print_me_ != 2) return;
	int n = x_.size();
	if(n < 2) return;

	t->open_layer(layer_);
	vector<terminal::coord> cc(n);
	for(int i=0; i<n; ++i)
	{
	    cc[i].x = x_[i].termspecific_id();
	    cc[i].y = y_[i].termspecific_id();
	}

        if(clip_)
        {
            if(!t->clip(terminal::coord(terminal::id(0,1),terminal::id(0,2)),
                        terminal::coord(terminal::id(1,1),terminal::id(1,2))))
            {
                warning::print("This terminal doesn't support clipping","line::draw(...)");
            }
        }

	if(fill_)
	{
	    t->set_color(fillcolor_);
	    t->fill_polygon(cc);
	}

	if(draw_line_)
	{
	    t->set_linestyle(linestyle_);
	    t->set_linewidth(linewidth_.termspecific_id());
	    t->set_color(linecolor_);
	    t->draw_lines(cc);
	}

	if(arrow_fore_)
	    draw_arrow(cc[n-2].x, cc[n-2].y, cc[n-1].x, cc[n-1].y,
		       arrow_length_fore_, arrow_width_fore_,t);
	if(arrow_back_)
	    draw_arrow(cc[1].x, cc[1].y, cc[0].x, cc[0].y,
		       arrow_length_back_, arrow_width_back_,t);

        if(clip_) t->noclip();

	t->close_layer(layer_);
    }

    void line::draw_arrow(terminal::id x1,terminal::id y1,
			  terminal::id x2,terminal::id y2,
			  const length &l, const length &w,terminal *t)
    {

	int dx_id = t->lincombi(1,x1,-1,x2);
	int dy_id = t->lincombi(1,y1,-1,y2);

	length w_minus = -1*w;
	w_minus.specialize(t);

        length a = 0.9*l;
        a.specialize(t);

	t->rotate(dx_id,dy_id);
	t->translate(x2,y2);
	vector<terminal::coord> cc;
        cc.push_back(terminal::coord(a.termspecific_id(), terminal::id(0,2)));
	cc.push_back(terminal::coord(l.termspecific_id(), w.termspecific_id()));
	cc.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
	cc.push_back(terminal::coord(l.termspecific_id(), w_minus.termspecific_id()));
	t->fill_polygon(cc);
	t->reset_transformation();

/*
	int dx_id = t->lincombi(1,x1,-1,x2);
	int dy_id = t->lincombi(1,y1,-1,y2);
	int atan_id = t->atan(dx_id,dy_id);

	int sin_id = t->sin_dbl(atan_id);
	int cos_id = t->cos_dbl(atan_id);
	int min_sin_id = t->neg_dbl(sin_id);
	int min_cos_id = t->neg_dbl(cos_id);

	int xx1 = t->lincombi(terminal::double_id(0,sin_id), w.termspecific_id(),
			      terminal::double_id(0,cos_id), l.termspecific_id());
	int yy1 = t->lincombi(terminal::double_id(0,min_cos_id), w.termspecific_id(),
			      terminal::double_id(0,sin_id), l.termspecific_id());

	int xx2 = t->lincombi(terminal::double_id(0,min_sin_id), w.termspecific_id(),
			      terminal::double_id(0,cos_id), l.termspecific_id());
	int yy2 = t->lincombi(terminal::double_id(0,cos_id), w.termspecific_id(),
			      terminal::double_id(0,sin_id), l.termspecific_id());


	t->translate(x2,y2);
	vector<terminal::coord> cc;
	cc.push_back(terminal::coord(xx1,yy1));
	cc.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
	cc.push_back(terminal::coord(xx2,yy2));
	t->draw_lines(cc);
	t->reset_transformation();
*/
	
    }

}
