#ifndef __BLOP_LABEL_H__
#define __BLOP_LABEL_H__

#include "box.h"
#include "sym.h"

namespace blop
{
    class container;

    class label : public box
    {
	public:
	    enum anchorsys { original, rotated };
	private:
            static length &default_gap_();
	    static color default_textcolor_;
	    static sym::position   default_xalign_, default_yalign_;
	    static anchorsys    default_anchorsystem_;
	    static double default_angle_;

	    var    text_;
	    double angle_;
            length angle_dx_, angle_dy_;
	    color  color_;
	    length x_,y_;
	    sym::position    xalign_,yalign_;
	    anchorsys        anchorsystem_;

	    static label *last_;
	    static sym::position default_direction_;

	public:
            label &above_grid(bool f) { box::above_grid(f); return *this; }


	    static void default_textcolor(const color &c) {default_textcolor_ = c;}
	    static void default_xalign(sym::position i) { default_xalign_ = i; }
	    static void default_yalign(sym::position i) { default_yalign_ = i; }
	    static void default_anchorsystem(anchorsys f) { default_anchorsystem_ = f; }
	    static void default_align(sym::position i,sym::position j) { default_xalign_ = i; default_yalign_ = j;}
	    static void default_angle(double a) { default_angle_ = a; }
            static void default_gap(const length &l); 

	    // return a pointer to the last label drawn with any of the xdraw functions (x=f,p,c)
	    static label *last() { return last_; }
	    static void           default_direction(sym::position d) { default_direction() = d; }
            static sym::position &default_direction();

	    label(const var &s="");
	    label(const label &);
	    const label &operator=(const label &); 

	    // --------------------------------------------------------------
	    // Static functions, creating a new label, and adding it 
	    // to the current frame, pad or canvas, respectively, from (x1,y1) to (x2,y2)
	    // the 4th function, 'draw' accepts one more parameter (the first), which specifies
	    // the container, into which the label should be added (the 'fdraw', 'pdraw' and 'cdraw'
	    // functions call this function with frame::current(), pad::current()
	    // and canvas::current() as the first argument) (see design-concepts.html#mknew )

	    static label &fdraw(const var &, length x, length y);
	    static label &pdraw(const var &, length x, length y);
	    static label &cdraw(const var &, length x, length y);
	    static label &fdraw(const var &, sym::position dir = label::default_direction());
	    static label &pdraw(const var &, sym::position dir = label::default_direction());
	    static label &cdraw(const var &, sym::position dir = label::default_direction());
	    static label &draw (container *parent,
				const var &, length x, const length y);
            static label &draw (container *parent,
                                const var &, sym::position dir = label::default_direction());

            static label &fdraw(const var &, const plottable &p, double x, const length &dy = 0.0);
            //static label &fdraw(const var &, const plottable *p, double x, const length &dy = 0.0);

	    const var    &text()   const;
	    label        &text     (const var &s);

            // Set the angle of the label (in rads!) If you want to give it in degrees,
            // specify .angle(90*unit::deg);
	    double        angle()  const;
	    label        &angle    (double a);

	    const color  &textcolor()  const;
	    label        &textcolor(const color &c);
	    label        &tc(const color &c) { return textcolor(c); }

	    void print(terminal *);

	    label &x(const length &l);
	    label &y(const length &l);
	    label &x(const length &l,sym::position xal);
	    label &y(const length &l,sym::position yal);

	    label &xalign(sym::position);
	    label &xalign(sym::position, anchorsys asys);
	    label &yalign(sym::position);
	    label &yalign(sym::position, anchorsys asys);
	    label &align(sym::position,sym::position);
	    label &align(sym::position,sym::position, anchorsys asys);

	    label &anchorsystem(anchorsys asys) { anchorsystem_ = asys; return *this;}

	    sym::position xalign() const { return xalign_; }
	    sym::position yalign() const { return yalign_; }
	    anchorsys anchorsystem() const { return anchorsystem_; } 

	    label &left   (const length &);
	    label &right  (const length &);
	    label &xcenter(const length &);
	    label &width  (const length &);

	    label &bottom (const length &);
	    label &top    (const length &);
	    label &ycenter(const length &);
	    label &height (const length &);

	    const length &left() const { return box::left(); }
	    const length &right() const { return box::right(); }
	    const length &xcenter() const { return box::xcenter(); }
	    const length &width() const { return box::width(); }
	    const length &bottom() const { return box::bottom(); }
	    const length &top() const { return box::top(); }
	    const length &ycenter() const { return box::ycenter(); }
	    const length &height() const { return box::height(); }

	    void prepare_for_draw();

	    // needed only for CINT, dummy functions
	    bool operator== (const label &) const;
	    bool operator<  (const label &) const {return false;}
	    bool operator>  (const label &) const {return false;}

	    label &layer(const var &l) { grob::layer(l); return *this; }
    };
}
#endif
