#ifndef __BLOP_AXIS_H__
#define __BLOP_AXIS_H__

#include "label.h"
#include "var.h"
#include "constants.h"
#include "color.h"
#include "tic.h"
#include "function.h"
#include "units.h"
#include <utility>

namespace blop
{

    class axis  : public grob
	{
	private:
	    axis *transformed_axis_;
	    function transform_, transform_inverse_;
	    bool own_tic_calculation_;

	    vector<pair<double,double> > cuts_;
	    length cut_x1_,cut_x2_,cut_x3_,cut_x4_,cut_y1_,cut_y2_,cut_y3_,cut_y4_;
	    length cut_gap_;

	    static length &default_cut_gap_();

	    void print_cuts(terminal *);

	    color axiscolor_;
	    static color default_axiscolor_;

	    double unit_value_;   // the numerical value of the unit used for the axis
	    string unit_symbol_;  // the symbol (LaTeX) of the unit (the user-supplied string like Ohm*A parsed, transformed)
	    string unit_format_;  // the format, how the symbol should be appended to the title

#ifndef __CINT__
	    friend class frame;
	    friend class mframe;
#endif

	    double           scale_;
	    label            scalelabel_;

	    // band limits of the decorations
	    length bl1_,bl2_,bl3_,bl4_;

	    int              id_;
	    bool             logscale_;
	    
	    label            title_;
	    bool             title_autoset_;

	    std::vector<blop::tic>      tics_,minor_tics_;
	    string           tics_format_,minor_tics_format_;
            blop::function   tics_format_func_;
	    length           ticlength_,minor_ticlength_;
            static length    &default_ticlength_();
            static length    &default_minor_ticlength_();
	    bool             draw_tics_,draw_minor_tics_;
            static bool      default_draw_tics_;
            int              n_minor_tics_;

	    double           min_, max_;
	    bool             min_fixed_,max_fixed_;

	    double tic_start_,tic_incr_,tic_end_;
	    bool   tic_start_fixed_, tic_incr_fixed_, tic_end_fixed_;
	    std::vector<blop::tic> user_tics_;

	    length labelsep_,titlesep_,label_maxsize_,label_maxdist_,
		title_axis_sep_,script_size_;

	    length beg_,end_,pos_;
	    bool pos_changed_;

	    int autoextend_min_, autoextend_max_;

	    void operator= (const axis &);

            bool symmetric_range_;
            static bool default_symmetric_range_;

	public:
	    //html <a name='tics'> </a>
	    class tic_setter
		{
		private:
		    axis *axis_;
		public:
		    tic_setter(axis *a) : axis_(a) {}
		    blop::axis::tic_setter &operator()();
		    blop::axis::tic_setter &operator()(var);
		    blop::axis::tic_setter &operator()(var,var);
		    blop::axis::tic_setter &operator()(var,var,var);
		};

	    tic_setter tics;

                // This is a function-like class, which defines the parenthesis
                // operators. These operators return a reference to the same
                // class, so they can be called subsequently, providing a very
                // user-friendly interface. Every axis contains one instance
                // of this class as a member (called 'tics').

	    //html <a name='color'> </a>
	    // ------------------- color of the axis -----------------
	    axis &axiscolor(const color &);
	    const color &axiscolor() const;
	    static void default_axiscolor(const color &);


	    //html <a name='ticlength'>
	    // ------------- ticlength  ----------------------------
	    // set or get the length of the major tics of this axis

	    axis &ticlength(const length &l) {ticlength_ = l; return *this;}
	    const length &ticlength() const  {return ticlength_;}

            // Set the default values (for all axes created after this point)
            static void default_ticlength(const length &);
            static void default_minor_ticlength(const length &);

	    // set or get the length of the minor tics of this axis
	    axis &minor_ticlength(const length &l) {minor_ticlength_ = l; return *this;}
	    const length &minor_ticlength() const {return minor_ticlength_;}
            //html </a>

	    //html <a name='ticformat'> </a>
	    // ---------------- ticformat --------------------------
	    // set or get the format of the major ticlabels (this is the
	    // format string of a printf command)
	    
	    axis &ticformat(const var &f) 
            {
                tics_format_ = f.str();
                tics_format_func_ = unset;
                return *this;
            }
            axis &ticformat(const blop::function &f)
            {
                tics_format_ = "";
                tics_format_func_ = f;
                return *this;
            }
	    var   ticformat() const { return tics_format_; }

	    // set or get the format of the minor ticlabels (this is the
	    // format string of a printf command)

	    axis &minor_ticformat(const var &f) {minor_tics_format_ = f.str(); return *this;}
	    var   minor_ticformat() const { return minor_tics_format_; }

            
            // --------------------------------------------------------------------

            axis &title_pos(sym::position pos);
            axis &title_pos(sym::position pos, sym::position align);

	    // --------------------------------------------------------------------
	    // these functions control if the major and minor
	    // tics are printed    

            //html <a name="draw_tics">
	    axis &draw_tics(bool i = true) {draw_tics_ = i; return *this;}
	    axis &draw_minor_tics(bool i=true) {draw_minor_tics_ = i; return *this;}
            static void default_draw_tics(bool f);
            //html </a>

	    // --------------------------------------------------------------------
	    // these functions control the number of minor tic intervals between major ones

            //html <a name="n_minor_tics">
            axis &n_minor_tics(int n) { n_minor_tics_ = n; return *this; }
            //html </a>

            // ---------------------------------------------------------------------
            // Force the axis range to be symmetric around 0

            //html <a name="symmetric_range">
            axis &symmetric_range(bool s) { symmetric_range_ = s; return *this; }
            static void default_symmetric_range(bool s);
            //html </a>

	    // ------------------------------------------------------------
	    // set or get the separating gap between the title
	    // and the ticlabels (if ticlabels exist), or between
	    // the title and the axis itself, if no ticlabels
	    // are shown

	    const length &titlesep()  const     {return titlesep_;}
	    axis &titlesep(const length &l)     {titlesep_ = l; return *this;}


	    // --------------------------------------------------------
	    // Set or get the distance between the axis and
	    // the axis title. By default this is a derived
	    // length calculated from the tic labels' sizes, 
	    // and from the 'labelsep' (distance between the 
	    // axis and the tic labels) and 'titlesep' (see
	    // above

	    const length &titleaxissep() const  {return title_axis_sep_;}
	    axis &titleaxissep(const length &l);


	    // ---------------------------------------------------
	    // set or get the distance between the axis and
	    // the tic labels

	    const length &labelsep() const     {return labelsep_;}
	    axis &labelsep(const length &l) {labelsep_ = l; return *this;}


	    // -------------------------------------------------------
	    // get the size (width, if y1 or y2 axis, or height, 
	    // if x1 or x2 axis) of all the scripts decorating
	    // this axis (ticlabels, title and the separating
	    // gaps between them)

	    const length &scriptsize() const {return script_size_;}
	    

	    //html <a name='logscale'> </a>
	    // ----------------  set/get logscale -----------------
	    // If the user changes to logscale, and a previously set
	    // minimum is negative or 0, then this minimum is released
	    // in order to avoid the warning message appearing with
	    // negative values on a logscale

	    axis        &logscale(bool l);
	    bool         logscale() const {return logscale_;}

	    //html <a name='unit'> </a>
	    // ---------------- set the unit ------------------------
	    axis &unit(const var &u, const var &format=unit::default_format());

	    //html <a name='scale'> </a>
	    // ---------------- set/get scale factor ---------------
	    // set or get the scaling factor of this axis (a scaling
	    // factor of 100 for example means, that every ticlabel 
	    // shown on this axis is divided by 100, and x10^2 is 
	    // shown at the end of the axis

	    axis &scale(double s);
	    double scale() const;


	    // ----------------  set/release range -----------------
	    // set the ranges of the axis. 'unset' is also a valid
	    // value, which releases the range

	    double min() {return min_;}
	    double max() {return max_;}


	    axis         &range(double a,double b);
	    axis         &min(double d);
	    axis         &max(double d);

	    bool          min_fixed() const {return min_fixed_;}
	    bool          max_fixed() const {return max_fixed_;}

	    axis         &autoextend_min(bool f = true);
	    axis         &autoextend_max(bool f = true);
	    axis         &autoextend    (bool f = true);

	    axis         &autoextend_min_soft(bool f = true);
	    axis         &autoextend_max_soft(bool f = true);

	    // ----------  clear_autosettings --------------------------
	    // clear all settings, which were not set directly by the user.
	    // the range is for example not cleared by this function, if it
	    // has been set via the min(double a), max(double b) or range(double a,double b)
	    // function, where 'a' or 'b' are not 'unset'

	    axis         &clear_autosettings();


	    // ----------  clear ---------------------------------------
	    // This could also be called 'reset'. Sets the axis
	    // to its default status, every user settings are
	    // cleared as well

	    axis         &clear();


	    // -----------------  extend_range --------------------
	    // extend the range of the axis such that it
	    // includes the provided value, 'a'

	    axis         &extend_range(double a);


	    // -------------------  titlebox -------------------------
	    // return a pointer to the title of this axis as a titlebox
	    // (see label.h for more details). You can call the member
	    // functions of 'label' on the returned pointer to set
	    // any properties of the title

	    label       *titlebox();


	    // -------------------  get/set axis title ---------------
	    // set or get the title (the text which appears) of this axis
	    // The 'auto' flag can be used to indicate whether this is an
	    // automatically set title (by some automatic mechanism, like
	    // reading in column titles from the datafile)

	    axis         &title(const var &s, bool autoset = false);
	    const label  &title() const { return title_; }
            label        &title() { return title_; }
	    bool          title_autoset() const { return title_autoset_; }
	    



	    // ---------  Cut out regions from the axis  --------------

	    axis         &cut(double low, double high);
	    axis         &cut();  // reset: clear cut-out regions

	    axis         &cut_gap(length l);
	    const length &cut_gap() const;
	    static void   default_cut_gap(length l);


	    // ---------  Show transformed values of another axis -----

	    axis         &transform(axis *orig,
				    const function &f=_1, const function &finverse=_1,
				    bool own_tic_calculation = false);


	    // --- and finally some undocumented functions, which will probably
	    // --- not be called by the user

	    length &begin() {return beg_;}
	    length &end() {return end_;}

	    void print(terminal *t);
	    void print(terminal *t, bool print_line);
	    void print_tics(terminal *t,const length &p1, const length &p2,
			    const length &p,bool mirror = false);
	    void print_line(terminal *);
	    void print_ticlabels(terminal *);
	    void print_title(terminal *);

	    double map_point(double);

	    axis         &calculate_tics();

	    void calculate_lengths();

	    void prepare_for_draw();


	    axis& pos(const length &pos, const length &begin,const length &end);
	    axis& pos(const length &pos);
	    bool pos_changed() const { return pos_changed_; }

	    enum {x1=1,x2=2,y1=4,y2=8};

	    axis(int id);
	    axis(const axis &o);
	    virtual ~axis();

	    int id() const {return id_;}

	    friend class axis::tic_setter;

	    axis &layer(const var & l) { grob::layer(l); return *this; }
	};

    class tic_setter
	{
	private:
	    int axis_;
	    axis *get_axis();
	public:
	    tic_setter(int i);
	    tic_setter &operator()();
	    tic_setter &operator()(var);
	    tic_setter &operator()(var,var);
	    tic_setter &operator()(var,var,var);
	};


}
#endif
