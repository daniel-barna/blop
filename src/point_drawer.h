#ifndef __BLOP_POINT_DRAWER_H__
#define __BLOP_POINT_DRAWER_H__

#include "terminal.h"
#include "length.h"
#include "factory.h"
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

    class point_drawer : public factory_base<point_drawer>
	{
            FACTORY(point_drawer);
        protected:
            bool fill_;
	public:
	    virtual void draw(terminal *) = 0;
	    virtual void draw(terminal *,const length &size) = 0;
	    virtual smartptr<point_drawer> clone() const = 0;
	    virtual void prepare_for_draw(const length &)=0;

            point_drawer() { fill_ = false; }
	    virtual ~point_drawer() {}
	    virtual bool equals(smartptr<point_drawer> other) = 0;

            virtual point_drawer &fill(bool f) { fill_ = f; return *this; }

            static smartptr<point_drawer> get(unsigned int n);

	};

    typedef point_drawer point_type;
    
    bool equals(smartptr<point_drawer> p1, smartptr<point_drawer> p2);

    class square : public point_drawer
	{
            FACTORY(square);
	private:
	    length l1,l2;
	public:
            square(bool filled=false);
	    smartptr<point_drawer> clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(smartptr<point_drawer> other)
		{
		    if(auto p = other.dyncast<square>()) return p -> fill_ == fill_;
		    return false;
		}
	};
    class fsquare : public square
    {
        FACTORY(fsquare);
    public:
        fsquare() : square(true) {}
    };

    class diamond : public point_drawer
	{
            FACTORY(diamond);
	private:
	    length l1,l2;
	public:
            diamond(bool filled=false);
	    smartptr<point_drawer> clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(smartptr<point_drawer> other)
		{
		    if(auto p = other.dyncast<diamond>())
		    {
			return p->fill_ == fill_;
		    }
		    return false;
		}
	};
    class fdiamond : public diamond
    {
        FACTORY(fdiamond);
    public:
        fdiamond() : diamond(true) {}
    };

    class triangle : public point_drawer
	{
            FACTORY(triangle);
	private:
	    length x1,x2,y1,y2;
	    bool up_;
	public:
	    triangle(bool filled=false,bool dir_up=true);
	    smartptr<point_drawer> clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(smartptr<point_drawer> other)
		{
		    if(auto p = other.dyncast<triangle>())
		    {
			return fill_ == p->fill_ && up_ == p->up_;
		    }
		    return false;
		}
	};
    class ftriangle : public triangle
    {
        FACTORY(ftriangle);
    public:
        ftriangle() : triangle(true,true) {}
    };
    class triangledown : public triangle
    {
        FACTORY(triangledown);
    public:
        triangledown() : triangle(false,false) {}
    };
    class ftriangledown : public triangle
    {
        FACTORY(ftriangledown);
    public:
        ftriangledown() : triangle(true,false) {}
    };

    class circle : public point_drawer
	{
            FACTORY(circle);
	private:
	    length r_;
	public:
	    circle(bool filled = false);
	    smartptr<point_drawer> clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(smartptr<point_drawer> other) { return false; }
	};
    class fcircle : public circle
    {
        FACTORY(fcircle);
    public:
        fcircle() : circle(true) {}
    };

    class plus : public point_drawer
	{
            FACTORY(plus);
	private:
	    length l1_,l2_;
	public:
	    smartptr<point_drawer> clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(smartptr<point_drawer> other) { return false; }
	};

    class cross : public point_drawer
	{
            FACTORY(cross);
	private:
	    length l1_,l2_;
	public:
	    smartptr<point_drawer> clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(smartptr<point_drawer> other) { return false; }
	};

    class star4 : public point_drawer
	{
            FACTORY(star4);
	private:
	    length l1_,l2_,l3_,l4_;
	public:
	    star4(bool fill=false);
	    smartptr<point_drawer> clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(smartptr<point_drawer> other)
		{
		    if(auto p = other.dyncast<star4>())
		    {
			return fill_ == p->fill_;
		    }
		    return false;
		}
	};
    class fstar4 : public star4
    {
        FACTORY(fstar4);
    public:
        fstar4() : star4(true) {}
    };

    class autopoint : public point_drawer
	{
            FACTORY(autopoint);
	private:
	    smartptr<point_drawer> drawer_;

	public:
            autopoint() : drawer_(0) {}
	    autopoint(const autopoint &);
	    ~autopoint() {}

	    smartptr<point_drawer> clone() const;
	    void draw(terminal *);
	    void draw(terminal *,const length &);
	    void prepare_for_draw(const length &);
	    bool equals(smartptr<point_drawer> other);
            smartptr<point_drawer> drawer() const { return drawer_; }
	    void drawer(smartptr<point_drawer> d) { drawer_ = d; }
	};

//    smartptr<point_drawer> point_by_index(unsigned int ind);

#ifndef __MAKECINT__    
    typedef std::variant<blop::square,blop::fsquare,blop::diamond,blop::fdiamond,blop::triangle,blop::ftriangle,blop::triangledown,blop::ftriangledown,blop::circle,blop::fcircle,blop::plus,blop::cross,blop::star4,blop::fstar4,blop::autopoint> point_drawers;
#endif

}
#endif
