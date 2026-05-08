#ifndef __BLOP_ARROWHEAD_H__
#define __BLOP_ARROWHEAD_H__

#include "length.h"
#include "grob.h"
#include "constants.h"

namespace blop
{
    class arrowhead : public grob
    {
    public:
	virtual arrowhead *clone() const = 0;

	class simple;
	class filled;

	arrowhead &layer(const var & l) { grob::layer(l); return *this; }

	virtual arrowhead &size(const length &l) = 0;
	virtual arrowhead &angle(double a) = 0;
    };


    class arrowhead::simple : public arrowhead
    {
    private:
	length length_,l_,t1_,t2_;
	double angle_;
	void set_t_();
    public:
	simple(const length &l, double a=45);
	simple(const arrowhead::simple &o);
	void prepare_for_draw();
	void print(terminal *);
	
	simple &size(const length  &l);
	const length &size() const;
	simple &angle(double a);
	double angle() const;
	virtual arrowhead *clone() const;
    };
    
    class arrowhead::filled : public arrowhead
    {
    private:
	bool l1_eq_l2_;
	length length_,l_,t1_,t2_,length2_, l2_;
	double angle_;
	void set_t_();
    public:
	filled(const length &l=EX, double a=45);
	filled(const length &l, double a, const length &l2);
	filled(const filled &);
	void prepare_for_draw();
	void print(terminal *);
	
	filled &size(const length  &l);
	filled &size2(const length &l);
	const length &size() const;
	const length &size2() const;
	filled &angle(double a);
	double angle() const;
	virtual arrowhead *clone() const;
    };
    


}

#endif
