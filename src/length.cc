#include "warning.h"
#include "length.h"
#include "terminal.h"
#include "exc.hh"
#include "axis.h"
#include <iostream>
#include <new>
#include <cmath>
#include <cstdio>
#include <stdlib.h>

namespace blop
{
    int &length::initializer::count()
    {
	static int c = 0;
	return c;
    }

    length::initializer::initializer()
    {
	// if it is the first call, initialize all global lengths
	if(count()++ == 0)
	{
	    new(&ZERO) length(base_id_t(terminal::ZERO));
	    new(&PW)   length(base_id_t(terminal::PW));
	    new(&PH)   length(base_id_t(terminal::PH));
	    new(&PS)   length(base_id_t(terminal::PS));
	    new(&LW)   length(base_id_t(terminal::LW));
	    new(&CM)   length(base_id_t(terminal::CM));
	    new(&MM)   length(base_id_t(terminal::MM));
	    new(&PT)   length(base_id_t(terminal::PT));
	    new(&EM)   length(base_id_t(terminal::EM));
	    new(&EX)   length(base_id_t(terminal::EX));
	}
    }
    length::initializer::~initializer()
    {
	if(--count() == 0)
	{
	    // do something
	}
    }

    std::vector<length::base_t *> length::registered_entries_;

    int &length::base_t::count()
    {
	static int c = 0;
	return c;
    }

    // -----------------  length_base  ----------------------
    
    ostream &operator<<(ostream &out,const length::base_t &l)
    {
	l.print(out);
	return out;
    }

    length::base_t::base_t()
	: termspecific_id_(0),reference_count(0)
    {
	++count();
    }

    length::base_t::base_t(const length::base_t &other)
	: termspecific_id_(other.termspecific_id_), reference_count(0)
    {
	++count();
    }

    length::base_t::~base_t()
    {
	--count();
    }

    void length::base_t::reset() {termspecific_id_ = 0;}

    bool length::base_t::specialized() const
    {
	return termspecific_id_ != 0;
    }

    terminal::id length::base_t::termspecific_id() const {return termspecific_id_;}

    void length::base_t::termspecific_id(int i) {termspecific_id_ = i;}


    // ------------------ base_max_t  ------------------------------

    length::base_max_t::~base_max_t()
    {
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(--(lengths_[i]->reference_count) == 0)
	    {
		(const_cast<length::base_t *>(lengths_[i]))->~base_t();
		length::deallocate(lengths_[i]);
	    }
	}
    }

    length::base_max_t::base_max_t(const length::base_max_t &o)
    {
	for(unsigned int i=0; i<o.lengths_.size(); ++i)
	{
	    void *ptr = length::allocate();
	    length::base_t *p = o.lengths_[i]->clone(ptr);
	    p->reference_count = 1;
	    lengths_.push_back(p);
	}
    }

    void length::base_max_t::print(ostream &out) const
    {
	out<<"max(";
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(i > 0) out<<",";
	    lengths_[i]->print(out);
	}
	out<<")";
    }

    length::base_t *length::base_max_t::clone(void *ptr) const
    {
	return new(ptr) length::base_max_t(lengths_);
    }

    length::base_max_t::base_max_t(const std::vector<blop::length::base_t *> &v)
    {
	for(unsigned int i=0; i<v.size(); ++i)
	{
	    void *ptr = length::allocate();
	    length::base_t *p = v[i]->clone(ptr);
	    p->reference_count = 1;
	    lengths_.push_back(p);
	}
    }

    void length::base_max_t::dependencies(std::vector<blop::length::base_t *> *v) const
    {
	for(unsigned int c=0; c<lengths_.size(); ++c)
	{
	    bool found = false;
	    for(unsigned int i=0; i<v->size(); ++i)
	    {
		if((*v)[i] == lengths_[c])
		{
		    found = true;
		    break;
		}
	    }
	    if(!found) v->push_back(lengths_[c]);
	}
    }

    bool length::base_max_t::depends_on(length::base_t *l) const
    {
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(lengths_[i] == l) return true;
	    if(lengths_[i]->depends_on(l)) return true;
	}
	return false;
    }

    bool length::base_max_t::specialize(terminal *term)
    {
	if(lengths_.empty())
	{
	    termspecific_id(terminal::ZERO);
	    return true;
	}
	vector<terminal::id> length_ids;
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(lengths_[i] == 0)
	    {
		warning::print("0 pointer","length::base_max_t::specialize");
		continue;
	    }
	    if(!lengths_[i]->specialized())
	    {
		if(!lengths_[i]->specialize(term)) return false;
	    }
	    length_ids.push_back(lengths_[i]->termspecific_id());
	}
	termspecific_id(term->maxlength(length_ids));
	return true;
    }



    // ------------------ base_min_t  ------------------------------

    length::base_min_t::~base_min_t()
    {
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(--(lengths_[i]->reference_count) == 0)
	    {
		(const_cast<length::base_t *>(lengths_[i]))->~base_t();
		length::deallocate(lengths_[i]);
	    }
	}
    }

    length::base_min_t::base_min_t(const length::base_min_t &o)
    {
	for(unsigned int i=0; i<o.lengths_.size(); ++i)
	{
	    void *ptr = length::allocate();
	    length::base_t *p = o.lengths_[i]->clone(ptr);
	    p->reference_count = 1;
	    lengths_.push_back(p);
	}
    }

    void length::base_min_t::print(ostream &out) const
    {
	out<<"min(";
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(i > 0) out<<",";
	    lengths_[i]->print(out);
	}
	out<<")";
    }

    length::base_t *length::base_min_t::clone(void *ptr) const
    {
	return new(ptr) length::base_min_t(lengths_);
    }

    length::base_min_t::base_min_t(const std::vector<blop::length::base_t *> &v)
    {
	for(unsigned int i=0; i<v.size(); ++i)
	{
	    void *ptr = length::allocate();
	    length::base_t *p = v[i]->clone(ptr);
	    p->reference_count = 1;
	    lengths_.push_back(p);
	}
    }

    void length::base_min_t::dependencies(std::vector<blop::length::base_t *> *v) const
    {
	for(unsigned int c=0; c<lengths_.size(); ++c)
	{
	    bool found = false;
	    for(unsigned int i=0; i<v->size(); ++i)
	    {
		if((*v)[i] == lengths_[c])
		{
		    found = true;
		    break;
		}
	    }
	    if(!found) v->push_back(lengths_[c]);
	}
    }

    bool length::base_min_t::depends_on(length::base_t *l) const
    {
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(lengths_[i] == l) return true;
	    if(lengths_[i]->depends_on(l)) return true;
	}
	return false;
    }

    bool length::base_min_t::specialize(terminal *term)
    {
	if(lengths_.empty())
	{
	    termspecific_id(terminal::ZERO);
	    return true;
	}
	vector<terminal::id> length_ids;
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(lengths_[i] == 0)
	    {
		warning::print("0 pointer","length::base_min_t::specialize");
		continue;
	    }
	    if(!lengths_[i]->specialized())
	    {
		if(!lengths_[i]->specialize(term)) return false;
	    }
	    length_ids.push_back(lengths_[i]->termspecific_id());
	}
	termspecific_id(term->minlength(length_ids));
	return true;
    }



    // -----------------  base_axis_t  ---------------------

    length::base_axis_t::base_axis_t(axis *a,double v1,double v2)
    {
	axis_ = a;
	value1_ = v1;
	value2_ = v2;
    }

    bool length::base_axis_t::specialize(terminal *t)
    {
	if(axis_->begin().specialized() == false && axis_->begin().specialize(t) == false) return false;
	if(axis_->end()  .specialized() == false && axis_->end()  .specialize(t) == false) return false;

	double d;
	if(value2_ != unset)
	{
	    d = axis_->map_point(value2_) - axis_->map_point(value1_);
	}
	else
	{
	    d = axis_->map_point(value1_);
	}

	termspecific_id(t->lincombi(-d,axis_->begin().termspecific_id(),
				    d,axis_->end().termspecific_id()));
	return true;
    }

    length::base_t *length::base_axis_t::clone(void *ptr) const
    {
	return new(ptr) length::base_axis_t(axis_,value1_,value2_);
    }

    void length::base_axis_t::print(ostream &out) const
    {
	if(value2_ != unset)
	{
	    out<<"SOMEAXIS("<<value1_<<","<<value2_<<")";
	}
	else
	{
	    out<<"SOMEAXIS("<<value1_<<")";
	}
    }

    void length::base_axis_t::dependencies(std::vector<blop::length::base_t *> *v) const
    {
	v->push_back(axis_->begin().length_base_ptr_);
	v->push_back(axis_->end().length_base_ptr_);
    }

    bool length::base_axis_t::depends_on(length::base_t *l) const
    {
	if(l == axis_->begin().length_base_ptr_ ||
	   l == axis_->end().length_base_ptr_) return true;
	if(axis_->begin().length_base_ptr_->depends_on(l)) return true;
	if(axis_->end().length_base_ptr_->depends_on(l)) return true;
	return false;
    }


    // -----------------  length_ref  ----------------

    void length::base_ref_t::dependencies
			(std::vector<blop::length::base_t *> *v) const
    {
	bool found = false;
	for(unsigned int i=0; i<v->size(); ++i)
	{
	    if((*v)[i] == the_length_)
	    {
		found = true;
		break;
	    }
	}
	if(!found) v->push_back(the_length_);
    }

    bool length::base_ref_t::depends_on(length::base_t *l) const
    {
	if(l == the_length_) return true;
	if(the_length_->depends_on(l)) return true;
	return false;
    }

    length::base_ref_t::base_ref_t(const length::base_ref_t &other)
	: the_length_(other.the_length_)
    {
	++(the_length_->reference_count);
    }

    length::base_ref_t::base_ref_t(length::base_t *o)
	: the_length_(o)
    {
	++(o->reference_count);
    }

    length::base_ref_t::~base_ref_t()
    {
	if(--(the_length_->reference_count) == 0)
	{
	    (const_cast<length::base_t *>(the_length_))->~base_t();
	    length::deallocate(the_length_);
	}
    }

    bool length::base_ref_t::specialized() const
    {
	return the_length_->specialized();
    }

    terminal::id length::base_ref_t::termspecific_id() const
    {
	return the_length_->termspecific_id();
    }

    bool length::base_ref_t::specialize(terminal *t)
    {
	return the_length_->specialize(t);
	/*
	if(!the_length_->specialized()) return false;
	bool result = the_length_->specialize(t);
	return result;
	*/
    }

    length::base_t *length::base_ref_t::clone(void *ptr) const
    {
	return new(ptr) length::base_ref_t(*this);
    }

    void length::base_ref_t::print(ostream &out) const
    {
	the_length_->print(out);
    }


    // -------------------  scaleable  ---------------

    void length::base_scaleable_t::dependencies(std::vector<blop::length::base_t *> *v) const
    {
	if(the_length_) v->push_back(the_length_);
    }

    bool length::base_scaleable_t::depends_on(length::base_t *l) const
    {
	if(the_length_ == 0) return false;
	if(l == the_length_) return true;
	if(the_length_->depends_on(l)) return true;
	return false;
    }

    length::base_scaleable_t::base_scaleable_t(double factor)
    {
	factor_ = factor;
	the_length_ = 0;
    }

    length::base_scaleable_t::base_scaleable_t(const length::base_scaleable_t &o)
    {
	factor_ = o.factor_;
	if(o.the_length_)
	{
	    void *ptr = length::allocate();
	    the_length_ = o.the_length_->clone(ptr);
	}
	else
	{
	    the_length_ = 0;
	}
    }

    length::base_scaleable_t::~base_scaleable_t()
    {
	if(the_length_)
	{
	    (const_cast<length::base_t *>(the_length_))->~base_t();
	    length::deallocate(the_length_);
	}
    }

    bool length::base_scaleable_t::specialize(terminal *t)
    {
	if(the_length_ == 0)
	{
	    warning::print("The scale is not set for a scaleable length",
			   "length::base_scaleable_t::specialize(...)");
	    termspecific_id(terminal::ZERO);
	    return true;
	}
	if(!the_length_->specialized())
	{
	    if(the_length_->specialize(t) == false)
	    {
		return false;
	    }
	}
	termspecific_id(t->lincombi(factor_,the_length_->termspecific_id()));
	return true;
    }

    length::base_t *length::base_scaleable_t::clone(void *ptr) const
    {
	base_t *result = new(ptr) length::base_scaleable_t(*this);
	return result;
    }

    void length::base_scaleable_t::print(ostream &out) const
    {
	out<<factor_<<"*(";
	if(the_length_ == 0) out<<"NULL"; 
	else the_length_->print(out);
	out<<")";
    }

    void length::base_scaleable_t::set_scale(length::base_t *s)
    {
	if(the_length_ == 0)
	{
	    void *ptr = length::allocate();
	    the_length_ = s->clone(ptr);
	}
	else
	{
	    if(s->depends_on(the_length_))
	    {
		warning::print("Recursive definition of a length",
			       "base_scaleable_t::set_scale(length::base_t *)");
		return;
	    }
	    (const_cast<length::base_t *>(the_length_))->~base_t();
	    s->clone(the_length_);
	}
	
    }

    // ----------------  length_id --------------------

    length::base_id_t::base_id_t(const length::base_id_t &o)
    {
	termspecific_id_ = o.termspecific_id_;
    }

    length::base_id_t::base_id_t(int i)
    {
	termspecific_id_ = i;
	if(termspecific_id_ < terminal::ZERO ||
	   termspecific_id_ >= terminal::NOF_UNITS)
	{
	    warning::print("Bad id","length::base_id_t::base_id_t(int)");
	    termspecific_id_ = terminal::ZERO;
	}
    }

    length::base_t *length::base_id_t::clone(void *placement) const
    {
	return new(placement) length::base_id_t(*this);
    }

    void length::base_id_t::print(ostream &out) const
    {
	const char *unitname[terminal::NOF_UNITS];
	for(int i=0; i<terminal::NOF_UNITS; ++i) unitname[i] = "???";
	unitname[terminal::ZERO] = "ZERO";
	unitname[terminal::PW  ] = "PW";
	unitname[terminal::PH  ] = "PH";
	unitname[terminal::PS  ] = "PS";
	unitname[terminal::LW  ] = "LW";
	unitname[terminal::CM  ] = "CM";
	unitname[terminal::MM  ] = "MM";
	unitname[terminal::PT  ] = "PT";
	unitname[terminal::EM  ] = "EM";
	unitname[terminal::EX  ] = "EX";

	if(termspecific_id_ < terminal::ZERO ||
	   termspecific_id_ >= terminal::NOF_UNITS)
	{
	    out<<"["<<termspecific_id_<<"]";
	}
	else
	{
	    out<<unitname[termspecific_id_];
	}
    }

    // ---------------  length_textsize --------------

    length::base_textsize_t::base_textsize_t(
	const length::base_textsize_t &other)
    {
	angle_ = other.angle_;
	text_  = other.text_;
	direction_ = other.direction_;
	termspecific_id_ = other.termspecific_id_;
    }

    length::base_textsize_t::base_textsize_t(const string &s,
					     int dir,double an)
	:  angle_(an),text_(s), direction_(dir) {}

    bool length::base_textsize_t::specialize(terminal *t)
    {
	if(direction_ == horizontal)
	    termspecific_id(t->textwidth (text_,angle_));
	if(direction_ == vertical  )
	    termspecific_id(t->textheight(text_,angle_));
	return true;
    }
					 
    void length::base_textsize_t::text(const string &s)
    {
	text_ = s;
    }

    void length::base_textsize_t::angle(double a)
    {
	angle_ = a;
    }

    void length::base_textsize_t::print(ostream &out) const
    {
	if(direction_ == horizontal) out<<"width(\""<<text_<<"\")";
	else out<<"height(\""<<text_<<"\")";
    }

    length::base_textwidth_t::base_textwidth_t(const string &s,
					       double angle)
	: base_textsize_t(s,horizontal,angle) {}

    length::base_t *length::base_textwidth_t::clone(void *placement) const
    {
	return new(placement) length::base_textwidth_t(text_,angle_);
    }

    length::base_textheight_t::base_textheight_t(const string &s,
						 double angle)
	: base_textsize_t(s,vertical,angle) {}

    length::base_t *length::base_textheight_t::clone(void *placement) const
    {
	return new(placement) length::base_textheight_t(text_,angle_);
    }

    length_temporary width(const var &s,double angle)
    {
	return length::base_textwidth_t(s.str(),angle);
    }

    length_temporary height(const var &s,double angle)
    {
	return length::base_textheight_t(s.str(),angle);
    }

    // ---------------- length_max_textsize ------------

    void length::base_max_textsize_t::add(const string &s)
    {
	texts_.push_back(s);
    }

    length::base_max_textsize_t::base_max_textsize_t(
	const length::base_max_textsize_t &other)
	: texts_(other.texts_)
    {
    }

    length::base_max_textsize_t::base_max_textsize_t(const std::vector<std::string> &v)
	: texts_(v)
    {
	termspecific_id_ = 0;
    }
    

    void length::base_max_textsize_t::clear()
    {
	texts_.clear();
    }

    length::base_max_textwidth_t::base_max_textwidth_t() {}
    length::base_max_textwidth_t::base_max_textwidth_t(const base_max_textwidth_t &other)
	: base_max_textsize_t(other)
    {
    }
	
    length::base_max_textwidth_t::base_max_textwidth_t (const vector<string> &s)
	: base_max_textsize_t(s)
    {
    }

    bool length::base_max_textwidth_t::specialize(terminal *t)
    {
	termspecific_id(t->max_textwidth(texts_));
	return true;
    }

    length::base_t *length::base_max_textwidth_t::clone(void *ptr) const
    {
	return new(ptr) length::base_max_textwidth_t(*this);
    }

    void length::base_max_textwidth_t::print(ostream &out) const
    {
	out<<"maxwidth(";
	for(vector<var>::size_type i=0; i<texts_.size(); ++i)
	{
	    if(i > 0) out<<",";
	    out<<"\""<<texts_[i]<<"\"";
	}
	out<<")";
    }

    length::base_max_textheight_t::base_max_textheight_t() {}
    length::base_max_textheight_t::base_max_textheight_t(const base_max_textheight_t &other)
	: base_max_textsize_t(other)
    {
    }

    length::base_max_textheight_t::base_max_textheight_t (const vector<string> &s)
	: base_max_textsize_t(s)
    {
    }

    bool length::base_max_textheight_t::specialize(terminal *t)
    {
	termspecific_id(t->max_textheight(texts_));
	return true;
    }

    length::base_t *length::base_max_textheight_t::clone(void *ptr) const
    {
	return new(ptr) length::base_max_textheight_t(*this);
    }

    void length::base_max_textheight_t::print(ostream &out) const
    {
	out<<"maxheight(";
	for(vector<string>::size_type i=0; i<texts_.size(); ++i)
	{
	    if(i > 0) out<<",";
	    out<<"\""<<texts_[i]<<"\"";
	}
	out<<")";
	return;
    }

    length::base_max_textlheight_t::base_max_textlheight_t() {}
    length::base_max_textlheight_t::base_max_textlheight_t(const base_max_textlheight_t &other)
	: base_max_textsize_t(other)
    {
    }

    length::base_max_textlheight_t::base_max_textlheight_t (const vector<string> &s)
	: base_max_textsize_t(s)
    {
    }

    bool length::base_max_textlheight_t::specialize(terminal *t)
    {
	termspecific_id(t->max_textlheight(texts_));
	return true;
    }

    length::base_t *length::base_max_textlheight_t::clone(void *ptr) const
    {
	return new(ptr) length::base_max_textlheight_t(*this);
    }

    void length::base_max_textlheight_t::print(ostream &out) const
    {
	out<<"maxlheight(";
	for(vector<string>::size_type i=0; i<texts_.size(); ++i)
	{
	    if(i > 0) out<<",";
	    out<<"\""<<texts_[i]<<"\"";
	}
	out<<")";
	return;
    }

    length_temporary maxwidth (const std::vector<std::string> &s)
    {
        length::base_max_textwidth_t mmm(s);
        return mmm;
    }

    length_temporary maxwidth (const std::vector<blop::var> &s)
    {
	vector<string> v;
	for(unsigned int i=0; i<s.size(); ++i) v.push_back(s[i].str());
	length::base_max_textwidth_t mmm(v);
	return mmm;
    }

    length_temporary maxwidth (const std::vector<blop::tic> &s)
    {
	vector<string> v;
	for(unsigned int i=0; i<s.size(); ++i) v.push_back(s[i].label().str());
	length::base_max_textwidth_t mmm(v);
	return mmm;
    }

    length_temporary maxheight(const vector<var> &s)
    {
	vector<string> v;
	for(unsigned int i=0; i<s.size(); ++i) v.push_back(s[i].str());
	length::base_max_textheight_t mmm(v);
	return mmm;
    }

    length_temporary maxlheight(const vector<var> &s)
    {
	vector<string> v;
	for(unsigned int i=0; i<s.size(); ++i) v.push_back(s[i].str());
	length::base_max_textlheight_t mmm(v);
	return mmm;
    }

    length_temporary length::max(vector<blop::length> &v)
    {
	vector<blop::length::base_t *> vv;
	for(unsigned int i=0; i<v.size(); ++i)
	{
	    vv.push_back(v[i].length_base_ptr_);
	}
	length::base_max_t mmm(vv);
	return mmm;
    }
    length_temporary length::max(const length &l1, const length &l2)
    {
	vector<blop::length> v;
	v.push_back(l1);
	v.push_back(l2);
	return length::max(v);
    }
    length_temporary length::max(const length &l1, const length &l2, const length &l3)
    {
	vector<blop::length> v;
	v.push_back(l1);
	v.push_back(l2);
	v.push_back(l3);
	return length::max(v);
    }

    length_temporary length::min(vector<blop::length> &v)
    {
	vector<blop::length::base_t *> vv;
	for(unsigned int i=0; i<v.size(); ++i)
	{
	    vv.push_back(v[i].length_base_ptr_);
	}
	length::base_min_t mmm(vv);
	return mmm;
    }
    length_temporary length::min(const length &l1, const length &l2)
    {
	vector<blop::length> v;
	v.push_back(l1);
	v.push_back(l2);
	return length::min(v);
    }
    length_temporary length::min(const length &l1, const length &l2, const length &l3)
    {
	vector<blop::length> v;
	v.push_back(l1);
	v.push_back(l2);
	v.push_back(l3);
	return length::min(v);
    }

    // -----  length_lincombi -----------------

    void length::base_lincombi_t::set_scale(length::base_t *s)
    {
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    lengths_[i]->set_scale(s);
	}
    }

    void length::base_lincombi_t::dependencies(
	vector<length::base_t *> *v) const
    {
	for(unsigned int c=0; c<lengths_.size(); ++c)
	{
	    bool found = false;
	    for(unsigned int i=0; i<v->size(); ++i)
	    {
		if((*v)[i] == lengths_[c])
		{
		    found = true;
		    break;
		}
	    }
	    if(!found) v->push_back(lengths_[c]);
	}
    }

    bool length::base_lincombi_t::depends_on(length::base_t *l) const
    {
	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(l == lengths_[i]) return true;
	    if(lengths_[i]->depends_on(l)) return true;
	}
	return false;
    }

    length::base_lincombi_t::base_lincombi_t() {}

    length::base_lincombi_t::base_lincombi_t(const length::base_lincombi_t &other)
    {
	factors_ = other.factors_;
	lengths_ = other.lengths_;
	for(vector<const length::base_t *>::size_type i=0; i<lengths_.size(); ++i)
	{
	    ++(lengths_[i]->reference_count);
	}
	termspecific_id_ = other.termspecific_id_;
	reference_count = 0;
    }

    length::base_lincombi_t::base_lincombi_t(double factor, length::base_t *b)
    {
	factors_.push_back(factor);
	lengths_.push_back(b);
	++(b->reference_count);
	termspecific_id_ = 0;
	reference_count = 0;
    }

    void length::base_lincombi_t::print(ostream &out) const
    {
	if(factors_.empty())
	{
	    out<<"ZERO";
	    return;
	}
	for(vector<double>::size_type i=0; i<factors_.size(); ++i)
	{
	    bool lincombi = (dynamic_cast<const length::base_lincombi_t*>(lengths_[i]) != 0 ||
			     dynamic_cast<const length::base_ref_t     *>(lengths_[i]) != 0 );

	    if(factors_[i] != 1)
	    {
		if(i>0)
		{
		    if(factors_[i] >= 0) out<<" + "<<factors_[i];
		    else out<<" - "<<-factors_[i];
		}
		else
		{
		    out<<factors_[i];
		}
		out<<"*";
	    }
	    else
	    {
		if(i > 0) out<<" + ";
	    }
	    if(factors_[i] != 1 && lincombi) out<<"(";
	    lengths_[i]->print(out);
	    if(factors_[i] != 1 && lincombi) out<<")";
	}
    }

    bool length::base_lincombi_t::specialize(terminal *t)
    {
	if(lengths_.empty())
	{
	    termspecific_id(terminal::ZERO);
	    return true;
	}
	vector<terminal::id> length_ids;
	for(vector<double>::size_type i = 0; i<lengths_.size(); ++i)
	{
	    if(lengths_[i] == 0)
	    {
		warning::print("0 pointer","length_lincombi::specialize");
		exit(0);
	    }
	    if(!lengths_[i]->specialized())
	    {
		if(!lengths_[i]->specialize(t)) return false;
	    }
	    length_ids.push_back(lengths_[i]->termspecific_id());
	}
	termspecific_id(t->lincombi(factors_,length_ids));
	return true;
    }

    void length::base_lincombi_t::add(double f,length::base_t *l)
    {
	if(::fabs(f) < 0.001) return;

	if(dynamic_cast<base_id_t *>(l))
	{
	    if(l->termspecific_id() == terminal::ZERO) return;
	}

	for(unsigned int i=0; i<lengths_.size(); ++i)
	{
	    if(lengths_[i] == l)
	    {
		factors_[i] += f;
		return;
	    }
	}

	factors_.push_back(f);
	lengths_.push_back(l);
	++(l->reference_count);
    }

    void length::base_lincombi_t::multiply(double f)
    {
	for(vector<double>::size_type i=0; i<factors_.size(); ++i)
	{
	    factors_[i] *= f;
	}
    }

    length::base_lincombi_t::~base_lincombi_t()
    {
	for(vector<double>::size_type i=0; i<lengths_.size(); ++i)
	{
	    if(--(lengths_[i]->reference_count) == 0)
	    {
		(const_cast<length::base_t *>(lengths_[i]))->~base_t();
		length::deallocate(lengths_[i]);
	    }
	}
    }

    length::base_t *length::base_lincombi_t::clone(void *placement) const 
    {
	return new(placement) length::base_lincombi_t(*this);
    }


    // ----------------------------------------------------------


    length::~length()
    {
	--(length_base_ptr_->reference_count);
	if(length_base_ptr_->reference_count == 0)
	{
	    length_base_ptr_->~base_t();
	    deallocate(length_base_ptr_);
	}
    }

    length::length(const char *)
    {
	locked_ = false;
    }

    length::length(length::base_t *l)
    {
	length_base_ptr_ = l;
	++(length_base_ptr_->reference_count);
	locked_ = false;
    }

/*
    void length::init(const length::base_t &l)
    {
	void *ptr = allocate();
	length_base_ptr_ = l.clone(ptr);
	length_base_ptr_ -> reference_count = 1;
    }
*/

    length::length(const length::base_t &l)
    {
	void *ptr = allocate();
	length_base_ptr_ = l.clone(ptr);
	length_base_ptr_ -> reference_count = 1;
//	init(l);
	locked_ = false;
    }

    length::length(int factor)
    {
	void *ptr = allocate();
	if(factor == 0)
	{
	    length_base_ptr_ = new(ptr) length::base_id_t(terminal::ZERO);
	}
	else
	{
	    length_base_ptr_ = new(ptr) length::base_scaleable_t(1.0*factor);
	}
	length_base_ptr_->reference_count = 1;
	locked_ = false;

    }

    length::length(double factor)
    {
	void *ptr = allocate();
	if(factor == 0)
	{
	    length_base_ptr_ = new(ptr) length::base_id_t(terminal::ZERO);
	}
	else if(factor == unset)
	{
	    length_base_ptr_ = new(ptr) length::base_unset_t;
	}
	else
	{
	    length_base_ptr_ = new(ptr) length::base_scaleable_t(factor);
	}
	length_base_ptr_->reference_count = 1;
	locked_ = false;
    }

    length::length(const length &l)
    {
	void *ptr = allocate();
	length_base_ptr_ = l.length_base_ptr_->clone(ptr);
	length_base_ptr_->reference_count = 1;
	locked_ = false;
    }

    void length::set_scale(const length &scale)
    {
	if(length_base_ptr_ == 0)
	{
	    warning::print("0 pointer","length::set_scale");
	    return;
	}
	length_base_ptr_->set_scale(scale.length_base_ptr_);
	locked_ = false;
    }

    const length &length::operator= (const length &l)
    {
	if(&l == this) return *this;
	if(locked_)
	{
	    warning::print("Length locked","length::operator=(const length &)");
	    return *this;
	}
	if(l.length_base_ptr_->depends_on(length_base_ptr_))
	{
	    warning::print("Recursive definition of a length",
			   "length::operator=(const length &)");
	    return *this;
	}
	int ref_count = length_base_ptr_->reference_count;
	length_base_ptr_->~base_t();
	l.length_base_ptr_->clone(length_base_ptr_);
	length_base_ptr_->reference_count = ref_count;
	return *this;
    }

    const length &length::operator= (double factor)
    {
	if(locked_)
	{
	    warning::print("Length locked","length::operator=(double)");
	    return *this;
	}
	int ref_count = length_base_ptr_ -> reference_count;
	length_base_ptr_ -> ~base_t();
	if(factor != 0)
	{
	    new(length_base_ptr_) base_scaleable_t(factor);
	}
	else
	{
	    new(length_base_ptr_) base_id_t(terminal::ZERO);
	}
	length_base_ptr_ -> reference_count = ref_count;
	return *this;
    }

    void length::operator+= (const length &l)
    {
	if(locked_)
	{
	    warning::print("Length locked","length::operator+=(const length &)");
	    return;
	}
	if(l.length_base_ptr_->depends_on(length_base_ptr_))
	{
	    warning::print("Recursive definition of a length",
			   "length::operator+= (const length &)");
	    return;
	}
	if(base_lincombi_t *t
	   = dynamic_cast<base_lincombi_t *>(length_base_ptr_))
	{
	    t->add(1,l.clone_base());
	}
	else
	{
	    void *ptr = allocate();
	    base_t *old = length_base_ptr_->clone(ptr);
	    length_base_ptr_->~base_t();
	    
	    base_lincombi_t *new_lincombi = new(length_base_ptr_) base_lincombi_t;
	    new_lincombi->add(1.0,old);
	    new_lincombi->add(1.0,l.clone_base());
	}
    }

    void length::operator-= (const length &l)
    {
	if(locked_)
	{
	    warning::print("Length locked","length::operator-=(const length &)");
	    return;
	}
	if(l.length_base_ptr_->depends_on(length_base_ptr_))
	{
	    warning::print("Recursive definition of a length",
			   "length::operator-= (const length &)");
	    return;
	}
	if(base_lincombi_t *t
	   = dynamic_cast<base_lincombi_t *>(length_base_ptr_))
	{
	    t->add(-1,l.clone_base());
	}
	else
	{
	    void *ptr = allocate();
	    base_t *old = length_base_ptr_->clone(ptr);
	    length_base_ptr_->~base_t();
	    
	    base_lincombi_t *new_lincombi = new(length_base_ptr_) base_lincombi_t;
	    new_lincombi->add(1.0,old);
	    new_lincombi->add(-1.0,l.clone_base());
	}
    }


    void length::operator*= (double f)
    {
	if(locked_)
	{
	    warning::print("Length locked","length::operator*=(const length &)");
	    return;
	}

	if(base_lincombi_t *t = dynamic_cast<base_lincombi_t *>(length_base_ptr_))
	{
	    t->multiply(f);
	}
	else if(base_scaleable_t *t = dynamic_cast<base_scaleable_t *>(length_base_ptr_))
	{
	    t->factor_ *= f;
	}
	else
	{
	    void *ptr = allocate();
	    base_t *old = length_base_ptr_->clone(ptr);
	    length_base_ptr_->~base_t();
	    base_lincombi_t *new_lincombi = new(length_base_ptr_) base_lincombi_t;
	    new_lincombi->add(f,old);
	}
    }

    void length::operator /= (double f)
    {
	if(locked_)
	{
	    warning::print("Length locked","length::operator/=(const length &)");
	    return;
	}

	if(base_lincombi_t *t = dynamic_cast<base_lincombi_t *>(length_base_ptr_))
	{
	    t->multiply(1.0/f);
	}
	else if(base_scaleable_t *t = dynamic_cast<base_scaleable_t *>(length_base_ptr_))
	{
	    t->factor_ *= 1.0/f;
	}
	else
	{
	    void *ptr = allocate();
	    base_t *old = length_base_ptr_->clone(ptr);
	    length_base_ptr_->~base_t();
	    base_lincombi_t *new_lincombi = new(length_base_ptr_) base_lincombi_t;
	    new_lincombi->add(1.0/f,old);
	}
    }


    int length::specialize(terminal *t) const
    {
	length::base_t *ppp = const_cast<base_t *>(length_base_ptr_);
	return ppp->specialize(t);
    }

    bool length::specialized() const
    {
	return length_base_ptr_->specialized();
    }

    terminal::id length::termspecific_id() const
    {
	return length_base_ptr_->termspecific_id();
    }
	    
    length::base_t *length::clone_base() const
    {
	void *ptr = allocate();
	base_t *result = length_base_ptr_->clone(ptr);
	return result;
    }

    bool length::initialized() const
    {
	return (dynamic_cast<base_unset_t *>(length_base_ptr_) == 0);
    }

    // ------------ length_temporary -------------

    length_temporary::length_temporary(const length::base_t &l)
    {
	l.clone(length_base_ptr_);
	length_base_ptr_->reference_count = 1;
    }

    length_temporary::length_temporary(length::base_t *l)
	: length(l)
    {
    }

    length_temporary::length_temporary(const length &l)
    {
	length *ll = const_cast<length *>(&l);
	ll->length_base_ptr_->clone(length_base_ptr_);
	length_base_ptr_->reference_count = 1;
    }

    length::base_t *length_temporary::clone_base() const
    {
	return length_base_ptr_;
    }

    length length::operator! () const
    {
	return base_ref_t(length_base_ptr_);
    }

    // --------------------------------------------

    void length::print(ostream &out) const
    {
	if(length_base_ptr_ == 0) out<<"!NULLPOINTER!";
	length_base_ptr_->print(out);
    }

    ostream &operator<< (ostream &out, const length &l)
    {
	l.print(out);
	return out;
    }

    length ZERO("do nothing"); // = length::base_id_t(terminal::ZERO);
    length PW  ("do nothing"); //   = length::base_id_t(terminal::PW);
    length PH  ("do nothing"); //   = length::base_id_t(terminal::PH);
    length PS  ("do nothing"); //   = length::base_id_t(terminal::PS);
    length LW  ("do nothing"); //   = length::base_id_t(terminal::LW);
    length CM  ("do nothing"); //   = length::base_id_t(terminal::CM);
    length MM  ("do nothing"); //   = length::base_id_t(terminal::MM);
    length PT  ("do nothing"); //   = length::base_id_t(terminal::PT);
    length EM  ("do nothing"); //   = length::base_id_t(terminal::EM);
    length EX  ("do nothing"); //   = length::base_id_t(terminal::EX);


    // ----------  alloc, deallocate ------------------------
    void *length::allocate()
    {
	static unsigned int s = 0;

	if(s == 0)
	{
	    if(s < sizeof(length::base_ref_t))      s = sizeof(length::base_ref_t);
	    if(s < sizeof(length::base_textwidth_t)) s = sizeof(length::base_textwidth_t);
	    if(s < sizeof(length::base_textheight_t)) s = sizeof(length::base_textheight_t);
	    if(s < sizeof(length::base_max_textwidth_t)) s = sizeof(length::base_max_textwidth_t);
	    if(s < sizeof(length::base_max_textheight_t)) s = sizeof(length::base_max_textheight_t);
	    if(s < sizeof(length::base_lincombi_t)) s = sizeof(length::base_lincombi_t);
	    if(s < sizeof(length::base_id_t))     s = sizeof(length::base_id_t);
	    if(s < sizeof(length::base_max_t))    s = sizeof(length::base_max_t);
	    if(s < sizeof(length::base_scaleable_t)) s = sizeof(length::base_scaleable_t);
	}
	void *ptr =  new char[s];
	return ptr;
    }

    void length::deallocate(const length::base_t *p)
    {
	delete [] ((char*)p);
    }

    length_temporary operator+ (const length &l1,const length &l2)
    {
	length::base_t *l1base = l1.clone_base();
	length::base_t *l2base = l2.clone_base();

	length::base_lincombi_t *lc1 =
	    dynamic_cast<length::base_lincombi_t *>(l1base);

	length::base_lincombi_t *lc2 =
	    dynamic_cast<length::base_lincombi_t *>(l2base);

	length::base_lincombi_t *result = 0;

	if(lc1)
	{
	    result = lc1;
	    if(lc2)
	    {
		for(unsigned int i=0; i<lc2->factors_.size(); ++i)
		{
		    result->add(lc2->factors_[i],lc2->lengths_[i]);
		}
	    }
	    else
	    {
		result->add(1.0, l2base);
	    }
	}
	else if(lc2)
	{
	    result = lc2;
	    result->add(1.0, l1base);
	}
	else
	{
	    void *ptr = length::allocate();
	    result = new(ptr) length::base_lincombi_t;
	    result->add(1.0, l1base);
	    result->add(1.0, l2base);
	}
	return result;
    }

    length_temporary operator- (const length &l)
    {
	return -1*l;
    }

    length_temporary operator- (const length &l1,const length &l2)
    {
	length::base_t *l1base = l1.clone_base();
	length::base_t *l2base = l2.clone_base();
	length::base_lincombi_t *lc1
	    = dynamic_cast<length::base_lincombi_t *>(l1base);
	length::base_lincombi_t *lc2
	    = dynamic_cast<length::base_lincombi_t *>(l2base);

	length::base_lincombi_t *result = 0;
	if(lc1)
	{
	    if(lc2)
	    {
		for(unsigned int i=0; i<lc2->factors_.size(); ++i)
		{
		    lc1->add(-lc2->factors_[i],lc2->lengths_[i]);
		}
	    }
	    else
	    {
		lc1->add(-1.0, l2base);
	    }
	    result = lc1;
	}
	else if(lc2)
	{
	    lc2->multiply(-1);
	    lc2->add(1.0, l1base);
	    result = lc2;
	}
	else
	{
	    void *ptr = length::allocate();
	    result = new(ptr) length::base_lincombi_t;
	    result->add(1.0, l1base);
	    result->add(-1.0, l2base);
	}
	return result;
    }

    length_temporary operator* (double f,const length &l)
    {
	length::base_t *lbase = l.clone_base();

	length::base_lincombi_t *lc =
	    dynamic_cast<length::base_lincombi_t *>(lbase);

	if(lc)
	{
	    lc->multiply(f);
	    return lc;
	}

	void *ptr = length::allocate();
	lc = new(ptr) length::base_lincombi_t;
	lc->add(f,lbase);
	return lc;
    }

    length_temporary operator* (const length &l,double f)
    {

	length::base_t *lbase = l.clone_base();

	length::base_lincombi_t *lc =
	    dynamic_cast<length::base_lincombi_t *>(lbase);

	if(lc)
	{
	    lc->multiply(f);
	    return lc;
	}

	void *ptr = length::allocate();
	lc = new(ptr) length::base_lincombi_t;
	lc->add(f,lbase);

	return lc;
    }
    length_temporary operator/ (const length &l,double f)
    {
	length::base_t *lbase = l.clone_base();

	length::base_lincombi_t *lc =
	    dynamic_cast<length::base_lincombi_t *>(lbase);

	if(lc)
	{
	    lc->multiply(1.0/f);
	    return lc;
	}

	void *ptr = length::allocate();
	lc = new(ptr) length::base_lincombi_t;
	lc->add(1.0/f,lbase);

	return lc;
    }

    bool length::depends_on(const length &l) const
    {
	return length_base_ptr_->depends_on(l.length_base_ptr_);
    }

    void length::register_me() const
    {
	registered_entries_.push_back(length_base_ptr_);
    }

    void length::clear_regs()
    {
	registered_entries_.clear();
    }

    void length::detect_deadlock()
    {
	for(unsigned int i=0; i<registered_entries_.size(); ++i)
	{
	    registered_entries_[i]->print(cout);
	    cout<<endl;
	}
    }

    void length::specialize_regs(terminal *term)
	TRY
    {
	// first collect the dependencies of all registered entries as well
	for(unsigned int i=0; i<registered_entries_.size(); ++i)
	{
	    registered_entries_[i]->dependencies(&registered_entries_);
	}

	// then reset all of these
	for(unsigned int i=0; i<registered_entries_.size(); ++i)
	{
	    registered_entries_[i]->reset();
	}
	
	bool everything_ok = false;
	while(!everything_ok)
	{
	    everything_ok = true;
	    int success = 0;
	    for(unsigned int i=0; i<registered_entries_.size(); ++i)
	    {
		if(!registered_entries_[i]->specialized())
		{
		    if(!registered_entries_[i]->specialize(term))
		    {
			everything_ok = false;
		    }
		    else
		    {
			++success;
		    }
		}
	    }
	    if(!everything_ok && !success)
	    {
		err("Failed to do anything");
	    }
	}
    }
    CATCH("length::specialize_regs")

    length_temporary lincombi(double factor,int id)
    {
	return length::base_lincombi_t(factor,new length::base_id_t(id));
    }

}


