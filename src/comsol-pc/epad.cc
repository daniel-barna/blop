#include "epad.h"
#include "box.h"
#include <vector>

using namespace std;

namespace blop
{
    epad::epad(const length &x1, const length &y1,
	       const length &x2, const length &y2)
	: pad(x1,y1,x2,y2), width_fixed_(false), height_fixed_(false)
    {
	name("epad");
	left(0.0);
	bottom(0.0);
	width(0.0);
	height(0.0);
    }

    void epad::prepare_for_draw()
    {
	if(print_me_<1) return;
	recalculate_size_();
	pad::prepare_for_draw();
    }

    void epad::recalculate_size_()
    {
	if(!content_.empty())
	{
	    vector<length> widths,heights;
	    for(unsigned int i=0; i<content_.size(); ++i)
	    {
		box *b = dynamic_cast<box*>(content_[i]);
		// Take into account only contents with visibility>0, that is
		// visible(2), or invisible but present(1)
		if(b && b->visibility()>0)
		{
		    if(!width_fixed_ )  widths .push_back(!b->width());
		    if(!height_fixed_)  heights.push_back(!b->height());
		}
	    }
	    if(!width_fixed_) width(length::max(widths));
	    if(!height_fixed_) height(length::max(heights));
	}
	else
	{
	    if(!width_fixed_)  width(0.0);
	    if(!height_fixed_) height(0.0);
	}
    }

    void epad::add(grob *g)
    {
	if(g == 0) return;
	container::add(g);
	recalculate_size_();
    }

    bool epad::remove(grob *g)
    {
	bool removed = container::remove(g);
	if(removed) recalculate_size_();
	return removed;
    }
}
