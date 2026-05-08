#ifndef __BLOP_POINT_H__
#define __BLOP_POINT_H__

#include "grob.h"
#include "point_drawer.h"
#include "color.h"

namespace blop
{
    class point : public grob
    {
    private:
	static point_drawer *default_type_;
	point_drawer *type_;

	static color default_color_;
	color color_;

	static length default_size_;
	length size_;

	length x_, y_;
    public:

	point();
	~point();

	void                 default_pointtype(const point_drawer &);
	point               &pointtype(const point_drawer &);
	point               &pt       (const point_drawer &);
	const point_drawer  &pointtype() const;

	void default_pointcolor(const color &);
	point &pointcolor(const color &);
	point &pc(const color &);
	const color & pointcolor() const;

	void default_pointsize(const length &);
	point &pointsize(const length &);
	point &ps(const length &);
	const length &pointsize() const;

	static point &draw(container *parent, length x, length y);
	static point &fdraw(length x, length y);
	static point &pdraw(length x, length y);
	static point &cdraw(length x, length y);

	point &x(length x) { x_ = x; return *this; }
	const length &x() const { return x_; }

	point &y(length y) { y_ = y; return *this; }
	const length &y() const { return y_; }

	point &layer(const var & l) { grob::layer(l); return *this; }

	void print(terminal *t);
	void prepare_for_draw();
    };
}

#endif
