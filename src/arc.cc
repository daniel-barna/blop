#include "arc.h"
#include "container.h"
#include "canvas.h"
#include "frame.h"
#include "warning.h"
#include "sym.h"

namespace blop
{
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
    }

    
    arc &arc::draw (container *parent,
		      const length &x, const length &y,
		      const length &rx,
		      const length &ry,
		      double angle1,
		      double angle2)
    {
	arc *a = new arc(x,y,rx,ry,angle1,angle2);
	a->autodel(true);
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
	t->close_layer(layer_);
    }

}
