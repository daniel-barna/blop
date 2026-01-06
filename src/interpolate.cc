#include "interpolate.hh"
#include "geometry.h"

#ifdef HAVE_GTS_H
#include <gts.h>
#endif

namespace blop
{

#ifdef HAVE_GTS_H
    delaunay_interpolator::delaunay_interpolator(const delaunay_surface &s)
        : surface_(s), initialized_(false), nx_(0), ny_(0), xmin_(0), xmax_(0), ymin_(0), ymax_(0)
    {
    }

    void delaunay_interpolator::clear()
    {
        interpolator<y_type>::points_modified_ = true;
        surface_.clear();
        initialized_ = false;
        triangles_.clear();
        nx_ = ny_ = 0;
    }

    delaunay_interpolator &delaunay_interpolator::add_point(double x, double y, double z)
    {
        interpolator<y_type>::points_modified_ = true;
	surface_.add_vertex(x,y,z);
	initialized_ = false;
	for(unsigned int i=0; i<triangles_.size(); ++i) triangles_[i].clear();
	triangles_.clear();
	nx_ = ny_ = 0;
	dx_ = dy_ = 0;
	xmin_ = xmax_ = ymin_ = ymax_ = 0;
	return *this;
    }
    
    double delaunay_interpolator::operator()(double x, double y) const
    {
	if(x<xmin_ || xmax_<x || y<ymin_ || ymax_<y) return out_of_range_value();

	const int ix = (int)((x-xmin_)/dx_);
	const int iy = (int)((y-ymin_)/dy_);
	const int ind = index_(ix,iy);

	for(unsigned int i=0; i<triangles_[ind].size(); ++i)
	{
	    GtsVertex *v1, *v2, *v3;
	    vertices(triangles_[ind][i], &v1, &v2, &v3);
	    geom::triangle tt(geom::vec3(v1->p.x, v1->p.y, 0),
			      geom::vec3(v2->p.x, v2->p.y, 0),
			      geom::vec3(v3->p.x, v3->p.y, 0));

	    if(tt.inside(geom::vec3(x,y,0)))
	    {
		tt.vertex(1).coord.z = v1->p.z;
		tt.vertex(2).coord.z = v2->p.z;
		tt.vertex(3).coord.z = v3->p.z;
		const geom::plane p(tt);
		const geom::line  l(geom::point(x,y,0),geom::vec3(0,0,1));
		const geom::point solution(p,l);
		return solution.coord.z;
	    }
	}
	//warning::print("This should never happen","delaunay_interpolator::operator()(x,y)");
	return out_of_range_value();
    }

    double delaunay_interpolator::interpolate(const std::vector<blop::var> &args) const
    {
	if(!initialized_) initialize_();
	if(args.size()<2)
	{
	    warning::print("delaunay_interpolator expects 2 arguments","delaunay_interpolator::interpolate(...)");
	    return 0;
	}
	const double x = args[0].dbl();
	const double y = args[1].dbl();
	return operator()(x,y);
    }

    void delaunay_interpolator::initialize_() const
    {
	for(int i=0; i<surface_.nfaces(); ++i)
	{
	    const GtsTriangle *t = &(surface_.face(i)->triangle);
	    double x1,x2,y1,y2;
	    bbox(t,&x1,&x2,&y1,&y2);
	    if(i==1 || x1<=xmin_) xmin_=x1;
	    if(i==1 || x2>=xmax_) xmax_=x2;
	    if(i==1 || y1<=ymin_) ymin_=y1;
	    if(i==1 || y2>=ymax_) ymax_=y2;
	}

	nx_ = ny_ = std::max(std::min(80,surface_.nfaces()),3);
	triangles_.resize(nx_*ny_);

	dx_ = (xmax_-xmin_)/nx_;
	dy_ = (ymax_-ymin_)/ny_;
	
	for(int i=0; i<surface_.nfaces(); ++i)
	{
	    const GtsTriangle *t = &(surface_.face(i)->triangle);
	    double tx1,tx2,ty1,ty2; // bounding box of the triangle
	    bbox(t,&tx1,&tx2,&ty1,&ty2);
	    for(int ix=0; ix<nx_; ++ix)
	    {
		// the limits of this cell. make it a bit larger to avoid
		// problems arising from floating-point unprecision
		const double x1 = xmin_ + (ix-0.01)*dx_;
		const double x2 = xmin_ + (ix+1.01)*dx_;

		for(int iy=0; iy<ny_; ++iy)
		{
		    const double y1 = ymin_ + (iy-0.01)*dy_;
		    const double y2 = ymin_ + (iy+1.01)*dy_;

		    if(tx1<=x2 && tx2>=x1 && ty1<=y2 && ty2>=y1)
		    {
			triangles_[index_(ix,iy)].push_back(t);
		    }
		}
	    }
	}

	initialized_ = true;
    }

#endif

}
