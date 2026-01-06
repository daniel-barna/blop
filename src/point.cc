#include "point.h"
#include "canvas.h"
#include "exc.hh"
#include "frame.h"

namespace blop
{
    point_drawer *point::default_type_ = new circle(true);
    color point::default_color_(0,0,0);
    length point::default_size_(new length::base_id_t(terminal::PS));

    point::point()
    {
	type_ = default_type_->clone();
	size_ = default_size_;
	color_ = default_color_;
    }

    point::~point()
    {
	delete type_;
    }

    void point::default_pointtype(const point_drawer &t)
    {
	delete default_type_;
	default_type_ = t.clone();
    }
    point &point::pointtype(const point_drawer &t)
    {
	delete type_;
	type_ = t.clone();
	return *this;
    }
    point &point::pt(const point_drawer &t)
    {
	return pointtype(t);
    }
    const point_drawer &point::pointtype() const
    {
	return *type_;
    }

    void point::default_pointcolor(const color &c)
    {
	default_color_ = c;
    }
    point &point::pointcolor(const color &c)
    {
	color_ = c;
	return *this;
    }
    point &point::pc(const color &c)
    {
	return pointcolor(c);
    }
    const color &point::pointcolor() const {return color_;}

    void point::default_pointsize(const length &l)
    {
	default_size_ = l;
    }
    point &point::pointsize(const length &l)
    {
	size_ = l;
	return *this;
    }
    point &point::ps(const length &l)
    {
	return pointsize(l);
    }
    const length &point::pointsize() const {return size_; }

    void point::print(terminal *t)
    {
	if(print_me_ < 2) return;
	t->open_layer(layer_);
	t->set_color(color_);
	t->translate(x_.termspecific_id(), y_.termspecific_id());
	type_->draw(t);
	t->reset_transformation();
	t->close_layer(layer_);
    }

    point &point::draw(container *parent, length x, length y)
    {
	if(parent == 0) err("point::draw ==> parent t== 0");

	point *p = new point();
	p->x(x).y(y);
	p->autodel(true);
	parent->add(p);
	return *p;
    }
    point &point::fdraw(length x,length y)
    {
	return point::draw(&frame::current(), x, y);
    }
    point &point::pdraw(length x,length y)
    {
	return point::draw(&pad::current(), x, y);
    }
    point &point::cdraw(length x,length y)
    {
	return point::draw(&canvas::current(), x, y);
    }

    void point::prepare_for_draw()
    {
	if(print_me_ < 1) return;
	x_.set_scale(x_unit());
	y_.set_scale(y_unit());
	x_.register_me();
	y_.register_me();
	size_.register_me();
	type_->prepare_for_draw(size_);
    }

}
