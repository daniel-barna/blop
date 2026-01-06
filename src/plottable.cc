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
	autodel_ = o.autodel_;
	ordered_ = o.ordered_;

	parent_ = 0;

	xmin_ = o.xmin_;
	xmax_ = o.xmax_;
	ymin_ = o.ymin_;
	ymax_ = o.ymax_;

	pointsize_ = o.pointsize_;

	if(point_drawer_) delete point_drawer_;
	point_drawer_ = 0;
	if(o.point_drawer_) point_drawer_ = o.point_drawer_->clone();

	if(graph_drawer_) delete graph_drawer_;
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
	delete point_drawer_;
	delete graph_drawer_;
	frame::remove_from_all(this);
	legendbox::remove_from_all(this);
    }

    plottable::plottable()
	: level_(0), permanent_(false), autodel_(false), ordered_(0), 
	  parent_(0),
	  xmin_(unset), xmax_(unset), ymin_(unset), ymax_(unset),
	  point_drawer_(0), pointcolor_(black), linecolor_(black), linestyle_(sym::solid),
	  fillcolor_(black), fill_(false), legendcolor_(black), 
	  xaxis_(axis::x1), yaxis_(axis::y1),
	  modified_(true), layer_("default")
    {
	graph_drawer_ = new lines;
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
	delete point_drawer_;
	point_drawer_ = d.clone();
	return *this;
    }

    plottable &plottable::drawstyle(const graph_drawer &d)
    {
	modified_ = true;
	delete graph_drawer_ ;
	graph_drawer_ = d.clone();
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
	if(point_drawer_) delete point_drawer_;
	point_drawer_ = 0;
	if(o.point_drawer_) point_drawer_ = o.point_drawer_->clone();
	pointcolor_ = o.pointcolor_;
	linecolor_ = o.linecolor_;
	linewidth_ = o.linewidth_;
	linestyle_ = o.linestyle_;
	if(graph_drawer_) delete graph_drawer_;
	graph_drawer_ = o.graph_drawer_->clone();
	xaxis_ = o.xaxis_;
	yaxis_ = o.yaxis_;
	legend_ = o.legend_;

	fillcolor_ = o.fillcolor_;
	fill_      = o.fill_;
        legendcolor_  = o.legendcolor_;
    }

    // ---------------------------------------

    plottables::plottables(const std::initializer_list<plottable*> &l)
    {
        for(auto &e : l) add(e);
    }


    plottables &plottables::add(plottable *p, const std::map<std::string,blop::var> &np)
    {
	plottables_.push_back(p);
        named_params_.push_back(np);
	return *this;
    }
    plottables &plottables::clear()
    {
	plottables_.clear();
        named_params_.clear();
	return *this;
    }
    unsigned int plottables::size() const
    {
	return plottables_.size();
    }
    plottable *plottables::operator[](unsigned int i)
    {
	return plottables_[i];
    }

    plottables &plottables::fillcolor(const color &c)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->fillcolor(c);
	return *this;
    }
    plottables &plottables::fillcolor(const std::vector<blop::color> &c)
    {
        const unsigned int np = plottables_.size(); 
        const unsigned int nc = c.size();
	for(unsigned int i=0; i<np; ++i) plottables_[i]->fillcolor(c[i%nc]);
	return *this;
    }
    plottables &plottables::linecolor(const color &c)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->linecolor(c);
	return *this;
    }
    plottables &plottables::linecolor(const std::vector<blop::color> &c)
    {
        const unsigned int np = plottables_.size(); 
        const unsigned int nc = c.size();
	for(unsigned int i=0; i<np; ++i) plottables_[i]->linecolor(c[i%nc]);
	return *this;
    }
    plottables &plottables::pointcolor(const color &c)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->pointcolor(c);
	return *this;
    }
    plottables &plottables::pointcolor(const std::vector<blop::color> &c)
    {
        const  unsigned int np = plottables_.size(); 
        const  unsigned int nc = c.size();
	for(unsigned int i=0; i<np; ++i) plottables_[i]->pointcolor(c[i%nc]);
	return *this;
    }
    plottables &plottables::allcolor(const color &c)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->allcolor(c);
	return *this;
    }
    plottables &plottables::allcolor(const std::vector<blop::color> &c)
    {
        const unsigned int np = plottables_.size(); 
        const unsigned int nc = c.size();
	for(unsigned int i=0; i<np; ++i) plottables_[i]->allcolor(c[i%nc]);
	return *this;
    }
    
    plottables &plottables::drawstyle(const graph_drawer &d)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->drawstyle(d);
	return *this;
    }
    plottables &plottables::drawstyle(const std::vector<blop::graph_drawers> &d)
    {
        const unsigned int np = plottables_.size(); 
        const unsigned int nd = d.size();
	for(unsigned int i=0; i<np; ++i) std::visit([&](auto&& arg){plottables_[i]->drawstyle(arg);},d[i%nd]);
	return *this;
    }
    plottables &plottables::pointtype(const point_drawer &d)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->pointtype(d);
	return *this;
    }
    plottables &plottables::pointtype(const std::vector<blop::point_drawers> &d)
    {
        const unsigned int np = plottables_.size(); 
        const unsigned int nd = d.size();
	for(unsigned int i=0; i<np; ++i) std::visit([&](auto&& arg){plottables_[i]->pointtype(arg);},d[i%nd]);
	return *this;
    }
    plottables &plottables::pointsize(const length &s)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->pointsize(s);
	return *this;
    }
    plottables &plottables::pointsize(const std::vector<blop::length> &d)
    {
        const unsigned int np = plottables_.size(); 
        const unsigned int nd = d.size();
	for(unsigned int i=0; i<np; ++i) plottables_[i]->pointsize(d[i%nd]);
	return *this;
    }
    plottables &plottables::linestyle(sym::linestyle s)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->linestyle(s);
	return *this;
    }
    plottables &plottables::linestyle(const std::vector<blop::sym::linestyle> &d)
    {
        const unsigned int np = plottables_.size(); 
        const unsigned int nd = d.size();
	for(unsigned int i=0; i<np; ++i) plottables_[i]->linestyle(d[i%nd]);
	return *this;
    }

    plottables &plottables::linewidth(const length &w)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->linewidth(w);
	return *this;
    }
    plottables &plottables::linewidth(const std::vector<blop::length> &d)
    {
        const unsigned int np = plottables_.size(); 
        const unsigned int nd = d.size();
	for(unsigned int i=0; i<np; ++i) plottables_[i]->linewidth(d[i%nd]);
	return *this;
    }
    
    plottables &plottables::legend(const var &leg)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i)
        {
            plottables_[i]->legend(replace("%l",plottables_[i]->legend(),leg));
        }
	return *this;
    }
    plottables &plottables::legend(const std::vector<blop::var> &d)
    {
        const unsigned int np = plottables_.size(); 
        const unsigned int nd = d.size();
	for(unsigned int i=0; i<np; ++i) plottables_[i]->legend(d[i%nd]);
	return *this;
    }
    plottables &plottables::legend(blop::function f)
    {
        for(unsigned int i=0; i<plottables_.size(); ++i)
        {
            function::named_params(named_params_[i]);
            plottables_[i]->legend(f(i));
        }
        function::clear_named_params();
        return *this;
    }
    plottables &plottables::legend(std::function<blop::var(int)> f)
    {
        for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->legend(f(i));
        return *this;
    }

    plottables &plottables::autodel(bool b)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->autodel(b);
	return *this;
    }

}
