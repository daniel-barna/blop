#ifndef __BLOP_PLOTTABLES_H__
#define __BLOP_PLOTTABLES_H__

#include "graph_drawer.h"
#include "plottable.h"

namespace blop
{
    class plottables
    {
    private:
	std::vector<smartptr<plottable>> plottables_;
        std::vector<std::map<std::string,blop::var>> named_params_;
    public:
#ifndef __MAKECINT__
        plottables(const std::initializer_list<smartptr<plottable>> &p);
#endif
        plottables() {}
	plottables &add(smartptr<plottable> p, const std::map<std::string,blop::var> &np = std::map<std::string,blop::var>());
	plottables &clear();
	unsigned int size() const;
	smartptr<plottable> operator[](unsigned int);
        std::vector<smartptr<plottable>>::iterator begin() { return plottables_.begin(); }
        std::vector<smartptr<plottable>>::iterator end() { return plottables_.end(); }

	plottables &fillcolor(const color &c);
	plottables &fc(const color &c) { fillcolor(c); return *this; }
        plottables &fillcolor(const std::vector<blop::color> &c); 
        plottables &fc(const std::vector<blop::color> &c) { return fillcolor(c); }
	    
	plottables &linecolor(const color &c);
	plottables &lc(const color &c) { linecolor(c); return *this; }
        plottables &linecolor(const std::vector<blop::color> &c);
        plottables &lc(const std::vector<blop::color> &c) { return linecolor(c); }

	plottables &pointcolor(const color &c);
	plottables &pc(const color &c) { pointcolor(c); return *this; }
        plottables &pointcolor(const std::vector<blop::color> &c);
        plottables &pc(const std::vector<blop::color> &c) { return pointcolor(c); }

	plottables &allcolor(const color &c);
	plottables &ac(const color &c) { allcolor(c); return *this; }
        plottables &allcolor(const std::vector<blop::color> &c);
        plottables ac(const std::vector<blop::color> &c) { return allcolor(c); }

	plottables &drawstyle(const graph_drawer &d);
	plottables &ds(const graph_drawer &d) { drawstyle(d); return *this; }
#ifndef __MAKECINT__
        plottables &drawstyle(const std::vector<blop::graph_drawers> &d);
        plottables &ds(const std::vector<blop::graph_drawers> &d) { return drawstyle(d); }
#endif

	plottables &pointtype(const point_drawer &d);
	plottables &pt(const point_drawer &d) { pointtype(d); return *this; }
#ifndef __MAKECINT__
        plottables &pointtype(const std::vector<blop::point_drawers> &d);
        plottables &pt(const std::vector<blop::point_drawers> &d) { return pointtype(d); }
#endif        

	plottables &pointsize(const length &d);
	plottables &ps(const length &d) { pointsize(d); return *this; }
        plottables &pointsize(const std::vector<blop::length> &d);
        plottables &ps(const std::vector<blop::length> &d) { return pointsize(d); }

	plottables &linestyle(sym::linestyle s);
	plottables &ls(sym::linestyle s) { linestyle(s); return *this; }
        plottables &linestyle(const std::vector<blop::sym::linestyle> &s);
        plottables &ls(const std::vector<blop::sym::linestyle> &s) { return linestyle(s); }

	plottables &linewidth(const length &w);
	plottables &lw(const length &w) { linewidth(w); return *this; }
        plottables &linewidth(const std::vector<blop::length> &w);
        plottables &lw(const std::vector<blop::length> &w) { return linewidth(w); }

        // Set the legend of all plottables within the collection. The string %l is replaced by
        // the original legend of the individual plottables
        plottables &legend(const var &leg);
        plottables &legend(const char *leg) { return legend(var(leg)); }
        plottables &legend(const string &leg) { return legend(var(leg)); }
//        plottables &autodel(bool b);
        plottables &legend(const std::vector<blop::var> &l);
        plottables &legend(blop::function f);
#ifndef __MAKECINT__
        plottables &legend(std::function<blop::var(int)> f);
#endif        
    };
    
}

#endif
