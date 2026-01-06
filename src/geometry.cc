#include "geometry.h"
#include "constants.h"
#include "warning.h"
#include <iostream>

using namespace std;

namespace blop
{
    namespace geom
    {
        // treat a scalar product as non-0 if its absolute value is larger than this
        const double scalar_product_tolerance = 1e-6;

        const vec3 &vec3::operator*=(const rotation &t)
        {
            *this = t*(*this);
            return *this;
        }
        const point &point::operator*=(const transformation &t)
        {
            *this = t*(*this);
            return *this;
        }
        const line &line::operator*=(const transformation &t)
        {
            p0_ *= t;
            dir_ *= t.get_rotation();
            return *this;
        }
        const plane &plane::operator*=(const transformation &t)
        {
            origin_ *= t;
            normal_ *= t.get_rotation();
            return *this;
        }

	line intersection(const plane &p1, const plane &p2)
	{
	    // the intersection line lies on both planes, i.e. its direction
	    // must be perpendicular to both normal vectors. Such a vector can
	    // be constructed as the vector-product of the two normal-vectors
	    const vec3 n1 = p1.normal();
	    const vec3 n2 = p2.normal();
	    const vec3 d(n1%n2,1);

	    // Now only a point along this line must be chosen. It is arbitrary,
	    // but let's choose the one which is closest to the origin, i.e.
	    // thee vector pointing to it from the origin is perpendicular to
	    // the line (d). it satisfies then the following equations:
	    // (r-p1)*n1 = 0; (r-p2)*n2 = 0; r*d=0;
	    // this is a linear system of equations:
	    // [ n1x  n1y  n1z ] [rx]   [n1*p1]           [rx]
	    // [ n2x  n2y  n2z ]*[ry] = [n2*p2]   = [m] * [ry]
	    // [ dx   dy   dz  ] [rz]   [0    ]           [rz]
	    // inver this matrix, etc.
/*
	    double m[3][3];
	    m[0][0] = n1.x; // a
	    m[0][1] = n1.y; // b
	    m[0][2] = n1.z; // c
	    m[1][0] = n2.x; // d
	    m[1][1] = n2.y; // e
	    m[1][2] = n2.z; // f
	    m[2][0] = d.x;  // g
	    m[2][1] = d.y;  // h
	    m[2][2] = d.z;  // k
*/

	    const double det_m = n1.x*(n2.y*d.z-n2.z*d.y) + n1.y*(n2.z*d.x-d.z*n2.x) + n1.z*(n2.x*d.y-n2.y*d.x);
	    double m_inv[3][3];
	    m_inv[0][0] = (n2.y*d.z-n2.z*d.y)/det_m;
	    m_inv[0][1] = (n1.z*d.y-n1.y*d.z)/det_m;
	    m_inv[0][2] = (n1.y*n2.z-n1.z*n2.y)/det_m;
	    m_inv[1][0] = (n2.z*d.x-n2.x*d.z)/det_m;
	    m_inv[1][1] = (n1.x*d.z-n1.z*d.x)/det_m;
	    m_inv[1][2] = (n1.z*n2.x-n1.x*n2.z)/det_m;
	    m_inv[2][0] = (n2.x*d.y-n2.y*d.x)/det_m;
	    m_inv[2][1] = (d.x*n1.y-n1.x*d.y)/det_m;
	    m_inv[2][2] = (n1.x*n2.y-n1.y*n2.x)/det_m;

/*
	    cerr<<"remove matrix inversion check"<<endl;
	    for(int i=0; i<3; ++i)
	    {
		for(int k=0; k<3; ++k)
		{
		    double result = 0;
		    for(int j=0; j<3; ++j)
		    {
			result += m[i][j]*m_inv[j][k];
		    }
		    if( (i==k && fabs(result-1.0)>0.000001) ||
			(i!=k && fabs(result)    >0.000001))
		    {
			cerr<<"Error in matrix inversion"<<endl;
			exit(1);
		    }
		}
	    }
*/

	    const double a0 = n1*p1.origin().coord;
	    const double a1 = n2*p2.origin().coord;
	    const vec3 p0(m_inv[0][0]*a0+m_inv[0][1]*a1,
			  m_inv[1][0]*a0+m_inv[1][1]*a1,
			  m_inv[2][0]*a0+m_inv[2][1]*a1);
	
	    return line(p0,d);
	}

        // -----------------------------------  plane_with_cs  ---------------------------------

        plane_with_cs::plane_with_cs(const point &origin, const vec3 &x, const vec3 &y)
            : plane(origin, vec3(x%y,1))
        {
            x_ = x/abs(x);
            y_ = y/abs(y);
            if(fabs(x_*y_)>scalar_product_tolerance)
            {
                warning::print("x and y are not perpendicular","plane_with_cs::plane_with_cs(const point &origin, const vec3 &x, const vec3 &y)");
                y_ = vec3(normal_%x,1);
            }
        }
        

	// ------------------------------------------  triangle --------------------------------

	vec3 triangle::edge(int i) const
	{
	    --i;
	    if(i<0 || 2<i)
	    {
		cerr<<"Vertex number out of range: "<<i+1<<endl;
		return vec3(std::sqrt(-1.0),std::sqrt(-1.0), std::sqrt(-1.0));
	    }
	    const point v1 = vertex_[i];
	    const point v2 = (i<2?vertex_[i+1]:vertex_[0]);
	    return v2-v1;
	}
    
	const point &triangle::vertex(int i) const
	{
	    --i;
	    if(i<0 || 2<i)
	    {
		cerr<<"Vertex number out of range: "<<i+1<<endl;
		static point dummy(std::sqrt(-1.0),std::sqrt(-1.0), std::sqrt(-1.0));
		return dummy;
	    }
	    return vertex_[i];
	}

	point &triangle::vertex(int i)
	{
	    --i;
	    if(i<0 || 2<i)
	    {
		cerr<<"Vertex number out of range: "<<i+1<<endl;
		static point dummy(std::sqrt(-1.0),std::sqrt(-1.0), std::sqrt(-1.0));
		return dummy;
	    }
	    return vertex_[i];
	}
    
	triangle &triangle::vertex(int i, const point &v)
	{
	    --i;
	    if(i<0 || 2<i) cerr<<"Vertex number out of range: "<<i+1<<endl;
	    else vertex_[i] = v;
	    return *this;
	}
    
	// For each edge, the point 'v' should be on the same side of the edge,
	// as the third vertex. 
	bool triangle::inside(const point &v)
	{
	    for(int i=0; i<3; ++i)
	    {
		const point &v1 = vertex_[i];
		const point &v2 = (i<2?vertex_[i+1]:vertex_[0]);
		const point &v3 = (i==0?vertex_[2]:(i==1?vertex_[0]:vertex_[1]));
		if( ((v-v1)%(v2-v1))*((v3-v1)%(v2-v1)) < 0 ) return false;
	    }
	    return true;
	}
	

	// --------------------  transformations and necessary helper functions -----------------

        void rotation::init_(const vec3 &v, double theta)
        {
            axis_ = v/abs(v);
            angle_ = theta;
            matrix_[0][0] = std::cos(theta)+axis_.x*axis_.x*(1-std::cos(theta));
            matrix_[0][1] = axis_.x*axis_.y*(1-std::cos(theta))-axis_.z*std::sin(theta);
            matrix_[0][2] = axis_.x*axis_.z*(1-std::cos(theta))+axis_.y*std::sin(theta);
            matrix_[0][3] = 0;

            matrix_[1][0] = axis_.y*axis_.x*(1-std::cos(theta))+axis_.z*std::sin(theta);
            matrix_[1][1] = std::cos(theta)+axis_.y*axis_.y*(1-std::cos(theta));
            matrix_[1][2] = axis_.y*axis_.z*(1-std::cos(theta))-axis_.x*std::sin(theta);
            matrix_[1][3] = 0;

            matrix_[2][0] = axis_.z*axis_.x*(1-std::cos(theta))-axis_.y*std::sin(theta);
            matrix_[2][1] = axis_.z*axis_.y*(1-std::cos(theta))+axis_.x*std::sin(theta);
            matrix_[2][2] = std::cos(theta)+axis_.z*axis_.z*(1-std::cos(theta));
            matrix_[2][3] = 0;

            matrix_[3][0] = 0;
            matrix_[3][1] = 0;
            matrix_[3][2] = 0;
            matrix_[3][3] = 1;
        }

        rotation::rotation(const rotation &rhs) : transformation(rhs)
        {
            angle_ = rhs.angle_;
            axis_ = rhs.axis_;
        }

	rotation::rotation(vec3 v, vec3 vprime)
	{
	    v /= abs(v);
	    vprime /= abs(vprime);
	    vec3 a = v%vprime;
	    const double sin_angle = abs(a);
	    const double cos_angle = v*vprime;
	    double angle = atan2(sin_angle,cos_angle);
	    if(angle<0) cerr<<"This should never happen"<<endl;
	    a /= abs(a);

	    // If it is an almost-180 deg rotation, the axis can not be determined unambigously.
	    // Choose just one axis
	    if(sin_angle<1e-4)
	    {
		//cerr<<"Warning: Axis of rotation can not be determined unambiguously"<<endl;
		a = v%vec3(1,0,0);
		vec3 t = v%vec3(0,1,0);
		if(abs(t)>abs(a)) a = t;
		t = v%vec3(0,0,1);
		if(abs(t)>abs(a)) a = t;
		a /= abs(a);
		if(angle<3.1415/2) angle = 0;
		else angle = 3.14159265;
	    }
	    init_(a,angle);
	}

	rotation::rotation(vec3 v1, vec3 v2, vec3 v1prime, vec3 v2prime)
	{
	    if(v1==v1prime && v2==v2prime)
	    {
		init_(vec3(0,0,1),0);
		return;
	    }

	    v1 /= abs(v1);
	    v2 /= abs(v2);
	    v1prime /= abs(v1prime);
	    v2prime /= abs(v2prime);

            if(fabs(v1*v2-v1prime*v2prime)>1e-3)
            {
                warning::print(var("Angle between v1(") & v1.x & "," & v1.y & "," & v1.z & "),v2(" & v2.x & "," & v2.y & "," & v2.z & ") is not the same as v1prime("
                               & v1prime.x & "," & v1prime.y & "," & v1prime.z & "),v2prime(" & v2prime.x & "," & v2prime.y & "," & v2prime.z & ")",
                               "rotation::rotation(vec3 v1, vec3 v2, vec3 v1prime, vec3 v2prime)");
            }

	    if(abs(v2%v2prime)>abs(v1%v1prime))
	    {
		std::swap(v1,v2);
		std::swap(v1prime,v2prime);
	    }

	    // The first rotation is a rotation around the axis a1, which brings v1 into v1'
	    // a1 being perpendicular to both of these vectors
	    rotation r1(v1,v1prime);

	    if(v2 == v2prime)
	    {
		*this = r1;
		return;
	    }

	    // First, apply this rotation to v2
	    v2 = r1*v2;

	    // And then search for a rotation which brings it (i.e. the rotated v2) into v2prime,
	    // leaving v1prime unaffected - that is, a rotation around v1prime
	    vec3 a2 = v1prime;

	    // now v2 should be rotatable into v2' around a2. That is, they need to have the same angle
	    // w.r.t. a2. Check this now!
	    if(std::fabs(v2*a2-v2prime*a2)>1e-6)
	    {
                warning::print("Angles between vectors do not satisfy constraints in rotation(vec3 v1, vec3 v2, vec3 v1prime, vec3 v2prime)",
                               "rotation::rotation(vec3 v1, vec3 v2, vec3 v1prime, vec3 v2prime)");
	    }
	    // get the normalized perpendicular components of v2 and v2' to a2
	    vec3 k2 = v2-a2*(a2*v2);
	    vec3 k2prime = v2prime-a2*(a2*v2prime);

	    // They had no perpendicular component to the second rotation axis, so we need no more rotations
	    if(abs(k2)<1e-20 && abs(k2prime)<1e-20)
	    {
		*this = r1;
		return;
	    }

	    k2 /= abs(k2);
	    k2prime /= abs(k2prime);

	    vec3 A2 = k2%k2prime;  // the magnitude of this vector is sin(angle2);

	    const double sin2 = abs(A2);
	    const double cos2 = k2*k2prime;
	    double angle2 = std::atan2(sin2,cos2);

	    // the two vectors are back-to-back or in the same direction. 
	    if(sin2<1e-4)
	    {
		if(angle2<3.1415/2) angle2=0;
		else angle2 = 3.14159265;
	    }
	    else
	    {
		A2 /= abs(A2);
		if(std::fabs(std::fabs(a2*A2)-1)>1e-6) std::cerr<<"Error: some problem with angles in rotation(vec3,vec3,vec3,vec3)"<<std::endl; // should never happen
		a2 = A2;
	    }
	    rotation r2(a2,angle2);
	    *this = r2*r1;
	}

        rotation transformation::get_rotation() const
        {
            rotation result;
            for(int i=0; i<3; ++i)
            {
                for(int j=0; j<3; ++j)
                {
                    result.matrix_[i][j] = matrix_[i][j];
                }
            }
            result.set_axis_angle_from_matrix_();
            return result;
        }

        void transformation::remove_translation()
        {
            for(int i=0; i<3; ++i)
            {
                matrix_[i][3] = 0;
                matrix_[3][i] = 0;
            }
        }

        void transformation::invert()
        {
            double tmpmatrix[4][4];
            for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) tmpmatrix[i][j] = matrix_[i][j];

	    const double det =
		+ matrix_[0][0]*(matrix_[1][1]*matrix_[2][2]-matrix_[1][2]*matrix_[2][1])
		+ matrix_[0][1]*(matrix_[1][2]*matrix_[2][0]-matrix_[1][0]*matrix_[2][2])
		+ matrix_[0][2]*(matrix_[1][0]*matrix_[2][1]-matrix_[1][1]*matrix_[2][0]);

	    for(int i=0; i<4; ++i)
	    {
		for(int j=0; j<4; ++j)
		{
		    matrix_[j][i] = cofactor(tmpmatrix,i,j)/det;
		}
	    }
        }

	transformation transformation::inverse() const
	{
            transformation result(*this);
            result.invert();
            return result;
	}

	ostream &operator<<(ostream &out, const transformation &t)
	{
	    for(int i=0; i<4; ++i)
	    {
		for(int j=0; j<4; ++j)
		{
		    out<<std::setw(13)<<t.matrix_[i][j]<<" ";
		}
		out<<endl;
	    }
	    return out;
	}

        transformation rotation_around_axis(const line &axis, double angle)
        {
            return translation(axis.origin().coord)*rotation(axis.dir(),angle)*translation(-axis.origin().coord);
        }


        void mirroring::init_(const point &P0, const vec3 &normal)
        {
            p0_=P0;
            normal_=normal;
            double n[3]  = { normal.x, normal.y, normal.z };
            double p0[3] = { P0.coord.x, P0.coord.y, P0.coord.z };
            for(int i=0; i<3; ++i)
            {
                for(int j=0; j<3; ++j)
                {
                    matrix_[i][j] = ((i==j)-2*n[i]*n[j]);
                }
                matrix_[i][3] = 2*n[i] * (p0[0]*n[0] + p0[1]*n[1] + p0[2]*n[2]);
            }
            for(int j=0; j<4; ++j) matrix_[3][j] = (j==3);
        }
    }

    geom::vec3 project(const geom::vec3 &v, const geom::plane &pl)
    {
        return v - pl.normal()*(pl.normal()*v);
    }

    geom::vec3 project(const geom::vec3 &v, const geom::line &l)
    {
        return l.dir()*(l.dir()*v);
    }

    geom::line project(const geom::line &l, const geom::plane &pl)
    {
        return blop::geom::line(project(l.p0(),pl), project(l.dir(),pl));
    }

    geom::point project(const geom::point &p, const geom::plane &pl)
    {
        const geom::vec3 diff = p-pl.origin();
        const geom::vec3 diff_in_plane = diff - pl.normal()*(pl.normal()*diff);
        return pl.origin()+diff_in_plane;
    }

    geom::point project(const geom::point &p, const geom::line &l)
    {
        const geom::vec3 diff = p-l.origin();
        return l.origin()+l.dir()*(l.dir()*diff);
    }

    int intersection(const std::vector<double> &X1, const std::vector<double> &Y1,
		     const std::vector<double> &X2, const std::vector<double> &Y2,
		     std::vector<double> &x, std::vector<double> &y)
    {
	x.clear();
	y.clear();
	// Loop over all segment pairs of the two polylines. A segment of the first curve is described by:
	// (x1,y1)   - starting point of the segment
	// (dx1,dy1) - vector pointing from start to the end
	// similarly for the second line. Any point on this segment can be parametrized by the t1 parameter:
	// (x,y) = (x1,y1)+t1*(dx1,dy1)
	// The point (x,y) falls within the segment if 0<=t1<=1
	// The equation for the intersection point:
	// (x1,y1)+t1*(dx1,dy1) = (x2,y2)+t2*(dx2,dy2)    and 0<=t1<=1 && 0<=t2<=1
	// Solving these equations:
	// t1 = (dx2*(y2-y1)+dy2*(x1-x2))/(dy1*dx2-dy2*dx1)
	// t2 = (dx1*(y2-y1)+dy1*(x1-x2))/(dy1*dx2-dy2*dx1)

	// loop over all segment pairs
        const unsigned int N1 = ::min(X1.size(),Y1.size());
        const unsigned int N2 = ::min(X2.size(),Y2.size());
	for(unsigned int i=1; i<N1; ++i)
	{
	    if(X1[i-1]==unset || X1[i]==unset || Y1[i-1]==unset || Y1[i]==unset) continue;
	    const double x1 = X1[i-1];
	    const double y1 = Y1[i-1];
	    const double dx1 = X1[i]-X1[i-1];
	    const double dy1 = Y1[i]-Y1[i-1];
	    for(unsigned int j=1; j<N2; ++j)
	    {
		if(X2[j-1]==unset || X2[j]==unset || Y2[j-1]==unset || Y2[j]==unset) continue;
		const double x2 = X2[j-1];
		const double y2 = Y2[j-1];
		const double dx2 = X2[j]-X2[j-1];
		const double dy2 = Y2[j]-Y2[j-1];
		const double t1 = (dx2*(y2-y1)+dy2*(x1-x2))/(dy1*dx2-dy2*dx1);
		const double t2 = (dx1*(y2-y1)+dy1*(x1-x2))/(dy1*dx2-dy2*dx1);

		if(0<=t1 && t1<=1 && 0<=t2 && t2<=1)
		{
		    // the intersection point's coordinates
		    const double int_x = x1+t1*dx1;
		    const double int_y = y1+t1*dy1;
		    
		    // check if this point has already been found. what about finite precision???
		    bool found = false;
		    for(unsigned int f=0; f<x.size(); ++f)
		    {
			if(x[f]==int_x && y[f]==int_y)
			{
			    found = true;
			    break;
			}
		    }
		    
		    if(!found)
		    {
			x.push_back(int_x);
			y.push_back(int_y);
		    }
		}
	    }
	}
	return (int)x.size();
    }


    

}
