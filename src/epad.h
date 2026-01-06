#ifndef __BLOP_EPAD_H__
#define __BLOP_EPAD_H__

#include "pad.h"

namespace blop
{
    class epad : public pad
    {
    private:
	bool width_fixed_, height_fixed_;
	void recalculate_size_();
    public:
	epad(const length &x1=0.0, const length &y1=0.0,
	     const length &x2=1.0, const length &y2=1.0);

	//------------------- add/remove objects -----------------------
	// and resize this epad also, to the maximum of the widths/heights
	// of its contents

	void add(grob *g);
	bool remove(grob *g);
	void prepare_for_draw();


/*
	epad &left(const length &l);
	epad &right(const length &l) { box::right(l); return *this; }
	epad &width(const length &l);
	epad &xcenter(const length &l) { box::xcenter(l); return *this; }
	epad &bottom(const length &l) { box::bottom(l); return *this; }
	epad &top(const length &l) { box::top(l); return *this; }
	epad &height(const length &l) { box::height(l); return *this; }
	epad &ycenter(const length &l) { box::ycenter(l); return *this; }
*/

	epad &layer(const var & l) { pad::layer(l); return *this; }

	epad &fix_width(bool f) { width_fixed_ = f; return *this; }
	epad &fix_height(bool f) { height_fixed_ = f; return *this; }
    };
}

#ifdef __MAKECINT__
#pragma link off function blop::epad::recalculate_size_();
#endif

#endif
