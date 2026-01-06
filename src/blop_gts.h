#ifndef __BLOP_BLOP_GTS_H__
#define __BLOP_BLOP_GTS_H__
#include <vector>
#include "config.h"
#include "function.h"
#include "array.h"
#include "dgraph.h"
#ifdef HAVE_GTS_H

#ifndef __MAKECINT__
#  include <gts.h>
#else
class GtsFace;
class GtsVertex;
class GtsSurface;
class GtsEdge;
class GtsTriangle;
class GNode;
#endif

/*
  This file provides an interface to GTS functionality
 */


namespace blop
{
    
    double xmax(const GtsTriangle *);
    double xmin(const GtsTriangle *);
    double ymax(const GtsTriangle *);
    double ymin(const GtsTriangle *);
    void   bbox(const GtsTriangle *, double *xmin, double *xmax, double *ymin, double *ymax, double *zmin=0, double *zmax=0);
    void   vertices(const GtsTriangle *, GtsVertex **v1, GtsVertex **v2, GtsVertex **v3);

    class gts_surface
    {
    protected:

	// helper arrays. upon initialization of a surface, these should be initialized
	// using the initialize_faces_(), initialize_vertices_(), initialize_edges_()
	// member functions (which 
	mutable std::vector<GtsFace*>   faces_;
	mutable std::vector<GtsVertex*> vertices_;
	mutable std::vector<GtsEdge*>   edges_;
	mutable double enclosing_triangle_x_[3], enclosing_triangle_y_[3];
	mutable GNode *bounding_boxes_;

	void clear_vertices_() const;
	void clear_edges_   () const;
	void clear_faces_   () const;
	void clear_enclosing_triangle_() const;
	void clear_bounding_boxes_() const;

	// Any derived class, when modified, should set this flag to false (i.e. when adding
	// for example a vertex). Any derived class should also clear/reset the faces_, vertices_,
	// edges_ etc helper arrays and the enclosing_triangle_x/y_ arrays
	mutable bool initialized_;

	mutable GtsSurface *surface_;


    public:
        gts_surface();
	virtual ~gts_surface();

	virtual gts_surface &clear();

	virtual void initialize() const = 0;

	virtual GtsSurface *surface() const;

	virtual int         nvertices() const;
	virtual const GtsVertex  *vertex(unsigned int i) const;

	virtual int         nfaces() const;
	virtual const GtsFace    *face(unsigned int i) const;

	virtual int         nedges() const;
	virtual const GtsEdge    *edge(unsigned int i) const;

	virtual void   enclosing_triangle(double x[3], double y[3], double scaling=1) const;
	virtual GNode *bounding_boxes() const;
    };

    //html <a name="delaunay">
    // A Delaunay-triangulated surface. z-values at discrete (x,y) values must be provided
    // via the add_vertex(x,y,z) function. The obtained delaunay-triangulation in the (x,y)
    // plane can be plotted via the plot_triangles function
    //html </a>
    class delaunay_surface : public gts_surface
    {
    protected:
	void initialize() const;

    public:
	delaunay_surface();
	delaunay_surface(const delaunay_surface &rhs);
	~delaunay_surface();

	// add a vertex
	virtual delaunay_surface &add_vertex(double x, double y, double z);

	// plot the triangles in the (x,y) plane
	dgraph &plot_triangles();
	dgraph &mplot_triangles();

	void    get_triangle_grid(std::vector<double> &x, std::vector<double> &y);

        double z_at(double x, double y) const;
    };

    //html <a name='intersection_z'>
    void intersection_z(gts_surface &s, double z, std::vector<double> &xx, std::vector<double> &yy);
    void intersection_z(gts_surface &s, double z, array &xx, array &yy);
    //html </a>

    //html <a name="isoline">
    void isoline(const function &f, double zcut, 
		 std::vector<double> &xx,
		 std::vector<double> &yy,
		 double p1min, double p1max,
		 double p2min, double p2max,
		 int np1=100, int np2=100);
    void isoline(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z,
		 double zcut,
		 std::vector<double> &iso_x, std::vector<double> &iso_y);
    void isoline(const array &x, const array &y, const array &z,
		 double zcut,
		 array &iso_x, array &iso_y);
    void isoline(const var &filename, double zcut, std::vector<double> &iso_x, std::vector<double> &iso_y,
		 const function &f1=unset, const function &f2=unset, const function &f3=unset);

    void isoline(const dgraph &g, double zcut, std::vector<double> &iso_x, std::vector<double> &iso_y);

    void isoline_at_coords(const var &filename, double zcut, std::vector<double> &iso_x, std::vector<double> &iso_y,
                           const function &f1=unset, const function &f2=unset, const function &f3=unset);

    //html </a>

} 

#ifdef __MAKECINT__
#pragma link off blop::gts_surface::vertices_;
#pragma link off blop::gts_surface::edges_;
#pragma link off blop::gts_surface::faces_;
#pragma link off blop::gts_surface::surface_;
#pragma link off blop::gts_surface::surface();
#endif



#endif
#endif

