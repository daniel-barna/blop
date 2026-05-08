#ifndef __BLOP_GROB_H__
#define __BLOP_GROB_H__

#include "length.h"
#include "terminal.h"
#include <vector>

namespace blop
{
    class pad;
    class container;

    /*
      grob == GRaphical OBject
      This is meant to be the base class for all graphical objects
    */


    class grob
    {
	friend class container;
    private:
	container *parent_;
	std::string name_;
        bool autodel_;

        bool above_grid_ = false;

    protected:

	virtual bool parent(container *p);

	    // When a grob is added into a container, the container
	    // calls this function to notify the grob about being
	    // added into a container. Derived classes can overload
	    // this function to make additional bookkeeping, but
	    // they should always call grob::parent(...) as well.
	    // If the function returns false (as in the case of
	    // the 'canvas'), it means that this object can not
	    // be added into a container, and the container will not
	    // store a reference to this object

	int print_me_;

	    // indicates status:
	    // 0 ==> do not print this object, do not specialize its lengths
	    // 1 ==> do not print, but calculate lengths
	    // 2 ==> print

	static length &default_x_unit_();
	static length &default_y_unit_();
	static bool default_use_parent_x_unit_, default_use_parent_y_unit_;

	length x_unit_, y_unit_, parent_cwidth_, parent_cheight_;

	bool modified_;

	var layer_;

    public:

        virtual grob &above_grid(bool f) { above_grid_ = f; return *this; }
        bool above_grid() const { return above_grid_; }

	string name() const {return name_;}
	void   name(const string &s) {name_ = s;}
	virtual bool modified() const { return modified_; }
	virtual void modified(bool f) { modified_ = f;    }

	// -----------  Returns the parent of this grob  --------------------------

	virtual container       *parent()       { return parent_; }
        virtual const container *parent() const { return parent_; }


	static void default_x_unit(length l);
	static void default_y_unit(length l);
	static void default_unit(length);
	enum { use_parent = 100 };
	static void default_x_unit(int);
	static void default_y_unit(int);
	static void default_unit(int);

	length x_unit();
	length y_unit();
	grob &x_unit(length);
	grob &x_unit(int);
	grob &y_unit(length);
	grob &y_unit(int);
	grob &unit(length);
	grob &unit(int);

	// -------------------------------------------------------------------------
	// This member function should make all necessary preparations,
	// which are needed for the object to be drawn (axes have to
	// calculate their ranges, lengths have to be registered, etc)
	// The different graphical objects, depending on their type,
	// will need different sets of lengths in order to be
	// drawn onto a terminal. These lengths will have to be
	// specialized (see length.h). In order to specialize all
	// needed lengths, they should be registered (length::register_me())
	// This member function should call length::register_me() on
	// the lengths, which are needed for it to be drawn.
	
	virtual void prepare_for_draw() = 0;


	
	// -------------------------------------------------------------------------
	// grobs can be contained in containers. Sine grobs can be 
	// created as automatic variables also (when their destructor
	// is automatically called when they scope expires), containers
	// do not deallocate these objects when they are cleared.
	// However, when creating grobs dynamically (operator new),
	// one's task is easier if containers deallocate the
	// objects. 
	// Containers (when deleted or cleared) will deallocate
	// those grobs in them, which have autodel=true 
	// autodel is false by default
	
        grob &autodel(bool b) { autodel_ = b; return *this; }
	bool autodel() const  { return autodel_; }

	// --------------------  change layer --------------------------------

	virtual grob      &layer(const var &l)  { layer_ = l; return *this; }
	virtual const var &layer() const { return layer_; }


	grob();


	// -------------------------------------------------------------------------
	// The destructor scans all existing containers (every container
	// is registered behind the scenes), and removes the currently 
	// destroyed grob from all containers, so you never have to worry,
	// that a container holds a reference to an unexisting grob.

	virtual ~grob();



	virtual void print(terminal *) = 0;


	// -------------------------------------------------------------------------
	// switch on or off the object (that is, the object will
	// be printed or not)

	virtual void on ()       {print_me_ = 2;}   // object will be printed
	virtual void invisible() {print_me_ = 1;}   // object not printed, but lengths calculated
	virtual void off()       {print_me_ = 0;}   // not printed, no lengths are calculated
	virtual int  visibility() const { return print_me_; } 


	virtual void print_hierarchy(int level=0);

        virtual grob &linecolor(const color &c) { return *this; }
        virtual grob &fillcolor(const color &c) { return *this; }
        virtual grob &linestyle(sym::linestyle s) { return *this; }
        virtual grob &linewidth(const length &l)  { return *this; }

    };

    class grobs
    {
    private:
        vector<grob*> grobs_;
    public:
        void add(grob *g) { grobs_.push_back(g); }

        grobs &linecolor(const color &c);
        grobs &lc(const color &c) { return linecolor(c); }

        grobs &fillcolor(const color &c);
        grobs &fc(const color &c) { return fillcolor(c); }

        grobs &linestyle(sym::linestyle s);
        grobs &ls(sym::linestyle s) { return linestyle(s); }

        grobs &linewidth(const length &l);
        grobs &lw(const length &l) { return linewidth(l); }
    };


}



#endif
