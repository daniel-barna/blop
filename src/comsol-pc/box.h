#ifndef __BLOP_BOX_H__
#define __BLOP_BOX_H__

#include "var.h"
#include "grob.h"
#include <string>

namespace blop 
{
    using namespace std;

    class box : public grob
    {
    private:
	friend class container;
	enum history_values {LeftBottom,RightTop,WidthHeight,Center} xhistory_[2], yhistory_[2];
	

    private:
	length left_,right_,width_,xcenter_;
	length bottom_,top_,height_,ycenter_;
	bool   width_fixed_, height_fixed_;

	static void set_(const length &l,
			 length &left,
			 length &right,
			 length &width,
			 length &center,
			 bool width_fixed,
			 history_values history[2],
			 history_values todo,
			 bool &left_modified,
			 bool &right_modified);

	void set_left_(const length &, bool &left_modified, bool &right_modified);
	void set_right_(const length &, bool &left_modified, bool &right_modified);
	void set_xcenter_(const length &, bool &left_modified, bool &right_modified);
	void set_width_(const length &, bool &left_modified, bool &right_modified);
	void set_bottom_(const length &, bool &bottom_modified, bool &top_modified);
	void set_top_(const length &, bool &bottom_modified, bool &top_modified);
	void set_height_(const length &, bool &bottom_modified, bool &top_modified);
	void set_ycenter_(const length &, bool &bottom_modified, bool &top_modified);

    public:
	box();

        box &above_grid(bool f) { grob::above_grid(f); return *this; }

	const length &left() const;
	virtual box  &left(const length &);

	const length &right() const;
	virtual box  &right(const length &);

	const length &width() const;
	virtual box  &width(const length &);

	const length &xcenter() const;
	virtual box  &xcenter(const length &);

	const length &bottom() const;
	virtual box  &bottom(const length &);

	const length &top() const;
	virtual box  &top(const length &);

	const length &height() const;
	virtual box  &height(const length &);

	const length &ycenter() const;
	virtual box  &ycenter(const length &);

	virtual box  &fix_width(bool f);
	virtual box  &fix_height(bool f);

	virtual ~box() {}

	void prepare_for_draw();
	void print(terminal *) {}
    };

}

#ifdef __MAKECINT__
#pragma link off function blop::box::prepare_for_draw();
#endif

#endif



