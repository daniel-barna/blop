#include "label.h"
#include "container.h"
#include "frame.h"
#include "canvas.h"
#include "exc.hh"
#include "warning.h"
#include "bloputils.h"

namespace blop
{
    length &label::default_gap_()
    {
        length::base_id_t b(terminal::EX);
        static blop::length l(b);
        return l;
    }
    void label::default_gap(const length &l)
    {
        default_gap_() = l;
    }

    sym::position label::default_xalign_ = sym::left;
    sym::position label::default_yalign_ = sym::base;
    label::anchorsys label::default_anchorsystem_ = label::rotated;
    color label::default_textcolor_(0,0,0);
    double label::default_angle_ = 0;
    label *label::last_ = 0;
    sym::position &label::default_direction()
    {
        static sym::position dir = sym::down;
        return dir;
    }

    label &label::draw(container *parent,
		       const var &text, length x, length y)
    {
	if(parent == 0) err("label::draw ==> parent == 0");

	last_ = new label(text);
	last_->x(x).y(y);
	last_->autodel(true);
	parent->add(last_);
	return *last_;
    }
    label &label::draw(container *parent, const var &text, sym::position dir)
    {
        label *last = parent->get_last<label>();
	if(!last)
        {
            return label::draw(parent,text,
                               (dir==sym::left?0.95:0.05),
                               (dir==sym::up?0.05:0.95))
                .xalign(dir==sym::left?sym::right:sym::left)
                .yalign(dir==sym::up  ?sym::bottom:sym::top);
        }
        if(dir == sym::up)
        {
	    return label::draw(parent,
                               text,
                               (last->xalign_==sym::left?last->left():(last->xalign_==sym::right?last->right():last->xcenter())),
                               last->top()+default_gap_()).xalign(last->xalign_).yalign(sym::bottom);
        }
        if(dir == sym::left)
        {
	    return label::draw(parent,text,last->left()-default_gap_(),last->bottom()).xalign(sym::right).yalign(sym::bottom);
        }
        if(dir == sym::right)
        {
	    return label::draw(parent,text,last->right()+default_gap_(),last->bottom()).xalign(sym::left).yalign(sym::bottom);
        }
        return label::draw(parent,
                           text,
                           (last->xalign_==sym::left?last->left():(last->xalign_==sym::right?last->right():last->xcenter())),
                           last->bottom()-default_gap_()).xalign(last->xalign_).yalign(sym::top);
    }


    label &label::fdraw(const var &text, const plottable &p, double xvalue, const length &dy)
    {
        function x = _1;
        if(p.drawstyle()) x = p.drawstyle()->get_x(&p);
        function y = _2;
        if(p.drawstyle()) y = p.drawstyle()->get_y(&p);

        double yvalue = unset;

        double yy_closest = unset;
        double xx_closest = unset;

        double xx_last=0, yy_last=0;
        for(unsigned int i=0; i<p.size(); ++i)
        {
            std::vector<blop::var> result;
            x.meval_dbl(*(p.get(i)),result);
            const double xx = result[0].dbl();
            y.meval_dbl(*(p.get(i)),result);
            const double yy = result[0].dbl();

            if(i==0 || std::abs(xx-xvalue) < (std::abs(xx_closest-xvalue)))
            {
                xx_closest = xx;
                yy_closest = yy;
            }

            if(i>0 && (xvalue-xx_last)*(xvalue-xx) <= 0)
            {
                yvalue = yy_last + (yy-yy_last)/(xx-xx_last)*(xvalue-xx_last);
                break;
            }

            xx_last = xx;
            yy_last = yy;
        }
        
        if(yvalue == unset)
        {
            return label::fdraw(text,
                                (p.xaxis() == axis::x1 ? x1len(xx_closest) : x2len(xx_closest)),
                                (p.yaxis() == axis::y1 ? y1len(yy_closest) : y2len(yy_closest))+dy);

        }

        return label::fdraw(text,
                            (p.xaxis() == axis::x1 ? x1len(xvalue) : x2len(xvalue)),
                            (p.yaxis() == axis::y1 ? y1len(yvalue) : y2len(yvalue))+dy);
    }

    label &label::fdraw(const var &text, length x, length y)
    {
	return label::draw(&frame::current(),text,x,y);
    }

    label &label::pdraw(const var &text, length x, length y)
    {
	return label::draw(&pad::current(),text,x,y);
    }
    
    label &label::cdraw(const var &text, length x, length y)
    {
	return label::draw(&canvas::current(),text,x,y);
    }

    label &label::fdraw(const var &text, sym::position dir)
    {
        return draw(&frame::current(),text,dir);
    }

    label &label::pdraw(const var &text, sym::position dir)
    {
        return draw(&pad::current(),text, dir);
    }
    
    label &label::cdraw(const var &text, sym::position dir)
    {
        return draw(&canvas::current(),text, dir);
    }
    

    bool label::operator== (const label &o) const
    {
	return
	    text_ == o.text_ &&
	    angle_ == o.angle_ &&
	    color_ == o.color_ &&
	    xalign_ == o.xalign_ &&
	    yalign_ == o.yalign_;
    }

    void label::print(terminal *t)
    {
	if(print_me_ < 2) return;
	t->open_layer(layer_);
	t->set_color(color_);
/*
  // debugging
	vector<terminal::coord> c;
	c.push_back(terminal::coord(x_.termspecific_id(),y_.termspecific_id()));
	c.push_back(terminal::coord(x_.termspecific_id(),terminal::id(1,2)));
	t->draw_lines(c);
*/
	t->draw_text(terminal::coord(x_.termspecific_id(),
				     y_.termspecific_id()),
		     text_.str(),
		     xalign_,
		     yalign_,
                     angle_dx_.termspecific_id(),
                     angle_dy_.termspecific_id(),
		     anchorsystem_==rotated);
	t->close_layer(layer_);
    }

    void label::prepare_for_draw()
    {
	if(print_me_ < 1) return;
	x_.set_scale(x_unit());
	y_.set_scale(y_unit());
        angle_dx_.set_scale(x_unit());
        angle_dy_.set_scale(y_unit());

	x_.register_me();
	y_.register_me();
        angle_dx_.register_me();
        angle_dy_.register_me();
    }

    label::label(const var &s)
	: text_(s)
    {
	
	name("label");

	angle_ = default_angle_;
        angle_dx_ = MM*::cos(angle_);
        angle_dy_ = MM*::sin(angle_);
	color_ = default_textcolor_;
	xalign_ = default_xalign_;
	yalign_ = default_yalign_;
	anchorsystem_ = default_anchorsystem_;

	box::left(0.0);
	box::width(blop::width(text_,angle_));

	box::bottom(0.0);
	box::height(blop::height(text_,angle_));
    }

    const label &label::operator= (const label &o)
    {
	text_ = o.text_;
	angle_ = o.angle_;
        angle_dx_ = o.angle_dx_;
        angle_dy_ = o.angle_dy_;
	color_ = o.color_;
	x_ = o.x_;
	y_ = o.y_;
	xalign_ = o.xalign_;
	yalign_ = o.yalign_;

	box::operator=(o);

	return *this;
    }

    label::label(const label &o)
    {
	*this = o;
    }

    const var    &label::text()   const {return text_;}
    double        label::angle()  const {return angle_;}
    const color  &label::textcolor()  const {return color_;}

    label     &label::text(const var &s) 
    {
	text_ = s; 
	box::width (blop::width (text_,angle_));
	box::height(blop::height(text_,angle_));

	return *this;
    }
    label     &label::angle(double a)
    {
	angle_ = a;
        angle_dx_ = MM*::cos(angle_);
        angle_dy_ = MM*::sin(angle_);
	box::width (blop::width (text_,angle_));
	box::height(blop::height(text_,angle_));

	return *this;
    }

    label     &label::textcolor(const color &c)
    {
	color_ = c; return *this;
    }

    label &label::xalign(sym::position a)
    {
	xalign_ = a;
	if(a == sym::left)
	{
	    box::width(blop::width(text_,angle_));
	    box::left(!x_);
	}
	else if(a == sym::right)
	{
	    box::width(blop::width(text_,angle_));
	    box::right(!x_);
	}
	else
	{
	    box::width(blop::width(text_,angle_));
	    box::left(!x_ - 0.5*!box::width());
	}
	return *this;
    }

    label &label::xalign(sym::position a, anchorsys asys)
    {
	anchorsystem(asys);
	xalign(a);
	return *this;
    }


    label &label::x(const length &l)
    {
	x_ = l;
	xalign(xalign_);
	return *this;
    }
    label &label::x(const length &l,sym::position al)
    {
	x_ = l;
	xalign(al);
	return *this;
    }

    label &label::yalign(sym::position a)
    {
	yalign_ = a;
	if(a == sym::bottom)
	{
	    box::height(blop::height(text_,angle_));
	    box::bottom(!y_);
	}
	else if(a == sym::top)
	{
	    box::height(blop::height(text_,angle_));
	    box::top(!y_);
	}
	else
	{
	    box::height(blop::height(text_,angle_));
	    box::bottom(!y_ - 0.5*!box::height());
	}
	return *this;
    }

    label &label::yalign(sym::position a, anchorsys asys)
    {
	anchorsystem(asys);
	yalign(a);
	return *this;
    }


    label &label::y(const length &l)
    {
	y_ = l;
	yalign(yalign_);
	return *this;
    }
    label &label::y(const length &l,sym::position al)
    {
	y_ = l;
	yalign(al);
	return *this;
    }

    label &label::align(sym::position xx, sym::position yy)
    {
	xalign(xx);
	yalign(yy);
	return *this;
    }

    label &label::align(sym::position xx, sym::position yy, anchorsys asys)
    {
	anchorsystem(asys);
	align(xx,yy);
	return *this;
    }


    label &label::left(const length &l)
    {
	x(l,sym::left);
	return *this;
    }

    label &label::right(const length &l)
    {
	x(l,sym::right);
	return *this;
    }

    label &label::xcenter(const length &l)
    {
	x(l,sym::center);
	return *this;
    }

    label &label::width(const length &l)
    {
	warning::print("Please, do not try to set the width of a label!",
		       "label::width(const length &)");
	return *this;
    }

    label &label::bottom(const length &l)
    {
	y(l,sym::bottom);
	return *this;
    }

    label &label::top(const length &l)
    {
	y(l,sym::top);
	return *this;
    }

    label &label::ycenter(const length &l)
    {
	y(l,sym::center);
	return *this;
    }

    label &label::height(const length &l)
    {
	warning::print("Please, do not try to set the height of a label!",
		       "label::height(const length &)");
	return *this;
    }

}
