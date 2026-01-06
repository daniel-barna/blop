#include "color_legend.h"
#include "graph_drawer.h"
#include "warning.h"
#include "plottable.h"
#include "frame.h"
#include "global.h"
#include <limits>

namespace blop
{
    using namespace std;

    length color_legend::default_sample_width_ = 3*length(length::base_id_t(terminal::EX));
    void color_legend::default_sample_width(const length &l) {default_sample_width_ = l;}

    color_legend &color_legend::mknew()
    {
	color_legend *l = new color_legend;
	l->autodel(true);
	//frame::current().rmarginobject(l);
	pad::current().add(l);
	l->right(1.0);
	return *l;
    }

    void color_legend::init_()
    {
	name("color_legend");
	box::bottom(0.0);
	box::top(1.0);
	box::left(0.0);
	sample_width_ = default_sample_width_;
	sample_right_ = !sample_width_;
	tic_left_ = !sample_right_ - length(length::base_id_t(terminal::EX));
	sep_ = EX;
	ticlabel_pos_ = !sample_right_+!sep_;
	// ticlabel_width_ = to be set up later;
        ticlabel_width_fixed_ = false;
	box::width(!sample_width_ + !sep_ + !ticlabel_width_);
	//box::fix_width(true);
    }

    color_legend::color_legend()
    {
	init_();
    }

    color_legend::color_legend(graphd_colorscale *d)
    {
	init_();
	// register the colorscale as owner
	owners_.push_back(d);
    }

    color_legend::color_legend(const color_legend &rhs)
    {
    }

    const color_legend &color_legend::operator= (const color_legend &rhs)
    {
	return rhs;
    }


    void color_legend::remove_owner(graphd_colorscale *o)
    {
	for(unsigned int i=0; i<owners_.size(); ++i)
	{
	    if(owners_[i] == o)
	    {
		owners_.erase(owners_.begin()+i);
		--i;
	    }
	}
    }

    double color_legend::min() const
    {
	return min_;
    }
    double color_legend::max() const
    {
	return max_;
    }

    void color_legend::clear_tics()
    {
	colortics_.clear();
    }

    void color_legend::calculate_tics()
    {
	if(global::debug>0) cout<<"[blop] [color_legend] calculate_tics starts..."<<endl;
	// Tics have already been calcualted, so simply return
	if(!colortics_.empty())
	{
	    if(global::debug>0)
	    {
		cout<<"[blop] [color_legend]   tics have already been calculated"<<endl;
		cout<<"[blop] [color_legend] calculate_tics finished."<<endl;
	    }
	    return;
	}

	min_ = numeric_limits<double>::max();
	max_ = -numeric_limits<double>::max();
	bool cmin_fixed = false;
	bool cmax_fixed = false;
	bool clogscale  = false;
	for(unsigned int i=0; i<owners_.size(); ++i)
	{
	    if(!cmin_fixed && owners_[i]->color_min()<min_)
	    {
		min_ = owners_[i]->color_min();
	    }
	    if(owners_[i]->color_min_fixed()) cmin_fixed = true;
	    if(!cmax_fixed && owners_[i]->color_max()>max_)
	    {
		max_ = owners_[i]->color_max();
	    }
	    if(owners_[i]->color_max_fixed()) cmax_fixed = true;
	    if(owners_[i]->color_logscale()) clogscale = true;
	}

	if(min_ == unset || !finite(min_) || max_ == unset || !finite(max_))
	{
	    // if there are multiple owners, it can easily be that they are plotted
	    // in different frames, i.e. one of these owner graphs already reaches pass=2
	    // where it tries to set the color-range/color-tics, whereas the other graph(s)
	    // - being in a different frame - have not yet been set up yet, i.e. give
	    // unreal values for min/max. Therefore only warn if there is one owner.
	    // This is a stupid workaround, not a real solution!

	    if(owners_.size()<=1) warning::print(var("Problem with color-range [") & min_ & var(" .. ") & max_ & var("]"), "color_legend::calculate_tics()");

	    if(min_ == unset || !finite(min_))
	    {
		if(max_!=unset && finite(max_)) min_ = max_-10;
		else min_ = 0;
	    }
	    if(max_ == unset || !finite(max_))
	    {
		if(min_!=unset && finite(min_)) max_ = min_+10;
		else max_ = 10;
	    }
	}

	vector<pair<double,double> > cuts_dummy;
	double delta = unset;
	tic scaletic;
	scaletic.value(unset);
	blop::calculate_tics(min_, cmin_fixed,
			     max_, cmax_fixed,
			     delta, false,     // tic incrementation, fixed(=false)
			     unset, unset,
			     scaletic,
			     1.0,
			     cuts_dummy,
			     clogscale,   // logscale
			     clogscale,   // normalform tics
			     true,         // normalform scale
			     colortics_);
        if(!ticlabel_width_fixed_) ticlabel_width_ = maxwidth(colortics_);
	if(global::debug>0) cout<<"[blop] [color_legend] calculate_tics finished."<<endl;
    }

    color_legend &color_legend::ticlabel_width(const length &l)
    {
        ticlabel_width_fixed_ = true;
        ticlabel_width_ = l;
        return *this;
    }

    color_legend &color_legend::title(const var &t)
    {
	title_.text(t).angle(90*unit::deg)
	    .left(!ticlabel_pos_+!ticlabel_width_+!sep_)
	    .ycenter(0.5*!height()); //!ycenter());
	if(t.str() != "")
	{
	    box::width(!sample_width_ + 2*!sep_ + !ticlabel_width_ + !title_.width());
	}
	else
	{
	    box::width(!sample_width_ + !sep_ + !ticlabel_width_);
	}
	return *this;
    }
    const var &color_legend::title() const
    {
	return title_.text();
    }
    
    color_legend &color_legend::sample_width(const length &l)
    {
	sample_width_ = l;
	return *this;
    }

    void color_legend::print(terminal *term)
    {
	if(print_me_<2) return;
	if(owners_.empty())
	{
	    warning::print("No owners of this color_legend",
			   "color_legend::print(terminal*)");
	    return;
	}

	term->open_layer(layer_);
        term->subpicture_begin(terminal::coord(left().termspecific_id(), bottom().termspecific_id()),
                               terminal::coord(right().termspecific_id(), top().termspecific_id()));

	
	// first draw the color samples
	int csamples = 0;
	bool clogscale=false;
	for(unsigned int i=0; i<owners_.size(); ++i)
	{
	    // take the maximum of the color-samples from the owners
	    if(owners_[i]->color_samples()>csamples) csamples=owners_[i]->color_samples();

	    // make color-logscale if any of the owners have this flag set
	    if(owners_[i]->color_logscale()) clogscale = true;
	}
	vector<terminal::coord> c;
	for(int i=0; i<csamples; ++i)
	{
	    c.clear();
	    const double pos = double(i)/csamples;
	    const double val = (clogscale==false ? min_ + (max_-min_)*pos :
				::pow(10,pos*(::log10(max_)-
					      ::log10(min_))+ ::log10(min_))  );
	    
	    c.push_back(terminal::coord(terminal::id(0.0,1),        terminal::id((double)i/csamples,2)));
	    c.push_back(terminal::coord(sample_right_.termspecific_id(), terminal::id((double)i/csamples,2)));
	    c.push_back(terminal::coord(sample_right_.termspecific_id(), terminal::id(double(i+1)/csamples,2)));
	    c.push_back(terminal::coord(terminal::id(0.0,1),        terminal::id(double(i+1)/csamples,2)));
	    c.push_back(terminal::coord(terminal::id(0.0,1),        terminal::id(double(i)/csamples,2)));
	    term->set_color(owners_[0]->map_color(val,min_,max_));
	    term->fill_polygon(c);
	}

	term->set_color(black);
	term->set_linewidth(LW.termspecific_id());
	
	// Then draw a surrounding box around the color samples
	c.clear();
	c.push_back(terminal::coord(terminal::id(0.0,1), terminal::id(0,2)));
	c.push_back(terminal::coord(sample_right_.termspecific_id(),terminal::id(0,2)));
	c.push_back(terminal::coord(sample_right_.termspecific_id(),terminal::id(1,2)));
	c.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(1,2)));
	c.push_back(terminal::coord(terminal::id(0.0,1),terminal::id(0,2)));
	term->draw_lines(c);
	
	// and finally draw the tics + ticlabels
	for(unsigned int i=0; i<colortics_.size(); ++i)
	{
	    double pos = 0;
	    if(!clogscale) pos = (colortics_[i].value()-min_)/(max_-min_);
	    else           pos =
			       (::log10(colortics_[i].value())-::log10(min_))/
			       (::log10(max_)- ::log10(min_));
	    term->draw_text(terminal::coord(ticlabel_pos_.termspecific_id(),
					    terminal::id(pos, 2)),
			    colortics_[i].label().str(),
			    sym::left, sym::center, 0);
	    term->draw_line(terminal::coord(tic_left_.termspecific_id(),
					    terminal::id(pos, 2)),
			    terminal::coord(sample_right_.termspecific_id(),
					    terminal::id(pos, 2)));
	}
	title_.print(term);

        term->subpicture_end();
	term->close_layer(layer_);
    }

    
    void color_legend::prepare_for_draw()
    {
	if(print_me_<1) return;
	box::prepare_for_draw();
	sample_right_.register_me();
	tic_left_.register_me();
	ticlabel_pos_.register_me();
	title_.prepare_for_draw();
    }

    color_legend::~color_legend()
    {
	// Clear the link to myself in all of the owners
	for(unsigned int i=0; i<owners_.size(); ++i)
	{
	    owners_[i]->colorlegend_ = 0;
	}
    }
}

