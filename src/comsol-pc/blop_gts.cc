#include "blop_gts.h"
#include "warning.h"
#include "constants.h"
#include "bloputils.h"
#include "ignore.h"
#include "geometry.h"
#include "plot.h"
#include <algorithm>
#include <cmath>
using namespace std;

#ifdef HAVE_GTS_H

namespace blop 
{
    inline double sq(double a) { return a*a; }

    double xmin(const GtsTriangle *t)
    {
	GtsVertex *v1, *v2, *v3;
	vertices(t,&v1,&v2,&v3);
	if(v1==v2 || v1==v3 || v2==v3) warning::print("Error in xmin(GtsTriangle*). Contact the author");
	return std::min(v1->p.x,std::min(v2->p.x,v3->p.x));
    }
    double xmax(const GtsTriangle *t)
    {
	GtsVertex *v1, *v2, *v3;
	vertices(t,&v1,&v2,&v3);
	return std::max(v1->p.x,std::max(v2->p.x,v3->p.x));
    }
    double ymin(const GtsTriangle *t)
    {
	GtsVertex *v1, *v2, *v3;
	vertices(t,&v1,&v2,&v3);
	return std::min(v1->p.y,std::min(v2->p.y,v3->p.y));
    }
    double ymax(const GtsTriangle *t)
    {
	GtsVertex *v1, *v2, *v3;
	vertices(t,&v1,&v2,&v3);
	return std::max(v1->p.y,std::max(v2->p.y,v3->p.y));
    }

    void   bbox(const GtsTriangle *t, double *xmin, double *xmax, double *ymin, double *ymax, double *zmin, double *zmax)
    {
	GtsVertex *v1, *v2, *v3;
	vertices(t,&v1,&v2,&v3);
	if(v1==v2 || v1==v3 || v2==v3) warning::print("Error in bbox(GtsTriangle*,...). Contact the author");
	*xmin = std::min(v1->p.x,std::min(v2->p.x,v3->p.x));
	*xmax = std::max(v1->p.x,std::max(v2->p.x,v3->p.x));
	*ymin = std::min(v1->p.y,std::min(v2->p.y,v3->p.y));
	*ymax = std::max(v1->p.y,std::max(v2->p.y,v3->p.y));
	if(zmin) *zmin = std::min(v1->p.z,std::min(v2->p.z,v3->p.z));
	if(zmax) *zmax = std::max(v1->p.z,std::max(v2->p.z,v3->p.z));
    }

    void   vertices(const GtsTriangle *t, GtsVertex **v1, GtsVertex **v2, GtsVertex **v3)
    {
	*v1 = t->e1->segment.v1;
	*v2 = t->e1->segment.v2;
	*v3 = ((t->e2->segment.v1==*v1||t->e2->segment.v1==*v2)?t->e2->segment.v2:t->e2->segment.v1);
    }



    gint vertex_collecting_function(gpointer f, gpointer v)
    {
	GtsVertex *vertex = (GtsVertex*)f;
	vector<GtsVertex*> *vertexvector = (vector<GtsVertex*>*)v;
	vertexvector->push_back(vertex);
	return 0;
    }
    const GtsVertex *gts_surface::vertex(unsigned int i) const
    {
	if(vertices_.empty())
	{
	    gts_surface_foreach_vertex(surface(),vertex_collecting_function,(gpointer)&vertices_);
	}
	if(i<0 || vertices_.size()<=i) return 0;
	return vertices_[i];
    }

    gint face_collecting_function(gpointer f, gpointer v)
    {
	GtsFace *face = (GtsFace*)f;
	vector<GtsFace*> *facevector = (vector<GtsFace*>*)v;
	facevector->push_back(face);
	return 0;
    }
    const GtsFace *gts_surface::face(unsigned int i) const
    {
	if(faces_.empty())  // initialize the faces array
	{
	    gts_surface_foreach_face(surface(),face_collecting_function,(gpointer)&faces_);
	}
	if(i<0 || faces_.size()<=i) return 0;
	return faces_[i];
    }


    gint edge_collecting_function(gpointer f, gpointer v)
    {
	GtsEdge *edge = (GtsEdge*)f;
	vector<GtsEdge*> *edgevector = (vector<GtsEdge*>*)v;
	edgevector->push_back(edge);
	return 0;
    }
    const GtsEdge *gts_surface::edge(unsigned int i) const
    {
	if(edges_.empty())
	{
	    gts_surface_foreach_edge(surface(),edge_collecting_function,(gpointer)&edges_);
	}
	if(i<0 || edges_.size()<=i) return 0;
	return edges_[i];
    }

    int gts_surface::nvertices() const
    {
	return gts_surface_vertex_number(surface());
    }
    int gts_surface::nfaces() const
    {
	return gts_surface_face_number(surface());
    }
    int gts_surface::nedges() const
    {
	return gts_surface_edge_number(surface());
    }

    gts_surface::~gts_surface()
    {
	clear();
    }

    GtsSurface *gts_surface::surface() const
    {
	if(!initialized_) initialize();
	return surface_;
    }

    void gts_surface::clear_vertices_() const
    {
	//for(unsigned int i=0; i<vertices_.size(); ++i) gts_object_destroy(GTS_OBJECT(vertices_[i]));
	vertices_.clear();
    }
    void gts_surface::clear_edges_() const
    {
	//for(unsigned int i=0; i<edges_.size(); ++i) gts_object_destroy(GTS_OBJECT(edges_[i]));
	edges_.clear();
    }
    void gts_surface::clear_faces_() const
    {
	//for(unsigned int i=0; i<faces_.size(); ++i) gts_object_destroy(GTS_OBJECT(faces_[i]));
	faces_.clear();
    }
    void gts_surface::clear_enclosing_triangle_() const
    {
	for(int i=0; i<3; ++i) enclosing_triangle_x_[i] = enclosing_triangle_y_[i] = 0;
    }
    void gts_surface::clear_bounding_boxes_() const
    {
	if(bounding_boxes_) gts_bb_tree_destroy(bounding_boxes_, TRUE);
	bounding_boxes_ = 0;
    }

    gts_surface &gts_surface::clear()
    {
	if(surface_) gts_object_destroy(GTS_OBJECT(surface_));
	surface_ = 0;
	clear_vertices_();
	clear_edges_();
	clear_faces_();
	clear_enclosing_triangle_();
	clear_bounding_boxes_();
	initialized_ = false;
	return *this;
    }

    void gts_surface::enclosing_triangle(double x[3], double y[3], double scale) const
    {
	if(enclosing_triangle_x_[0] == 0.0 &&
	   enclosing_triangle_x_[1] == 0.0 &&
	   enclosing_triangle_x_[2] == 0.0 &&
	   enclosing_triangle_y_[0] == 0.0 &&
	   enclosing_triangle_y_[1] == 0.0 &&
	   enclosing_triangle_y_[2] == 0.0)
	{
	    const int n = nvertices();
	    double xmin=0, xmax=0, ymin=0, ymax=0;
	    for(int i=0; i<n; ++i)
	    {
		const GtsVertex *v = vertex(i);
		const double x = v->p.x;
		const double y = v->p.y;
		if(i==0 || x<xmin) xmin = x;
		if(i==0 || x>xmax) xmax = x;
		if(i==0 || y<ymin) ymin = y;
		if(i==0 || y>ymax) ymax = y;
	    }
	    const double a = xmax-xmin;
	    const double b = ymax-ymin;
	    const double xcenter = 0.5*(xmin+xmin);
	    const double A = a/2+b/tan(60*unit::deg);

	    enclosing_triangle_x_[0] = -A + xcenter;
	    enclosing_triangle_y_[0] = ymin;
	    enclosing_triangle_x_[1] =  A + xcenter;
	    enclosing_triangle_y_[1] = ymin;
	    enclosing_triangle_x_[2] = xcenter;
	    enclosing_triangle_y_[2] = ymin + 2*A*std::sin(60*unit::deg);
	}
	double
	    cog_x=enclosing_triangle_x_[0],
	    cog_y=enclosing_triangle_y_[0]; // centers of gravity;
	for(int i=1; i<3; ++i)
	{
	    cog_x += enclosing_triangle_x_[i];
	    cog_y += enclosing_triangle_y_[i];
	}
	cog_x /= 3;
	cog_y /= 3;
	for(int i=0; i<3; ++i)
	{
	    const double dx = enclosing_triangle_x_[i] - cog_x;
	    const double dy = enclosing_triangle_y_[i] - cog_y;
	    x[i] = cog_x + scale*dx;
	    y[i] = cog_y + scale*dy;
	}
    }

    GNode *gts_surface::bounding_boxes() const
    {
	if(bounding_boxes_ == 0) bounding_boxes_ = gts_bb_tree_surface(surface());
	return bounding_boxes_;
    }

    gts_surface::gts_surface()
	: bounding_boxes_(0), initialized_(false), surface_(0) 
    {
	for(int i=0; i<3; ++i) enclosing_triangle_x_[i] = enclosing_triangle_y_[i] = 0;
    }

    // ----------------------------------------------------------

    double delaunay_surface::z_at(double x, double y) const
    {
        for(int i=0; i<nfaces(); ++i)
        {
	    GtsVertex *v1, *v2, *v3;
	    vertices(&(face(i)->triangle), &v1, &v2, &v3);
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
        return unset;
    }

    void delaunay_surface::get_triangle_grid(vector<double> &x, vector<double> &y)
    {
	const GtsVertex *vlast = 0;
	for(int i=0; i<nedges(); ++i)
	{
	    const GtsEdge *e = edge(i);
	    const GtsVertex *v1 = e->segment.v1;
	    const GtsVertex *v2 = e->segment.v2;
	    if(v2==vlast) swap(v1,v2);
	    if(v1==vlast)
	    {
		x.push_back(v2->p.x);
		y.push_back(v2->p.y);
	    }
	    else
	    {
		if(!x.empty())
		{
		    x.push_back(unset);
		    y.push_back(unset);
		}
		x.push_back(v1->p.x);
		y.push_back(v1->p.y);
		x.push_back(v2->p.x);
		y.push_back(v2->p.y);
	    }
	    vlast = v2;
	}
    }

    dgraph &delaunay_surface::plot_triangles()
    {
	vector<double> x,y;
	get_triangle_grid(x,y);
	dgraph &result = plot(x,y);
	result.ds(lines());
	return result;
    }
    dgraph &delaunay_surface::mplot_triangles()
    {
	vector<double> x,y;
	get_triangle_grid(x,y);
	dgraph &result = mplot(x,y);
	result.ds(lines());
	return result;
    }

    delaunay_surface &delaunay_surface::add_vertex(double x, double y, double z)
    {
	vertices_.push_back(gts_vertex_new(gts_vertex_class(),x,y,z));
	initialized_ = false;
	clear_faces_();
	clear_edges_();
	clear_enclosing_triangle_();
	clear_bounding_boxes_();
	return *this;
    }

    delaunay_surface::delaunay_surface()
    {
    }

    delaunay_surface::delaunay_surface(const delaunay_surface &rhs)
    {
	vertices_.resize(rhs.vertices_.size());
	for(unsigned int i=0; i<vertices_.size(); ++i)
	{
	    vertices_[i] = gts_vertex_new(gts_vertex_class(),rhs.vertices_[i]->p.x,rhs.vertices_[i]->p.y,rhs.vertices_[i]->p.z);
	}
    }

    delaunay_surface::~delaunay_surface()
    {
    }



    void delaunay_surface::initialize() const
    {
	// Delete first the surface_ if it was already set in a previous initialize operation
	if(surface_) gts_object_destroy(GTS_OBJECT(surface_));
	surface_ = 0;
	clear_edges_();
	clear_faces_();

	if(vertices_.size()<3)
	{
	    warning::print("delaunay_surface has less than 3 vertices","delaunay_surface::initialzie()");
	    return;
	}

	surface_ = gts_surface_new(gts_surface_class(),
				   gts_face_class(),
				   gts_edge_class(),
				   gts_vertex_class());

	if(vertices_.size() == 3)
	{
	    GtsEdge *e1 = gts_edge_new(gts_edge_class(), vertices_[0], vertices_[1]);
	    GtsEdge *e2 = gts_edge_new(gts_edge_class(), vertices_[1], vertices_[2]);
	    GtsEdge *e3 = gts_edge_new(gts_edge_class(), vertices_[2], vertices_[0]);
	    gts_surface_add_face(surface_, gts_face_new(gts_face_class(), e1, e2, e3));
	    return;
	}

	// now create the enclosing triangle 
	GtsVertex *enclosing_v1=0, *enclosing_v2=0, *enclosing_v3=0;
	{
	    GtsTriangle *enclosing_t;
	    {
		GSList * list = NULL;
		for (unsigned int i = 0; i < vertices_.size(); ++i)
		{
		    list = g_slist_prepend (list, vertices_[i]);
		}
		enclosing_t = gts_triangle_enclosing (gts_triangle_class (), list, 100.);
		g_slist_free (list);
	    }


	    gts_triangle_vertices(enclosing_t, &enclosing_v1, &enclosing_v2, &enclosing_v3);
	    gts_surface_add_face (surface_, gts_face_new (gts_face_class (),
							  enclosing_t->e1,
							  enclosing_t->e2,
							  enclosing_t->e3));

	}

	for(unsigned int i=0; i<vertices_.size(); ++i)
	{
	    GtsVertex * v1 = vertices_[i];
	    GtsVertex * v = (GtsVertex *)(gts_delaunay_add_vertex (surface_, v1, NULL));
	    g_assert (v != v1);
	    if (v != NULL)
	    {
		warning::print(var("Duplicate datapoint (") &
			       var(v->p.x) & var(",") &
			       var(v->p.y) & var(",") &
			       var(v->p.z) & var(")"),
			       "delaunay_surface::initialize()");
		gts_vertex_replace (v1, v);
	    }
	}

	gts_allow_floating_vertices = TRUE;
	gts_object_destroy (GTS_OBJECT (enclosing_v1));
	gts_object_destroy (GTS_OBJECT (enclosing_v2));
	gts_object_destroy (GTS_OBJECT (enclosing_v3));
	gts_allow_floating_vertices = FALSE;
    
	if(gts_delaunay_check(surface_)) warning::print("Surface is not Delaunay","delaunay_surface::initialize()");

	initialized_ = true;

    }


    // -------------------------------------------------------------

    GtsVertex * gts_next_vertex (GtsVertex * v)
    {
	GSList * j = v->segments;
	
	while (j)
	{
	    GtsSegment * s = (GtsSegment*)(j->data);
	    
	    if (GTS_OBJECT (s)->reserved == s)
	    {
		GTS_OBJECT (s)->reserved = NULL;
		return s->v1 != v ? s->v1 : s->v2;
	    }
	    j = j->next;
	}
	
	return NULL;
    }

    GSList * gts_chain_segments (GSList * segments)
    {
	GSList *chains = NULL, *i = segments;
	
	while (i)
	{
	    GTS_OBJECT (i->data)->reserved = i->data;
	    i = i->next;
	}
	
	i = segments;
	while (i)
	{
	    GtsSegment * s = (GtsSegment*)(i->data);
	    
	    if (GTS_OBJECT (s)->reserved == s)
	    {
		GSList * chain = g_slist_prepend (NULL, s->v1);
		GtsVertex * v = s->v2;
		
		GTS_OBJECT (s)->reserved = NULL;
		while (v)
		{
		    chain = g_slist_prepend (chain, v);
		    v = gts_next_vertex (v);
		}
		chains = g_slist_prepend (chains, chain);
	    }
	    i = i->next;
	}
    
	return chains;
    }

    void intersection_z(gts_surface &s, double z, std::vector<double> &xx, std::vector<double> &yy)
    {
	// Create the cutplane. Make it static, so that we do not need to create/delete
	// these GTS objects across multiple calls to intersection_z, but only update
	// their coordinates
	static GtsSurface *cutplane = 0; 
	{
	    static GtsFace *cutplane_face = 0;
	    static GtsVertex *cutplane_v1 = 0;
	    static GtsVertex *cutplane_v2 = 0;
	    static GtsVertex *cutplane_v3 = 0;

	    // If this is the first call, i.e. these GTS objects have not yet been created, create them.
	    if(!cutplane)
	    {
		cutplane = gts_surface_new(gts_surface_class (),
					   gts_face_class (),
					   gts_edge_class (),
					   gts_vertex_class ());
		
		cutplane_v1 = gts_vertex_new(gts_vertex_class(),0,0,0);
		cutplane_v2 = gts_vertex_new(gts_vertex_class(),0,1,0);
		cutplane_v3 = gts_vertex_new(gts_vertex_class(),1,0,0);
		cutplane_face = gts_face_new(gts_face_class(),
					     gts_edge_new(gts_edge_class(),cutplane_v1,cutplane_v2),
					     gts_edge_new(gts_edge_class(),cutplane_v2,cutplane_v3),
					     gts_edge_new(gts_edge_class(),cutplane_v3,cutplane_v1));
		gts_surface_add_face(cutplane, cutplane_face);
	    }

	    // now only change the coordinates of this cutplane
	    // get the enclosing triangle's corner coordinates (scaled by 2 to surely
	    // enclose all the (x,y) points of the surface's vertices)
	    double enclosing_x[3], enclosing_y[3];
	    s.enclosing_triangle(enclosing_x, enclosing_y, 2);

	    cutplane_v1->p.x = enclosing_x[0];
	    cutplane_v1->p.y = enclosing_y[0];
	    cutplane_v1->p.z = z;
	    cutplane_v2->p.x = enclosing_x[1];
	    cutplane_v2->p.y = enclosing_y[1];
	    cutplane_v2->p.z = z;
	    cutplane_v3->p.x = enclosing_x[2];
	    cutplane_v3->p.y = enclosing_y[2];
	    cutplane_v3->p.z = z;
	}


	GNode *cutplane_bb = gts_bb_tree_surface(cutplane);
	GSList *intersection = gts_surface_intersection(s.surface(), cutplane,
							s.bounding_boxes(), cutplane_bb);
	gts_bb_tree_destroy(cutplane_bb, TRUE);

	GSList *chains = gts_chain_segments(intersection);

	int ichain=0;
	for(GSList *i = chains; i; i=i->next, ++ichain)
	{
	    // put a separator (unset,unset) point
	    if(!xx.empty() && xx.back()!=unset)
	    {
		xx.push_back(unset);
		yy.push_back(unset);
	    }
	    int n = 0;
	    for(GSList *j = (GSList *)(i->data); j; j=j->next)
	    {
		GtsPoint * p = (GtsPoint *)(j->data);
		const double x = p->x;
		const double y = p->y;

		// skip single separated points...
		if(n == 0 && j->next == 0) continue;

		// and also those which agree with the last one
		if(!xx.empty() && xx.back() == x && yy.back() == y) continue;

		++n;
		xx.push_back(x);
		yy.push_back(y);
	    }
	    if(n == 1)
	    {
		xx.resize(xx.size()-1);
		yy.resize(yy.size()-1);
	    }
	    g_slist_free ((GSList *)(i->data));
	}

	g_slist_free(chains);
    }
    void intersection_z(gts_surface &s, double z, array &xx, array &yy)
    {
	GNode *surface_bb = gts_bb_tree_surface(s.surface());

	GtsSurface *cutplane = gts_surface_new(gts_surface_class (),
					       gts_face_class (),
					       gts_edge_class (),
					       gts_vertex_class ());


	GtsVertex *enclosing_v1=0, *enclosing_v2=0, *enclosing_v3=0;
	{
	    GtsTriangle *enclosing_t;
	    GSList * list = NULL;
	    for(int i=0; i<s.nvertices(); ++i)
	    {
		list = g_slist_prepend (list, (gpointer)s.vertex(i));
	    }
	    enclosing_t = gts_triangle_enclosing (gts_triangle_class(), list, 100.);
	    g_slist_free (list);
	    gts_triangle_vertices(enclosing_t, &enclosing_v1, &enclosing_v2, &enclosing_v3);
	    enclosing_v1->p.z = z;
	    enclosing_v2->p.z = z;
	    enclosing_v3->p.z = z;

	    gts_surface_add_face (cutplane,
				  gts_face_new (gts_face_class(),
						gts_edge_new(gts_edge_class(),enclosing_v1,enclosing_v2),
						gts_edge_new(gts_edge_class(),enclosing_v2,enclosing_v3),
						gts_edge_new(gts_edge_class(),enclosing_v3,enclosing_v1)));
	}

	GNode *cutplane_bb = gts_bb_tree_surface(cutplane);

	GSList *intersection = gts_surface_intersection(s.surface(), cutplane,
							surface_bb, cutplane_bb);
	GSList *chains = gts_chain_segments(intersection);

	int ichain=0;
	for(GSList *i = chains; i; i=i->next, ++ichain)
	{
	    // put a separator (unset,unset) point
	    if(!xx.empty() && xx.back()!=unset)
	    {
		xx.push_back(var(unset));
		yy.push_back(var(unset));
	    }
	    int n = 0;
	    for(GSList *j = (GSList *)(i->data); j; j=j->next)
	    {
		GtsPoint * p = (GtsPoint *)(j->data);
		const double x = p->x;
		const double y = p->y;

		// skip single separated points...
		if(n == 0 && j->next == 0) continue;

		// and also those which agree with the last one
		if(!xx.empty() && xx.back() == x && yy.back() == y) continue;

		++n;
		xx.push_back(var(x));
		yy.push_back(var(y));
	    }
	    if(n == 1)
	    {
		xx.resize(xx.size()-1);
		yy.resize(yy.size()-1);
	    }
	    g_slist_free ((GSList *)(i->data));
	}

	g_slist_free(chains);

    }


    void isoline(const var &filename, double zcut, vector<double> &iso_x, vector<double> &iso_y,
		 const function &f1, const function &f2, const function &f3)
    {
	iso_x.clear();
	iso_y.clear();
	delaunay_surface surf;
	function filter = unset;
	if(f1.initialized()) filter.append(f1);
	if(f2.initialized()) filter.append(f2);
	if(f3.initialized()) filter.append(f3);

	if(filter.initialized() && filter.components() < 3)
	{
	    warning::print("Filter has too few components","isoline(filename,zcut,iso_x,iso_y,f1,f2,f3)");
	    return;
	}

	vector<double> x,y,z;
	int lineno = 0;
	istream *file = openin(filename);
	vector<var> filter_result(filter.initialized()?filter.components():0);
        bool last_line_empty = false;
        bool last2_line_empty = false; // 2nd last line empty
        int blockno=0, Blockno=0;
	for(array line; getline(*file,line); ++lineno)
	{
            if(last_line_empty && !line.empty()) ++blockno;
            if(last_line_empty && last2_line_empty && !line.empty()) ++Blockno;
            last2_line_empty = last_line_empty;
            last_line_empty = line.empty();

            function::clear_named_params();
	    function::named_param("linenumber",lineno);
            function::named_param("blocknumber",blockno);
            function::named_param("Blocknumber",Blockno);

	    if(line.empty()) continue;
	    if(line[0][0] == '#') continue;
	    if(filter.initialized() && filter.nargs()>(int)line.size()) continue;
	    if(!filter.initialized() && line.size()<3) continue; 
	    bool skip_this_point = false;
	    for(unsigned int i=0; i<line.size(); ++i)
	    {
		if((filter.initialized()?filter.uses_arg(i+1):true) && ignore::it(line[i]))
		{
		    skip_this_point=true;
		    break;
		}
	    }
	    if(skip_this_point) continue;
	    if(filter.initialized())
	    {
		
		filter.meval(line,filter_result);
		surf.add_vertex(filter_result[0].dbl(),filter_result[1].dbl(),filter_result[2].dbl());
	    }
	    else
	    {
		surf.add_vertex(line[0].dbl(),line[1].dbl(),line[2].dbl());
	    }
	}

	intersection_z(surf,zcut,iso_x,iso_y);
    }

    void isoline(const function &f, double zcut, 
		 std::vector<double> &xx,
		 std::vector<double> &yy,
		 double p1min, double p1max,
		 double p2min, double p2max,
		 int np1, int np2)
    {
	delaunay_surface surf;
	const double dp1 = (p1max-p1min)/np1;
	const double dp2 = (p2max-p2min)/np2;
	for(double p1=p1min; p1<p1max+0.1*dp1; p1+=dp1)
	{
	    for(double p2=p2min; p2<p2max+0.1*dp2; p2+=dp2)
	    {
		surf.add_vertex(p1,p2,f(p1,p2).dbl());
	    }
	}
	intersection_z(surf,zcut,xx,yy);
    }

    void isoline(const vector<double> &x, const vector<double> &y, const vector<double> &z,
		 double zcut,
		 vector<double> &iso_x, vector<double> &iso_y)
    {
	delaunay_surface surf;
	if(x.size() != y.size() || x.size() != z.size())
	{
	    warning::print("The size of the x/y/z arrays do not match","isoline(const vector<double> &, ....)");
	}
	unsigned int n = ::min(x.size(),::min(y.size(),z.size()));
	for(unsigned int i=0; i<n; ++i)
	{
	    surf.add_vertex(x[i],y[i],z[i]);
	}
	intersection_z(surf,zcut,iso_x,iso_y);
    }

    void isoline(const dgraph &g,
		 double zcut,
		 vector<double> &iso_x, vector<double> &iso_y)
    {
	delaunay_surface surf;
	for(unsigned int i=0; i<g.size(); ++i)
	{
            const datapoint *p = g.get(i);
	    surf.add_vertex((*p)[0],(*p)[1],(*p)[2]);
	}
	intersection_z(surf,zcut,iso_x,iso_y);
    }

    void isoline(const array &x, const array &y, const array &z,
		 double zcut,
		 array &iso_x, array &iso_y)
    {
	delaunay_surface surf;
	if(x.size() != y.size() || x.size() != z.size())
	{
	    warning::print("The size of the x/y/z arrays do not match","isoline(const vector<double> &, ....)");
	}
	unsigned int n = ::min(x.size(),::min(y.size(),z.size()));
	for(unsigned int i=0; i<n; ++i)
	{
	    surf.add_vertex(x[i].dbl(),y[i].dbl(),z[i].dbl());
	}
	intersection_z(surf,zcut,iso_x,iso_y);
    }


/*

    inline double getx(GPtrArray *v, int i)
    {
	return ((GtsVertex*)g_ptr_array_index(v,i))->p.x;
    }
    inline double gety(GPtrArray *v, int i)
    {
	return ((GtsVertex*)g_ptr_array_index(v,i))->p.y;
    }

    void center_of_mass(GPtrArray *vertices, double *x, double *y)
    {
	*x = 0;
	*y = 0;
	for(unsigned int i=0; i<vertices->len; ++i)
	{
	    *x += getx(vertices,i);
	    *y += gety(vertices,i);
	}
	*x /= vertices->len;
	*y /= vertices->len;
    }

    int closest_point(GPtrArray *vertices, double x, double y)
    {
	int result = -1;
	double min_distance = 0;
	for(unsigned int i=0; i<vertices->len; ++i)
	{
	    const double xi = getx(vertices,i);
	    const double yi = gety(vertices,i);
	    const double distance = std::sqrt(sq(xi-x)+sq(yi-y));
	    if(i==0 || distance<min_distance)
	    {
		result = i;
		min_distance = distance;
	    }
	}
	return result;
    }

    void gift_wrapping_algorithm(GPtrArray *vertices, vector<unsigned int> &result)
    {
	result.clear();
	
	// find the leftmost (and lowermost) point
	{
	    double index = 0;
	    double xmin = getx(vertices,0);
	    double ymin = gety(vertices,0);
	    for(unsigned int i=1; i<vertices->len; ++i)
	    {
		const double x = getx(vertices,i);
		const double y = gety(vertices,i);
		if(x<xmin)
		{
		    index = i;
		    xmin = x;
		    ymin = y;
		}
		if(x==xmin)
		{
		    if(y<ymin)
		    {
			index = i;
			xmin = x;
			ymin = y;
		    }
		}
	    }
	    // we have the first point on the hull
	    result.push_back(index);
	}

	double x_last_hull = getx(vertices,result.back());
	double y_last_hull = gety(vertices,result.back());
	double x_butlast_hull = x_last_hull;
	double y_butlast_hull = y_last_hull-1;
	double dx_last_hull = x_butlast_hull-x_last_hull;
	double dy_last_hull = y_butlast_hull-y_last_hull;
	double length_last_hull = std::sqrt(dx_last_hull*dx_last_hull+dy_last_hull*dy_last_hull);

	while(true)
	{
	    unsigned int ind = 0;
	    double min_cos = 1;
	    double max_length = 0;
	    for(unsigned int i=0; i<vertices->len; ++i)
	    {
		if(i==result.back()) continue;
		//if(find(result.begin(), result.end(), i) != result.end()) continue;
		const double x = getx(vertices,i);
		const double y = gety(vertices,i);
		
		const double dx = x - x_last_hull;
		const double dy = y - y_last_hull;
		
		const double length = std::sqrt(dx*dx+dy*dy);
		const double cos_angle = (dx*dx_last_hull+dy*dy_last_hull)/(length*length_last_hull);
		if(i==0 || cos_angle<min_cos || (cos_angle == min_cos && length>max_length))
		{
		    ind = i;
		    min_cos = cos_angle;
		    max_length = length;
		}
	    }

	    if(find(result.begin()+1, result.end(), ind) != result.end())
	    {
		cerr<<"This should probably never happen"<<endl;
	    }

	    // the next found point is the first one: stop the loop
	    if(ind == result[0] || ind<0) break;
	    
	    result.push_back(ind);
	    x_butlast_hull = x_last_hull;
	    y_butlast_hull = y_last_hull;
	    x_last_hull = getx(vertices,ind);
	    y_last_hull = gety(vertices,ind);
	    dx_last_hull = x_butlast_hull-x_last_hull;
	    dy_last_hull = y_butlast_hull-y_last_hull;
	    length_last_hull = std::sqrt(dx_last_hull*dx_last_hull+dy_last_hull*dy_last_hull);
	}
    }
*/

}

#endif
// HAVE_GTS_H

