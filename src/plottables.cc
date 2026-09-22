#include "plottables.h"

namespace blop
{
    plottables::plottables(const std::initializer_list<smartptr<plottable>> &l)
    {
        for(auto &e : l) add(e);
    }


    plottables &plottables::add(smartptr<plottable> p, const std::map<std::string,blop::var> &np)
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
    smartptr<plottable> plottables::operator[](unsigned int i)
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

    /*
    plottables &plottables::autodel(bool b)
    {
	for(unsigned int i=0; i<plottables_.size(); ++i) plottables_[i]->autodel(b);
	return *this;
    }
    */

    
}
