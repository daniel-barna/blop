
#include "point_drawer.h"
#include "graph.h"
#include "exc.hh"
#include "frame.h"
#include <cmath>
#include <typeinfo>

namespace blop
{
    const point_drawer &point_drawer::get(unsigned int n)
    {
        return point_by_index(n);
    }

    bool equals(point_drawer *p1, point_drawer *p2)
    {
	if(p1 == 0 && p2 != 0) return false;
	if(p2 == 0 && p1 != 0) return false;
	if(p1 == 0 && p2 == 0) return true;
	if(typeid(*p1) != typeid(*p2)) return false;
	return p1->equals(p2);
    }

    // --------- circle  -------------------------


    circle::circle(bool filled)
    {
        fill(filled);
    }

    point_drawer *circle::clone() const
    {
	return new circle(*this);
    }

    void circle::prepare_for_draw(const length &size)
    {
	r_ = 0.5*!size;
	r_.register_me();
    }
    
    void circle::draw(terminal *t,const length &s)
    {
	r_ = 0.5 * !s;
	if(!r_.specialize(t))
	{
	    cerr<<"Failed to specialize length in 'circle::draw'"<<endl;
	    cerr<<"This graph will not be drawn"<<endl;
	    return;
	}
	draw(t);
    }

    void circle::draw(terminal *t)
    {
	if(fill_) t->fill_arc(terminal::coord(0,0),r_.termspecific_id());
	else t->draw_arc(terminal::coord(0,0),r_.termspecific_id());
    }



    // --------- square  -------------------------

    square::square(bool filled)
    {
        fill(filled);
    }

    point_drawer *square::clone() const
    {
	return new square(*this);
    }

    void square::prepare_for_draw(const length &size)
    {
	l1 = -0.5 * !size;
	l2 =  0.5 * !size;
	l1.register_me();
	l2.register_me();
    }
    
    void square::draw(terminal *t,const length &s)
    {
	l1 = -0.5 * !s;
	l2 =  0.5 * !s;
	if(!l1.specialize(t) || !l2.specialize(t))
	{
	    cerr<<"Failed to specialize length in 'square::draw'"<<endl;
	    cerr<<"This graph will not be drawn"<<endl;
	    return;
	}
	draw(t);
    }

    void square::draw(terminal *t)
    {
	vector<terminal::coord> cc;
	cc.push_back(terminal::coord(l1.termspecific_id(),l1.termspecific_id()));
	cc.push_back(terminal::coord(l2.termspecific_id(),l1.termspecific_id()));
	cc.push_back(terminal::coord(l2.termspecific_id(),l2.termspecific_id()));
	cc.push_back(terminal::coord(l1.termspecific_id(),l2.termspecific_id()));
	cc.push_back(terminal::coord(l1.termspecific_id(),l1.termspecific_id()));
	if(fill_) t->fill_polygon(cc);
	else t->draw_lines(cc);
    }


    //  ----------------  diamond ------------------------------

    diamond::diamond(bool filled)
    {
        fill(filled);
    }

    point_drawer *diamond::clone() const
    {
	return new diamond(*this);
    }

    void diamond::prepare_for_draw(const length &size)
    {
	l1 = -1/::sqrt(2.0) * !size;
	l2 =  1/::sqrt(2.0) * !size;
	l1.register_me();
	l2.register_me();
    }
    
    void diamond::draw(terminal *t,const length &s)
    {
	l1 = -1/::sqrt(2.0) * !s;
	l2 =  1/::sqrt(2.0) * !s;
	if(!l1.specialize(t) || !l2.specialize(t))
	{
	    cerr<<"Failed to specialize length in 'diamond::draw'"<<endl;
	    cerr<<"This graph will not be drawn"<<endl;
	    return;
	}
	draw(t);
    }

    void diamond::draw(terminal *t)
    {
	vector<terminal::coord> cc;
	cc.push_back(terminal::coord(l1.termspecific_id(),0));
	cc.push_back(terminal::coord(0,l1.termspecific_id()));
	cc.push_back(terminal::coord(l2.termspecific_id(),0));
	cc.push_back(terminal::coord(0,l2.termspecific_id()));
	cc.push_back(terminal::coord(l1.termspecific_id(),0));
	if(fill_) t->fill_polygon(cc);
	else t->draw_lines(cc);
    }

    // -----------------  triangle ----------------------------


    triangle::triangle(bool filled,bool dir_up)
	: up_(dir_up)
    {
        fill(filled);
    }

    point_drawer *triangle::clone() const
    {
	return new triangle(*this);
    }

    void triangle::prepare_for_draw(const length &size)
    {
	x1 = -0.6 * !size;
	x2 =  0.6 * !size;
	if(up_)
	{
	    y1 = -0.34 * !size;
	    y2 = 0.692 * !size;
	}
	else
	{
	    y1 = 0.34 * !size;
	    y2 = -0.692 * !size;
	}
	x1.register_me();
	x2.register_me();
	y1.register_me();
	y2.register_me();
    }
    
    void triangle::draw(terminal *t,const length &s)
    {
	x1 = -0.6 * !s;
	x2 =  0.6 * !s;
	y1 = -0.34 * !s;
	y2 = 0.692 * !s;
	if(!x1.specialize(t) ||	!x2.specialize(t) || !y1.specialize(t) || !y2.specialize(t))
	{
	    cerr<<"Failed to specialize length in 'triangle::draw'"<<endl;
	    cerr<<"This graph will not be drawn"<<endl;
	    return;
	}
	draw(t);
    }

    void triangle::draw(terminal *t)
    {
	vector<terminal::coord> cc;
	cc.push_back(terminal::coord(x1.termspecific_id(),y1.termspecific_id()));
	cc.push_back(terminal::coord(x2.termspecific_id(),y1.termspecific_id()));
	cc.push_back(terminal::coord(0,y2.termspecific_id()));
	cc.push_back(terminal::coord(x1.termspecific_id(),y1.termspecific_id()));
	if(fill_) t->fill_polygon(cc);
	else t->draw_lines(cc);
    }

    // ---------------- plus  -----------------------

    point_drawer *plus::clone() const
    {
	return new plus;
    }

    void plus::prepare_for_draw(const length &size)
    {
	l1_ = -0.5 * !size;
	l2_ =  0.5 * !size;
	l1_.register_me();
	l2_.register_me();
    }

    void plus::draw(terminal *t,const length &s)
    {
	l1_ = -0.5 * !s;
	l2_ =  0.5 * !s;
	if(!l1_.specialize(t) || !l2_.specialize(t))
	{
	    cerr<<"Failed to specialize length in 'plus::draw'"<<endl;
	    cerr<<"This graph will not be drawn"<<endl;
	    return;
	}
	draw(t);
    }

    void plus::draw(terminal *t)
    {
	t->draw_line(terminal::coord(l1_.termspecific_id(), terminal::id(0,2)),
		     terminal::coord(l2_.termspecific_id(), terminal::id(0,2)));
	t->draw_line(terminal::coord(terminal::id(0,1), l1_.termspecific_id()),
	             terminal::coord(terminal::id(0,1), l2_.termspecific_id()));
    }

    // ---------------- cross  -----------------------

    point_drawer *cross::clone() const
    {
	return new cross;
    }

    void cross::prepare_for_draw(const length &size)
    {
	l1_ = -0.5/::sqrt(2) * !size;
	l2_ =  0.5/::sqrt(2) * !size;
	l1_.register_me();
	l2_.register_me();
    }

    void cross::draw(terminal *t,const length &s)
    {
	l1_ = -0.5/::sqrt(2) * !s;
	l2_ =  0.5/::sqrt(2) * !s;
	if(!l1_.specialize(t) || !l2_.specialize(t))
	{
	    cerr<<"Failed to specialize length in 'cross::draw'"<<endl;
	    cerr<<"This graph will not be drawn"<<endl;
	    return;
	}
	draw(t);
    }

    void cross::draw(terminal *t)
    {
	t->draw_line(terminal::coord(l1_.termspecific_id(), l1_.termspecific_id()),
		     terminal::coord(l2_.termspecific_id(), l2_.termspecific_id()));
	t->draw_line(terminal::coord(l1_.termspecific_id(), l2_.termspecific_id()),
		     terminal::coord(l2_.termspecific_id(), l1_.termspecific_id()));
    }


    // ---------------- star4  -----------------------

    star4::star4(bool filled)  { fill(filled); }
    
    point_drawer *star4::clone() const
    {
	return new star4(fill_);
    }

    void star4::prepare_for_draw(const length &size)
    {
	l1_ = -0.5 * !size;
	l2_ =  0.5 * !size;
	l3_ = -0.1 * !size;
	l4_ =  0.1 * !size;
	l1_.register_me();
	l2_.register_me();
	l3_.register_me();
	l4_.register_me();
    }

    void star4::draw(terminal *t,const length &s)
    {
	l1_ = -0.5 * !s;
	l2_ =  0.5 * !s;
	l3_ = -0.1 * !s;
	l4_ =  0.1 * !s;
	if(!l1_.specialize(t) || !l2_.specialize(t) || !l3_.specialize(t) || !l4_.specialize(t))
	{
	    cerr<<"Failed to specialize length in 'star4::draw'"<<endl;
	    cerr<<"This graph will not be drawn"<<endl;
	    return;
	}
	draw(t);
    }

    void star4::draw(terminal *t)
    {
	vector<terminal::coord> cc;
	cc.push_back(terminal::coord(l1_.termspecific_id(),terminal::id(0,2)));
	cc.push_back(terminal::coord(l3_.termspecific_id(),l3_.termspecific_id()));
	cc.push_back(terminal::coord(terminal::id(0,1),l1_.termspecific_id()));
	cc.push_back(terminal::coord(l4_.termspecific_id(),l3_.termspecific_id()));
	cc.push_back(terminal::coord(l2_.termspecific_id(),terminal::id(0,2)));
	cc.push_back(terminal::coord(l4_.termspecific_id(),l4_.termspecific_id()));
	cc.push_back(terminal::coord(terminal::id(0,1),l2_.termspecific_id()));
	cc.push_back(terminal::coord(l3_.termspecific_id(),l4_.termspecific_id()));
	cc.push_back(terminal::coord(l1_.termspecific_id(),terminal::id(0,2)));
	if(fill_) t->fill_polygon(cc);
	else t->draw_lines(cc);	
    }

    // ---------------------- autopoint --------------------------------

    autopoint::autopoint(const autopoint &rhs)
    {
	if(rhs.drawer_) drawer_ = rhs.drawer_->clone();
	else drawer_ = 0;
    }

    point_drawer *autopoint::clone() const
    {
	return new autopoint(*this);
    }

    void autopoint::draw(terminal *t)
    {
	if(drawer_) drawer_->draw(t);
	else warning::print("Type is not set for autopoint","autopoint::draw(terminal*)");
    }
    void autopoint::draw(terminal *t, const length &l)
    {
	if(drawer_) drawer_->draw(t,l);
	else warning::print("Type is not set for autopoint","autopoint::draw(terminal*,const length &)");
    }
    void autopoint::prepare_for_draw(const length &l)
    {
	if(drawer_) drawer_->prepare_for_draw(l);
	else warning::print("Type is not set for autopoint","autopoint::prepare_for_draw(const length &)");
    }
    bool autopoint::equals(point_drawer *other)
    {
	if(!drawer_) return false;
	return drawer_->equals(other);
    }


    const point_drawer &point_by_index(unsigned int ind)
    {
        static vector<point_drawer*> the_types;
        if(the_types.empty())
        {
            the_types.push_back(new fsquare);
            the_types.push_back(new square);
            the_types.push_back(new fdiamond);
            the_types.push_back(new diamond);
            the_types.push_back(new ftriangledown);
            the_types.push_back(new triangledown);
            the_types.push_back(new ftriangle);
            the_types.push_back(new triangle);
            the_types.push_back(new fcircle);
            the_types.push_back(new circle);
            the_types.push_back(new fstar4);
            the_types.push_back(new star4);
            the_types.push_back(new plus);
            the_types.push_back(new cross);
        }
        return *the_types[ind%the_types.size()];
    }
}
