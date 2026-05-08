#include "canvas.h"
#include "frame.h"
#include "exc.hh"
#include "warning.h"

namespace blop
{
    // ------------------  canvas ------------------

    void canvas::print_hierarchy(int level)
    {
	for(int i=0; i<level*3; ++i) cout<<" ";
	cout<<name()<<"("<<this<<")"<<endl;
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    content_[i]->print_hierarchy(level+1);
	}
    }

    canvas *canvas::current_canvas_ = 0;

    void canvas::clear()
    {
	pad::clear();
	current_pad_ = 0;
    }

    bool canvas::parent(container *)
    {
	warning::print("A canvas is the topmost in the hierarchy, "
		       "it can not be added into another container",
		       "canvas::parent(container*)");
	return false;
    }

    canvas::canvas()
	: pad(ZERO,ZERO,PW,PH,true)
    {
	name("canvas");
    }

    canvas::~canvas()
    {
	if(current_pad_    == this) current_pad_    = 0;
	if(current_canvas_ == this) current_canvas_ = 0;
    }

    void canvas::cd_specific()
    {
	current_canvas_ = this;
        pad::cd_specific();
	//current_pad_ = this;
	//frame::current_ = 0;
    }

    canvas &canvas::def()
    {
	static canvas c;
	static bool first_call = true;
	if(first_call)
	{
	    first_call = false;
	    c.cd();
	}
	return c;
    }

    canvas &canvas::current()
    {
	if(current_canvas_ == 0) def().cd();
	return *current_canvas_;
    }

    void canvas::print(terminal *term)
	TRY
    {
	length::clear_regs();
	for(unsigned int i=0; i<content_.size(); ++i)
	{
	    content_[i]->prepare_for_draw();
	}

	term->picture_begin();

	length::specialize_regs(term);

	draw_border_bg(term);

	for(vector<grob *>::size_type i=0; i<content_.size(); ++i)
	{
	    term->reset_transformation();
	    content_[i]->print(term);
	}

	term->picture_end();
	modified(false);
    }
    CATCH("canvas::print(terminal *)")
}
