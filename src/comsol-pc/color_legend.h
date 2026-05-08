#ifndef __COLOR_LEGEND_H__
#define __COLOR_LEGEND_H__

#include "box.h"
#include "label.h"
#include <vector>

/*
  color_legend is a class (a grob, a graphical object), which displays
  a color bar, colors representing numerical values.
  This object is typically used by cbox and similar drawstyles, which
  plot data on a 2D map by colors.

 */

namespace blop
{
    class graphd_colorscale;
    class tic;

    class color_legend : public box
    {
    private:
	void init_();
	std::vector<blop::tic> colortics_;
	static length default_sample_width_;
	length sample_width_;
	length sample_right_;
	length tic_left_;
	length sep_;
	length ticlabel_pos_;
	length ticlabel_width_;
        bool   ticlabel_width_fixed_;
	label  title_;
	std::vector<blop::graphd_colorscale*> owners_;
	friend class graphd_colorscale;
	double min_, max_;
	color_legend(const color_legend &);
	const color_legend &operator= (const color_legend &rhs);

    public:
	color_legend();
	color_legend(graphd_colorscale *d);
	~color_legend();

	// -------------------------------------------------------------
	// Create a color_legend and put it to the current pad, right-aligned

	static color_legend &mknew();

	// -------------------------------------------------------------
	// Specify the title of the color_legend (i.e. the text which is
	// printed vertically to the right of the color-bar)

	color_legend &title(const var &t);
	const var &title() const;
	
	// -------------------------------------------------------------
	// Specify the width of the the color-sample bar. The default
	// is 3*EX

	color_legend &sample_width(const length &l);
	static void   default_sample_width(const length &l);
        color_legend &ticlabel_width(const length &l);

	void print(terminal *term);
	void prepare_for_draw();
	void calculate_tics();
	void clear_tics();
	void remove_owner(graphd_colorscale *);
	const std::vector<blop::tic> tics() const { return colortics_; }
	double min() const;
	double max() const;
    };
}


#endif
