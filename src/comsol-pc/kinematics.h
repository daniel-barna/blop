#ifndef __BLOP_KINEMATICS_H__
#define __BLOP_KINEMATICS_H__
#include "units.h"
#include "function.h"

namespace blop
{
    namespace phys
    {
        /*
          A utility class to quickly convert between relativistic velocities, energies and momenta.
          Example: calculate the momentum of an antiproton with Ekin=100 keV 

          double p = kin(938*unit::MeVc2).Ekin(100*unit::keV).p();

          The constructor sets the mass of the particle. Each of the other member functions
          Ekin(double), v(double), etc reset all other kinematic variables to 0.
          The 'getting' funtions (Ekin(), p(), etc) calculate this quantity from that other one,
          which has been set before.

          Right now, setting the mass (via the m(double) member function) resets all
          other variables to 0. In the future the mass will be also calculable from any other
          2 variables
         */

        class kin
        {
        private:
            double m_, Ekin_, E_, p_, v_;
        public:
            kin(double m=0) : m_(m), Ekin_(0), E_(0), p_(0), v_(0) {}
            kin &m   (double a) { m_=a; Ekin_=E_=p_=v_=0; return *this; }

            kin &Ekin(double a)
            {
                Ekin_= a;
                E_   = Ekin_+m_*cons::c*cons::c;
                p_   = ::sqrt(::pow(E_,2)-::pow(m_*cons::c*cons::c,2))/cons::c;
                v_   = cons::c*::sqrt(1-::pow(m_*cons::c*cons::c/E_,2));
                return *this;
            }
            kin &E   (double a)
            {
                E_    = a;
                Ekin_ = E_-m_*cons::c*cons::c;
                p_    = ::sqrt(::pow(E_,2)-::pow(m_*cons::c*cons::c,2))/cons::c;
                v_    = cons::c*::sqrt(1-::pow(m_*cons::c*cons::c/E_,2));
                return *this;
            }
            kin &p   (double a)
            {
                p_    = a;
                v_    = p_/::sqrt(m_*m_+::pow(p_/cons::c,2));
                E_    = ::sqrt(::pow(p_*cons::c,2)+::pow(m_*cons::c*cons::c,2));
                Ekin_ = E_-m_*cons::c*cons::c;
                return *this;
            }
            kin &v   (double a)
            {
                v_    = a;
                p_    = m_*v_/::sqrt(1-::pow(v_/cons::c,2));
                E_    = ::sqrt(m_*m_*v_*v_*cons::c*cons::c/(1-::pow(v_/cons::c,2))+::pow(m_*cons::c*cons::c,2));
                Ekin_ = E_-m_*cons::c*cons::c;
                return *this;
            }

            inline double v()    const {return v_;}
            inline double beta() const {return v_/cons::c; }
            inline double gamma()const {return 1/::sqrt(1-beta()*beta()); }
            inline double p()    const {return p_;}
            inline double Ekin() const {return Ekin_;}
            inline double E()    const {return E_;}

        };

        class kin_func
        {
        private:
            double m_;
            function Ekin_, E_, p_, v_;
        public:
            kin_func(double m=0) : m_(m), Ekin_(0), E_(0), p_(0), v_(0) {}
            kin_func &m   (double a) { m_=a; Ekin_=E_=p_=v_=0.0; return *this; }

            kin_func &Ekin(const function &a)
            {
                Ekin_= a;
                E_   = Ekin_+m_*cons::c*cons::c;
                p_   = blop::sqrt(blop::pow(E_,2)-::pow(m_*cons::c*cons::c,2))/cons::c;
                v_   = cons::c*blop::sqrt(1-blop::pow(m_*cons::c*cons::c/E_,2));
                return *this;
            }
            kin_func &E   (const function &a)
            {
                E_    = a;
                Ekin_ = E_-m_*cons::c*cons::c;
                p_    = blop::sqrt(blop::pow(E_,2)-::pow(m_*cons::c*cons::c,2))/cons::c;
                v_    = cons::c*blop::sqrt(1-blop::pow(m_*cons::c*cons::c/E_,2));
                return *this;
            }
            kin_func &p   (const function &a)
            {
                p_    = a;
                v_    = p_/blop::sqrt(m_*m_+blop::pow(p_/cons::c,2));
                E_    = blop::sqrt(blop::pow(p_*cons::c,2)+::pow(m_*cons::c*cons::c,2));
                Ekin_ = E_-m_*cons::c*cons::c;
                return *this;
            }
            kin_func &v   (const function &a)
            {
                v_    = a;
                p_    = m_*v_/blop::sqrt(1-blop::pow(v_/cons::c,2));
                E_    = blop::sqrt(m_*m_*v_*v_*cons::c*cons::c/(1-blop::pow(v_/cons::c,2))+::pow(m_*cons::c*cons::c,2));
                Ekin_ = E_-m_*cons::c*cons::c;
                return *this;
            }

            inline function v()    const {return v_;}
            inline function beta() const {return v_/cons::c; }
            inline function gamma()const {return 1/blop::sqrt(1-beta()*beta()); }
            inline function p()    const {return p_;}
            inline function Ekin() const {return Ekin_;}
            inline function E()    const {return E_;}

        };

    }
}


#endif
