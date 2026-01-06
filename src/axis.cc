#include "axis.h"
#include "frame.h"
#include <cmath>
#include <cstdio>
#include "exc.hh"
#include "warning.h"
#include <algorithm>

namespace blop
{

    void axis::operator=(const axis &) {cerr<<"this shouldn't be called"<<endl; exit(1);}

    color axis::default_axiscolor_(0,0,0);

    bool axis::default_symmetric_range_ = false;
    bool axis::default_draw_tics_ = true;
    void axis::default_draw_tics(bool f) {default_draw_tics_ = f;}

    void axis::default_symmetric_range(bool s)
    {
        axis::default_symmetric_range_ = s;
    }

    length &axis::default_ticlength_()
    {
        static length l = 2*MM;
        return l;
    }

    length &axis::default_minor_ticlength_()
    {
        static length l = MM;
        return l;
    }

    void axis::default_ticlength(const length &l)
    {
        default_ticlength_() = l;
    }

    void axis::default_minor_ticlength(const length &l)
    {
        default_minor_ticlength_() = l;
    }

    axis &axis::transform(axis *orig,
			  const function &f, const function &finverse, bool own_tic_calc)
    {
	transformed_axis_ = orig;
	transform_ = f;
	transform_inverse_ = finverse;
	own_tic_calculation_ = own_tic_calc;
	return *this;
    }

    axis &axis::axiscolor(const color &c)
    {
	axiscolor_ = c;
	title_.textcolor(c);
	scalelabel_.textcolor(c);
	modified_ = true;
	return *this;
    }

    const color &axis::axiscolor() const {return axiscolor_;}

    void axis::default_axiscolor(const color &c) {default_axiscolor_ = c;}

    axis &axis::autoextend_min(bool f)
    {
	if(transformed_axis_)
	{
	    transformed_axis_->autoextend_min(f);
	}
	if(f == true) autoextend_min_ = 2;
	else autoextend_min_ = -2;
	return *this;
    }

    axis &axis::autoextend_max(bool f)
    {
	if(transformed_axis_)
	{
	    transformed_axis_->autoextend_max(f);
	}
	if(f == true) autoextend_max_ = 2;
	else autoextend_max_ = -2;
	return *this;
    }

    axis &axis::autoextend(bool f)
    {
	if(transformed_axis_)
	{
	    transformed_axis_->autoextend(f);
	}
	if(f == true) autoextend_min_ = autoextend_max_ = 2;
	else autoextend_min_ = autoextend_max_ = -2;
	return *this;
    }

    axis &axis::autoextend_min_soft(bool f)
    {
	if(transformed_axis_)
	{
	    transformed_axis_->autoextend_min_soft(f);
	}
	if(autoextend_min_ != 2 && autoextend_min_ != -2)
	{
	    if(f == true) autoextend_min_ = 1;
	    else autoextend_min_ = -1;
	}
	return *this;
    }

    axis &axis::autoextend_max_soft(bool f)
    {
	if(transformed_axis_)
	{
	    transformed_axis_->autoextend_max_soft(f);
	}
	if(autoextend_max_ != 2 && autoextend_max_ != -2)
	{
	    if(f == true) autoextend_max_ = 1;
	    else autoextend_max_ = -1;
	}
	return *this;
    }


    void axis::prepare_for_draw()
    {
	if(print_me_ < 1) return;
	if(draw_tics_) ticlength_.register_me();
	if(draw_minor_tics_) minor_ticlength_.register_me();
	beg_.register_me();
	end_.register_me();
	pos_.register_me();
	bl1_.register_me();
	bl2_.register_me();
	if(title_.text().str() != "") title_.prepare_for_draw();
	scalelabel_.prepare_for_draw();
	if(!cuts_.empty())
	{
	    cut_gap_.register_me();
	    cut_x1_.register_me();
	    cut_x2_.register_me();
	    cut_x3_.register_me();
	    cut_x4_.register_me();
	    cut_y1_.register_me();
	    cut_y2_.register_me();
	    cut_y3_.register_me();
	    cut_y4_.register_me();
	}
    }


    axis& axis::pos(const length &p,
		   const length &l2,const length &l3)
    {
	pos_ = p;
	beg_ = l2;
	end_ = l3;
        return *this;
    }

    axis& axis::pos(const length &p)
    {
	pos_ = p;
	pos_changed_ = true;
        return *this;
    }

    axis &axis::title_pos(sym::position pos)
    {
        title_pos(pos,pos);
        return *this;
    }

    axis &axis::title_pos(sym::position pos, sym::position align)
    {
        if(id_==x1 || id_==x2)
        {
            if     (pos==sym::center) title_.x(0.5*(!beg_+!end_),align);
            else if(pos==sym::left)   title_.x(!beg_,align);
            else if(pos==sym::right)  title_.x(!end_,align);
            else warning::print("Bad position for axis title");
            return *this;
        }
        if(id_==y1 || id_==y2)
        {
            if     (pos==sym::bottom) title_.y(!beg_,align);
            else if(pos==sym::center) title_.y(0.5*(!beg_+!end_),align);
            else if(pos==sym::top)    title_.y(!end_,align);
            else warning::print("Bad position for axis title");
            return *this;
        }
        return *this;
    }

    axis::axis(int id)  : id_(id), tics(this)
    {
	pos_changed_ = false;
	title_autoset_ = false;
	transformed_axis_ = 0;
	cut_gap_ = default_cut_gap_();
	cut_x1_ = -0.5*!cut_gap_-EX;
	cut_x2_ = -0.5*!cut_gap_+EX;
	cut_x3_ =  0.5*!cut_gap_-EX;
	cut_x4_ =  0.5*!cut_gap_+EX;
	cut_y1_ = -2*EX;
	cut_y2_ = -1*EX;
	cut_y3_ = EX;
	cut_y4_ = 2*EX;

	unit_value_  = 1;
	unit_symbol_ = "";
	unit_format_ = "";

	axiscolor_ = default_axiscolor_;
	scale_ = unset;
	scalelabel_.off();
	scalelabel_.textcolor(axiscolor_);
	if(id_ == x1)
	{
	    title_.angle(0);
	    title_.y(!pos_ - !title_axis_sep_,sym::top);
	    title_.x(0.5*(!beg_+!end_),sym::center);
	}
	if(id_ == x2)
	{
	    title_.angle(0);
	    title_.y(!pos_ + !title_axis_sep_,sym::bottom);
	    title_.x(0.5*(!beg_+!end_),sym::center);
	}
	if(id_ == y1)
	{
	    title_.angle(90*unit::deg);
	    title_.x(!pos_ - !title_axis_sep_,sym::right);
	    title_.y(0.5*(!beg_+!end_),sym::center);
	}
	if(id_ == y2)
	{
	    title_.angle(90*unit::deg);
	    title_.x(!pos_ + !title_axis_sep_,sym::left);
	    title_.y(0.5*(!beg_+!end_),sym::center);
	}

	logscale_ = false;

	title_.text("");
	title_.textcolor(default_axiscolor_);

	tics_format_ = minor_tics_format_ = "%g";

	ticlength_ = default_ticlength_();
	minor_ticlength_ = default_minor_ticlength_();

	draw_tics_ = default_draw_tics_;
	draw_minor_tics_ = false;
        n_minor_tics_ = 0;

	min_ = unset;
	max_ = unset;
	min_fixed_ = false;
	max_fixed_ = false;

	tic_start_ = tic_incr_ = tic_end_ = unset;
	tic_start_fixed_ = tic_incr_fixed_ = tic_end_fixed_ = false;

	labelsep_ = EX;
	titlesep_ = EX;
	label_maxsize_ = 0;
	label_maxdist_ = 0;
	title_axis_sep_ = !label_maxdist_ + !titlesep_;
	script_size_ = 0;

	autoextend_min_ = autoextend_max_ = 1;

	if(id != x1 && id != x2 && id != y1 && id != y2)
	{
	    warning::print("Bad id","axis::axis()");
	    exit(1);
	}
        
        symmetric_range_ = default_symmetric_range_;
    }

    axis::axis(const axis &o) : tics(this)
    {
	unit_value_  = o.unit_value_;
	unit_symbol_ = o.unit_symbol_;
	unit_format_ = o.unit_format_;
	
	pos_changed_ = o.pos_changed_;
	title_autoset_ = o.title_autoset_;
	transformed_axis_ = 0;
	cut_gap_ = o.cut_gap_;
	cut_x1_ = o.cut_x1_;
	cut_x2_ = o.cut_x2_;
	cut_x3_ = o.cut_x3_;
	cut_x4_ = o.cut_x4_;
	cut_y1_ = o.cut_y1_;
	cut_y2_ = o.cut_y2_;
	cut_y3_ = o.cut_y3_;
	cut_y4_ = o.cut_y4_;

	scale_ = o.scale_;
	id_ = o.id_;
	logscale_          = o.logscale_;

	title_             = o.title_;

	tics_              = o.tics_;
	minor_tics_        = o.minor_tics_;
	tics_format_       = o.tics_format_;
        tics_format_func_  = o.tics_format_func_;
	minor_tics_format_ = o.minor_tics_format_;
	ticlength_         = o.ticlength_;
	minor_ticlength_   = o.minor_ticlength_;
	draw_tics_         = o.draw_tics_;
	draw_minor_tics_   = o.draw_minor_tics_;

	min_ = o.min_;
	max_ = o.max_;
	min_fixed_ = o.min_fixed_;
	max_fixed_ = o.max_fixed_;

	tic_start_ = o.tic_start_;
	tic_incr_  = o.tic_incr_;
	tic_end_   = o.tic_end_;
	tic_start_fixed_ = o.tic_start_fixed_;
	tic_incr_fixed_  = o.tic_incr_fixed_;
	tic_end_fixed_   = o.tic_end_fixed_;
	user_tics_       = o.user_tics_;

	cerr<<"The length copying should be checked"<<endl;
	labelsep_ = o.labelsep_; 
	titlesep_ = o.titlesep_;
	label_maxsize_ = o.label_maxsize_;
	label_maxdist_ = o.label_maxdist_;
	title_axis_sep_ = !label_maxdist_ + titlesep_;

	autoextend_min_ = o.autoextend_min_;
	autoextend_max_ = o.autoextend_max_;
    }

    axis::~axis() {}

    axis &axis::min(double a)
    {
	min_ = a;
	if(a == unset) min_fixed_ = false;
	else min_fixed_ = true;
	if(transformed_axis_)
	{
            if(a == unset) transformed_axis_->min(unset);
	    else transformed_axis_->min(transform_inverse_(a).dbl());
	}
	modified_ = true;
	return *this;
    }

    axis &axis::max(double a)
    {
	max_ = a;
	if(a == unset) max_fixed_ = false;
	else max_fixed_ = true;
	if(transformed_axis_)
	{
            if(a == unset) transformed_axis_->max(unset);
	    else transformed_axis_->max(transform_inverse_(a).dbl());
	}
	modified_ = true;
	return *this;
    }

    axis &axis::range(double a,double b)
    {
	min(a);
        max(b);
	modified_ = true;
	return *this; 
    }

    void axis::calculate_lengths()
    {
	if(tics_.empty())
	{
	    label_maxsize_ = 0;
	    label_maxdist_ = 0;
	}
	else
	{
	    vector<var> l;
	    for(unsigned int i=0; i<tics_.size(); ++i)
	    {
		l.push_back(tics_[i].label());
	    }
	    
	    if(id_ == x1 || id_ == x2)
	    {
		label_maxsize_ = maxlheight(l);
	    }
	    else
	    {
		label_maxsize_ =  maxwidth(l);
	    }
	    label_maxdist_ = !labelsep_ + !label_maxsize_;
	}

	if(id_ == x1 || id_ == y1)
	{
	    if(tics_.empty()) bl1_ = !pos_;
	    else bl1_ = !pos_ - !labelsep_;

	    bl2_ = !pos_ - !label_maxdist_;
	    bl3_ = !pos_ - !title_axis_sep_;
	    if(id_ == x1) bl4_ = !bl3_ - !title_.height();
	    else bl4_ = !bl3_ - !title_.width();
	}
	else
	{
	    if(tics_.empty()) bl1_ = !pos_;
	    else bl1_ = !pos_ + !labelsep_;

	    bl2_ = !pos_ + !label_maxdist_;
	    bl3_ = !pos_ + !title_axis_sep_;
	    if(id_ == x2) bl4_ = !bl3_ + !title_.height();
	    if(id_ == x2) bl4_ = !bl3_ + !title_.width();
	}

	if(title_.text().str() == "")
	{
	    script_size_ = !label_maxdist_;
	}
	else
	{
	    if(id_ == x1 || id_ == x2)
	    {
		script_size_ = !title_axis_sep_ + title_.height();
	    }
	    else
	    {
		script_size_ = !title_axis_sep_ + title_.width();
	    }
	}
    }

    label *axis::titlebox()
    {
	return &title_;
    }

    axis &axis::title(const var &s, bool autoset)
    {
	title_autoset_ = autoset;
	title_.text(s.str());
	if(id_ == x1 || id_ == x2) title_.angle(0);
	else title_.angle(90*unit::deg);
	if(unit_symbol_ != "" && unit_format_ != "")
	{
	    var f = replace(var("%s"),var(unit_symbol_),var(unit_format_));
	    title_.text(title_.text() & " " & f);
	}
	calculate_lengths();
	modified_ = true;
	return *this;
    }


    axis &axis::clear()
    {
	transformed_axis_ = 0;

	user_tics_.clear();

	draw_tics_ = true;

	logscale_ = false;
	title_.text("");
	tics_.clear();
	minor_tics_.clear();

	min_fixed_ = max_fixed_ = false;
	min_ = max_ = unset;
	tic_start_ = tic_incr_ = tic_end_ = unset;
	tic_start_fixed_ = false;
	tic_incr_fixed_ = false;
	tic_end_fixed_ = false;
	return *this;
    }

    axis &axis::extend_range(double a)
    {
	if(a == unset) return *this;
	if(transformed_axis_)
	{
	    transformed_axis_->extend_range(transform_inverse_(a).dbl());
	}

	if(a == unset) return *this;
	if(min_ == unset) min_ = a;
	else
	{
	    if(! min_fixed_)
	    {
		if(a < min_) min_ = a;
	    }
	}
	if(max_ == unset) max_ = a;
	else
	{
	    if(! max_fixed_)
	    {
		if(max_ < a) max_ = a;
	    }
	}
	return *this;
    }

    axis &axis::logscale(bool l)
    {
	if(transformed_axis_)
	{
	    warning::print("Can not set logscale for a transformed axis");
	    return *this;
	}
	logscale_ = l;
	if(logscale_ == true && min_ != unset && min_ <= 0) min(unset);
	modified_ = true;
	if(l) draw_minor_tics_ = true;
	return *this;
    }

    axis &axis::clear_autosettings()
    {
	if(!min_fixed_) min_ = unset;
	if(!max_fixed_) max_ = unset;
	if(!tic_start_fixed_) tic_start_ = unset;
	if(!tic_incr_fixed_)  tic_incr_  = unset;
	if(!tic_end_fixed_)   tic_end_   = unset;
	tics_.clear();
	if(title_autoset_) title("");
	minor_tics_.clear();
	return *this;
    }

    void axis::print_cuts(terminal *t)
    {
	if(cuts_.empty()) return;

	t->set_color(axiscolor_);

	vector<terminal::coord> cuts1, cuts2;
	if(id_ == x1 || id_ == x2)
	{
	    cuts1.push_back(terminal::coord(cut_x1_.termspecific_id(),
					    cut_y1_.termspecific_id()));
	    cuts1.push_back(terminal::coord(cut_x2_.termspecific_id(),
					    cut_y2_.termspecific_id()));
	    cuts1.push_back(terminal::coord(cut_x1_.termspecific_id(),
					    cut_y3_.termspecific_id()));
	    cuts1.push_back(terminal::coord(cut_x2_.termspecific_id(),
					    cut_y4_.termspecific_id()));

	    cuts2.push_back(terminal::coord(cut_x3_.termspecific_id(),
					    cut_y1_.termspecific_id()));
	    cuts2.push_back(terminal::coord(cut_x4_.termspecific_id(),
					    cut_y2_.termspecific_id()));
	    cuts2.push_back(terminal::coord(cut_x3_.termspecific_id(),
					    cut_y3_.termspecific_id()));
	    cuts2.push_back(terminal::coord(cut_x4_.termspecific_id(),
					    cut_y4_.termspecific_id()));

	}
	else
	{
	    cuts1.push_back(terminal::coord(cut_y1_.termspecific_id(),
					    cut_x1_.termspecific_id()));
	    cuts1.push_back(terminal::coord(cut_y2_.termspecific_id(),
					    cut_x2_.termspecific_id()));
	    cuts1.push_back(terminal::coord(cut_y3_.termspecific_id(),
					    cut_x1_.termspecific_id()));
	    cuts1.push_back(terminal::coord(cut_y4_.termspecific_id(),
					    cut_x2_.termspecific_id()));

	    cuts2.push_back(terminal::coord(cut_y1_.termspecific_id(),
					    cut_x3_.termspecific_id()));
	    cuts2.push_back(terminal::coord(cut_y2_.termspecific_id(),
					    cut_x4_.termspecific_id()));
	    cuts2.push_back(terminal::coord(cut_y3_.termspecific_id(),
					    cut_x3_.termspecific_id()));
	    cuts2.push_back(terminal::coord(cut_y4_.termspecific_id(),
					    cut_x4_.termspecific_id()));
	}

	for(unsigned int i=0; i<cuts_.size(); ++i)
	{
	    double norm = map_point(cuts_[i].second); //*(1+1e-6));
	    if(norm == unset) continue;

	    int cutpos_id = t->lincombi(1-norm,beg_.termspecific_id(),
					norm, end_.termspecific_id());
	    if(id_ == x1 || id_ == x2)
	    {
		t->translate(cutpos_id,pos_.termspecific_id());
	    }
	    else
	    {
		t->translate(pos_.termspecific_id(),cutpos_id);
	    }
	    t->draw_lines(cuts1);
	    t->draw_lines(cuts2);
	    t->reset_transformation();
	}
    }


    void axis::print_tics(terminal *t,const length &p1, const length &p2,
			  const length &p3,bool mirror)
    {
	if(!draw_tics_ && !draw_minor_tics_) return;

	t->set_color(axiscolor_);

	int coord_id = t->newlength();

	if(draw_tics_)
	{
	    terminal::id ticlength_id = ticlength_.termspecific_id();
	    if( ((id_ == x1 || id_ == y1) && mirror==true) ||
		((id_ == x2 || id_ == y2) && mirror==false) )
	    {
		ticlength_id = t->lincombi(-1,ticlength_id);
	    }

	    for(vector<tic>::size_type i=0; i<tics_.size(); ++i)
	    {
		double norm = map_point(tics_[i].value());

		// skip those points, for which map_point returns unset
		// (for example those which fall into a cut
		if(norm == unset) continue;

		t->overwrite(coord_id,
			     1-norm,p1.termspecific_id(),
			     norm,  p2.termspecific_id());
		terminal::coord start;
		vector<terminal::id> dx,dy;
		if(id_ == x1 || id_ == x2)
		{
		    start.x = coord_id;
		    start.y = p3.termspecific_id();
		    dx.push_back(ZERO.termspecific_id());
		    dy.push_back(ticlength_id);
		}
		else
		{
		    start.x = p3.termspecific_id();
		    start.y = coord_id;
		    dx.push_back(ticlength_id);
		    dy.push_back(ZERO.termspecific_id());
		}
		t->draw_rlines(start,dx,dy);
	    }
	}

	if(draw_minor_tics_)
	{
	    terminal::id minor_ticlength_id = minor_ticlength_.termspecific_id();
	    if( ((id_ == x1 || id_ == y1) && mirror==true) ||
		((id_ == x2 || id_ == y2) && mirror==false) )
	    {
		minor_ticlength_id = t->lincombi(-1,minor_ticlength_id);
	    }

	    for(vector<tic>::size_type i=0; i<minor_tics_.size(); ++i)
	    {
		double norm = map_point(minor_tics_[i].value());
		if(norm == unset) continue;
		t->overwrite(coord_id,
			     1-norm,p1.termspecific_id(),
			     norm,  p2.termspecific_id());
		terminal::coord start;
		vector<terminal::id> dx,dy;
		if(id_ == x1 || id_ == x2)
		{
		    start.x = coord_id;
		    start.y = p3.termspecific_id();
		    dx.push_back(ZERO.termspecific_id());
		    dy.push_back(minor_ticlength_id);
		}
		else
		{
		    start.x = p3.termspecific_id();
		    start.y = coord_id;
		    dx.push_back(minor_ticlength_id);
		    dy.push_back(ZERO.termspecific_id());
		}
		t->draw_rlines(start,dx,dy);
	    }
	}
    }

    void axis::print_line(terminal *term)
    {
	term->set_color(axiscolor_);

	vector<terminal::coord> c;
	if(id_ == y1 || id_ == y2)
	{
	    c.push_back(terminal::coord(pos_.termspecific_id(),beg_.termspecific_id()));
	    c.push_back(terminal::coord(pos_.termspecific_id(),end_.termspecific_id()));
	}
	else
	{
	    c.push_back(terminal::coord(beg_.termspecific_id(),pos_.termspecific_id()));
	    c.push_back(terminal::coord(end_.termspecific_id(),pos_.termspecific_id()));
	}
	term->draw_lines(c);
    }

    void axis::print_title(terminal *term)
    {
	if(title_.text().str() != "")
	{
	    term->set_color(axiscolor_);
	    title_.print(term);
	}
    }

    void axis::print_ticlabels(terminal *term)
    {
	term->set_color(axiscolor_);
	scalelabel_.print(term);

	int coord_id = term->newlength();
	for(vector<tic>::size_type i=0; i<tics_.size(); ++i)
	{
	    double norm = map_point(tics_[i].value());
	    if(norm == unset) continue;
	    term->overwrite(coord_id,
			    1-norm,beg_.termspecific_id(),
			    norm,end_.termspecific_id());

	    // draw the tic label
	    int xalign = sym::left,yalign = sym::bottom;
	    terminal::id xpos,ypos;

	    //if(id_ == x1)  // default! overwrite later if needed
	    {
		xpos.length_id = coord_id;
		xalign = sym::center;
		ypos = bl2_.termspecific_id();
		yalign = sym::base;
	    }
	    if(id_ == x2)
	    {
		xpos.length_id = coord_id;
		xalign = sym::center;
		ypos = bl1_.termspecific_id();
		yalign = sym::bottom;
	    }
	    if(id_ == y1)
	    {
		xpos = bl1_.termspecific_id();
		xalign = sym::right;
		ypos.length_id = coord_id;
		yalign = sym::center;
	    }
	    if(id_ == y2)
	    {
		xpos = bl1_.termspecific_id();
		xalign = sym::left;
		ypos.length_id = coord_id;
		yalign = sym::center;
	    }
	    term->draw_text(terminal::coord(xpos,ypos),tics_[i].label().str(),
			    xalign,yalign,0);

	}
    }    

    void axis::print(terminal *term)
    {
	if(print_me_ < 2) return;
	term->open_layer(layer_);
	print_title(term);
	print_line(term);
	print_ticlabels(term);
	print_tics(term, beg_, end_, pos_);
	print_cuts(term);
	term->close_layer(layer_);
    }

    void axis::print(terminal *term, bool printline)
    {
	if(print_me_ < 2) return;
	print_title(term);
	if(printline) print_line(term);
	print_ticlabels(term);
	print_tics(term, beg_, end_, pos_);
	print_cuts(term);
    }

    double axis::map_point(double p)
    {
	if(transformed_axis_)
	{
	    return transformed_axis_->map_point(transform_inverse_(p).dbl());
	}

	if(p == unset || std::isnan(p) || std::isinf(p)) return unset;
	for(unsigned int i=0; i<cuts_.size(); ++i)
	{
	    if(cuts_[i].first < p && p < cuts_[i].second) return unset;
	}

	double effective_range = max_ - min_;
	double effective_value = p - min_;

	if(logscale_)
	{
	    effective_range = ::log10(max_) - ::log10(min_);
	    effective_value = ::log10(p)    - ::log10(min_);
	}

	for(unsigned int i=0; i<cuts_.size(); ++i)
	{
	    if(cuts_[i].first < max_ && min_ < cuts_[i].second)
	    {
		double l = std::max(cuts_[i].first,min_);
		double h = std::min(cuts_[i].second,max_);
		if(!logscale_) effective_range -= h-l;
		else effective_range -= ::log10(h)-::log10(l);
	    }

	    if(cuts_[i].first < p && min_ < cuts_[i].second)
	    {
		double l = std::max(cuts_[i].first,min_);
		double h = std::min(cuts_[i].second,p);
		if(!logscale_) effective_value -= h-l;
		else effective_value -= ::log10(h)-::log10(l);
	    }
	}

	double mapped = effective_value / effective_range;

	if(mapped == unset || std::isnan(mapped) || std::isinf(mapped)) return unset;
	return mapped;
    }

    axis &axis::calculate_tics()
    {
	if(!draw_tics_) return *this;
	tics_.clear();

	// if this is a transformed axis, synchronize the range
	// with the original axis
	if(transformed_axis_)
	{
	    transformed_axis_->calculate_tics();
	    min_ = transform_(transformed_axis_->min_).dbl();
	    max_ = transform_(transformed_axis_->max_).dbl();

	    // if it should not itself calculate its tics, then copy from the original,
	    // and return
	    if(!own_tic_calculation_)
	    {
		for(unsigned int i=0; i<transformed_axis_->tics_.size(); ++i)
		{
                    var ticlabel;
		    const double v = transform_(transformed_axis_->tics_[i].value()).dbl();
                    if(tics_format_func_.initialized())
                    {
                        ticlabel = tics_format_func_(v);
                    }
                    else
                    {
                        ticlabel = var::sprintf(tics_format_.c_str(),v);
                    }
		    tics_.push_back(tic(v,ticlabel));
		}
		calculate_lengths();
		return *this;
	    }
	}

	if(min_ == unset || max_ == unset)
	{
	    return *this;
	}
	
	if(max_ <= min_)
	{
	    if(id_ == x1) warning::print("Zero x1-range","axis::calculate_tics()");
	    if(id_ == x2) warning::print("Zero x2-range","axis::calculate_tics()");
	    if(id_ == y1) warning::print("Zero y1-range","axis::calculate_tics()");
	    if(id_ == y2) warning::print("Zero y2-range","axis::calculate_tics()");
	    if(!min_fixed_) min_ -= 1;
	    if(!max_fixed_) max_ += 1;
	}


	// make tic calculation if the user requested it (tic_start_!=unset ||
	// tic_end_!=unset || tic_incr_!=unset),
	// or if no user_tics have been specified
	if(tic_start_ != unset || tic_end_  != unset || tic_incr_  != unset ||
	   user_tics_.empty())
	   
	{
	    tic scaletic;
	    scaletic.value(scale_);
	    blop::calculate_tics(min_,
				 min_fixed_ || (transformed_axis_!=0),
				 max_,
				 max_fixed_ || (transformed_axis_!=0),
				 tic_incr_,
				 tic_incr_fixed_,     // stepfixed
				 (tic_start_fixed_?tic_start_:unset),
				 (tic_end_fixed_  ?tic_end_  :unset),
				 scaletic,
				 unit_value_,
				 //(unit_str_!=""?unit::value(unit_):1.0),
				 cuts_,
				 logscale_,
				 (logscale_?2:0), // normalform tics
				 true,      // normalform scale
				 tics_,
				 tics_format_.c_str(),
                                 tics_format_func_,
                                 "%g",
                                 5,
                                 symmetric_range_);
	    scalelabel_.text(var("$\\times$") & scaletic.label());

	    if(draw_minor_tics_ && cuts_.empty())
	    {
                if(logscale_)
                {
                    if(!tics_.empty())
                    {
                        double scale = 1;
                        if(scaletic.value() != unset) scale = scaletic.value();
                        int    expint = (int)::round(::log10(tics_[0].value()*scale*unit_value_)-1);
                        for(int m=2; m<10 && m*::pow(10.0,expint)<tics_[0].value()*scale*unit_value_; ++m)
                        {
                            double v = m*::pow(10.0,expint);
                            if(v<min_) continue;
                            minor_tics_.push_back(tic(v,""));
                        }
                    }
                    for(unsigned int i=0; i<tics_.size(); ++i)
                    {
                        double scale = 1;
                        if(scaletic.value() != unset) scale = scaletic.value();

                        const double start = tics_[i].value()/(scale*unit_value_);
                        const double stop  = (i<tics_.size()-1?tics_[i+1].value()/(scale*unit_value_):max_/(scale*unit_value_));

                        //double expdbl = ::log10(start);
                        //int    expint = (int)(0.1+::round(expdbl));
                        const double    expdbl = ::log10(start);
                        const int    expint = (int)(::round(expdbl));
                        const int    mant   = int(::pow(10.0, expdbl-expint)+0.01);

                        for(int m=mant+1; m<10 && m*::pow(10.0,expint)<stop; ++m)
                        {
                            minor_tics_.push_back(tic(m*::pow(10.0,expint)*scale*unit_value_,""));
                        }
                    }
                }
                else
                {
                    if(tics_.size() >= 2)
                    {
                        // check if they are equidistant
                        bool equidistant = true;
                        for(unsigned int i=2; i<tics_.size(); ++i)
                        {
                            if( ((tics_[i].value()-tics_[i-1].value())-(tics_[1].value()-tics_[0].value()))/(tics_[1].value()-tics_[0].value()) > 0.001)
                            {
                                equidistant = false;
                                break;
                            }
                        }
                        if(equidistant)
                        {
                            int n_minor_tic_intervals = n_minor_tics_;
                            if(n_minor_tic_intervals == 0)
                            {
                                n_minor_tic_intervals = 10;
                                int shown_tics = 0;
                                for(unsigned int i=0; i<tics_.size(); ++i)
                                {
                                    if(min_ <= tics_[i].value() && tics_[i].value() <= max_) ++shown_tics;
                                }
                                if(shown_tics > 5) n_minor_tic_intervals = 5;
                                if(shown_tics > 6) n_minor_tic_intervals = 4;
                                if(shown_tics > 12) n_minor_tic_intervals = 2;
                            }

                            for(unsigned int i=0; i<tics_.size()-1; ++i)
                            {
                                const double t1 = tics_[i].value();
                                const double t2 = tics_[i+1].value();
                                const double delta = (t2-t1)/n_minor_tic_intervals;
                                for(int m=1; m<n_minor_tic_intervals; ++m)
                                {
                                    minor_tics_.push_back(tic(t1+m*delta,""));
                                }
                            }
                            {
                                const double delta = (tics_[1].value()-tics_[0].value())/n_minor_tic_intervals;
                                for(double m=tics_[0].value()-delta; m>min_; m-=delta)
                                {
                                    minor_tics_.push_back(tic(m,""));
                                }

                            }
                            {
                                const double delta = (tics_.back().value()-tics_[tics_.size()-2].value())/n_minor_tic_intervals;
                                for(double m=tics_.back().value()+delta; m<max_; m+=delta)
                                {
                                    minor_tics_.push_back(tic(m,""));
                                }

                            }
                        }
                        else
                        {
                            cerr<<"Non-equidistant tics, can not calculate minor tics"<<endl;
                        }
                    }
                }
	    }
	}

	// add the user-tics
	if(!user_tics_.empty())
	{
	    for(unsigned int i=0; i<user_tics_.size(); ++i)
	    {
		tics_.push_back(user_tics_[i]);
	    }
	}

	for(unsigned int i=0; i<tics_.size(); ++i)
	{
	    extend_range(tics_[i].value());
	}
	calculate_lengths();

	return *this;
    }

    axis::tic_setter &axis::tic_setter::operator()()
    {
	axis_->user_tics_.clear();
	axis_->modified_ = true;
	operator()(unset,unset,unset);
	return *this;
    }

    // Set the tic incrementation (i.e. stepsize)
    axis::tic_setter &axis::tic_setter::operator()(var i)
    {
	axis_->draw_tics_ = true;
	axis_->tic_incr_ = i.dbl();
	if(i != unset) axis_->tic_incr_fixed_ = true;
	else axis_->tic_incr_fixed_ = false;
	axis_->modified_ = true;
	return *this;
    }

    // only set the increment and limits for the tics. do not create
    // user-tics, since start and end values might optionally be
    // 'unset', in which case they will be calculated from the
    // actual range of the axis. 
    axis::tic_setter &axis::tic_setter::operator()(var incr,var start,var end)
    {
	axis_->draw_tics_ = true;

	axis_->tic_start_ = start.dbl();
	if(start.dbl() != unset) axis_->tic_start_fixed_ = true;
	else axis_->tic_start_fixed_ = false;

	axis_->tic_incr_ = incr.dbl();
	if(incr.dbl() != unset) axis_->tic_incr_fixed_ = true;
	else axis_->tic_incr_fixed_ = false;

	axis_->tic_end_ = end.dbl();
	if(end.dbl() != unset) axis_->tic_end_fixed_ = true;
	else axis_->tic_end_fixed_ = false;

	axis_->modified_ = true;
	return *this;
    }

    // Set a single tic at value 'v', having label 'l'
    axis::tic_setter &axis::tic_setter::operator()(var v, var l)
    {
	// First check if at this value the user has already set
	// a label. Change this, if so.
	for(unsigned int i=0; i<axis_->user_tics_.size(); ++i)
	{
	    if(axis_->user_tics_[i].value() == v.dbl() &&
	       axis_->user_tics_[i].label().str() == l.str()) return *this;
	}

	// otherwise add this value/label pair to user_tics_
	axis_->user_tics_.push_back(tic(v.dbl(),l));
	axis_->modified_ = true;
	return *this;
    }

    axis &axis::titleaxissep(const length &l)
    {
	title_axis_sep_ = l;
	modified_ = true;
	return *this;
    }

    axis &axis::unit(const var &u, const var &format)
    {
	// First, remove any previous unit indication from the title
	if(unit_format_ != "" && title_.text().str() != "" && unit_symbol_ != "")
	{
	    string unit_indicator = replace(var("%s"),var(unit_symbol_),var(unit_format_));
	    string t = title_.text().str();
	    string::size_type pos = t.rfind(unit_indicator);
	    if(pos == t.size()-unit_indicator.size())
	    {
		t.erase(pos);
		title_.text(t);
	    }
	}

	if(!find_unit_or_cons(u,&unit_value_,&unit_symbol_))
	{
	    unit_value_  = 1;
	    unit_symbol_ = "";
	    unit_format_ = "";
	    return *this;
	}

	unit_format_ = format.str();
	if(unit_symbol_ != "" && unit_format_ != "")
	{
	    var f = replace(var("%s"),var(unit_symbol_),var(unit_format_));
	    title_.text(title_.text() & " " & f);
	}
	
	return *this;
    }

    axis &axis::scale(double s)
    {
	if(transformed_axis_)
	{
	    warning::print("Can not (yet) set scale for a transformed axis");
	    return *this;
	}
	modified_ = true;
	const double mind = 0.0000001;
	if(s == unset || ::fabs(s-1) < mind)
	{
	    scalelabel_.off();
	    scale_ = 1;
	    return *this;
	}
	scalelabel_.on();
	double exponent = ::log10(s);
	if(::fabs(::floor(exponent)-exponent) < mind)
	{
	    scale_ = ::pow(10,::floor(exponent));
	}
	else
	{
	    scale_ = s;
	}
	if(id_ == x1 || id_ == x2)
	{
	    scalelabel_.y(pos_).yalign(sym::center).x(end_ + EX);
	}
	else
	{
	    scalelabel_.x(pos_).xalign(sym::center).y(end_ + EX);
	}
	return *this;
    }

    double axis::scale() const
    {
	return scale_;
    }

    void axis::default_cut_gap(length l)
    {
	default_cut_gap_() = l;
    }

    length &axis::default_cut_gap_()
    {
	static length l = length::base_id_t(terminal::EX);
	return l;
    }

    axis &axis::cut_gap(length l)
    {
	modified_ = true;
	cut_gap_ = l;
	return *this;
    }

    const length &axis::cut_gap() const { return cut_gap_; }

    axis &axis::cut()
    {
	if(transformed_axis_)
	{
	    warning::print("Can not yet define cuts for a transformed axis");
	    return *this;
	}
	modified_ = true;
	cuts_.clear();
	return *this;
    }

    axis &axis::cut(double low, double high)
    {
	modified_ = true;
	if(low >= high)
	{
	    warning::print(var("Axis-cut low value (") & low & ") is bigger than high value (" & high & ")",
			   "axis::cut(double low, double high)");
	    return *this;
	}

	if(cuts_.empty())
	{
	    cuts_.push_back(pair<double,double>(low,high));
	    return *this;
	}

	for(unsigned int i=0; i<cuts_.size(); )
	{
	    // if no overlap, do nothing
	    if(cuts_[i].second < low || cuts_[i].first > high)
	    {
		++i;
		continue;
	    }

	    low = std::min(low,cuts_[i].first);
	    high = std::max(high,cuts_[i].second);
	    for(unsigned int j=i+1; j<cuts_.size(); ++j)
	    {
		cuts_[j-1].first = cuts_[j].first;
		cuts_[j-1].second = cuts_[j].second;
	    }
	    cuts_.pop_back();
	}

	// and finally insert it into the correct place

/*
  unsigned int i=0; 
  for(; i<cuts_.size() && high < cuts_[i].first; ++i);
  cuts_.insert(cuts_.begin()+i, pair<double,double>(low,high));
*/

	for(unsigned int i=0; i<cuts_.size(); ++i)
	{
	    if(high < cuts_[i].first)
	    {
		cuts_.insert(cuts_.begin()+i, pair<double,double>(low,high));
		return *this;
	    }
	}
	// if no insertion, so we got here, put it to the end
	cuts_.push_back(pair<double,double>(low,high));
	return *this;
    }



    // ------- tic_setter - global --------------------

    tic_setter::tic_setter(int a) : axis_(a) {}

    axis *tic_setter::get_axis()
    {
	switch(axis_)
	{
	case axis::x1 : return frame::current().x1axis();
	case axis::x2 : return frame::current().x2axis();
	case axis::y1 : return frame::current().y1axis();
	case axis::y2 : return frame::current().y2axis();
	default: return 0;
	}
	return 0;
    }

    tic_setter &tic_setter::operator()()
    {
	get_axis()->tics();
	return *this;
    }

    tic_setter &tic_setter::operator()(var a)
    {
	get_axis()->tics(a);
	return *this;
    }

    tic_setter &tic_setter::operator()(var a,var b)
    {
	get_axis()->tics(a,b);
	return *this;
    }

    tic_setter &tic_setter::operator()(var a,var b,var c)
    {
	get_axis()->tics(a,b,c);
	return *this;
    }

}
