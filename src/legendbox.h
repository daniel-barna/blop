#ifndef __BLOP_LEGENDBOX_H__
#define __BLOP_LEGENDBOX_H__

#include "label.h"
#include "plottable.h"
#include "pad.h"
#include "line.h"
#include <vector>

namespace blop
{
    class graph_drawer;

    class legendbox : public box
	{
	private:
	    length calculated_width_, calculated_height_;

	    color bordercolor_, fillcolor_;
	    static color default_bordercolor_, default_fillcolor_;

	    bool draw_border_, fill_;
	    static bool default_draw_border_, default_fill_;

	    bool multi_;
	    static bool default_multi_;

	    int max_multisample_;
	    length bordersep_,legend_sample_sep_,sample_sample_sep_;
	    static length default_bordersep_;
	    static length default_legend_sample_sep_;
	    static length default_sample_sample_sep_;

	    length max_legendwidth_;
	    length legend_xpos_;
	    
	    length sample_length_;
	    static length default_sample_length_;

	    length row_sep_;
	    static length default_row_sep_;

	    length borderwidth_;
	    static length default_borderwidth_;

	    //bool skip_(plottable *);

	public:

	    struct entry
	    {

                bool line;

		label   legend;

                // number indicating the position (1, 2, ... etc) of this entry in case
                // it is part of a multi-legend, i.e. if several graphs share the legend,
                // and the samples are shown in the same line, next to each other.
		int     multi_number;
                
		plottable  *gr;

		length  sample_x,sample_y;

	    };

	private:
            bool skip_(entry *e);
	    std::vector<blop::legendbox::entry*> entries_;

	public:
	    static legendbox &mknew( container &parent = pad::current() );

	    legendbox();
	    ~legendbox();


	    // ------------  switch multi-mode on/off --------------------------
	    // In multi-mode all graphs with the same legend are put in one line
	    // with one single legend text, and all samples are presented in this
	    // line, next to each other. If multi-mode is off, every graph is
	    // put in a separate line

	    legendbox &multi(bool);
	    bool multi() const { return multi_; }
	    static void default_multi(bool f);


	    // ---------  Add or remove a graph from the legendbox  -------------------

	    void add(blop::plottable *);
	    virtual bool remove(blop::plottable *);
	    static void remove_from_all(blop::plottable *);

            // add just a label, without a graph
            label &add(const var &);

            legendbox &add_line();

	    // ---------  Remove all graphs from the legendbox  -----------------------

	    void clear();


	    // ---------------------------------------------
	    
	    void update();

	    // ------------------------------------------
	    // Overwrite the automatically alculated width/height
	    // if len == unset, width/height is switched back to the
	    // automatically calculated values

	    legendbox &width (length len);
	    legendbox &height(length len);
	    
	    const length &width() const { return box::width(); }
	    const length &height() const { return box::height(); }

	    //-----------------------------------------------------------
	    // Set or get the width of the border 

	    legendbox    &borderwidth(const length &);
	    const length &borderwidth() const;
	    static void   default_borderwidth(const length &);


	    //----------------------------------------------------------
	    // SET THE COLOR OF THE BORDER
	    // this automatically switches border-drawing ON

	    legendbox   &bordercolor(const color &);
	    const color &bordercolor() const;
	    static void  default_bordercolor(const color &);


	    //----------------------------------------------------------
	    // Switch border-drawing on or off

	    legendbox  &draw_border(bool i = true);
	    bool        draw_border() const;
	    static void default_draw_border(bool f); 


	    //----------------------------------------------------------
	    // Set the background color of the legendbox
	    // this automatically switches background-drawing ON

	    legendbox   &fillcolor(const color &);
	    const color &fillcolor() const;
	    static void  default_fillcolor(const color &c); 


	    //----------------------------------------------------------
	    // Switch off or on the filled background 

	    legendbox   &fill(bool i);
	    bool         fill() const;
	    static void  default_fill(bool f);


	    //----------------------------------------------------------
	    // Set the length of the samples

	    legendbox    &samplen(const length &);
	    const length &samplen() const;
	    static void   default_samplen(const length &l);

            legendbox    &sample_length(const length &l) { samplen(l); return *this; }
            static void default_sample_length(const length &l) { default_samplen(l); }


	    //----------------------------------------------------------
	    // Set the separation between text and the surrounding border

	    legendbox    &bordersep(const length &);
	    const length &bordersep() const;
	    static void   default_bordersep(const length &l);


	    //----------------------------------------------------------
	    // Set the separation between the text and the sample

	    legendbox    &legend_sample_sep(const length &l);
	    const length &legend_sample_sep() const;
	    static void   default_legend_sample_sep(const length &l);


	    //----------------------------------------------------------
	    // Set the separation between two samples in a multi-sample box

	    legendbox    &sample_sample_sep(const length &l);
	    const length &sample_sample_sep() const;
	    static void   default_sample_sample_sep(const length & l);


	    //----------------------------------------------------------
	    // Set separation between rows

	    legendbox    &row_sep(const length &l);
	    const length &row_sep() const;
	    static void   default_row_sep(const length &l);


	    //----------------------------------------------------------
	    // A vector keeping the pointers of all existing legendboxes
	    static std::vector<blop::legendbox*> &all();


	    void print(terminal *);
	    void prepare_for_draw();


	    legendbox &layer(const var &l) { grob::layer(l); return *this; }

	    // ------------------------------------------------------------------
	    // and overwrite the functions inherited from box, to return a legendbox instead of a box

	    legendbox &left   (const length &);
	    legendbox &right  (const length &);
	    legendbox &xcenter(const length &);
	    //legendbox &width  (const length &);  this has been overwritten above

	    legendbox &bottom (const length &);
	    legendbox &top    (const length &);
	    legendbox &ycenter(const length &);
	    //legendbox &height (const length &);  this has been overwritten above

	    const length &left() const { return box::left(); }
	    const length &right() const { return box::right(); }
	    const length &xcenter() const { return box::xcenter(); }
	    //const length &width() const { return box::width(); } this has been overwritten above
	    const length &bottom() const { return box::bottom(); }
	    const length &top() const { return box::top(); }
	    const length &ycenter() const { return box::ycenter(); }
	    //const length &height() const { return box::height(); } this has been overwritten above

	};


}

#ifdef __MAKECINT__
#pragma link off function blop::legendbox::prepare_for_draw();
#endif

#endif
