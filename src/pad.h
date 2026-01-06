#ifndef __BLOP_PAD_H__
#define __BLOP_PAD_H__

#include "terminal.h"
#include "color.h"
#include "container.h"
#include "label.h"

#include <vector>
#include <cmath>
#include <cstdio>


namespace blop
{
    class frame;

    class pad : public container
	{

	    // A 'pad' is a rectangular area with its own coordinate system,
	    // for drawing objects

	private:
            label   title_;

	    color bordercolor_;
	    static color default_bordercolor_;

	    bool draw_border_;
	    static bool default_draw_border_;

	    length borderwidth_;
	    static length default_borderwidth_;

	    bool border_3D_;
	    static bool default_border_3D_;

	    color fillcolor_;
	    static color default_fillcolor_;

	    bool  fill_;
	    static bool default_fill_;

	protected:

	    static pad *current_pad_;

	    void draw_border_bg(terminal *);

	public:
	    pad(const length &x1=0.0,const length &y1=0.0,
		const length &x2=1.0,const length &y2=1.0,
		bool is_canvas = 0);
	    virtual ~pad();

            // -------------- clear ---------------------------------------------
            // Override the inherited clear, in order to remove the title

            void clear();


	    // -------------  Return currently active pad -----------------------
	    // This can as well be the uppermost pad, which is the current canvas
	    // This function never returns 0 !
	    
	    static pad &current();

	   
	    // ------------- Create new pads ------------------------------------
	    // These functions  create a  new  sub-pad  ('mknew'  places the new
	    // pad into the current canvas,  while the last  argument of 'mksub'
	    // specifies the container, in  which the new  pad should be placed)
	    // The first four arguments  specify the lower  left and upper right
	    // corners of this new pad within the coordinate system of its parent.
	    // Each of  these  arguments can  either be a  length,  or simply a
	    // double.  In the latter case it means the fraction of the width or
	    // height of the parent (for example x1=0.5 means, that the left side
	    // of the new pad will be placed to the half of its  parent's width)

	    static pad &mknew(length x1, length y1,
			      length x2, length y2);

	    static pad &mksub(length x1, length y1,
			      length x2, length y2,
			      container &parent = pad::current());

            // ------------ Set the title of the pad  -------------------------------------

            pad &title(const var &t);
            var title() const { return title_.text(); }


	    // ---------  override container:: and box:: dimension functions --------------
	    // so that they return a pad& instead of container& or box&

	    pad &cleft(const length &l) { container::cleft(l); return *this; }
	    pad &cright(const length &l) { container::cright(l); return *this; }
	    pad &cwidth(const length &l) { container::cwidth(l); return *this; }
	    pad &cxcenter(const length &l) { container::cxcenter(l); return *this; }
	    pad &cbottom(const length &l) { container::cbottom(l); return *this; }
	    pad &ctop(const length &l) { container::ctop(l); return *this; }
	    pad &cheight(const length &l) { container::cheight(l); return *this; }
	    pad &cycenter(const length &l) { container::cycenter(l); return *this; }

	    const length &cleft() const { return container::cleft(); }
	    const length &cright() const { return container::cright(); }
	    const length &cwidth() const { return container::cwidth(); }
	    const length &cxcenter() const { return container::cxcenter(); }
	    const length &cbottom() const { return container::cbottom(); }
	    const length &ctop() const { return container::ctop(); }
	    const length &cheight() const { return container::cheight(); }
	    const length &cycenter() const { return container::cycenter(); }

	    pad &lmargin(const length &l) { container::lmargin(l); return *this; }
	    pad &rmargin(const length &l) { container::rmargin(l); return *this; }
	    pad &bmargin(const length &l) { container::bmargin(l); return *this; }
	    pad &tmargin(const length &l) { container::tmargin(l); return *this; }
	    const length &lmargin() const { return container::lmargin(); }
	    const length &rmargin() const { return container::rmargin(); }
	    const length &bmargin() const { return container::bmargin(); }
	    const length &tmargin() const { return container::tmargin(); }

	    pad &left(const length &l) { box::left(l); return *this; }
	    pad &right(const length &l) { box::right(l); return *this; }
	    pad &width(const length &l) { box::width(l); return *this; }
	    pad &xcenter(const length &l) { box::xcenter(l); return *this; }
	    pad &bottom(const length &l) { box::bottom(l); return *this; }
	    pad &top(const length &l) { box::top(l); return *this; }
	    pad &height(const length &l) { box::height(l); return *this; }
	    pad &ycenter(const length &l) { box::ycenter(l); return *this; }
	    const length &left() const { return box::left(); }
	    const length &right() const { return box::right(); }
	    const length &width() const { return box::width(); }
	    const length &bottom() const { return box::bottom(); }
	    const length &top() const { return box::top(); }
	    const length &height() const { return box::height(); }

	    // ----------   Switch on/off border drawing -----------------------

	    pad         &draw_border(bool i);
	    bool         draw_border() const;
	    static void  default_draw_border(bool);


	    // --------   Specify the color of the border -----------------------

	    pad         &bordercolor        (const color &);
	    color        bordercolor        () const;
	    static void  default_bordercolor(const color &); 



	    // ----------  Specify the width of the border  --------------------

	    pad             &borderwidth(const length &l);
	    const length    &borderwidth() const;
	    static void      default_borderwidth(const length &);


	    // --------   Switch on/off 3D border drawing. ---------------------
	    // In the case of 3D border drawing, the 'bordercolor' is ignored, 
	    // and the background color is used for the border to create a 3D effect

	    pad         &border_3D(bool i);
	    bool         border_3D() const;
	    static void  default_border_3D(bool);


	    // ----------  Switch on/off background filling  --------------------

	    pad         &fill(bool i);
	    bool         fill() const;
	    static void  default_fill(bool);


	    // ----------   Specify the background color  -------------------------
	    // and at the same time switches to filled mode

	    pad         &fillcolor    (const color &);
	    color       fillcolor    () const;
	    static void default_fillcolor(const color &);

	    void print(terminal *term);

	    void cd_specific();
	    void prepare_for_draw();

	    pad &layer(const var & l) { grob::layer(l); return *this; }
	};



}

#endif



