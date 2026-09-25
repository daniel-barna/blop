#include "arc.h"
#include "container.h"
#include "canvas.h"
#include "frame.h"
#include "warning.h"
#include "sym.h"

namespace blop
{
    bool arc::default_arrow_fore_ = false;
    bool arc::default_arrow_back_ = false;
    double arc::default_arrowangle_  = 30*unit::deg;
    length &arc::default_arrowlength()
    {
	static length l = 3*MM;
	return l;
    }

    void arc::draw_arrow(terminal::id x0, terminal::id y0, const length &len, const length &width, double angle, terminal *t)
    {
        length a = 0.9*len;
        a.specialize(t);
	length width_minus = -1*width;
	width_minus.specialize(t);

        t->rotate(angle);
	t->translate(x0,y0);
        vector<terminal::coord> cc;
        cc.push_back(terminal::coord(a.termspecific_id(), terminal::id(0,2)));
	cc.push_back(terminal::coord(len.termspecific_id(), width.termspecific_id()));
	cc.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
	cc.push_back(terminal::coord(len.termspecific_id(), width_minus.termspecific_id()));
	t->fill_polygon(cc);
        t->reset_transformation();
    }


    double arc::default_angle1_ = 0;
    double arc::default_angle2_ = 360*unit::deg;
    sym::linestyle arc::default_linestyle_ = sym::solid;
    length &arc::default_linewidth_()
    {
	static length w = length::base_id_t(terminal::LW);
	return w;
    }
    color &arc::default_linecolor_()
    {
	static color c(0,0,0);
	return c;
    }
    color &arc::default_fillcolor_()
    {
	static color c(0,0,0);
	return c;
    }
    bool arc::default_draw_line_ = true;
    bool arc::default_fill_ = false;

    void arc::default_linestyle(sym::linestyle i)
    {
	default_linestyle_ = i;
    }
    void arc::default_linewidth(length l)
    {
	default_linewidth_() = l;
    }
    void arc::default_linecolor(color &c)
    {
	default_linecolor_() = c;
    }
    void arc::default_fillcolor(color &c)
    {
	default_fillcolor_() = c;
    }
    void arc::default_draw_line(bool f)
    {
	default_draw_line_ = f;
    }
    void arc::default_fill(bool f)
    {
	default_fill_ = f;
    }
    length &arc::default_rx()
    {
	static length rx = CM;
	return rx;
    }
    void arc::default_rx(length l)
    {
	default_rx() = l;
    }
    length &arc::default_ry()
    {
	static length ry = CM;
	return ry;
    }
    void arc::default_ry(length l)
    {
	default_ry() = l;
    }
    void arc::default_angle1(double a)
    {
	default_angle1_ = a;
    }
    double arc::default_angle1() { return default_angle1_; }
    void arc::default_angle2(double a)
    {
	default_angle2_ = a;
    }
    double arc::default_angle2() { return default_angle2_; }

    arc::arc(length x, length y, length rx, length ry, double angle1, double angle2)
    {
	x_ = x;
	y_ = y;
	rx_ = rx;
	ry_ = ry;
	angle1_ = angle1;
	angle2_ = angle2;

	linestyle_ = default_linestyle_;
	linewidth_ = default_linewidth_();
	linecolor_ = default_linecolor_();
	fillcolor_ = default_fillcolor_();
	draw_line_ = default_draw_line_;
	fill_ = default_fill_;

	arrow_length_fore_ = default_arrowlength();
	arrow_length_back_ = default_arrowlength();
	arrow_angle_fore_  = default_arrowangle_;
	arrow_angle_back_  = default_arrowangle_;
	arrow_width_fore_ = ::tan(arrow_angle_fore_/2) * !arrow_length_fore_;
	arrow_width_back_ = ::tan(arrow_angle_back_/2) * !arrow_length_back_;
	arrow_fore_ = default_arrow_fore_;
	arrow_back_ = default_arrow_back_;
    }

    
    arc &arc::draw (container::ptr parent,
                    const length &x, const length &y,
                    const length &rx,
                    const length &ry,
                    double angle1,
                    double angle2)
    {
        auto a = arc::create(x,y,rx,ry,angle1,angle2);
//	arc *a = new arc(x,y,rx,ry,angle1,angle2);
//	a->autodel(true);
	parent->add(a);
	return *a;
    }

    arc &arc::fdraw (const length &x, const length &y,
		     const length &rx,
		     const length &ry,
		     double angle1,
		     double angle2)
    {
	return arc::draw(&frame::current(), x, y, rx, ry, angle1, angle2);
    }
    arc &arc::pdraw (const length &x, const length &y,
		     const length &rx,
		     const length &ry,
		     double angle1,
		     double angle2)
    {
	return arc::draw(&pad::current(), x, y, rx, ry, angle1, angle2);
    }

    arc &arc::cdraw (const length &x, const length &y,
		     const length &rx,
		     const length &ry,
		     double angle1,
		     double angle2)
    {
	return arc::draw(&canvas::current(), x, y, rx, ry, angle1, angle2);
    }


    void arc::prepare_for_draw()
    {
	if(print_me_ == 0) return;
	if(parent() == 0)
	{
	    warning::print("Strange: no parent of arc");
	    return;
	}
	x_.set_scale(x_unit());
	y_.set_scale(y_unit());
	rx_.set_scale(x_unit());
	ry_.set_scale(y_unit());
	x_.register_me();
	y_.register_me();
	rx_.register_me();
	ry_.register_me();
	linewidth_.register_me();

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

    void arc::print(terminal *t)
    {
	if(print_me_ != 2) return;
	t->open_layer(layer_);
	if(fill_)
	{
	    t->set_color(fillcolor_);
	    t->fill_arc(terminal::coord(x_.termspecific_id(), y_.termspecific_id()),
			rx_.termspecific_id(), angle1_, angle2_);
	}
	t->set_color(linecolor_);
	t->set_linestyle(linestyle_);
	t->set_linewidth(linewidth_.termspecific_id());
	t->draw_arc(terminal::coord(x_.termspecific_id(), y_.termspecific_id()),
		    rx_.termspecific_id(), angle1_, angle2_);


        // Strange, I think the 90deg terms should have opposite signs here, but this is how it works
	if(arrow_fore_) draw_arrow(t->lincombi(1.0,x_.termspecific_id(),::cos(angle2_),rx_.termspecific_id()),
                                   t->lincombi(1.0,y_.termspecific_id(),::sin(angle2_),rx_.termspecific_id()),
                                   arrow_length_fore_,arrow_width_fore_,angle2_-90*unit::deg,t);
	if(arrow_back_) draw_arrow(t->lincombi(1.0,x_.termspecific_id(),::cos(angle1_),rx_.termspecific_id()),
                                   t->lincombi(1.0,y_.termspecific_id(),::sin(angle1_),rx_.termspecific_id()),
                                   arrow_length_back_,arrow_width_back_,angle1_+90*unit::deg,t);
        
	t->close_layer(layer_);
    }

}
