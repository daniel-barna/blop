#ifndef __BLOP_FRAME_H__
#define __BLOP_FRAME_H__

#include "axis.h"
#include "box.h"
#include "plottable.h"
#include "dgraph.h"
#include "fgraph.h"
#include "grob.h"
#include "legendbox.h"
#include "label.h"
#include "sym.h"

/*
  A frame is the place, within which graphs are displayed. It has
  - axes (4 axes: x1 and x2 are the horizontal axes, y1 and y2 the vert. ones)
  - legendbox (the explantion of the graphs)
*/

namespace blop
{
    class canvas;
    class epad;

    class frame : public container
	{
	private:
#ifndef __MAKECINT__
	    class grob_plottable
	    {
	    public:
		grob      *grob_;
		plottable *plottable_;
	        grob_plottable(grob *gg, plottable *pp) : grob_(gg), plottable_(pp) {}
	    };
	    vector<grob_plottable> ordering_;
#endif
	private:
	    void draw_grid(terminal *);

	    friend class canvas;

	    static sym::position default_legend_xalign_, default_legend_yalign_;
	    sym::position legend_xalign_, legend_yalign_;
            bool show_legend_;
            static bool default_show_legend_;

	    int draw_frame_;
	    int draw_axes_;
	    static int default_draw_frame_;
	    static int  default_draw_axes_;

	    int    grid_;
            int    grid_at_minor_tics_;
	    static int    default_grid_, default_grid_at_minor_tics_;

            // Function for a parametric grid. 
            function pgrid_func_;
            double pgrid_1_from_, pgrid_1_to_, pgrid_2_from_, pgrid_2_to_;
            int pgrid_1_n_, pgrid_2_n_;

	    color  gridcolor_, minor_gridcolor_;
            static color default_gridcolor_, default_minor_gridcolor_;

	    sym::linestyle    gridstyle_, minor_gridstyle_;
            static sym::linestyle    default_gridstyle_, default_minor_gridstyle_;

	    length gridwidth_, minor_gridwidth_;
            static length default_gridwidth_, default_minor_gridwidth_;

            length gridcut_;
	    length linewidth_;

	    static length default_linewidth_, default_gridcut_;

	    bool foreground_;
	    static bool default_foreground_;

	    bool grid_foreground_;
	    static bool default_grid_foreground_;

	    color framecolor_;
	    static color default_framecolor_;

	    friend class pad;

	    bool owns_x1axis_,owns_x2axis_,owns_y1axis_,owns_y2axis_;
	    axis *x1axis_, *x2axis_, *y1axis_, *y2axis_;

	    std::vector<blop::plottable*> graphs_;

	    length parent_width_, parent_height_;

	    static frame *current_;

	    legendbox *legendbox_;
	    bool       owns_legendbox_;

	    bool mirror_x1_, mirror_x2_, mirror_y1_, mirror_y2_;

	    void draw_axes(terminal *);
	    void draw_frame(terminal *);

	    // separation of the margin object
	    length marginobjectsep_;
	    static length default_marginobjectsep_;

	    frame(const frame &);
	    void operator= (const frame &);

	    enum {Left=0, Right=1, Bottom=2, Top=3};
	    epad *marginboxes_[4];
	    bool owns_marginboxes_[4];

	    label *title_;

	    void print_graph_(plottable *, terminal *);

	    bool nodistort_;

	public:


	    // -----------  Create a new frame ---------------------------------------------
	    // Creation of a new frame with all the necessary administration:
	    // creates a new frame within the given container (by default the current pad),
	    // autodel of the new frame is set to 'true'

	    static frame &mknew(container &parent = pad::current());

	    // ------------  Get a pointer to the legendbox used by this frame  ------------

	    legendbox *legend() {return legendbox_;}


	    // -----  Specify legend position (such as sym::left, sym::bottom, etc) ----

	    frame &legendpos(sym::position xalign,sym::position yalign);
	    static void default_legendpos(sym::position xalign, sym::position  yalign);


	    // ------------  Set an outer legendbox ----------------------------------------
	    // Set an outer legendbox (possibly to be shared by several frames)
	    // With this function you can set an outer legendbox
	    // to be used by this frame. This can be used for example
	    // to let several frames share a single legendbox. 
	    // This procedure does not work 100%-ly yet, there might
	    // be unexpected results, but most likely it will work.
	    // Try it!

            frame &legend(legendbox *l);
	    frame &legend(legendbox &l);

	    
	    // -----  Change back to own legendbox -----------------------------------------

	    frame &own_legend();

            // ------ Switch off/on the legendbox ------------------------------------------

            frame &show_legend(bool f = true) { show_legend_ = f; return *this; }
            static void default_show_legend(bool f) { default_show_legend_ = f; }

	    // ----- return reference to the current frame ---------------------------------
	    // Returns a reference to the current frame. If there is no
	    // current frame (for example a new pad has recently been created,
	    // and no frame has been created in it yet), it creates a new one,
	    // and makes it to be the current, and returns a reference to that.

	    static frame &current();

            // -----------  check the current frame, without creating a new one ------------
            
	    static frame *check_current() { return current_; }

	    bool parent(container *);
            const container *parent() const { return grob::parent(); }
            container       *parent()       { return grob::parent(); }

	    frame &add(plottable *);
	    frame &remove(plottable *);

	    
	    // -----  Add any grob (GRaphical OBject) into this frame  ---------------------

	    void add(grob *g);
	    bool remove(grob *g);

	    // ------------------  Clear  -------------------------------------------------
	    // Clear all content.  Objects with the 'autodel' flag set to 
	    // 'true' will also be deleted, not only removed from this frame

	    void clear();

	    
	    // -------  Clear all graphs, but not other contents --------------------------

	    frame &clear_graphs(bool all = false);
	    frame &clear_graphs_and_autosettings(bool all = false);


	    // ---------------  Frame title -----------------------------------------------
	    // Specify frame title. If the argument is an empty string,
	    // clear the title, and reset the topmargin to the default value

	    frame &title(const var &t);

	    // ------   Separation of the marginobject  -----------------------------------

	    frame &marginobjectsep(const length &l) { marginobjectsep_ = l; return *this; }
	    const length &marginobjectsep() const { return marginobjectsep_; }
	    static void default_marginobjectsep(const length &l) {default_marginobjectsep_ = l;}

	    frame &lmarginbox(epad *);
	    frame &rmarginbox(epad *);
	    frame &bmarginbox(epad *);
	    frame &tmarginbox(epad *);

	    epad *lmarginbox() const;
	    epad *rmarginbox() const;
	    epad *bmarginbox() const;
	    epad *tmarginbox() const;
	    
	    frame &lmarginobject(box *);
	    frame &rmarginobject(box *);
	    frame &bmarginobject(box *);
	    frame &tmarginobject(box *);

	    // ------   Switch off/on mirroring of axis tics ------------------------------

	    frame &mirror_x1(bool i) {mirror_x1_ = i; return *this;}
	    frame &mirror_x2(bool i) {mirror_x2_ = i; return *this;}
	    frame &mirror_y1(bool i) {mirror_y1_ = i; return *this;}
	    frame &mirror_y2(bool i) {mirror_y2_ = i; return *this;}

            // --------- Switch off/on the tics of the axes -------------------------------

            frame &draw_tics(bool f);

	    // ---------  override container:: and box:: dimension functions --------------
	    // so that they return a frame& instead of container& or box&

	    frame &cleft(const length &l)   { container::cleft(l); return *this; }
	    frame &cright(const length &l)  { container::cright(l); return *this; }
	    frame &cwidth(const length &l)  { container::cwidth(l); return *this; }
	    frame &cbottom(const length &l) { container::cbottom(l); return *this; }
	    frame &ctop(const length &l)    { container::ctop(l); return *this; }
	    frame &cheight(const length &l) { container::cheight(l); return *this; }

	    const length &cleft  () const { return container::cleft  (); }
	    const length &cright () const { return container::cright (); }
	    const length &cwidth () const { return container::cwidth (); }
	    const length &cbottom() const { return container::cbottom(); }
	    const length &ctop   () const { return container::ctop   (); }
	    const length &cheight() const { return container::cheight(); }

	    frame &lmargin(const length &l) { container::lmargin(l); return *this; }
	    frame &rmargin(const length &l) { container::rmargin(l); return *this; }
	    frame &bmargin(const length &l) { container::bmargin(l); return *this; }
	    frame &tmargin(const length &l) { container::tmargin(l); return *this; }
	    const length &lmargin() const { return container::lmargin(); }
	    const length &rmargin() const { return container::rmargin(); }
	    const length &bmargin() const { return container::bmargin(); }
	    const length &tmargin() const { return container::tmargin(); }

	    frame &left(const length &l) { box::left(l); return *this; }
	    frame &right(const length &l) { box::right(l); return *this; }
	    frame &width(const length &l) { box::width(l); return *this; }
	    frame &bottom(const length &l) { box::bottom(l); return *this; }
	    frame &top(const length &l) { box::top(l); return *this; }
	    frame &height(const length &l) { box::height(l); return *this; }

	    const length &left() const { return box::left(); }
	    const length &right() const { return box::right(); }
	    const length &width() const { return box::width(); }
	    const length &bottom() const { return box::bottom(); }
	    const length &top() const { return box::top(); }
	    const length &height() const { return box::height(); }

	    frame &loffset(const length &);
	    frame &roffset(const length &);
	    frame &toffset(const length &);
	    frame &boffset(const length &);
	    frame &offsets(const length &);

	    frame &caspect(double s);
	    frame &cxlock(sym::position a);
	    frame &cylock(sym::position a);

	    frame &nodistort(bool f);
	    bool   nodistort() const { return nodistort_; }

	    frame();
	    virtual ~frame();

	    static void default_gridwidth(const length &l) { default_gridwidth_ = l; }
	    frame &gridwidth(const length &);
	    const length &gridwidth() const { return gridwidth_; }

            static void default_minor_gridwidth(const length &l) { default_minor_gridwidth_ = l; }
            frame &minor_gridwidth(const length &);
            const length &minor_gridwidth() const { return minor_gridwidth_; }

	    static void default_gridcut(const length &l) { default_gridcut_ = l; }
	    frame &gridcut(const length &);
	    const length &gridcut() const;

	    static void default_linewidth(const length &l) { default_linewidth_ = l; }
	    frame &linewidth(const length &l);
	    const length &linewidth() const;

            frame &ticlength      (const length &l);
            frame &minor_ticlength(const length &l);

	    // ---------  Set grid ------------------------------------
	    // what = axis::x1, axis::x2, axis::y1, axis::y2 or
	    // a bitwise or (|) of them

	    frame &grid(int what);
            frame &grid(bool i);
	    int  grid() const { return grid_; }
            frame &grid_at_minor_tics(int what);
            int    grid_at_minor_tics() const { return grid_at_minor_tics_; }
	    static void default_grid(int);
	    static void default_grid(bool);
            static void default_grid_at_minor_tics(int);
            static void default_grid_at_minor_tics(bool);

            // define a parametric grid
            frame &grid(const function &f, double p1from, double p1to, int n1, double p2from, double p2to, int n2);
            frame &grid(const function &fx, const function &fy, double p1from, double p1to, int n1, double p2from, double p2to, int n2)
            {
                return grid(function(fx,fy),p1from,p1to,n1,p2from,p2to,n2);
            }

	    // --------  Set or get the color of the grid    ----------

	    frame &gridcolor(const color &);
            frame &gridcolor(double r, double g, double b) { return gridcolor(color(r,g,b)); }
	    const color &gridcolor() const { return gridcolor_; }
            static void default_gridcolor(const color &c) { default_gridcolor_ = c; }
            static void default_gridcolor(double r, double g, double b) { default_gridcolor(color(r,g,b)); }

            frame &minor_gridcolor(const color &);
            frame &minor_gridcolor(double r, double g, double b) { return minor_gridcolor(color(r,g,b)); }
            const color &minor_gridcolor() const { return minor_gridcolor_; }
            static void default_minor_gridcolor(const color &c) { default_minor_gridcolor_ = c; }
            static void default_minor_gridcolor(double r, double g, double b) { default_minor_gridcolor(color(r,g,b)); }
            

	    // --------  Set or get the color of the frame  --------------

	    frame &framecolor(const color &);
            frame &framecolor(double r, double g, double b) { return framecolor(color(r,g,b)); }
	    const color &framecolor() const;
	    static void default_framecolor(const color &);
            static void default_framecolor(double r, double g, double b) { default_framecolor(color(r,g,b)); }


	    // ---------  Set or get linestyle of grid ------------------
	    // which can be 'solid', 'dashed', 'dotted'

	    frame& gridstyle(sym::linestyle);
	    sym::linestyle gridstyle() const { return gridstyle_; }
            static void default_gridstyle(sym::linestyle s) { default_gridstyle_ = s; }

            frame &minor_gridstyle(sym::linestyle);
            sym::linestyle minor_gridstyle() const { return minor_gridstyle_; }
            static void default_minor_gridstyle(sym::linestyle s) { default_minor_gridstyle_ = s; }


	    // -----------  Get the number of graphs, and the 'n'th graph (0-based) --------

	    int    ngraphs() const { return graphs_.size(); }
	    plottable *get_graph(int n);
	    dgraph *lastd();
	    fgraph *lastf();
	    plottable *last();

	    // ------------ Get pointer to the axes -----------------------------------------

	    axis *x1axis() {return x1axis_;}
	    axis *x2axis() {return x2axis_;}
	    axis *y1axis() {return y1axis_;}
	    axis *y2axis() {return y2axis_;}


	    // ----------- Specify an external axis to be used ------------------------------

	    frame &x1axis(axis *a) { x1axis_ = a; owns_x1axis_ = false; return *this;}
	    frame &x2axis(axis *a) { x2axis_ = a; owns_x2axis_ = false; return *this;}
	    frame &y1axis(axis *a) { y1axis_ = a; owns_y1axis_ = false; return *this;}
	    frame &y2axis(axis *a) { y2axis_ = a; owns_y2axis_ = false; return *this;}


	    // ---------------- Draw in foreground? -------------------------------
	    // specify if the frame (axes) or the grid should be drawn in the
	    // foreground (that is, over the graphs and all contents of the frame)

	    frame &foreground(bool);
	    bool foreground() const;
	    static void default_foreground(bool);

	    frame &grid_foreground(bool b) { grid_foreground_ = b; return *this; }
	    bool grid_foreground() const { return grid_foreground_; }
	    static void default_grid_foreground(bool b) { default_grid_foreground_ = b; }


	    // -------  switch on/off drawing the frame --------------------------------------

	    frame &draw_frame(int b) {draw_frame_ = b; return *this;}
	    int    draw_frame() const { return draw_frame_; }
	    static void default_draw_frame(int b) { default_draw_frame_ = b; }

	    // -------  switch on/off drawing the axes ---------------------------------------
	    frame &draw_axes(int b) { draw_axes_ = b; return *this; }
	    frame &draw_axes(bool b)
	    {
		if(b) draw_axes_ = axis::x1|axis::x2|axis::y1|axis::y2;
		else draw_axes_ = 0;
		return *this;
	    }
	    bool   draw_axes() const { return draw_axes_; }
	    static void default_draw_axes(int b) { default_draw_axes_ = b; }
	    static void default_draw_axes(bool b)
	    {
		if(b) default_draw_axes_ = axis::x1|axis::x2|axis::y1|axis::y2;
		else default_draw_axes_ = 0;
	    }


	    bool modified() const;
	    void modified(bool f);

	    void cd_specific();

	    // inherited from 'grob' 
	    void print(terminal *);
	    void prepare_for_draw();

	    // a static vector, which keeps the pointers of
	    // all existing frames
	    static std::vector<blop::frame*> &all();

	    // remove a given plottable from all existing frames
	    static void remove_from_all(plottable *);

	    frame &layer(const var & l) { grob::layer(l); return *this; }

	};

}

#ifdef __MAKECINT__
#pragma link off function blop::frame::prepare_for_draw();
#endif


#endif
