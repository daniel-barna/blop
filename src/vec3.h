#ifndef __BLOP_VEC3_H__
#define __BLOP_VEC3_H__

#include <iostream>
#include <cmath>

namespace blop
{
    namespace geom
    {
        class transformation;
        class rotation;

	class vec3
	{
	public:
	    double x,y,z;

            // References to x,y,z
            double &x1, &x2, &x3;


	    vec3 ()            : x(0),y(0),z(0),x1(x),x2(y),x3(z) {}
	    vec3 (double X, double Y, double Z) : x(X),y(Y),z(Z),x1(x),x2(y),x3(z) {}

            // Create a vector with the given components, normalized to 'norm'
            vec3 (double X, double Y, double Z, double norm) : x1(x), x2(y), x3(z)
            {
                const double d = std::sqrt(X*X+Y*Y+Z*Z);
                x = X/d*norm;
                y = Y/d*norm;
                z = Z/d*norm;
            }

	    vec3 (const vec3 &v) : x(v.x),y(v.y),z(v.z),x1(x),x2(y),x3(z) {}

            // Set the vector to v, and normalize to 'norm', i.e. *this = v/abs(v)*norm;
            vec3 (const vec3 &v, double norm) : x(v.x), y(v.y), z(v.z),x1(x),x2(y),x3(z) { normalize(norm); }

	    inline const vec3& operator= (const vec3 &rhs)
		{
		    x = rhs.x;
		    y = rhs.y;
		    z = rhs.z;
		    return *this;
		}

	    inline void     assign (double X,double Y,double Z) {x = X; y = Y; z = Z; } 

	    // set the vectorcomponents to the given values

	    inline void  operator/= (double r)  {x /= r; y /= r; z /= r;}
	    inline void  operator/= (int r)  {x /= r; y /= r; z /= r;}
	    inline void  operator/= (short r)  {x /= r; y /= r; z /= r;}
	    inline void  operator/= (unsigned int r)  {x /= r; y /= r; z /= r;}
	    inline void  operator*= (double r)  {x *= r; y *= r; z *= r;}
	    inline void  operator*= (int r)  {x *= r; y *= r; z *= r;}
	    inline void  operator*= (short r)  {x *= r; y *= r; z *= r;}
	    inline void  operator*= (unsigned int r)  {x *= r; y *= r; z *= r;}

	    inline void  operator+= (const vec3 &v)
	    {
		x += v.x; 
		y += v.y;
		z += v.z;
	    }
	    inline void  operator-= (const vec3 &v)
	    {
		x -= v.x; 
		y -= v.y;
		z -= v.z;
	    }

	    inline vec3 &normalize(double length=1)
	    {
		(*this) *= length/std::sqrt(x*x + y*y + z*z);
                return *this;
	    } 

	    // a vec3 converts to true if all 3 components are finite
	    operator bool() const
	    {
		return finite(x) && finite(y) && finite(z);
	    }

            const vec3 &operator*= (const rotation &);
	};

        inline vec3 normalize(vec3 v, double norm = 1) { return v.normalize(norm); }

	inline bool operator== (const vec3 &v1, const vec3 &v2)
	{
	    return (v1.x==v2.x && v1.y==v2.y && v1.z==v2.z);
	}
	inline bool operator!= (const vec3 &v1, const vec3 &v2)
	{
	    return !(v1==v2);
	}
	inline bool operator< (const vec3 &v1, const vec3 &v2)
	{
	    if(v1.x<v2.x) return true;
	    if(v1.x>v2.x) return false;
	    if(v1.y<v2.y) return true;
	    if(v1.y>v2.y) return false;
	    if(v1.z<v2.z) return true;
	    if(v1.z>v2.z) return false;
            return false;
	}
	inline bool operator> (const vec3 &v1, const vec3 &v2)
	{
	    if(v1.x>v2.x) return true;
	    if(v1.x<v2.x) return false;
	    if(v1.y>v2.y) return true;
	    if(v1.y<v2.y) return false;
	    if(v1.z>v2.z) return true;
	    if(v1.z<v2.z) return false;
            return false;
	}

	inline bool operator<= (const vec3 &v1, const vec3 &v2)
	{
	    return !(v1>v2);
	}

	inline bool operator>= (const vec3 &v1, const vec3 &v2)
	{
	    return !(v1<v2);
	}

	inline double operator* (const vec3 &v1,const vec3 &v2)
	{
	    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}

	inline double abs(const vec3 &v)
	{
	    return std::sqrt(v*v);
	}

	inline vec3  operator* (const vec3 &v,double factor)
	{
	    return vec3(v.x*factor,v.y*factor,v.z*factor);
	}
	inline vec3  operator* (const vec3 &v,int factor)
	{
	    return vec3(v.x*factor,v.y*factor,v.z*factor);
	}

	inline vec3  operator* (double factor,const vec3 &v)
	{
	    return vec3(v.x*factor,v.y*factor,v.z*factor);
	}
	inline vec3  operator* (int factor,const vec3 &v)
	{
	    return vec3(v.x*factor,v.y*factor,v.z*factor);
	}

	inline vec3 operator/ (const vec3 &v,double d)
	{
	    return vec3(v.x/d,v.y/d,v.z/d);
	}
	inline vec3 operator/ (const vec3 &v,int d)
	{
	    return vec3(v.x/d,v.y/d,v.z/d);
	}

	inline vec3  operator+ (const vec3 &v1,
				 const vec3 &v2)
	{
	    return vec3(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);
	}

	inline vec3  operator- (const vec3 &v1,
				 const vec3 &v2)
	{
	    return vec3(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);
	}

	inline vec3 operator- (const vec3 &v)
	{
	    return (-1.0) * v;
	}

        // vectorial product of two vectors. chosing the % operator for this role is 
        // maybe a surprise, but this operator has the multiplicative precedence.

	inline vec3 operator% (const vec3 &v1,
				const vec3 &v2)
	{
	    return vec3(v1.y*v2.z-v1.z*v2.y,
			v1.z*v2.x-v1.x*v2.z,
			v1.x*v2.y-v1.y*v2.x);
	}

	inline std::ostream &operator<<(std::ostream &output,const vec3 &v)
	{
	    output<<v.x<<" "<<v.y<<" "<<v.z;
	    return output;
	}


    }
}


#endif
