#ifndef __BLOP_ARC_H__
#define __BLOP_ARC_H__

#include "grob.h"
#include "color.h"
#include "sym.h"

namespace blop
{
    class container;

    class arc : public grob
    {
    private:
	static sym::linestyle default_linestyle_;
	static length  &default_linewidth_();
	static color   &default_linecolor_();
	static color   &default_fillcolor_();
	static bool     default_draw_line_;
	static bool     default_fill_;
	static double   default_angle1_, default_angle2_;

	sym::linestyle linestyle_;
	length linewidth_;
	color linecolor_;
	color fillcolor_;
	bool draw_line_;
	bool fill_;

	length x_,y_,rx_,ry_;
	double angle1_,angle2_;

    public:
	static void default_linestyle(sym::linestyle);
	static void default_linewidth(length l);
	static void default_linecolor(color &c);
	static void default_fillcolor(color &c);
	static void default_draw_line(bool);
	static void default_fill(bool);
	static void default_rx(length);
	static length &default_rx();
	static void default_ry(length);
	static length &default_ry();
	static void default_angle1(double);
	static double default_angle1();
	static void default_angle2(double);
	static double default_angle2();

	arc(length x,
	    length y,
	    length rx = default_rx(),
	    length ry = default_ry(),
	    double angle1=default_angle1(),
	    double angle2=default_angle2());

	arc &linestyle(sym::linestyle i) { linestyle_ = i; return *this; }
        sym::linestyle linestyle() const { return linestyle_; }

	arc &linewidth(length l) { linewidth_ = l; return *this; }
	const length &linewidth() const { return linewidth_; }

	arc &linecolor(const color &c) { linecolor_ = c; return *this; }
	const color &linecolor() const { return linecolor_; }

	arc &fillcolor(const color &c) { fillcolor_ = c; return *this; }
	const color &fillcolor() const { return fillcolor_; }

	arc &draw_line(bool f) { draw_line_ = f; return *this; }
	bool draw_line() const { return draw_line_; }

	arc &fill(bool f) { fill_ = f; return *this; }
	bool fill() const { return fill_; }

	arc &x(const length &xx) { x_ = xx; return *this; }
	const length &x() const { return x_; }

	arc &y(const length &yy) { y_ = yy; return *this; }
	const length &y() const { return y_; }

	arc &rx(const length &r) { rx_ = r; return *this; }
	const length &rx() const { return rx_; }

	arc &ry(const length &r) { ry_ = r; return *this; }
	const length &ry() const { return ry_; }

	arc &angle1(double a) { angle1_ = a; return *this; }
	double angle1() const { return angle1_; }

	arc &angle2(double a) { angle2_ = a; return *this; }
	double angle2() const { return angle2_; }

	static arc &fdraw(const length &x, const length &y,
			  const length &rx = default_rx(),
			  const length &ry = default_ry(),
			  double angle1 = default_angle1(),
			  double angle2 = default_angle2());
	static arc &pdraw(const length &x, const length &y,
			  const length &rx = default_rx(),
			  const length &ry = default_ry(),
			  double angle1 = default_angle1(),
			  double angle2 = default_angle2());
	static arc &cdraw(const length &x, const length &y,
			  const length &rx = default_rx(),
			  const length &ry = default_ry(),
			  double angle1 = default_angle1(),
			  double angle2 = default_angle2());
	static arc &draw (container *parent,
			  const length &x, const length &y,
			  const length &rx = default_rx(),
			  const length &ry = default_ry(),
			  double angle1 = default_angle1(),
			  double angle2 = default_angle2());

	void prepare_for_draw();
	void print(terminal *);
	
    };
}



#endif
