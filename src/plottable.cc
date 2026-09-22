#include "plottable.h"
#include "axis.h"
#include "frame.h"
#include "legendbox.h"

namespace blop
{
    void plottable::copy(const plottable &o)
    {
	level_ = o.level_;
	permanent_ = o.permanent_;
//	autodel_ = o.autodel_;
	ordered_ = o.ordered_;

	parent_ = 0;

	xmin_ = o.xmin_;
	xmax_ = o.xmax_;
	ymin_ = o.ymin_;
	ymax_ = o.ymax_;

	pointsize_ = o.pointsize_;

	point_drawer_ = 0;
	if(o.point_drawer_) point_drawer_ = o.point_drawer_->clone();

//	if(graph_drawer_) delete graph_drawer_;
	graph_drawer_ = 0;
	if(o.graph_drawer_) graph_drawer_ = o.graph_drawer_->clone();

	pointcolor_ = o.pointcolor_;
	linecolor_ = o.linecolor_;
	linewidth_ = o.linewidth_;
	linestyle_ = o.linestyle_;
	fillcolor_ = o.fillcolor_;
	fill_      = o.fill_;
	legendcolor_ = o.legendcolor_;
	xaxis_ = o.xaxis_;
	yaxis_ = o.yaxis_;
	legend_ = o.legend_;

	layer_ = o.layer_;
    }

    plottable::~plottable()
    {
	frame::remove_from_all(this);
	legendbox::remove_from_all(this);
    }

    plottable::plottable()
	: level_(0), permanent_(false), 
          ordered_(0), 
	  parent_(0),
	  xmin_(unset), xmax_(unset), ymin_(unset), ymax_(unset),
	  point_drawer_(0), pointcolor_(black), linecolor_(black), linestyle_(sym::solid),
	  fillcolor_(black), fill_(false), legendcolor_(black), 
	  xaxis_(axis::x1), yaxis_(axis::y1),
	  modified_(true), layer_("default")
    {
        graph_drawer_ = lines::create();
	pointsize_ = PS;
	linewidth_ = LW;
    }

    plottable::plottable(const plottable &o)
    {
	modified_ = true;
	graph_drawer_ = 0;
	point_drawer_ = 0;
	copy(o);
    }

    const plottable &plottable::operator=(const plottable &o)
    {
	copy(o);
	return o;
    }

    plottable &plottable::pointtype(const point_drawer &d)
    {
	modified_ = true;
//	delete point_drawer_;
	point_drawer_ = d.clone();
	return *this;
    }

    plottable &plottable::pointtype(smartptr<point_drawer> d)
    {
        modified_ = true;
        point_drawer_ = d->clone();
        return *this;
    }

    plottable &plottable::drawstyle(const graph_drawer &d)
    {
	modified_ = true;
//	delete graph_drawer_ ;
	graph_drawer_ = d.clone();
	graph_drawer_->setup_when_added(this, parent_);
	return *this;
    }

    plottable &plottable::drawstyle(smartptr<graph_drawer> d)
    {
	modified_ = true;
        graph_drawer_ = d->clone();
	graph_drawer_->setup_when_added(this, parent_);
	return *this;
    }

    void plottable::copy_style(const plottable &o)
    {
	xmin_ = o.xmin_;
	xmax_ = o.xmax_;
	ymin_ = o.ymin_;
	ymax_ = o.ymax_;

	pointsize_ = o.pointsize_;
	point_drawer_ = nullptr;
	if(o.point_drawer_) point_drawer_ = o.point_drawer_->clone();
	pointcolor_ = o.pointcolor_;
	linecolor_ = o.linecolor_;
	linewidth_ = o.linewidth_;
	linestyle_ = o.linestyle_;
	graph_drawer_ = o.graph_drawer_->clone();
	xaxis_ = o.xaxis_;
	yaxis_ = o.yaxis_;
	legend_ = o.legend_;

	fillcolor_ = o.fillcolor_;
	fill_      = o.fill_;
        legendcolor_  = o.legendcolor_;
    }

    // ---------------------------------------

}
