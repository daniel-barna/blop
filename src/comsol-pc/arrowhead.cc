#include "arrowhead.h"
#include <cmath>
#include <vector>

namespace blop
{
    void arrowhead::simple::set_t_()
    {
	t1_ = !l_*(+1*::atan(angle_*3.1415/180/2));
	t2_ = !l_*(-1*::atan(angle_*3.1415/180/2));
    }

    arrowhead::simple::simple(const length &l, double a)
    {
	l_ = -1*!length_;
	length_ = l;
	angle_ = a;
	set_t_();
    }
    arrowhead::simple::simple(const arrowhead::simple &o)
    {
	length_ = o.length_;
	angle_ = o.angle_;
	l_ = -1*!length_;
	set_t_();
    }
    arrowhead *arrowhead::simple::clone() const
    {
	return new arrowhead::simple(*this);
    }
    arrowhead::simple &arrowhead::simple::size(const length &l)
    {
	length_ = l;
	set_t_();
	return *this;
    }
    const length &arrowhead::simple::size() const
    {
	return length_;
    }
    arrowhead::simple &arrowhead::simple::angle(double a)
    {
	angle_ = a;
	set_t_();
	return *this;
    }
    double arrowhead::simple::angle() const
    {
	return angle_;
    }

    void arrowhead::simple::prepare_for_draw()
    {
	l_.register_me();
	t1_.register_me();
	t2_.register_me();
    }

    void arrowhead::simple::print(terminal *t)
    {
	t->open_layer(layer_);
	vector<terminal::coord> c;
	c.push_back(terminal::coord(l_.termspecific_id(),t1_.termspecific_id()));
	c.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));
	c.push_back(terminal::coord(l_.termspecific_id(),t2_.termspecific_id()));
	t->draw_lines(c);
	t->close_layer(layer_);
    }


    // -------------------- filled arrowhead ----------------------------------

    void arrowhead::filled::set_t_()
    {
	t1_ = !l_*(+1*::atan(angle_*3.1415/180/2));
	t2_ = !l_*(-1*::atan(angle_*3.1415/180/2));
    }

    arrowhead::filled::filled(const length &l, double a)
    {
	length_ = l;
	length2_ = !length_;
	l1_eq_l2_ = true;
	angle_ = a;
	l_ = -1*!length_;
	l2_ = -1*!length2_;
	set_t_();
    }
    arrowhead::filled::filled(const length &l, double a, const length &l2)
    {
	length_ = l;
	if(l2.initialized())
	{
	    length2_ = l2;
	    l1_eq_l2_ = false;
	}
	else
	{
	    length2_ = !length_;
	    l1_eq_l2_ = true;
	}
	angle_ = a;
	l_ = -1*!length_;
	l2_ = -1*!length2_;
	set_t_();
    }
    arrowhead::filled::filled(const arrowhead::filled &o)
    {
	length_ = o.length_;
	l1_eq_l2_ = o.l1_eq_l2_;
	if(o.l1_eq_l2_) length2_ = !length_;
	else length2_ = o.length2_;
	angle_ = o.angle_;
	l_ = -1*!length_;
	l2_ = -1*!length2_;
	set_t_();
    }
    arrowhead *arrowhead::filled::clone() const
    {
	return new arrowhead::filled(*this);
    }

    arrowhead::filled &arrowhead::filled::size(const length &l)
    {
	length_ = l;
	set_t_();
	return *this;
    }
    const length &arrowhead::filled::size() const
    {
	return length_;
    }

    arrowhead::filled &arrowhead::filled::size2(const length &l)
    {
	length2_ = l;
	l1_eq_l2_ = false;
	return *this;
    }
    const length &arrowhead::filled::size2() const
    {
	return length2_;
    }

    arrowhead::filled &arrowhead::filled::angle(double a)
    {
	angle_ = a;
	set_t_();
	return *this;
    }
    double arrowhead::filled::angle() const
    {
	return angle_;
    }

    void arrowhead::filled::prepare_for_draw()
    {
	l_.register_me();
	l2_.register_me();
	t1_.register_me();
	t2_.register_me();
    }

    void arrowhead::filled::print(terminal *t)
    {
	t->open_layer(layer_);
	vector<terminal::coord> c;
	c.push_back(terminal::coord(l2_.termspecific_id(),terminal::id(0.0,2)));
	c.push_back(terminal::coord(l_.termspecific_id(),t1_.termspecific_id()));
	c.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0.0,2)));
	c.push_back(terminal::coord(l_.termspecific_id(),t2_.termspecific_id()));
	c.push_back(terminal::coord(l2_.termspecific_id(),terminal::id(0.0,2)));
	t->fill_polygon(c);
	t->close_layer(layer_);
    }

}
