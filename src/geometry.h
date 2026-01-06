#ifndef __BLOP_GEOMETRY_H__
#define __BLOP_GEOMETRY_H__

#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <vector>

#include "vec3.h"

namespace blop
{
    namespace geom
    {
        class point;
	class line;
	class plane;
	class triangle;
        class transformation;

        inline point  intersection(const line &, const plane &);  // intersection of line & plane
        inline point  intersection(const plane &, const line &);  // intersection of line & plane
        inline point  intersection(const line &, const line &);   // intersection of two lines, or midpoint of closest approach.
               line   intersection(const plane &, const plane &);    // defined in geometry.cc
	inline void   closest_approach(const line &l1, const line &l2, point &p1, point &p2); 
	inline double distance(const line &l1, const line &l2);  // distance (closest approach) between two lines
	inline double distance2(const line &l1, const line &l2);  // distance (closest approach) between two lines, squared
        inline double distance(const plane &pl, const point &pnt); 
	inline double distance(const point &pnt, const plane &pl);
        inline double distance(const line &l, const point &p);
        inline double distance(const point &p, const line &l);



        // ----------------------------- point -------------------------------------------------

        class point
        {
        public:
            vec3 coord;
            point(double x, double y, double z=0) : coord(x,y,z) {}
            point(const vec3 &v) : coord(v) {}
            point() : coord(0,0,0) {}

            // intersection of line and plane
            point(const line &l, const plane &p) { *this = intersection(p,l); }
            point(const plane &p, const line &l) { *this = intersection(p,l); }

	    // intersection of two lines (if they do not intersect, midpoint
	    // at closest approach
	    point(const line &l1, const line &l2) { *this = intersection(l1,l2); }

            const point &operator=(const point &rhs)   { coord = rhs.coord; return *this; }
            const point &operator+=(const vec3 &delta) { coord += delta; return *this; }
            const point &operator-=(const vec3 &delta) { coord -= delta; return *this; }

            const point &operator*=(const transformation &t);
        };

	inline bool operator== (const point &p1, const point &p2)
	{
	    return p1.coord==p2.coord;
	}
	inline bool operator!= (const point &p1, const point &p2)
	{
            return p1.coord!=p2.coord;
	}

        inline point origin() { return point(0,0,0); }

	inline std::ostream &operator<<(std::ostream &output,const geom::point &v)
	{
            output<<v.coord;
	    return output;
	}

        inline point operator/(const point &p, double r) {return point(p.coord.x/r,p.coord.y/r,p.coord.z/r);}
        inline point operator/(const point &p, int r) {return point(p.coord.x/r,p.coord.y/r,p.coord.z/r);}
        inline point operator/(const point &p, short r) {return point(p.coord.x/r,p.coord.y/r,p.coord.z/r);}
        inline point operator/(const point &p, unsigned int r) {return point(p.coord.x/r,p.coord.y/r,p.coord.z/r);}

        inline point operator*(const point &p, double r) {return point(p.coord.x*r,p.coord.y*r,p.coord.z*r);}
        inline point operator*(const point &p, int r) {return point(p.coord.x*r,p.coord.y*r,p.coord.z*r);}
        inline point operator*(const point &p, short r) {return point(p.coord.x*r,p.coord.y*r,p.coord.z*r);}
        inline point operator*(const point &p, unsigned int r) {return point(p.coord.x*r,p.coord.y*r,p.coord.z*r);}

        // The difference of two points is a vector
        inline vec3 operator-(const point &p1, const point &p2)
        {
            return p1.coord-p2.coord;
        }
        // A point and a vector can be added, resulting a translated point
        inline point operator+(const point &p, const vec3 &v)
        {
            return point(p.coord+v);
        }
        inline point operator+(const vec3 &v, const point  &p)
        {
            return point(p.coord+v);
        }
        inline point operator-(const point &p, const vec3 &v)
        {
            return point(p.coord-v);
        }

	class line
	{
	private:
	    point p0_;    // a point along a line
	    vec3  dir_;   // direction of the line

	public:
            // determined by a point on the line and a direction
            line(const point &p0, const vec3 &dir) : p0_(p0), dir_(dir,1) { }

            // determined by two points on the line
            explicit line(const point &p1, const point &p2) : p0_(p1), dir_(p2-p1,1) { }

	    // intersection of two planes
	    line(const plane &p1, const plane &p2) { *this = intersection(p1,p2); }

	    // parallel to line through point
            line(const line &l, const point &p) : p0_(p), dir_(l.dir_) {}
	    line(const point &p, const line &l) : p0_(p), dir_(l.dir_) {}

	    // normal to plane through point
            line(const plane &pl, const point &po);
	    line(const point &po, const plane &pl);

            const point &origin() const { return p0_; }
	    const point &p0()  const { return p0_;  }

            // return the normalized direction-vector of the line
	    const vec3  &dir() const { return dir_; }

	    const line &operator=(const line &rhs) { p0_ = rhs.p0_; dir_ = rhs.dir_; return *this; }

            const line &operator*=(const transformation &t);

            // Shift the line (its reference point) by a vector
            const line &operator+=(const vec3 &shift) { p0_+=shift; return *this; }
	};

        // Create a shifted line by a vector
        inline line operator+(const line &l, const vec3 &shift)
        {
            line result = l;
            result += shift;
            return result;
        }

	class plane
	{
	protected:
            point origin_;
	    vec3  normal_;
	public:
            plane(const point &origin, const vec3 &normal) : origin_(origin), normal_(normal) { normal_.normalize(); }

	    // paralell to plane through point
            plane(const plane &pl, const point &po) : origin_(po), normal_(pl.normal_) {}
	    plane(const point &po, const plane &pl) : origin_(po), normal_(pl.normal_) {}

	    // plane through 3 points
            plane(const point &p1, const point &p2, const point &p3) : origin_(p1)
            {
                normal_ = (p2-p1)%(p3-p1);
                normal_.normalize();
            }

	    // plane through the 3 corners of a triangle
            plane(const triangle &t);

            plane &origin(const vec3 &o)  { origin_ = o; return *this; }
            plane &normal(const vec3 &n)  { normal_ = n; return *this; }

	    const point &origin() const { return origin_; }
	    const vec3  &normal() const { return normal_; }

            const plane &operator*=(const transformation &t);

            // Shift the plane by a vector
            const plane &operator+=(const vec3 &shift) { origin_ += shift; return *this; }

            // Shfit the plane by a distance in the normal direction
            const plane &operator+=(double shift) { origin_ += shift*normal_; return *this; }

            static plane XY() { return plane(point(0,0,0),vec3(0,0,1)); }
            static plane YX() { return plane(point(0,0,0),vec3(0,0,1)); }
            static plane ZY() { return plane(point(0,0,0),vec3(1,0,0)); }
            static plane YZ() { return plane(point(0,0,0),vec3(1,0,0)); }
            static plane XZ() { return plane(point(0,0,0),vec3(0,1,0)); }
            static plane ZX() { return plane(point(0,0,0),vec3(0,1,0)); }

	};

        // Plane shifted by a vector
        inline plane operator+(const plane &p, const vec3 &shift)
        {
            plane result = p;
            result += shift;
            return result;
        }

        // Plane shifted by a distance in the normal direction
        inline plane operator+(const plane &p, double distance)
        {
            plane result = p;
            result += distance;
            return result;
        }

        // A plane with a 2D coordinate system, i.e. a local x and y axis (which lie in the plane,
        // and are perpendicular to each other)
        class plane_with_cs : public plane
        {
        private:
            plane &normal(const vec3 &n) { std::cerr<<"plane_with_cs::normal(vec3) should not be called"<<std::endl; return *this; }
        protected:
            vec3 x_, y_;
        public:
            plane_with_cs(const point &origin, const vec3 &x, const vec3 &y);
            const vec3 &x() const { return x_; }
            const vec3 &y() const { return y_; }
        };


        inline plane_with_cs xy_plane() { return plane_with_cs(origin(), vec3(1,0,0), vec3(0,1,0)); }
        inline plane_with_cs yx_plane() { return plane_with_cs(origin(), vec3(0,1,0), vec3(1,0,0)); }
        inline plane_with_cs yz_plane() { return plane_with_cs(origin(), vec3(0,1,0), vec3(0,0,1)); }
        inline plane_with_cs zy_plane() { return plane_with_cs(origin(), vec3(0,0,1), vec3(0,1,0)); }
        inline plane_with_cs xz_plane() { return plane_with_cs(origin(), vec3(1,0,0), vec3(0,0,1)); }
        inline plane_with_cs zx_plane() { return plane_with_cs(origin(), vec3(0,0,1), vec3(1,0,0)); }

        // Create a line through a point, normal to a plane
        inline line::line(const plane &pl, const point &po) : p0_(po), dir_(pl.normal(),1) {}
        inline line::line(const point &po, const plane &pl) : p0_(po), dir_(pl.normal(),1) {}

        // Return the closest-approach points between two lines. Upon return, p1 (along l1) and p2(along l2)
        // indicate the closest approach.
	inline void closest_approach(const line &l1, const line &l2, point &p1, point &p2)
	{
	    const point r1 = l1.p0();
	    const point r2 = l2.p0();
	    const vec3 d1 = l1.dir();
	    const vec3 d2 = l2.dir();
	    const double a = (r1-r2)*(d1+d2)/(d1*d2+1);
	    const double b = (r1-r2)*(d1-d2)/(d1*d2-1);
	    const double t1 = (b-a)/2;
	    const double t2 = (b+a)/2;
	    p1 = r1+t1*d1;
	    p2 = r2+t2*d2;
	}

        // Distance between two lines
	inline double distance(const line &l1, const line &l2)
	{
	    point p1, p2;
	    closest_approach(l1,l2,p1,p2);
	    return abs(p1-p2);
	}

        // Distance between two lines, squared
        inline double distance2(const line &l1, const line &l2)
        {
	    point p1, p2;
	    closest_approach(l1,l2,p1,p2);
            const vec3 d = p1-p2;
	    return d*d;
        }

	// Get the intersection of the two lines. If they are not crossing each
	// other, the midpoint at the shortest distance (closest approach) is returned
	inline point intersection(const line &l1, const line &l2)
	{
	    point p1,p2;
	    closest_approach(l1,l2,p1,p2);
	    return 0.5*(p1.coord+p2.coord);
	}

        // Create a point by the intersection of a plane and a line
	inline point intersection(const plane &p, const line &l)
        {
	    const double t = ((p.origin()-l.p0())*p.normal())/(l.dir()*p.normal());
	    return l.p0()+t*l.dir();
        }
	inline point intersection(const line &l, const plane &p) { return intersection(p,l); }

        // Distance between a plane and a point
	inline double distance(const plane &pl, const point &pnt)
	{
	    return (pnt.coord-pl.origin())*pl.normal();
	}
	inline double distance(const point &pnt, const plane &pl)
	{
	    return distance(pl,pnt);
	}


        // Ditsance between a line and a point
        inline double distance(const line &l, const point &p)
        {
            const vec3 diff = p-l.origin();
            return abs(diff-(l.dir()*diff)*l.dir());
        }
        inline double distance(const point &p, const line &l) { return distance(l,p); }

        // Calculate the midplane between two point
        inline plane midplane(const point &p1, const point &p2)
        {
            return plane(0.5*(p1.coord+p2.coord), p2-p1);
        }

        // determine the centre of the circle going through the three points
        inline point circle_center(const point &p1, const point &p2, const point &p3)
        {
            const plane pl1 = midplane(p2,p1);
            const plane pl2 = midplane(p2,p3);
            const line axis = intersection(pl1,pl2);
            return intersection(axis, plane(p1,p2,p3));
        }

        inline double circle_radius(const point &p1, const point &p2, const point &p3)
        {
            return abs(circle_center(p1,p2,p3)-p2);
        }

        // ----------------------------------------- triangle ------------------------------------

	class triangle 
	{
	private:
	    geom::point vertex_[3];
	public:
	    triangle() {}
	    triangle(const geom::point &v1, const geom::point &v2, const geom::point &v3) { vertex_[0] = v1; vertex_[1] = v2; vertex_[2] = v3; }
	
	    // access the 3 corners. index is 1-based
	    const geom::point     &vertex(int i) const;
	    geom::point           &vertex(int i);
	    triangle              &vertex(int i, const geom::point &v);
	
	    geom::vec3 edge(int i) const;
	
	    // return true if the point 'v' (its perpendicular projection
	    // to the plane of the triangle) is inside the triangle
	    bool inside(const geom::point &v);
	};
	
        // plane through the 3 corners of a triangle
        inline plane::plane(const triangle &t) : origin_(t.vertex(1))
        {
            normal_ = (t.vertex(2)-t.vertex(1))%(t.vertex(3)-t.vertex(1));
            normal_.normalize();
        }



	// -------------------------  transformations and necessary helper functions --------------------

	class transformation
	{
	protected:
	    double matrix_[4][4];
	    //friend vec3 operator*(const transformation &, const vec3 &);
	    friend geom::point operator*(const transformation &, const geom::point &);
	    friend transformation operator*(const transformation &t1, const transformation &t2);
	    friend std::ostream &operator<<(std::ostream &, const transformation &);
	public:
	    virtual ~transformation() {}
	    transformation()
	    {
		for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) matrix_[i][j]=(i==j?1:0);
	    }
	    transformation(const transformation &rhs)
	    {
		for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) matrix_[i][j] = rhs.matrix_[i][j];
	    }
	    const transformation &operator=(const transformation &rhs)
	    {
                for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) matrix_[i][j] = rhs.matrix_[i][j];
                return *this;
            }

            // invert this transformation
            virtual void invert();

            // return the inverse of this transformation
	    transformation inverse() const;

            // remove the translational part of this transformation, modifying 'this'
            void remove_translation();

            rotation get_rotation() const;
	};

	template <int N>
	    int parity(const int P[N])
	{
	    int G = 0;
	    for(unsigned int i=0; i<N; ++i)
	    {
		for(unsigned int j=i+1; j<N; ++j)
		{
		    if(P[i] > P[j]) ++G;
		}
	    }
	    return 1-2*(G%2);
	}

	template <int N>
	double matrix_minor(const double M[N][N], int ind1, int ind2)
	{
	    int I[N-1];
	    int J[N-1];
	    {
		int iout = 0;
		for(int i=0; i<N; ++i) if(i!=ind1) I[iout++] = i;
	    }
	    {
		int iout = 0;
		for(int i=0; i<N; ++i) if(i!=ind2) J[iout++] = i;
	    }

	    double result = 0;
	    do
	    {
		double subresult = 1;
		for(int i=0; i<N-1; ++i)
		{
		    subresult *= M[I[i]][J[i]];
		}
		result += parity<N-1>(J)*subresult;
	    } while(std::next_permutation(J,J+(N-1)));
	    return result;
	}

	template <int N>
	    double cofactor(const double M[N][N], int ind1, int ind2)
	{
	    const int P = 1-(ind1+ind2)%2*2;
	    return P * matrix_minor(M,ind1,ind2);
	}

	template <int N>
	    double determinant(double M[N][N])
	{
	    double result = 0;
	    int ind[N];
	    for(int i=0; i<N; ++i) ind[i] = i; // create the first permutation

	    for(bool ok=true; ok; ok=next_permutation(ind, ind+N))
	    {
		double tmp = 1;
		for(int i=0; i<N; ++i)
		{
		    tmp *= M[i][ind[i]] * parity<N>(ind);
		}
		result += tmp;
	    }
	    return result;
	}

	std::ostream &operator<<(std::ostream &out, const transformation &t);

        class mirroring : public transformation
        {
        private:
            point p0_;
            vec3  normal_;
            void init_(const point &P0, const vec3 &normal);

        public:
            mirroring(const geom::point &p0, const geom::vec3 &normal)
            {
                init_(p0,normal);
            }
            mirroring(const plane &p)
            {
                init_(p.origin(), p.normal());
            }
            void invert()
            {
            }
            mirroring inverse() const // a mirroring is its own inverse
            {
                return mirroring(*this);
            }
        };

	class translation : public transformation
	{
	private:
	    //friend vec3 operator*(const translation &, const vec3 &);
	    friend geom::point operator*(const translation &, const geom::point &);
	    vec3 vec_;
	    void init_(const vec3 &v)
	    {
		vec_ = v;
		for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) matrix_[i][j] = (i==j?1:0);
		matrix_[0][3] = v.x;
		matrix_[1][3] = v.y;
		matrix_[2][3] = v.z;
	    }
	public:
	    translation(const vec3 &v)
	    {
		init_(v);
	    }
	    translation(double x, double y, double z)
	    {
		init_(vec3(x,y,z));
	    }
	    const translation &operator=(const vec3 &v)
		{
		    init_(v);
		    return *this;
		}
            void invert()
            {
                transformation::invert();
                vec_ *= -1;
            }
	    translation inverse() const
	    {
		return translation(-vec_);
	    }
	};

/*
	inline vec3 operator*(const translation &t, const vec3 &v)
	{
	    return vec3(v.x+t.matrix_[0][3],v.y+t.matrix_[1][3],v.z+t.matrix_[2][3]);
	}
*/
	inline geom::point operator*(const translation &t, const geom::point &v)
	{
	    return geom::point(v.coord.x+t.matrix_[0][3],v.coord.y+t.matrix_[1][3],v.coord.z+t.matrix_[2][3]);
	}

        // Rotations - around the origin!
	class rotation : public transformation
	{
	private:
	    friend vec3 operator*(const rotation &, const vec3 &);
	    friend geom::point operator*(const rotation &, const geom::point &);
            friend class transformation;
	    void set_axis_angle_from_matrix_()
	    {
		double cos_angle = 0.5*(matrix_[0][0]+matrix_[1][1]+matrix_[2][2]-1);
                if(cos_angle<-1) cos_angle=-1;
                if(cos_angle> 1) cos_angle= 1;
		//const double sin_angle = std::sqrt(1-cos_angle*cos_angle);
		angle_ = std::acos(cos_angle);
		axis_.z = (matrix_[1][0]-matrix_[0][1]); // /2/sin_angle;
		axis_.y = (matrix_[0][2]-matrix_[2][0]); // /2/sin_angle;
		axis_.x = (matrix_[2][1]-matrix_[1][2]); // /2/sin_angle;
		const double a = abs(axis_);
		axis_ /= a;
		//if(std::fabs(a-1.0)>1e-6)
                //    std::cerr<<"Error: the axis of this rotation matrix is normalized to "<<a<<" instead of 1 (sin_angle="<<sin_angle<<")"<<std::endl;
	    }
	    friend rotation operator*(const rotation &, const rotation &);
	    double angle_;
	    vec3   axis_;
	    void init_(const vec3 &v, double theta);
	public:
	    rotation()
	    {
		init_(vec3(0,0,1),0);
	    }
	    rotation(const vec3 &v, double angle)
	    {
		init_(v,angle);
	    }
            rotation(double angle,const vec3 &v)
            {
                init_(v,angle);
            }
	    rotation(const rotation &rhs);


	    // A rotation which brings v1 --> v1', the axis of which is perpendicular to
	    // the plane spanned by (v1,v1'), i.e. the smallest angle rotation
	    rotation(vec3 v, vec3 vprime);

            // A rotation which brings v1-->v1', v2-->v2'
            // v1,v2 must have the same angle w.r.t. each other as
            // v1' and v2' (for obvious reasons)
	    rotation(vec3 v1, vec3 v2, vec3 v1prime, vec3 v2prime);
    
	    rotation &axis(const vec3 &v)
	    {
		init_(v,angle_);
		return *this;
	    }
	    rotation &angle(double a)
	    {
		init_(axis_,a);
		return *this;
	    }
	    rotation &set(const vec3 &v, double angle)
	    {
		init_(v,angle);
		return *this;
	    }
	    double     angle() const { return angle_; }
	    const vec3 axis () const { return axis_;  }

	    const rotation &operator=(const rotation &rhs)
		{
		    axis_ = rhs.axis_;
		    angle_ = rhs.angle_;
		    for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) matrix_[i][j] = rhs.matrix_[i][j];
		    return *this;
		}

            void invert()
            {
                transformation::invert();
                angle_ *= -1;
            }

	    rotation inverse() const
	    {
                rotation result(*this);
                result.invert();
                return result;
	    }
	};

        // This function returns a transformation corresponding to a rotation around a given axis
        transformation rotation_around_axis(const line &axis, double angle);

	inline vec3 operator* (const rotation &r, const vec3 &v)
	{
	    return vec3(r.matrix_[0][0]*v.x+r.matrix_[0][1]*v.y+r.matrix_[0][2]*v.z,
			r.matrix_[1][0]*v.x+r.matrix_[1][1]*v.y+r.matrix_[1][2]*v.z,
			r.matrix_[2][0]*v.x+r.matrix_[2][1]*v.y+r.matrix_[2][2]*v.z);
	}
	inline geom::point operator* (const rotation &r, const geom::point &v)
	{
	    return vec3(r.matrix_[0][0]*v.coord.x+r.matrix_[0][1]*v.coord.y+r.matrix_[0][2]*v.coord.z,
			r.matrix_[1][0]*v.coord.x+r.matrix_[1][1]*v.coord.y+r.matrix_[1][2]*v.coord.z,
			r.matrix_[2][0]*v.coord.x+r.matrix_[2][1]*v.coord.y+r.matrix_[2][2]*v.coord.z);
	}

	inline rotation operator*(const rotation &r1, const rotation &r2)
	{
	    rotation result;
	    for(int i=0; i<4; ++i)
	    {
		for(int k=0; k<4; ++k)
		{
		    result.matrix_[i][k] = 0;
		    for(int j=0; j<4; ++j) result.matrix_[i][k] += r1.matrix_[i][j]*r2.matrix_[j][k];
		}
	    }
	    result.set_axis_angle_from_matrix_();
	    return result;
	}

/*
	inline vec3 operator*(const transformation &t, const vec3 &v)
	{
	    const double in [4] = {v.x,v.y,v.z,1};
	    double out[3] = {0,0,0};
	    for(int i=0; i<3; ++i)
	    {
		for(int j=0; j<4; ++j)
		{
		    out[i] += t.matrix_[i][j]*in[j];
		}
	    }
	    return vec3(out[0],out[1],out[2]);
	}
*/
	inline geom::point operator*(const transformation &t, const geom::point &v)
	{
	    const double in [4] = {v.coord.x,v.coord.y,v.coord.z,1};
	    double out[3] = {0,0,0};
	    for(int i=0; i<3; ++i)
	    {
		for(int j=0; j<4; ++j)
		{
		    out[i] += t.matrix_[i][j]*in[j];
		}
	    }
	    return geom::point(out[0],out[1],out[2]);
	}

	inline transformation operator*(const transformation &t1, const transformation &t2)
	{
	    transformation result;
	    for(int i=0; i<4; ++i)
	    {
		for(int k=0; k<4; ++k)
		{
		    result.matrix_[i][k] = 0;
		    for(int j=0; j<4; ++j) result.matrix_[i][k] += t1.matrix_[i][j]*t2.matrix_[j][k];
		}
	    }
	    return result;
	}



        inline line operator*(const transformation &t, const line &l)
        {
            line result(l);
            result*=t;
            return result;
        }
        inline plane operator*(const transformation &t, const plane &p)
        {
            plane result(p);
            result*=t;
            return result;
        }

    }

    // Project a point perpendicularly onto a plane
    geom::point project(const geom::point &p, const geom::plane &pl);

    // Project a point perpendicularly onto a line
    geom::point project(const geom::point &p, const geom::line &l);

    // Project a line to a plane
    geom::line project(const geom::line &l, const geom::plane &pl);

    // Project a vector onto a plane (remove its normal component)
    geom::vec3 project(const geom::vec3 &v, const geom::plane &pl);

    // Project a vector onto a line (remove its normal component)
    geom::vec3 project(const geom::vec3 &v, const geom::line &l);


    //html <a name='intersection'>
    // Calculate the intersections of two polylines, the coordinates of which are given
    // in (x1,y1) and (x2,y2). The intersection coordinates are returned in (x,y).
    // The function returns the number of intersection points
    int intersection(const std::vector<double> &x1, const std::vector<double> &y1,
		     const std::vector<double> &x2, const std::vector<double> &y2,
		     std::vector<double> &x, std::vector<double> &y);

    //html </a>



    
}





#endif
