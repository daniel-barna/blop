#ifndef __BLOP_LINE_H__
#define __BLOP_LINE_H__

#include <vector>
#include "grob.h"
#include "color.h"
#include "sym.h"

namespace blop
{
    class container;

    class line : public grob
    {
    private:
	static bool    default_arrow_fore_, default_arrow_back_;
	static sym::linestyle     default_linestyle_;
	static length &default_linewidth_();
	static length &default_arrowlength_();
	static double  default_arrowangle_;
	static color   default_linecolor_;
	static bool    default_draw_line_;
	static color   default_fillcolor_;
	static bool    default_fill_;
        static bool    default_clip_;

	std::vector<blop::length> x_,y_;
	length linewidth_;
	sym::linestyle    linestyle_;
	color  linecolor_, fillcolor_;
	bool   draw_line_,fill_;

	length arrow_length_fore_, arrow_length_back_, arrow_width_fore_, arrow_width_back_;

	double arrow_angle_fore_, arrow_angle_back_;

	bool   arrow_fore_, arrow_back_;

        bool clip_;

	void draw_arrow(terminal::id x1,terminal::id y1,
			terminal::id x2,terminal::id y2,
			const length &l, const length &w, terminal *);

    public:
	
	line();

	// ---------------------------------------------------------------------
	// Set the default values (all lines created afterwards will have
	// these properties. Doesn't influence already existing objects)

	static void default_arrow(bool f)                { default_arrow_fore_ = f; }
	static void default_arrow_back(bool f)           { default_arrow_back_ = f; }
	static void default_arrowlength(const length &l) { default_arrowlength_() = l; }
	static void default_arrowangle (double d)        { default_arrowangle_ = d;  }

	static void           default_linestyle  (sym::linestyle i)  { default_linestyle_ = i; }
        static sym::linestyle default_linestyle()  { return default_linestyle_; }

	static void default_linewidth  (const length &l) { default_linewidth_() = l; }
        static const length &default_linewidth() { return default_linewidth_(); }
        
	static void default_linecolor  (const color &c)  { default_linecolor_ = c; }
        static color default_linecolor() { return default_linecolor_; }

	static void default_draw_line  (bool f)          { default_draw_line_ = f; }
	static void default_fillcolor  (const color &c)  { default_fillcolor_ = c; }
	static void default_fill       (bool f)          { default_fill_ = f; }
        static void default_clip       (bool f)          { default_clip_ = f; }


	// ----------  Add a new point to the line  ----------------------------

	line &operator() (const length &x, const length &y); // go to the specified point
	line &to(length x, length y);                        // the same as above
	line &rto(length x, length y);                       // x and y is relative to last point
	line &hto(length x);                                 // go horizontally to the x coord
	line &vto(length y);                                 // go vertically to the y coord
	line &close();                                       // close the line (go to first point)


	// ----------  Get points ----------------------------------------------

	int npoints() const;
	length &x(int i=-1);  // if argument is negative (default), return the 
	length &y(int i=-1);  // coordinates of the last point
	


	// ---------------------------------------------------------------------
	// The static functions drawing a line into the current frame, pad or canvas

	static line &fdraw(const length &x, const length &y);
	static line &pdraw(const length &x, const length &y);
	static line &cdraw(const length &x, const length &y);
	static line &draw (container *parent);



	// --------------  Set or read the linewidth  -------------------------

	line &linewidth(const length &l)   { linewidth_ = l; return *this; }
	line &lw(const length &l)          { return linewidth(l); }
	const length &linewidth() const    { return linewidth_; }



	// --------- Set or read the linestyle --------------------------------

	line &linestyle(sym::linestyle i) { linestyle_ = i; return *this;}
	line &ls(sym::linestyle i)        { return linestyle(i);}
	sym::linestyle linestyle() const  { return linestyle_; }



	// --------  Set or read the color  -----------------------------------

	line &linecolor(const color &c) { linecolor_ = c; return *this;}
	line &lc(const color &c)        { return linecolor(c);}
	const color &linecolor() const  { return linecolor_; }


        // -------------  whether it should be clipped when drawn ------------
        // (default: yes)

        line &clip(bool f)              { clip_ = f; return *this; }
        bool  clip() const              { return clip_;            }


	// --------  Switch on/off drawing of line  ---------------------------

	line &draw_line(bool f)         { draw_line_ = f; return *this; }
	bool  draw_line() const         { return draw_line_; }



	// --------------  Set the fillcolor  ---------------------------------

	line &fillcolor(const color &c) { fillcolor_ = c; return *this; }
	line &fc       (const color &c) { fillcolor_ = c; return *this; }
	const color &fillcolor() const  { return fillcolor_; }



	// ----------  Set the 'fill' property  -------------------------------

	line &fill(bool f) { fill_ = f; return *this; }
	bool  fill() const { return fill_; }



	// ---------   Set or read the arrowhead length  ----------------------

	line &arrowlength(const length &l) { arrow_length_fore_ = l; return *this; } // forward arrow
	line &al(const length &l) { return arrowlength(l); }
	const length &arrowlength() const { return arrow_length_fore_; }

	line &arrowlength_back(const length &l) { arrow_length_back_ = l; return *this; } // backward arrow
	line &al_back(const length &l) { return arrowlength_back(l); }
	const length &arrowlength_back() const { return arrow_length_back_; }



	// -----------  Set or read the arrowhead angle -----------------------

	line &arrowangle(double d);
	line &aa(double d) { return arrowangle(d); }
	double arrowangle() const { return arrow_angle_fore_; }

	line &arrowangle_back(double d);
	line &aa_back(double d) { return arrowangle_back(d); }
	double arrowangle_back() const { return arrow_angle_back_; }



	// -----  switch on/off the forward- and backward- arrows at the ends of the line  ------

	line &arrow(bool b) {arrow_fore_ = b; return *this;}
	line &arrow_back(bool b) {arrow_back_ = b; return *this;}
        line &arrows(bool b) { arrow_fore_ = arrow_back_ = b; return *this; }

	line &layer(const var & l) { grob::layer(l); return *this; }


	// ---------  Virtual functions of grob, which must be overwritten  --------------

	void prepare_for_draw();
	void print(terminal *);

    };

}

#endif
