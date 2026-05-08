#ifndef __BLOP_CANVAS_H__
#define __BLOP_CANVAS_H__

#include "pad.h"

namespace blop
{
    class canvas : public pad
	{

	// The 'canvas' is the root of the object hierarchy, it is the topmost pad

	private:
	    static canvas *current_canvas_;
	public:
	    void print_hierarchy(int i=0);
	    canvas();
	    ~canvas();
	    bool parent(container *p);
	    static canvas &def();
	    static canvas &current();
	    void print(terminal *term);

	    void cd_specific();

	    void invisible() { cerr<<"You can not make a canvas invisible"<<endl;}
	    void off()       { cerr<<"You can not swith off a canvas"<<endl;}

	    void clear();

	    canvas &layer(const var & l) { cerr<<"You can not change the layer of a canvas"<<endl; return *this; }
	};
}
#endif
