#ifndef __BLOP_POINT_DRAWER_H__
#define __BLOP_POINT_DRAWER_H__

#include "terminal.h"
#include "length.h"
#ifndef __MAKECINT__
#include <variant>
#endif

/*

If you add a new pointtype, don't forget to update frame::prepare_for_draw() in frame.cc

 */

namespace blop
{
    class graph;

    //----------------------------------------------------------
    // Each of these classes implement a specific shape for a point
    // (to be used for datapoint visualization)

    class point_drawer
	{
        protected:
            bool fill_;
	public:
	    virtual void draw(terminal *) = 0;
	    virtual void draw(terminal *,const length &size) = 0;
	    virtual point_drawer *clone() const = 0;
	    virtual void prepare_for_draw(const length &)=0;

            point_drawer() { fill_ = false; }
	    virtual ~point_drawer() {}
	    virtual bool equals(point_drawer *other) = 0;

            virtual point_drawer &fill(bool f) { fill_ = f; return *this; }

            static const point_drawer &get(unsigned int n);

	};

    typedef point_drawer point_type;
    
    bool equals(point_drawer *p1, point_drawer *p2);

    class square : public point_drawer
	{
	private:
	    length l1,l2;
	public:
            square(bool filled=false);
	    point_drawer *clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(point_drawer *other)
		{
		    if(square *p = dynamic_cast<square *>(other))
		    {
			return p -> fill_ == fill_;
		    }
		    return false;
		}
	};
    class fsquare : public square { public: fsquare() : square(true) {} };

    class diamond : public point_drawer
	{
	private:
	    length l1,l2;
	public:
            diamond(bool filled=false);
	    point_drawer *clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(point_drawer *other)
		{
		    if(diamond *p = dynamic_cast<diamond *>(other))
		    {
			return p->fill_ == fill_;
		    }
		    return false;
		}
	};
    class fdiamond : public diamond { public: fdiamond() : diamond(true) {} };

    class triangle : public point_drawer
	{
	private:
	    length x1,x2,y1,y2;
	    bool up_;
	public:
	    triangle(bool filled=false,bool dir_up=true);
	    point_drawer *clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(point_drawer *other)
		{
		    if(triangle *p = dynamic_cast<triangle*>(other))
		    {
			return fill_ == p->fill_ && up_ == p->up_;
		    }
		    return false;
		}
	};
    class ftriangle : public triangle { public: ftriangle() : triangle(true,true) {}};
    class triangledown : public triangle { public: triangledown() : triangle(false,false) {}};
    class ftriangledown : public triangle { public: ftriangledown() : triangle(true,false) {}};

    class circle : public point_drawer
	{
	private:
	    length r_;
	public:
	    circle(bool filled = false);
	    point_drawer *clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(point_drawer *other) { return false; }
	};
    class fcircle : public circle { public: fcircle() : circle(true) {}};

    class plus : public point_drawer
	{
	private:
	    length l1_,l2_;
	public:
	    point_drawer *clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(point_drawer *other) { return false; }
	};

    class cross : public point_drawer
	{
	private:
	    length l1_,l2_;
	public:
	    point_drawer *clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(point_drawer *other) { return false; }
	};

    class star4 : public point_drawer
	{
	private:
	    length l1_,l2_,l3_,l4_;
	public:
	    star4(bool fill=false);
	    point_drawer *clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(point_drawer *other)
		{
		    if(star4 *p = dynamic_cast<star4*>(other))
		    {
			return fill_ == p->fill_;
		    }
		    return false;
		}
	};
    class fstar4 : public star4 { public: fstar4() : star4(true) {}};

    class autopoint : public point_drawer
	{
	private:
	    point_drawer *drawer_;

	public:
            autopoint() : drawer_(0) {}
	    autopoint(const autopoint &);
	    ~autopoint() {if(drawer_) delete drawer_; }

	    point_drawer *clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(point_drawer *other);
            point_drawer *drawer() const { return drawer_; }
	    void drawer(point_drawer *d) { if(drawer_) delete drawer_; drawer_ = d; }

	};

    const point_drawer &point_by_index(unsigned int ind);

#ifndef __MAKECINT__    
    typedef std::variant<blop::square,blop::fsquare,blop::diamond,blop::fdiamond,blop::triangle,blop::ftriangle,blop::triangledown,blop::ftriangledown,blop::circle,blop::fcircle,blop::plus,blop::cross,blop::star4,blop::fstar4,blop::autopoint> point_drawers;
#endif

}
#endif
