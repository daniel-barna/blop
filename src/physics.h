#ifndef __BLOP_PHYSICS_H__
#define __BLOP_PHYSICS_H__

#include "vec3.h"
#include <cmath>
#include "units.h"

namespace blop
{
    namespace phys
    {
        class vec4 : public geom::vec3
        {
        public:
            double x0;        // Synonime for ct (0-th component)
            double &ct;       // The time-component of the
            double &E_over_c; // 
            geom::vec3   &x;  // Get the spatial component of this 4-vector as a vec3
            
            vec4()                               : x0(0),  ct(x0), E_over_c(x0),                x(*this) {}
            vec4(const geom::vec3 &v)            : x0(0),  ct(x0), E_over_c(x0), geom::vec3(v), x(*this) {}
            vec4(double X0, const geom::vec3 &v) : x0(X0), ct(x0), E_over_c(x0), geom::vec3(v), x(*this) {}
            vec4(double X0, double X1, double X2, double X3) : x0(X0), ct(x0), E_over_c(x0), geom::vec3(X1,X2,X3), x(*this) {}
            vec4(const vec4 &rhs)                : x0(rhs.x0), ct(x0), E_over_c(x0), geom::vec3(rhs), x(*this) {}
            const vec4 operator=(const vec4 &rhs)
            {
                x0 = rhs.x0;
                geom::vec3::operator=(rhs);
                return rhs;
            }

            double abs() const
            {
                return ::sqrt(x0*x0-x1*x1-x2*x2-x3*x3);
            }

            // Assume this is a 4-momentum vector, return the kinetic energy
            double Ekin() const
            {
                const double mc = ::sqrt(E_over_c*E_over_c-x*x);
                return E_over_c*cons::c-mc*cons::c;
            }

            // Assume this is a 4-momentum vector, return the mass
            double mass() const
            {
                return ::sqrt(E_over_c*E_over_c-x*x)/cons::c;
            }

            vec4 operator/(double f)
            {
                return vec4(x0/f,x1/f,x2/f,x3/f);
            }
        };

        inline ostream &operator<<(ostream &out, const vec4 &v)
        {
            out<<v.x0<<" "<<v.x1<<" "<<v.x2<<" "<<v.x3;
            return out;
        }

        inline double operator* (const vec4 &v1, const vec4 &v2)
        {
            return v1.x0*v2.x0 - v1.x1*v2.x1 - v1.x2*v2.x2 - v1.x3*v2.x3;
        }
        inline double abs(const vec4 &v)
        {
            return v.abs();
        }
        // Create a four-momentum from a mass and 3-momentum
        inline vec4 four_momentum(double mass, const geom::vec3 &p3)
        {
            // E/c (the 0th component of the 4-momentum) = sqrt(m^2 c^2 + p^2)
            return vec4(::sqrt((mass*cons::c)*(mass*cons::c)+p3*p3), p3);
        }
        inline vec4 four_momentum(double mass, double px, double py, double pz)
        {
            return four_momentum(mass,geom::vec3(px,py,pz));
        }

        

        class boost
        {
        private:
            geom::vec3   v_;
            double gamma_;
            friend vec4 operator* (const boost &, const vec4 &);
        public:
            boost() : v_(0,0,0), gamma_(1) {}
            boost(const geom::vec3 &v) : v_(v), gamma_(1/std::sqrt(1.0-(v*v)/(cons::c*cons::c))) {}
            boost(double vx, double vy, double vz) : v_(vx,vy,vz), gamma_(1/std::sqrt(1.0-(vx*vx+vy*vy+vz*vz)/(cons::c*cons::c))) {}
        };

        inline vec4 operator* (const boost &b, const vec4 &r)
        {
            if(b.v_.x==0.0 && b.v_.y==0.0 && b.v_.z==0.0) return r;
            // boost-direction normalized to 1
            const geom::vec3 boost_dir(b.v_,1.0);
            const geom::vec3 r_parallel = (r.x*boost_dir)*boost_dir;
            const geom::vec3 r_perpendicular = r.x-r_parallel;
            return vec4(b.gamma_*(r.ct-r.x*b.v_/cons::c),
                        r_perpendicular + b.gamma_*(r_parallel-b.v_*r.ct/cons::c));
        }

        // Return the momentum of the decay products (of masses m1 and m2) of a particle (of mass M)
        // in the rest-frame of the parent
        inline double two_body_decay_p(double M, double m1, double m2)
        {
            return ::sqrt(::pow(M*M-m1*m1-m2*m2,2)-4*m1*m1*m2*m2)*cons::c/(2*M);
        }

//        inline double invariant_mass(const vec4 &p1, const vec4 &p2)
//        {
//        }

    }
}

#endif
