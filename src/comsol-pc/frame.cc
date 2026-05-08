#include "frame.h"
#include "exc.hh"
#include "graph.h"
#include "fgraph.h"
#include "graph_drawer.h"
#include "epad.h"
#include "warning.h"
#include "global.h"

#include <cstdio>
#include <algorithm>
#include <vector>

namespace blop
{
    length frame::default_linewidth_ = LW;
    length frame::default_gridwidth_ = LW;
    length frame::default_minor_gridwidth_ = 0.7*LW;
    length frame::default_gridcut_ = EX;

    bool frame::default_show_legend_ = true;

    color frame::default_gridcolor_ = color(0.7,0.7,0.7);
    color frame::default_minor_gridcolor_ = color(0.85,0.85,0.85);

    sym::linestyle frame::default_gridstyle_ = sym::solid;
    sym::linestyle frame::default_minor_gridstyle_ = sym::solid;

    frame::frame()
    TRY
    {
	name("frame");
	layer("frame");

	title_ = 0;

	nodistort_ = false;

        show_legend_ = default_show_legend_;

	frame::all().push_back(this);

	marginobjectsep_ = default_marginobjectsep_;
	for(int i=0; i<4; ++i)
	{
	    marginboxes_[i] = 0;
	    owns_marginboxes_[i] = true;
	}

	legendbox_ = new legendbox;
        legendbox_->fix_width(true).fix_height(true);
	add(legendbox_);
	owns_legendbox_ = true;

	draw_frame_ = default_draw_frame_;
	draw_axes_ = default_draw_axes_;
	foreground_ = default_foreground_;
	grid_foreground_ = default_grid_foreground_;

	framecolor_ = default_framecolor_;

	mirror_x1_ = true;
	mirror_x2_ = true;
	mirror_y1_ = true;
	mirror_y2_ = true;

	grid_ = default_grid_;
        grid_at_minor_tics_ = default_grid_at_minor_tics_;
	gridcolor_ = default_gridcolor_;
        minor_gridcolor_ = default_minor_gridcolor_;
	gridstyle_ = default_gridstyle_;
        minor_gridstyle_ = default_minor_gridstyle_;
	gridwidth_ = default_gridwidth_;
        minor_gridwidth_ = default_minor_gridwidth_;
	gridcut_ = default_gridcut_;
	linewidth_ = default_linewidth_;

	x1axis_ = new axis(axis::x1); owns_x1axis_ = true;
	y1axis_ = new axis(axis::y1); owns_y1axis_ = true;
	x2axis_ = new axis(axis::x2); owns_x2axis_ = true;
	y2axis_ = new axis(axis::y2); owns_y2axis_ = true;

	/*
	  x1axis_->pos(!container::cbottom(),!container::cleft(),!container::cright());
	  x2axis_->pos(!container::ctop(),!container::cleft(),!container::cright());
	  y1axis_->pos(!container::cleft(),!container::cbottom(),!container::ctop());
	  y2axis_->pos(!container::cright(),!container::cbottom(),!container::ctop());
	*/
	x1axis_->pos(0.,0.,!cwidth());
	x2axis_->pos(!cheight(),0.,!cwidth());
	y1axis_->pos(0.,0.,!cheight());
	y2axis_->pos(!cwidth(),0.,!cheight());

	x1axis_->axiscolor(framecolor_);
	x2axis_->axiscolor(framecolor_);
	y1axis_->axiscolor(framecolor_);
	y2axis_->axiscolor(framecolor_);

	left(0.5*EX);
	right(!parent_width_ - 0.5*EX);
	bottom(0.5*blop::height("X"));
	top(!parent_height_ - 0.5*blop::height("X"));

	bmargin(!x1axis_->scriptsize());
	tmargin(!x2axis_->scriptsize());
	lmargin(!y1axis_->scriptsize());
	rmargin(!y2axis_->scriptsize());

	legend_xalign_ = default_legend_xalign_;
	legend_yalign_ = default_legend_yalign_;
	legendpos(legend_xalign_, legend_yalign_);
    }
    CATCH("frame::frame()")

    frame::~frame()
    {
	// remove this frame from the all() vector
	{
	    std::vector<frame*>::iterator i=std::find(frame::all().begin(),
						      frame::all().end(),
						      this);
	    if(i==frame::all().end())
		err("Unregistered frame (programming error, please report)");
	    frame::all().erase(i);
	}

	if(owns_legendbox_) delete legendbox_;

	if(owns_x1axis_) delete x1axis_;
	if(owns_x2axis_) delete x2axis_;
	if(owns_y1axis_) delete y1axis_;
	if(owns_y2axis_) delete y2axis_;

	if(current_ == this) current_ = 0;

        delete title_;

	for(int i=0; i<4; ++i)
	{
	    if(owns_marginboxes_[i] && marginboxes_[i] != 0)
		delete marginboxes_[i];
	}

        // Delete those plottables which have their autodel flag set
        for(unsigned int i=0; i<graphs_.size(); ++i)
        {
            if(graphs_[i]->autodel()) delete graphs_[i];
        }
    }

    frame::frame(const frame &)
    {
	err("frame::frame(const frame &) should not be called");
    }

    void frame::operator=(const frame &)
    {
	err("frame::operator=(const frame &) should not be called");
    }

    void frame::remove_from_all(plottable *p)
    {
        if(p==0) return;
	for(unsigned int i=0; i<all().size(); ++i)
	{
	    all()[i]->remove(p);
	}
    }

    std::vector<frame*> &frame::all()
    {
	static std::vector<frame*> *f = new std::vector<frame*>;
	return *f;
    }

    frame *frame::current_ = 0;

    // default values
    sym::position frame::default_legend_xalign_ = sym::right;
    sym::position frame::default_legend_yalign_ = sym::top;
    void frame::default_legendpos(sym::position x, sym::position y) { default_legend_xalign_ = x; default_legend_yalign_ = y; }

    int frame::default_draw_frame_ = sym::left | sym::top | sym::right | sym::bottom;
    int frame::default_draw_axes_ = axis::x1|axis::x2|axis::y1|axis::y2;

    bool frame::default_foreground_ = false;
    void frame::default_foreground(bool b) { default_foreground_ = b; }
    bool frame::default_grid_foreground_ = false;

    int  frame::default_grid_ = 0;
    int  frame::default_grid_at_minor_tics_ = 0;
    void frame::default_grid(int i) { default_grid_ = i; }
    void frame::default_grid(bool i) { if(i) default_grid_ = axis::x1|axis::y1; else default_grid_ = 0; }
    void frame::default_grid_at_minor_tics(int i) { default_grid_at_minor_tics_ = i; }
    void frame::default_grid_at_minor_tics(bool i) { if(i) default_grid_at_minor_tics_ = axis::x1|axis::y1; else default_grid_at_minor_tics_ = 0; }

    color frame::default_framecolor_ = black;
    void frame::default_framecolor(const color &c) { default_framecolor_ = c; }

    length frame::default_marginobjectsep_(new length::base_id_t(terminal::EX));

    frame &frame::nodistort(bool f)
    {
	nodistort_ = f;

	// if no distortion is requested, release the caspect
	// because we don't know yet in advance the caspect,
	// only during prepare_for_draw(...), when the axis ranges
	// are set
	if(nodistort_) container::caspect(-1);

	return *this;
    }

    frame &frame::caspect(double s)
    {
	if(s>0) nodistort_ = false;
	bmargin(!x1axis_->scriptsize());
	tmargin(!x2axis_->scriptsize());
	container::caspect(s);
	return *this;
    }
    frame  &frame::cxlock(sym::position a)
    {
	container::cxlock(a);
	return *this;
    }
    frame &frame::cylock(sym::position a)
    {
	container::cylock(a);
	return *this;
    }

    frame &frame::lmarginbox(epad *p)
    {
	// If there was a previous marginbox, and it is owned by this frame,
	// first delete it
	if(marginboxes_[Left] != 0 && owns_marginboxes_[Left])
	    delete marginboxes_[Left];

	// This is an external marginbox, so we do not own it!
	owns_marginboxes_[Left] = false;

	marginboxes_[Left] = p;

	// the corresponding margin is set
	lmargin(!y1axis_->scriptsize()+!marginobjectsep_+!p->width());

	modified_ = true;
	return *this;
    }
    epad *frame::lmarginbox() const { return marginboxes_[Left]; }

    frame &frame::rmarginbox(epad *p)
    {
	if(marginboxes_[Right] != 0 && owns_marginboxes_[Right])
	    delete marginboxes_[Right];
	owns_marginboxes_[Right] = false;
	marginboxes_[Right] = p;
	lmargin(!y2axis_->scriptsize()+!marginobjectsep_+!p->width());
	modified_ = true;
	return *this;
    }
    epad *frame::rmarginbox() const { return marginboxes_[Right]; }

    frame &frame::bmarginbox(epad *p)
    {
	if(marginboxes_[Bottom] != 0 && owns_marginboxes_[Bottom])
	    delete marginboxes_[Bottom];
	owns_marginboxes_[Bottom] = false;
	marginboxes_[Bottom] = p;
	lmargin(!x1axis_->scriptsize()+!marginobjectsep_+!p->height());
	modified_ = true;
	return *this;
    }
    epad *frame::bmarginbox() const { return marginboxes_[Bottom]; }

    frame &frame::tmarginbox(epad *p)
    {
	if(marginboxes_[Top] != 0 && owns_marginboxes_[Top])
	    delete marginboxes_[Top];
	owns_marginboxes_[Top] = false;
	marginboxes_[Top] = p;
	lmargin(!x2axis_->scriptsize()+!marginobjectsep_+!p->height());
	modified_ = true;
	return *this;
    }
    epad *frame::tmarginbox() const { return marginboxes_[Top]; }

    frame &frame::lmarginobject(box *b)
    {
	if(marginboxes_[Left] == 0)
	{
	    owns_marginboxes_[Left] = true;
	    marginboxes_[Left] = new epad;
	    marginboxes_[Left]->autodel(false);
	    marginboxes_[Left]->right(-!y1axis_->scriptsize()-!marginobjectsep_);
	    marginboxes_[Left]->width(!b->width());
	    marginboxes_[Left]->bottom(0.0);
	    marginboxes_[Left]->top(1.0);
	    marginboxes_[Left]->fix_height(true);
	    marginboxes_[Left]->borderwidth(0.0);
	    add(marginboxes_[Left]);
	    lmargin(!y1axis_->scriptsize()+!marginobjectsep_+!marginboxes_[Left]->width());
	}
	marginboxes_[Left]->add(b);
	modified_ = true;
	return *this;
    }
    frame &frame::rmarginobject(box *b)
    {
	if(marginboxes_[Right] == 0)
	{
	    owns_marginboxes_[Right] = true;
	    marginboxes_[Right] = new epad;
	    marginboxes_[Right]->autodel(false);
	    marginboxes_[Right]->left(1.0+!y2axis_->scriptsize()+!marginobjectsep_);
	    marginboxes_[Right]->width(!b->width());
	    marginboxes_[Right]->bottom(0.0);
	    marginboxes_[Right]->top(1.0);
	    marginboxes_[Right]->fix_height(true);
	    marginboxes_[Right]->borderwidth(0.0);

	    add(marginboxes_[Right]);
	    rmargin(!y2axis_->scriptsize()+!marginobjectsep_+!marginboxes_[Right]->width());
	}

	marginboxes_[Right]->add(b);
	modified_ = true;
	return *this;
    }
    frame &frame::bmarginobject(box *b)
    {
	if(marginboxes_[Bottom] == 0)
	{
	    owns_marginboxes_[Bottom] = true;
	    marginboxes_[Bottom] = new epad;
	    marginboxes_[Bottom]->autodel(false);
	    marginboxes_[Bottom]->left(0.0);
	    marginboxes_[Bottom]->right(1.0);
	    marginboxes_[Bottom]->fix_width(true);
	    marginboxes_[Bottom]->top(-!x1axis_->scriptsize()-!marginobjectsep_);
	    marginboxes_[Bottom]->height(!b->height());
	    marginboxes_[Bottom]->borderwidth(0.0);
	    add(marginboxes_[Bottom]);
	    bmargin(!x1axis_->scriptsize()+!marginobjectsep_+!marginboxes_[Bottom]->height());
	}
	marginboxes_[Bottom]->add(b);
	modified_ = true;
	return *this;
    }
    frame &frame::tmarginobject(box *b)
    {
	if(marginboxes_[Top] == 0)
	{
	    owns_marginboxes_[Top] = true;
	    marginboxes_[Top] = new epad;
	    marginboxes_[Top]->autodel(false);
	    marginboxes_[Top]->left(0.0);
	    marginboxes_[Top]->right(1.0);
	    marginboxes_[Top]->fix_width(true);
	    marginboxes_[Top]->bottom(1.0+!x2axis_->scriptsize()+!marginobjectsep_);
	    marginboxes_[Top]->height(!b->height());
	    marginboxes_[Top]->borderwidth(0.0);
	    add(marginboxes_[Top]);
	    tmargin(!x2axis_->scriptsize()+!marginobjectsep_+!marginboxes_[Top]->height());
	}
	marginboxes_[Top]->add(b);
	modified_ = true;
	return *this;
    }


    void frame::cd_specific()
    {
	current_ = this;
    }


    frame &frame::legend(legendbox *l)
    {
        if(!l)
        {
            warning::print("Empty pointer","frame::legend(legendbox*)");
            return *this;
        }
        return legend(*l);
    }

    frame &frame::legend(legendbox &l)
    {
	owns_legendbox_ = false;
	remove(legendbox_);
	delete legendbox_;
	legendbox_ = &l;
	for(std::vector<plottable*>::iterator i = graphs_.begin();
	    i != graphs_.end(); ++i)
	{
	    l.add(*i);
	}
	return *this;
    }

    frame &frame::own_legend()
    {
	if(owns_legendbox_) return *this;
	legendbox *new_legendbox = new legendbox;
        new_legendbox->fix_width(true).fix_height(true);

	for(std::vector<plottable*>::iterator i = graphs_.begin();
	    i != graphs_.end(); ++i)
	{
	    if(legendbox_) legendbox_->remove(*i);
	    new_legendbox->add(*i);
	}
	add(legendbox_ = new_legendbox);
	owns_legendbox_ = true;
	return *this;
    }

    frame &frame::legendpos(sym::position xalign,sym::position yalign)
    TRY
    {

	modified_ = true;
	legend_xalign_ = xalign;
	legend_yalign_ = yalign;

	//legendbox_->width(legendbox_->width());
	//legendbox_->height(legendbox_->height());

	if(xalign == sym::right)
	{
	    if(mirror_y1_) legendbox_->right(!cwidth() - 1.3*!y1axis_->ticlength());
	    else           legendbox_->right(!cwidth() - 1.3*!y2axis_->ticlength());
	}
	else if(xalign == sym::left) legendbox_->left(1.3*!y1axis_->ticlength());
	else legendbox_->xcenter(0.5*!cwidth());

	if(yalign == sym::top)
	{
	    if(mirror_x1_) legendbox_->top(!cheight()- 1.3*!x1axis_->ticlength());
	    else           legendbox_->top(!cheight()- 1.3*!x2axis_->ticlength());
	}
	else if(yalign == sym::bottom) legendbox_->bottom(1.3*!x1axis_->ticlength());
	else legendbox_->ycenter(0.5*!cheight());


	return *this;
    }
    CATCH("frame::legend(int,int)")

    frame &frame::title(const var &t)
    {
	// if an empty string, clear the title
	if(t.str() == "")
	{
	    if(title_) delete title_;
            title_ = 0;
	    return *this; 
	}

	if(!title_)
	{
	    title_ = new label(t);
	    tmarginobject(title_);
	}
	else title_->text(t);
	title_->x(0.5,sym::center).y(0.5,sym::center);
	return *this; 
    }


    frame &frame::loffset(const length &l)
    {
	modified_ = true;
	left(l);
	//cleft_ = !left() + !lmargin_;
	return *this;
    }
    frame &frame::roffset(const length &l)
    {
	modified_ = true;
	right(!parent_width_ - l);
	//cright_ = !right() - !rmargin_;
	return *this;
    }
    frame &frame::boffset(const length &l)
    {
	modified_ = true;
	bottom(l);
	//cbottom_ = !bottom() + !bmargin_;
	return *this;
    }
    frame &frame::toffset(const length &l)
    {
	modified_ = true;
	top(!parent_height_ - l);
	//ctop_ = !top() - !tmargin_;
	return *this;
    }

    frame &frame::offsets(const length &l)
    {
	modified_ = true;
	toffset(l);
	boffset(l);
	loffset(l);
	roffset(l);
	return *this;
    }

    frame &frame::mknew(container &parent)
    {
	frame *f = new frame;
	f->autodel(true);
	parent.add(f);
	f->cd();
	return *f;
    }

    frame &frame::current()
    TRY
    {
	if(!current_)
	{
	    return mknew();
	}
	return *current_;
    }
    CATCH("frame::current()")


    bool frame::parent(container *p)
    {
	grob::parent(p);
	parent_width_ = !p->cwidth();
	parent_height_ = !p->cheight();
	return true;
    }

    frame &frame::remove(plottable *g)
    {
        if(g==0) return *this;
	for(unsigned int i=0; i<graphs_.size(); ++i)
	{
	    if(graphs_[i] == g)
	    {
		graphs_[i]->parent(0);
		graphs_.erase(graphs_.begin()+i);
		modified_ = true;
		--i;
	    }
	}
	for(unsigned int i=0; i<ordering_.size(); ++i)
	{
	    if(ordering_[i].plottable_ == g)
	    {
		ordering_.erase(ordering_.begin()+i);
		--i;
	    }
	}
	if(legendbox_) legendbox_->remove(g);
	return *this;
    }

    void frame::add(grob *g)
    {
	ordering_.push_back(grob_plottable(g,0));
	container::add(g);
    }
    bool frame::remove(grob *g)
    {
        if(g==0) return false;
	for(unsigned int i=0; i<ordering_.size(); ++i)
	{
	    if(ordering_[i].grob_ == g)
	    {
		ordering_.erase(ordering_.begin()+i);
		--i;
	    }
	}
	return container::remove(g);
    }

    frame &frame::add(plottable *p)
    {
	if(p == 0) return *this; 
	graphs_.push_back(p);
	legendbox_->add(p);
	p->parent(this);
	modified_ = true;
	ordering_.push_back(grob_plottable(0,p));
	return *this;
    }
    
    frame &frame::clear_graphs(bool all)
    {
	modified_ = true;
	if(owns_x1axis_) x1axis_->clear_autosettings();
	if(owns_x2axis_) x2axis_->clear_autosettings();
	if(owns_y1axis_) y1axis_->clear_autosettings();
	if(owns_y2axis_) y2axis_->clear_autosettings();

	for(int i=0; i<(int)graphs_.size(); ++i)
	{
	    if(!graphs_[i]->permanent() || all==true)
	    {
		plottable *p = graphs_[i];
		p->parent(0);
		legendbox_->remove(p);
		graphs_.erase(graphs_.begin()+i);
		if(p->autodel()) delete p;
		--i;
		for(unsigned int j=0; j<ordering_.size(); ++j)
		{
		    if(ordering_[j].plottable_ == p)
		    {
			ordering_.erase(ordering_.begin()+j);
			--j;
		    }
		}
	    }
	}
	return *this;
    }

    frame &frame::clear_graphs_and_autosettings(bool all)
    {
	clear_graphs(all);
	x1axis()->clear_autosettings();
	x2axis()->clear_autosettings();
	y1axis()->clear_autosettings();
	y2axis()->clear_autosettings();
	return *this;
    }

    void frame::clear()
    {
	container::clear();

	// re-add the marginboxes 
	for(int i=0; i<4; ++i)
	{
	    if(owns_marginboxes_[i]) add(marginboxes_[i]);
	}

	if(owns_x1axis_) x1axis_->clear();
	if(owns_x2axis_) x2axis_->clear();
	if(owns_y1axis_) y1axis_->clear();
	if(owns_y2axis_) y2axis_->clear();

	if(owns_legendbox_) legendbox_->clear();
	else
	{
	    for(unsigned int i=0; i<graphs_.size(); ++i)
	    {
		legendbox_->remove(graphs_[i]);
	    }
	}
	modified_ = true;

	// first copy all the plottable pointers (because when deleting them,
	// their destructor will try to remove them again from the graphs_
	// vector, making a problem)
	vector<plottable*> graphs = graphs_;

	// then clear the graphs_ vector, so the destructor of the plottables
	// will not do anything
	graphs_.clear();

	// then delete all the graphs with the autodel flag set to true.
	// They will try to remove themselves, but they are already removed,
	// so no problem
	for(unsigned int i=0; i<graphs.size(); ++i)
	{
	    if(graphs[i]->autodel()) delete graphs[i];
	}

	// Finally, remove all graphs from the ordering_ array
	for(unsigned int i=0; i<ordering_.size(); ++i)
	{
	    if(ordering_[i].plottable_)
	    {
		ordering_.erase(ordering_.begin()+i);
		--i;
	    }
	}
	
    }


    static bool length_depends_on_any(const length &l, const length* d[], int n)
    {
	for(int i=0; i<n; ++i)
	{
	    if(l.depends_on(*(d[i]))) return true;
	}
	return false;
    }

    void frame::prepare_for_draw()
    {
	if(global::debug > 0) cout<<"[blop] [frame] prepare_for_draw starts..."<<endl;

	// switch off those marginboxes, which are empty, to avoid
	// unnecessary output code in the output file
	for(int i=0; i<4; ++i)
	{
	    if(marginboxes_[i] && marginboxes_[i]->empty()) marginboxes_[i]->off();
	}

	// Set the autopoints
	{
	    std::vector<int> n(14,0);
	    for(vector<plottable*>::size_type i=0; i<graphs_.size(); ++i)
	    {
		// now only check those which do not have autopoint, and which are drawn
		// with points
		if(!graphs_[i]->drawstyle() || !graphs_[i]->drawstyle()->draws_points()) continue;
		point_drawer *pt = graphs_[i]->pointtype();
		if(dynamic_cast<autopoint*>(pt)) continue;

		if(dynamic_cast<fsquare*>(pt)) ++n[0];
		if(dynamic_cast<square*>(pt)) ++n[1];
		if(dynamic_cast<fdiamond*>(pt)) ++n[2];
		if(dynamic_cast<diamond*>(pt)) ++n[3];
		if(dynamic_cast<ftriangledown*>(pt)) ++n[4];
		if(dynamic_cast<triangledown*>(pt)) ++n[5];
		if(dynamic_cast<ftriangle*>(pt)) ++n[6];
		if(dynamic_cast<triangle*>(pt)) ++n[7];
		if(dynamic_cast<fcircle*>(pt)) ++n[8];
		if(dynamic_cast<circle*>(pt)) ++n[9];
		if(dynamic_cast<plus*>(pt)) ++n[10];
		if(dynamic_cast<cross*>(pt)) ++n[11];
		if(dynamic_cast<fstar4*>(pt)) ++n[12];
		if(dynamic_cast<star4*>(pt)) ++n[13];
	    }

	    for(vector<plottable*>::size_type i=0; i<graphs_.size(); ++i)
	    {
		autopoint *pt = dynamic_cast<autopoint*>(graphs_[i]->pointtype());

		// now only check those which have autopoint
		if(!pt) continue;

		vector<int>::size_type min = 0;
		for(vector<int>::size_type p=0; p<n.size(); ++p) if(n[p]<n[min]) min=p;

		if(graphs_[i]->drawstyle() && graphs_[i]->drawstyle()->draws_points()) ++n[min];
		switch(min) {
		case 0: pt->drawer(new fsquare()); break;
		case 1: pt->drawer(new square()); break;
		case 2: pt->drawer(new fdiamond()); break; 
		case 3: pt->drawer(new diamond()); break; 
		case 4: pt->drawer(new ftriangledown()); break; 
		case 5: pt->drawer(new triangledown()); break;
		case 6: pt->drawer(new ftriangle()); break;
		case 7: pt->drawer(new triangle()); break;
		case 8: pt->drawer(new fcircle()); break;
		case 9: pt->drawer(new circle()); break;
		case 10: pt->drawer(new plus()); break;
		case 11: pt->drawer(new cross()); break;
		case 12: pt->drawer(new fstar4()); break; 
		case 13: pt->drawer(new star4()); break;
		default: pt->drawer(new fsquare()); 
		}
	    }
	}

	// set the autocolors
	{
	    std::map<int,int> linecolor, pointcolor;
	    for(vector<plottable*>::size_type i=0; i<graphs_.size(); ++i)
	    {
		// no need to check for non-autocolor, they will anyway show up as
		// -1 returned by sequence_index, which we will anyway skip
		if(graphs_[i]->drawstyle())
		{
		    if(graphs_[i]->drawstyle()->uses_linecolor()) ++linecolor [color::sequence_index(graphs_[i]->linecolor ())];
		    if(graphs_[i]->drawstyle()->draws_points()  ) ++pointcolor[color::sequence_index(graphs_[i]->pointcolor())];
		}
	    }
	    
	    for(vector<plottable*>::size_type i=0; i<graphs_.size(); ++i)
	    {
		if(!graphs_[i]->drawstyle()) continue;
		if(graphs_[i]->drawstyle()->uses_linecolor() && graphs_[i]->linecolor() == autocolor)
		{
		    int min = 0;
		    for(int c=0; c<(int)color::default_sequence().size(); ++c)
		    {
			if(linecolor[c]<linecolor[min]) min=c;
		    }
		    graphs_[i]->linecolor(color::default_sequence()[min]);
		    ++linecolor[min];
		}
		if(graphs_[i]->drawstyle()->draws_points() && graphs_[i]->pointcolor() == autocolor)
		{
		    int min = 0;
		    for(int c=1; c<(int)color::default_sequence().size(); ++c)
		    {
			if(pointcolor[c]<pointcolor[min]) min=c;
		    }
		    graphs_[i]->pointcolor(color::default_sequence()[min]);
		    ++pointcolor[min];
		}
	    }
	}

	for(vector<plottable*>::size_type i=0; i<graphs_.size(); ++i)
	{
	    // process now the data graphs in the first round, to
	    // estimate the axis ranges.
	    // skip fgraphs now, they will be processed in a second round,
	    // when the requested axis ranges are set up, so that
	    // fgraphs can estimate their parameter ranges (if undefined)
	    // from the axis ranges
	    if(dynamic_cast<fgraph*>(graphs_[i]) != 0) continue;

	    if(global::debug>0) cout<<"[blop] [frame] processing data graph["<<i<<"]"<<endl;

	    axis *xaxis =
		(graphs_[i]->xaxis() == axis::x1 ? x1axis_ : x2axis_);
	    axis *yaxis =
		(graphs_[i]->yaxis() == axis::y1 ? y1axis_ : y2axis_);

	    graphs_[i]->prepare_for_draw(xaxis,yaxis,this,1);

	    graph_drawer *drawer = graphs_[i]->drawstyle();
	    if(drawer == 0) err("No drawer specified");
	    // stupid, this should be called from within plottable::prepare_for_draw...!
	    drawer->set_ranges(graphs_[i],xaxis,yaxis);
	}

	// In a second loop process only the functions (fgraphs)
	for(vector<plottable*>::size_type i=0; i<graphs_.size(); ++i)
	{
	    if(dynamic_cast<fgraph*>(graphs_[i]) == 0) continue;
	    if(global::debug>0) cout<<"[blop] [frame] processing function graphs["<<i<<"]"<<endl;
	    axis *xaxis =
		(graphs_[i]->xaxis() == axis::x1 ? x1axis_ : x2axis_);
	    axis *yaxis =
		(graphs_[i]->yaxis() == axis::y1 ? y1axis_ : y2axis_);
	    graphs_[i]->prepare_for_draw(xaxis,yaxis,this,1);
	    graph_drawer *drawer = graphs_[i]->drawstyle();
	    if(drawer == 0) err("No drawer specified");
	    drawer->set_ranges(graphs_[i],xaxis,yaxis);
	}

	// Now another loop again over all graphs, make second level preparation
	for(vector<plottable*>::size_type i=0; i<graphs_.size(); ++i)
	{
	    if(global::debug>0) cout<<"[blop] [frame] reprocessing graph["<<i<<"]"<<endl;
	    axis *xaxis =
		(graphs_[i]->xaxis() == axis::x1 ? x1axis_ : x2axis_);
	    axis *yaxis =
		(graphs_[i]->yaxis() == axis::y1 ? y1axis_ : y2axis_);
	    graphs_[i]->prepare_for_draw(xaxis,yaxis,this,2);
	}

	if(owns_x1axis_) x1axis_->calculate_tics();
	if(owns_x2axis_) x2axis_->calculate_tics();
	if(owns_y1axis_) y1axis_->calculate_tics();
	if(owns_y2axis_) y2axis_->calculate_tics();

	bool nodistort_used = true;
	if(!nodistort_) nodistort_used = false;
	else
	{
	    if(x1axis_->logscale() || y1axis_->logscale())
	    {
		warning::print("Can not use nodistort with logscales");
		nodistort_used = false;
	    }

	    // The algorithm is quite complicated, difficult to understand
	    // even for me :-)
	    // so therefore make sure the conditions are quite simple,
	    // and the user has not messed up with complicated dependencies
	    // between margins, widths, etc
	    const length *donotdependonthis[] =
		{&lmargin(), &rmargin(), &cwidth(), &cleft(), &cright(), &cxcenter(),
		 &bmargin(), &tmargin(), &cheight(), &cbottom(), &ctop(), &cycenter()};
	    if(length_depends_on_any(lmargin(),donotdependonthis,12) ||
	       length_depends_on_any(rmargin(),donotdependonthis,12) ||
	       length_depends_on_any(bmargin(),donotdependonthis,12) ||
	       length_depends_on_any(tmargin(),donotdependonthis,12))
	    {
		warning::print("Some nontrivial length-dependencies detected "
			       "for the margins of this frame, nodistort will not be used");
		nodistort_used = false;
	    }
	}

	if(nodistort_used)
	{
	    container::caspect((y1axis_->max()-y1axis_->min())/(x1axis_->max()-x1axis_->min()));
	}

	container::prepare_for_draw();

	if(owns_x1axis_) x1axis_->prepare_for_draw();
	if(owns_x2axis_) x2axis_->prepare_for_draw();
	if(owns_y1axis_) y1axis_->prepare_for_draw();
	if(owns_y2axis_) y2axis_->prepare_for_draw();

        if(show_legend_)
        {
            legendbox_->update();
            legendbox_->prepare_for_draw();
        }

	if(mirror_x1_) cheight().register_me();
	if(mirror_y1_) cwidth().register_me();

	if(grid_)
	{
	    if(grid_)               gridwidth_.register_me();
            if(grid_at_minor_tics_) minor_gridwidth_.register_me();
	    if(!x1axis_->cuts_.empty() ||
	       !x2axis_->cuts_.empty() ||
	       !y1axis_->cuts_.empty() ||
	       !y2axis_->cuts_.empty()) gridcut_.register_me();
	}


	linewidth_.register_me();

	if(nodistort_used)
	{
	    container::caspect(-1);
	}
	if(global::debug>0) cout<<"[blop] [frame] prepare_for_draw finished"<<endl;
    }

    void frame::draw_frame(terminal *term)
    {
        if((draw_frame_&sym::left)==0 && (draw_frame_&sym::top)==0 && (draw_frame_&sym::right)==0 && (draw_frame_&sym::bottom)==0) return;

        if((draw_frame_&sym::left) && (draw_frame_&sym::top) && (draw_frame_&sym::right) && (draw_frame_&sym::bottom))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            term->draw_lines(c);
            return;
        }
        if((draw_frame_&sym::left) && (draw_frame_&sym::top) && (draw_frame_&sym::right))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            term->draw_lines(c);
            return;
        }
        if((draw_frame_&sym::left) && (draw_frame_&sym::top) && (draw_frame_&sym::bottom))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            term->draw_lines(c);
            return;
        }
        if((draw_frame_&sym::left) && (draw_frame_&sym::right) && (draw_frame_&sym::bottom))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            term->draw_lines(c);
            return;
        }
        if((draw_frame_&sym::top) && (draw_frame_&sym::right) && (draw_frame_&sym::bottom))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            term->draw_lines(c);
            return;
        }
        if((draw_frame_&sym::left) && (draw_frame_&sym::top))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            term->draw_lines(c);
            return;
        }
        if((draw_frame_&sym::left) && (draw_frame_&sym::right))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            {
                vector<terminal::coord> c;
                c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
                c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
                term->draw_lines(c);
            }
            {
                vector<terminal::coord> c;
                c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
                c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
                term->draw_lines(c);
            }
            return;
        }
        if((draw_frame_&sym::left) && (draw_frame_&sym::bottom))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            term->draw_lines(c);
            return;
        }
        if((draw_frame_&sym::top) && (draw_frame_&sym::right))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            term->draw_lines(c);
            return;
        }
        if((draw_frame_&sym::top) && (draw_frame_&sym::bottom))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            {
                vector<terminal::coord> c;
                c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
                c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
                term->draw_lines(c);
            }
            {
                vector<terminal::coord> c;
                c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
                c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
                term->draw_lines(c);
            }
            return;
        }
        if((draw_frame_&sym::right) && (draw_frame_&sym::bottom))
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            term->draw_lines(c);
            return;
        }
        if(draw_frame_&sym::left)
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            term->draw_lines(c);
        }
        if(draw_frame_&sym::top)
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(1,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            term->draw_lines(c);
        }
        if(draw_frame_&sym::right)
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(1,2)));
            term->draw_lines(c);
        }
        if(draw_frame_&sym::bottom)
        {
            term->set_linewidth(linewidth_.termspecific_id());
            term->set_color(framecolor_);
            term->set_linestyle(sym::solid);
            vector<terminal::coord> c;
            c.push_back(terminal::coord(terminal::id(0,1),terminal::id(0,2)));
            c.push_back(terminal::coord(terminal::id(1,1),terminal::id(0,2)));
            term->draw_lines(c);
        }
    }

    void frame::draw_axes(terminal *term)
    {
	term->set_linewidth(linewidth_.termspecific_id());
	term->set_color(framecolor_);
	term->set_linestyle(sym::solid);

	if(draw_axes_ & axis::x1)
	{
	    if(owns_x1axis_)
	    {
		x1axis_->print(term,
			       (x1axis_->pos_changed()||!draw_frame_)&&!x1axis_->tics_.empty());
		if(mirror_x1_ && !x1axis_->pos_changed() && draw_frame_)
		    x1axis_->print_tics(term,0.,!cwidth(),!cheight(),true);
	    }
	    else
	    {
		x1axis_->print_tics(term,0.,!cwidth(),0.);
		if(mirror_x1_) x1axis_->print_tics(term,0.,!cwidth(),!cheight(),true);
	    }	    
	}

	if(draw_axes_ & axis::y1)
	{
	    if(owns_y1axis_)
	    {
		y1axis_->print(term,
			       (y1axis_->pos_changed()||!draw_frame_)&&!y1axis_->tics_.empty());
		if(mirror_y1_ && !y1axis_->pos_changed() && draw_frame_)
		    y1axis_->print_tics(term,0.,!cheight(),!cwidth(),true);
	    }
	    else
	    {
		y1axis_->print_tics(term,0.,!cheight(),0.);
		if(mirror_y1_) y1axis_->print_tics(term,0.,!cheight(),!cwidth(),true);
	    }
	}

	if(draw_axes_ & axis::x2)
	{
	    if(owns_x2axis_)
	    {
		x2axis_->print(term,
			       (x2axis_->pos_changed()||!draw_frame_)&&!x2axis_->tics_.empty());
		if(mirror_x2_ && !x2axis_->pos_changed() && draw_frame_)
		    x2axis_->print_tics(term,0.,!cwidth(),0.,true);
	    }
	    else
	    {
		x2axis_->print_tics(term,0.,!cwidth(),!cheight());
		if(mirror_x2_) x2axis_->print_tics(term,0.,!cwidth(),0.,true);
	    }
	}

	if(draw_axes_ & axis::y2)
	{
	    if(owns_y2axis_)
	    {
		y2axis_->print(term,
			       (y2axis_->pos_changed()||!draw_frame_)&&!y2axis_->tics_.empty());
		if(mirror_y2_ && !y2axis_->pos_changed() && draw_frame_)
		    y2axis_->print_tics(term,0.,!cheight(),0.,true);
	    }
	    else
	    {
		y2axis_->print_tics(term,0.,!cheight(),!cwidth());
		if(mirror_y2_) y2axis_->print_tics(term,0.,!cheight(),0.,true);
	    }
	}
    }

    void frame::draw_grid(terminal *term)
    {
	term->open_layer("grid");

        /*
        if(pgrid_func_.initialized())
        {
            const double dp1 = (pgrid_1_to_-pgrid_1_from_)/pgrid_1_n_;
            const double dp2 = (pgrid_2_to_-pgrid_2_from_)/pgrid_2_n_;

            term->set_linewidth(gridwidth_.termspecific_id());
            term->set_linestyle(gridstyle_);
            term->set_color(gridcolor_);

            std::vector<blop::terminal::coord> c;

            std::vector<blop::var> args(2), result;

            for(double p1=pgrid_1_from_; p1<pgrid_1_to_+0.1*dp1; p1+=dp1)
            {
                c.clear();
                args[0] = p1;
                
                double p2 = pgrid_2_from_;
                args[1] = p2;
                pgrid_func_.meval(args, result);
                double ax_prev = x1axis_->map_point(result[0].dbl());
                double ay_prev = y1axis_->map_point(result[1].dbl());
                c.push_back(terminal::coord(terminal::id(ax_prev,1), terminal::id(ay_prev,2)));
                
                double p2step = dp2;
                while(true)
                {
                    double ax=0, ay=0;
                    if(p2+dp2>=pgrid_2_to_)
                    {
                        p2 = pgrid_2_to_;
                    }
                    else
                    {
                        args[1] = p2+dp2;
                    }
                    args[1] = p2;
                    pgrid_func_.meval(args, result);
                    if(result.size()<2) warning::print("Grid mapping function has too few components");
                    const double ax = x1axis_->map_point(result[0].dbl());
                    const double ay = y1axis_->map_point(result[1].dbl());
                    c.push_back(terminal::coord(terminal::id(ax,1), terminal::id(ay,2)));

                    
                    while(true)
                    {
                        args[1] = p2+p2step;
                        pgrid_func_.meval(args, result);
                        const double ax_next = x1axis_->map_point(result[0].dbl());
                        const double ay_next = y1axis_->map_point(result[1].dbl());
                        
                    }

                }
                term->draw_lines(c);
            }
            for(double p2=pgrid_2_from_; p2<pgrid_2_to_+0.1*dp2; p2+=dp2)
            {
                c.clear();
                args[1] = p2;
                for(double p1=pgrid_1_from_; p1<pgrid_1_to_+0.1*dp1; p1+=dp1)
                {
                    args[0] = p1;
                    pgrid_func_.meval(args, result);
                    if(result.size()<2) warning::print("Grid mapping function has too few components");
                    c.push_back(terminal::coord(terminal::id(x1axis_->map_point(result[0].dbl()),1),
                                                terminal::id(y1axis_->map_point(result[1].dbl()),2)));
                }
                term->draw_lines(c);
            }



        }
        else
        */
        {
            if(grid_ & axis::x1)
            {
                term->set_linewidth(gridwidth_.termspecific_id());
                term->set_linestyle(gridstyle_);
                term->set_color(gridcolor_);
                for(unsigned int i=0; i<x1axis_->tics_.size(); ++i)
                {
                    double v = x1axis_->map_point(x1axis_->tics_[i].value());
                    if(v == unset) continue;
                    if(y1axis_->cuts_.empty())
                    {
                        term->draw_line(terminal::coord(terminal::id(v,1),terminal::id(0.0,2)),
                                        terminal::coord(terminal::id(v,1),terminal::id(1.0,2)));
                    }
                    else
                    {
                        double start = 0.0;
                        for(unsigned int c=0; c<y1axis_->cuts_.size(); ++c)
                        {
                            int start_id = terminal::ZERO;
                            if(start != 0.0)
                            {
                                start_id = term->lincombi(1,terminal::id(start,2),
                                                          0.5,gridcut_.termspecific_id());
                            }
                            double end = y1axis_->map_point(y1axis_->cuts_[c].first);
                            int end_id = term->lincombi(1,terminal::id(end,2),
                                                        -0.5,gridcut_.termspecific_id());
                            term->draw_line(terminal::coord(terminal::id(v,1),start_id),
                                            terminal::coord(terminal::id(v,1), end_id));
                            start = y1axis_->map_point(y1axis_->cuts_[c].second);
                        }
                        int start_id = term->lincombi(1,terminal::id(start,2),
                                                      0.5,gridcut_.termspecific_id());
                        term->draw_line(terminal::coord(terminal::id(v,1),start_id),
                                        terminal::coord(terminal::id(v,1),terminal::id(1.0,2)));
		    
                    }
                }
                if(grid_at_minor_tics_&axis::x1)
                {
                    term->set_linewidth(minor_gridwidth_.termspecific_id());
                    term->set_linestyle(minor_gridstyle_);
                    term->set_color(minor_gridcolor_);
                    for(unsigned int i=0; i<x1axis_->minor_tics_.size(); ++i)
                    {
                        double v = x1axis_->map_point(x1axis_->minor_tics_[i].value());
                        if(v == unset) continue;
                        if(y1axis_->cuts_.empty())
                        {
                            term->draw_line(terminal::coord(terminal::id(v,1),terminal::id(0.0,2)),
                                            terminal::coord(terminal::id(v,1),terminal::id(1.0,2)));
                        }
                        else
                        {
                            double start = 0.0;
                            for(unsigned int c=0; c<y1axis_->cuts_.size(); ++c)
                            {
                                int start_id = terminal::ZERO;
                                if(start != 0.0)
                                {
                                    start_id = term->lincombi(1,terminal::id(start,2),
                                                              0.5,gridcut_.termspecific_id());
                                }
                                double end = y1axis_->map_point(y1axis_->cuts_[c].first);
                                int end_id = term->lincombi(1,terminal::id(end,2),
                                                            -0.5,gridcut_.termspecific_id());
                                term->draw_line(terminal::coord(terminal::id(v,1),start_id),
                                                terminal::coord(terminal::id(v,1), end_id));
                                start = y1axis_->map_point(y1axis_->cuts_[c].second);
                            }
                            int start_id = term->lincombi(1,terminal::id(start,2),
                                                          0.5,gridcut_.termspecific_id());
                            term->draw_line(terminal::coord(terminal::id(v,1),start_id),
                                            terminal::coord(terminal::id(v,1),terminal::id(1.0,2)));
		    
                        }
                    }
                }
            }
            if(grid_ & axis::y1)
            {
                term->set_linewidth(gridwidth_.termspecific_id());
                term->set_linestyle(gridstyle_);
                term->set_color(gridcolor_);
                for(unsigned int i=0; i<y1axis_->tics_.size(); ++i)
                {
                    double v = y1axis_->map_point(y1axis_->tics_[i].value());
                    if(v == unset) continue;
                    if(x1axis_->cuts_.empty())
                    {
                        term->draw_line(terminal::coord(terminal::id(0.0,1),terminal::id(v,2)),
                                        terminal::coord(terminal::id(1.0,1),terminal::id(v,2)));
                    }
                    else
                    {
                        double start = 0.0;
                        for(unsigned int c=0; c<x1axis_->cuts_.size(); ++c)
                        {
                            int start_id = terminal::ZERO;
                            if(start != 0.0)
                            {
                                start_id = term->lincombi(1,terminal::id(start,1),
                                                          0.5,gridcut_.termspecific_id());
                            }
                            double end = x1axis_->map_point(x1axis_->cuts_[c].first);
                            int end_id = term->lincombi(1,terminal::id(end,1),
                                                        -0.5,gridcut_.termspecific_id());
                            term->draw_line(terminal::coord(start_id,terminal::id(v,2)),
                                            terminal::coord(end_id,  terminal::id(v,2)));
                            start = x1axis_->map_point(x1axis_->cuts_[c].second);
                        }
                        int start_id = term->lincombi(1,terminal::id(start,1),
                                                      0.5,gridcut_.termspecific_id());
                        term->draw_line(terminal::coord(start_id,terminal::id(v,2)),
                                        terminal::coord(terminal::id(1.0,1),terminal::id(v,2)));
		    
                    }
                }
                if(grid_at_minor_tics_&axis::y1)
                {
                    term->set_linewidth(minor_gridwidth_.termspecific_id());
                    term->set_linestyle(minor_gridstyle_);
                    term->set_color(minor_gridcolor_);
                    for(unsigned int i=0; i<y1axis_->minor_tics_.size(); ++i)
                    {
                        double v = y1axis_->map_point(y1axis_->minor_tics_[i].value());
                        if(v == unset) continue;
                        if(x1axis_->cuts_.empty())
                        {
                            term->draw_line(terminal::coord(terminal::id(0.0,1),terminal::id(v,2)),
                                            terminal::coord(terminal::id(1.0,1),terminal::id(v,2)));
                        }
                        else
                        {
                            double start = 0.0;
                            for(unsigned int c=0; c<x1axis_->cuts_.size(); ++c)
                            {
                                int start_id = terminal::ZERO;
                                if(start != 0.0)
                                {
                                    start_id = term->lincombi(1,terminal::id(start,1),
                                                              0.5,gridcut_.termspecific_id());
                                }
                                double end = x1axis_->map_point(x1axis_->cuts_[c].first);
                                int end_id = term->lincombi(1,terminal::id(end,1),
                                                            -0.5,gridcut_.termspecific_id());
                                term->draw_line(terminal::coord(start_id,terminal::id(v,2)),
                                                terminal::coord(end_id,  terminal::id(v,2)));
                                start = x1axis_->map_point(x1axis_->cuts_[c].second);
                            }
                            int start_id = term->lincombi(1,terminal::id(start,1),
                                                          0.5,gridcut_.termspecific_id());
                            term->draw_line(terminal::coord(start_id,terminal::id(v,2)),
                                            terminal::coord(terminal::id(1.0,1),terminal::id(v,2)));
		    
                        }
                    }
                }
            }
            if(grid_ & axis::x2)
            {
                term->set_linewidth(gridwidth_.termspecific_id());
                term->set_linestyle(gridstyle_);
                term->set_color(gridcolor_);
                for(unsigned int i=0; i<x2axis_->tics_.size(); ++i)
                {
                    double v = x2axis_->map_point(x2axis_->tics_[i].value());
                    if(v == unset) continue;
                    term->draw_line(terminal::coord(terminal::id(v,1),terminal::id(0.0,2)),
                                    terminal::coord(terminal::id(v,1),terminal::id(1.0,2)));
                }
                if(grid_at_minor_tics_&axis::x2)
                {
                    term->set_linewidth(minor_gridwidth_.termspecific_id());
                    term->set_linestyle(minor_gridstyle_);
                    term->set_color(minor_gridcolor_);
                    for(unsigned int i=0; i<x2axis_->minor_tics_.size(); ++i)
                    {
                        double v = x2axis_->map_point(x2axis_->minor_tics_[i].value());
                        if(v == unset) continue;
                        term->draw_line(terminal::coord(terminal::id(v,1),terminal::id(0.0,2)),
                                        terminal::coord(terminal::id(v,1),terminal::id(1.0,2)));
                    }
                }
            }
            if(grid_ & axis::y2)
            {
                term->set_linewidth(gridwidth_.termspecific_id());
                term->set_linestyle(gridstyle_);
                term->set_color(gridcolor_);
                for(unsigned int i=0; i<y2axis_->tics_.size(); ++i)
                {
                    double v = y2axis_->map_point(y2axis_->tics_[i].value());
                    if(v == unset) continue;
                    term->draw_line(terminal::coord(terminal::id(0.0,1),terminal::id(v,2)),
                                    terminal::coord(terminal::id(1.0,1),terminal::id(v,2)));
                }
                if(grid_at_minor_tics_&axis::y2)
                {
                    term->set_linewidth(minor_gridwidth_.termspecific_id());
                    term->set_linestyle(minor_gridstyle_);
                    term->set_color(minor_gridcolor_);
                    for(unsigned int i=0; i<y2axis_->minor_tics_.size(); ++i)
                    {
                        double v = y2axis_->map_point(y2axis_->minor_tics_[i].value());
                        if(v == unset) continue;
                        term->draw_line(terminal::coord(terminal::id(0.0,1),terminal::id(v,2)),
                                        terminal::coord(terminal::id(1.0,1),terminal::id(v,2)));
                    }
                }
            }
        }
        term->set_linestyle(sym::solid);
        term->close_layer("grid");
    }

    class compare_graphs_by_level
    {
    public:
        bool operator()(plottable *g1, plottable *g2) { return g1->level()<g2->level(); }
    };

    void frame::print_graph_(plottable *p, terminal *term)
    {
        if(global::debug) cerr<<"[blop] [frame] print_graph_ starts"<<endl;

        graph_drawer *d = p->drawstyle();
        if(d == 0)
        {
            warning::print("No drawer specified for graph","frame::print_graph_(plottable *,terminal *)");
        }
        else
        {
            if(p->columns() < d->req_components())
            {
                warning::print(var("Graph has too few columns (") & p->columns() & ")",
                               "frame::print_graph_(plottable *,terminal *)");
            }
            else
            {
                term->open_layer(p->layer());
                d->draw(p,this,term);
                term->close_layer(p->layer());
            }
        }
        if(global::debug) cerr<<"[blop] [frame] print_graph_ starts"<<endl;
    }

    void frame::print(terminal *term)
        TRY
    {
        if(global::debug>0) cout<<"[blop] [frame] print starts..."<<endl;

        if(print_me_ < 2)
        {
            if(global::debug>0)
            {
                cout<<"[blop] [frame]   this frame is switched off."<<endl;
                cout<<"[blop] [frame] print finished."<<endl;
            }
	    
            return;
        }

        term->open_layer(layer_);

        term->subpicture_begin(terminal::coord(cleft().termspecific_id(),
                                               cbottom().termspecific_id()),
                               terminal::coord(cright().termspecific_id(),
                                               ctop().termspecific_id()));

        if(!grid_foreground_) draw_grid(term);
        term->set_color(black);
        if(!foreground_)
        {
            if(draw_frame_) draw_frame(term);
            if(draw_axes_ ) draw_axes(term);
        }

        term->set_color(black);

        std::vector<blop::plottable *> graphs(graphs_);
        std::sort(graphs.begin(), graphs.end(), compare_graphs_by_level());

        // first print the graphs with level<0
        unsigned int graph_index = 0;
        for(; graph_index<graphs.size() && graphs[graph_index]->level()<0; ++graph_index)
        {
            print_graph_(graphs[graph_index], term);
        }

        // then print all the contents and graphs with level==0, in the order
        // of adding them to the frame (but print the legendbox at the end!)
        for(unsigned int i=0; i<ordering_.size(); ++i)
        {
            if(ordering_[i].plottable_ && ordering_[i].plottable_->level()==0)
            {
                print_graph_(ordering_[i].plottable_, term);
            }
            if(ordering_[i].grob_ && ordering_[i].grob_ != legendbox_)
            {
                // Those which should be printed above the grid are postponed, if the grid is in the foreground, i.e. printed later
                if(grid_foreground_ && ordering_[i].grob_->above_grid()) continue;
                // print
                ordering_[i].grob_->print(term);
            }
        }

        // and finally print all the graphs with level>0
        for(; graph_index<graphs.size(); ++graph_index)
        {
            if(graphs[graph_index]->level()>0) print_graph_(graphs[graph_index],term);
        }

        if(grid_foreground_)
        {
            draw_grid(term);
            for(unsigned int i=0; i<ordering_.size(); ++i)
            {
                if(ordering_[i].grob_ && ordering_[i].grob_ != legendbox_ && ordering_[i].grob_->above_grid()) ordering_[i].grob_->print(term);
            }
        }
        if(foreground_)
        {
            if(draw_frame_) draw_frame(term);
            if(draw_axes_ ) draw_axes(term);
        }

        // at the very end, print the legendbox
        if(show_legend_ && owns_legendbox_) legendbox_->print(term);


        term->subpicture_end();

        term->close_layer(layer_);

        if(global::debug>0) cout<<"[blop] [frame] print finished."<<endl;
    }
    CATCH("frame::print(terminal *)")

    frame &frame::grid(const function &f, double p1from, double p1to, int n1, double p2from, double p2to, int n2)
    {
        pgrid_func_ = f;
        pgrid_1_from_ = p1from;
        pgrid_1_to_   = p1to;
        pgrid_1_n_    = n1;
        pgrid_2_from_ = p2from;
        pgrid_2_to_   = p2to;
        pgrid_2_n_    = n2;
        return *this;
    }

    frame& frame::grid(int g)
    {
        pgrid_func_ = unset;
        if(g != grid_) modified_ = true;
        grid_ = g;
        return *this;
    }
    frame &frame::grid(bool i)
    {
        pgrid_func_ = unset;
        if(i)
        {
            int new_grid = grid_ | (axis::x1|axis::y1);
            if(new_grid != grid_) modified_ = true;
            grid_ = new_grid;
        }
        else
        {
            if(grid_ != 0) modified_ = true;
            grid_ = 0;
        }
        return *this;
    }

    frame &frame::grid_at_minor_tics(int g)
    {
        if(g != grid_at_minor_tics_) modified_ = true;
        grid_at_minor_tics_ = g;
        return *this;
    }

    frame& frame::gridcolor(const color &c)
    {
        modified_ = true;
        gridcolor_ = c;
        return *this;
    }
    frame &frame::minor_gridcolor(const color &c)
    {
        modified_ = true;
        minor_gridcolor_ = c;
        return *this;
    }


    frame& frame::framecolor(const color &c)
    {
        modified_ = true;
        framecolor_ = c;
        return *this;
    }
    const color &frame::framecolor() const {return framecolor_; }

    frame& frame::gridstyle(sym::linestyle s)
    {
        modified_ = true;
        gridstyle_ = s;
        return *this;
    }
    frame& frame::minor_gridstyle(sym::linestyle s)
    {
        modified_ = true;
        minor_gridstyle_ = s;
        return *this;
    }


    frame &frame::linewidth(const length &l)
    {
        modified_ = true;
        linewidth_ = l;
        return *this;
    }
    const length &frame::linewidth() const { return linewidth_; }

    frame &frame::ticlength(const length &l)
    {
        if(x1axis_) x1axis_->ticlength(l);
        if(y1axis_) y1axis_->ticlength(l);
        if(x2axis_) x2axis_->ticlength(l);
        if(y2axis_) y2axis_->ticlength(l);
        return *this;
    }
    frame &frame::minor_ticlength(const length &l)
    {
        if(x1axis_) x1axis_->minor_ticlength(l);
        if(y1axis_) y1axis_->minor_ticlength(l);
        if(x2axis_) x2axis_->minor_ticlength(l);
        if(y2axis_) y2axis_->minor_ticlength(l);
        return *this;
    }

    frame &frame::draw_tics(bool f)
    {
        if(x1axis_) x1axis_->draw_tics(f);
        if(y1axis_) y1axis_->draw_tics(f);
        if(x2axis_) x2axis_->draw_tics(f);
        if(y2axis_) y2axis_->draw_tics(f);
        return *this;
    }

    

    frame& frame::gridwidth(const length &l)
    {
        modified_ = true;
        gridwidth_ = l;
        return *this;
    }

    frame &frame::minor_gridwidth(const length  &l)
    {
        modified_ = true;
        minor_gridwidth_ = l;
        return *this;
    }

    frame& frame::gridcut(const length &l)
    {
        modified_ = true;
        gridcut_ = l;
        return *this;
    }
    const length &frame::gridcut() const {return gridcut_;}

    frame &frame::foreground(bool b)
    {
        modified_ = true;
        foreground_ = b;
        return *this;
    }

    bool frame::foreground() const { return foreground_; }

    dgraph *frame::lastd()
    {
        for(unsigned int i=0; i<graphs_.size(); ++i)
        {
            if(dgraph *p = dynamic_cast<dgraph *>(graphs_[i])) return p;
        }
        return 0;
    }
    fgraph *frame::lastf()
    {
        for(unsigned int i=0; i<graphs_.size(); ++i)
        {
            if(fgraph *p = dynamic_cast<fgraph *>(graphs_[i])) return p;
        }
        return 0;
    }

    plottable *frame::last()
    {
        if(graphs_.empty()) return 0;
        return graphs_.back();
    }

    plottable *frame::get_graph(int n)
    {
        if(n<0 || (int)graphs_.size() <= n)
        {
            warning::print(var("graph index [") & n & "] is out of range",
                           "frame::get_graph(int)");
            return 0;
        }
        return graphs_[n];
    }

    bool frame::modified() const
    {
        if(container::modified()) return true;
        for(unsigned int i=0; i<graphs_.size(); ++i)
        {
            if(graphs_[i]->modified()) return true;
        }
        if(x1axis_->modified()) return true;
        if(y1axis_->modified()) return true;
        if(x2axis_->modified()) return true;
        if(y2axis_->modified()) return true;
        return false;
    }

    void frame::modified(bool f)
    {
        container::modified(f);
        for(unsigned int i=0; i<graphs_.size(); ++i)
        {
            graphs_[i]->modified(f);
        }
        x1axis_->modified(f);
        y1axis_->modified(f);
        x2axis_->modified(f);
        y2axis_->modified(f);
    }

}
