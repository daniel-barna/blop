#include "meas.h"

namespace blop
{


// Class to store measured values with errors. Knows the Gaussian error 
// propagation. Knows the error of enties in case of a histogram 
// (error = square root of entries, due to Poisson distribution).

meas::meas()
 : v(0.0), e(0.0)
{
}


meas::meas(const double V)
 : v(V), e(0.0)
{
}


meas::meas(const double V, const double E)
 : v(V), e(E)
{
}


meas::meas(const meas &m_other)
 : v(m_other.v), e(m_other.e)
{
}


meas::~meas()
{
}


void meas::fill()
{
    if ( isok() )
    {
	e=::sqrt(e*e+1.0);
	++v;
    }
}


void meas::fill(const double weight)
{
    if ( isok() && std::isfinite(weight) && weight!=unset )
    {
	e=::sqrt(e*e+weight*weight);
	v+=weight;
    }
}


void meas::fill(const var &weight)
{
    if ( isok() && std::isfinite(weight.dbl()) && weight.dbl()!=unset )
    {
	e=::sqrt(e*e+weight.dbl()*weight.dbl());
	v+=weight.dbl();
    }
}


void meas::remove()
{
    if ( isok() )
    {
	e=e*e-1.0;
	e=(e<0.0 ? 0.0 : ::sqrt(e));
	--v;
	v=(v<0.0 ? 0.0 : v);
    }
}


void meas::remove(const double weight)
{
    if ( isok() && std::isfinite(weight) && weight!=unset )
    {
	e=e*e-weight*weight;
	e=(e<0.0 ? 0.0 : ::sqrt(e));
	v-=weight;
	v=(v<0.0 ? 0.0 : v);
    }
}


void meas::remove(const var &weight)
{
    if ( isok() && std::isfinite(weight.dbl()) && weight.dbl()!=unset )
    {
	e=e*e-weight.dbl()*weight.dbl();
	e=(e<0.0 ? 0.0 : ::sqrt(e));
	v-=weight.dbl();
	v=(v<0.0 ? 0.0 : v);
    }
}


const meas& meas::operator=(const meas &m_other)
{
    e=m_other.e;
    v=m_other.v;
    return *this;
}


const meas& meas::operator=(const double V)
{
    e=0.0;
    v=V;
    return *this;
}


const meas& meas::operator+=(const meas &m_other)
{
    if ( isok() && m_other.isok() )
    {
	e=::sqrt(e*e+m_other.e*m_other.e);
	v+=m_other.v;
    }
    else
    {
	e=unset;
	v=unset;
    }
    return *this;
}


const meas& meas::operator+=(const double V)
{
    if ( isok() && std::isfinite(V) && V!=unset )
    {
	v+=V;
    }
    else
    {
	e=unset;
	v=unset;
    }
    return *this;
}


const meas& meas::operator-=(const meas &m_other)
{
    if ( isok() && m_other.isok() )
    {
	e=::sqrt(e*e+m_other.e*m_other.e);
	v-=m_other.v;
    }
    else
    {
	e=unset;
	v=unset;
    }
    return *this;
}


const meas& meas::operator-=(const double V)
{
    if ( isok() && std::isfinite(V) && V!=unset )
    {
	v-=V;
    }
    else
    {
	e=unset;
	v=unset;
    }
    return *this;
}


const meas& meas::operator*=(const meas &m_other)
{
    if ( isok() && m_other.isok() )
    {
	e=::sqrt(m_other.v*m_other.v*e*e+v*v*m_other.e*m_other.e);
	v*=m_other.v;
    }
    else
    {
	e=unset;
	v=unset;
    }
    return *this;
}


const meas& meas::operator*=(const double V)
{
    if ( isok() && std::isfinite(V) && V!=unset )
    {
	e*=::fabs(V);
	v*=V;
    }
    else
    {
	e=unset;
	v=unset;
    }
    return *this;
}


const meas& meas::operator/=(const meas &m_other)
{
    if ( isok() && m_other.isok() )
    {
	e=::sqrt(e*e/(m_other.v*m_other.v)+v*v/(m_other.v*m_other.v*m_other.v*m_other.v)*m_other.e*m_other.e);
	v/=m_other.v;
    }
    else
    {
	e=unset;
	v=unset;
    }
    return *this;
}


const meas& meas::operator/=(const double V)
{
    if ( isok() && std::isfinite(V) && V!=unset )
    {
	e/=::fabs(V);
	v/=V;
    }
    else
    {
	e=unset;
	v=unset;
    }
    return *this;
}


const meas& meas::operator++()
{
    if ( isok() )
    {
	++v;
    }
    return *this;
}


const meas& meas::operator--()
{
    if ( isok() )
    {
	--v;
    }
    return *this;
}


bool meas::operator==(const meas &m) const
{
    return (v==m.v && e==m.e ? true : false);
}


bool meas::operator==(const double d) const
{
    return (v==d && e==0.0 ? true : false);
}


bool meas::operator!=(const meas &m) const
{
    return (v!=m.v || e!=m.e ? true : false);
}


bool meas::operator!=(const double d) const
{
    return (v!=d || e!=0.0 ? true : false);
}


bool meas::operator<(const meas &m) const
{
    return (v<m.v ? true : false);
}


bool meas::operator<(const double d) const
{
    return (v<d ? true : false);
}


bool meas::operator>(const meas &m) const
{
    return (v>m.v ? true : false);
}


bool meas::operator>(const double d) const
{
    return (v>d ? true : false);
}


bool meas::operator<=(const meas &m) const
{
    return (v<=m.v ? true : false);
}


bool meas::operator<=(const double d) const
{
    return (v<=d ? true : false);
}


bool meas::operator>=(const meas &m) const
{
    return (v>=m.v ? true : false);
}


bool meas::operator>=(const double d) const
{
    return (v>=d ? true : false);
}


meas operator+(const meas &m, const meas &m_other)
{ 
    if ( m.isok() && m_other.isok() )
    {
	return meas(m.v+m_other.v, ::sqrt(m.e*m.e+m_other.e*m_other.e));
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator+(const meas &m, const double V)
{
    if ( m.isok() && std::isfinite(V) && V!=unset )
    {
	return meas(m.v+V, m.e);
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator+(const double V, const meas &m)
{
    if ( m.isok() && std::isfinite(V) && V!=unset )
    {
	return meas(V+m.v, m.e);
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator-(const meas &m, const meas &m_other)
{
    if ( m.isok() && m_other.isok() )
    {
	return meas(m.v-m_other.v, ::sqrt(m.e*m.e+m_other.e*m_other.e));
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator-(const meas &m, const double V)
{
    if ( m.isok() && std::isfinite(V) && V!=unset )
    {
	return meas(m.v-V, m.e);
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator-(const double V, const meas &m)
{
    if ( m.isok() && std::isfinite(V) && V!=unset )
    {
	return meas(V-m.v, m.e);
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator-(const meas &m)
{
    if ( m.isok() )
    {
	return meas(-m.v, m.e);
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator*(const meas &m, const meas &m_other)
{
    if ( m.isok() && m_other.isok() )
    {
	return meas(m.v*m_other.v, ::sqrt(m_other.v*m_other.v*m.e*m.e+m.v*m.v*m_other.e*m_other.e));
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator*(const meas &m, const double V)
{
    if ( m.isok() && std::isfinite(V) && V!=unset )
    {
	return meas(m.v*V, ::fabs(m.e*V));
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator*(const double V, const meas &m)
{
    if ( m.isok() && std::isfinite(V) && V!=unset )
    {
	return meas(V*m.v, ::fabs(V*m.e));
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator/(const meas &m, const meas &m_other)
{
    if ( m.isok() && m_other.isok() )
    {
	return meas(m.v/m_other.v, ::sqrt(m.e*m.e/(m_other.v*m_other.v)+m.v*m.v/(m_other.v*m_other.v*m_other.v*m_other.v)*m_other.e*m_other.e));
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator/(const meas &m, const double V)
{
    if ( m.isok() && std::isfinite(V) && V!=unset )
    {
	return meas(m.v/V, ::fabs(m.e/V));
    }
    else
    {
	return meas(unset, unset);
    }
}


meas operator/(const double V, const meas &m)
{
    if ( m.isok() && std::isfinite(V) && V!=unset )
    {
	return meas(V/m.v, ::fabs(V*m.e/(m.v*m.v)));
    }
    else
    {
	return meas(unset, unset);
    }
}


bool meas::isfinite() const
{
    return (std::isfinite(v) && std::isfinite(e) ? true : false);
}


bool meas::isinf() const
{
    return (std::isinf(v) || std::isinf(e) ? true : false);
}


bool meas::isnan() const
{
    return (std::isnan(v) || std::isnan(e) ? true : false);
}


bool meas::isset() const
{
    return (v!=unset && e!=unset ? true : false);
}


bool meas::isunset() const
{
    return (v==unset || e==unset ? true : false);
}


bool meas::isok() const
{
    return (std::isfinite(v) && std::isfinite(e) && v!=unset && e!=unset ? true : false);
}


bool meas::isfinite(const meas &m)
{
    return m.isfinite();
}


bool meas::isinf(const meas &m)
{
    return m.isinf();
}


bool meas::isnan(const meas &m)
{
    return m.isnan();
}


bool meas::isset(const meas &m)
{
    return m.isset();
}


bool meas::isunset(const meas &m)
{
    return m.isunset();
}


bool meas::isok(const meas &m)
{
    return m.isok();
}


var meas::to_var(const var &format) const
{
    var result = (v==unset ? var("unset") : var(v,format));
    result    &= " +/- ";
    result    &= (e==unset ? var("unset") : var(e,format));
    return result;
}

/*
meas &meas::transform(const function &f)
{
    if ( isok() && f.initialized() )
    {
	var mv=v;
	v = f(mv).dbl();
	e = ::fabs(f.derivative(1)(mv).dbl()*e);
    }
    else
    {
	v=unset;
	e=unset;
    }
    return *this;
}

meas apply(const function &f, const meas &m)
{
    if ( m.isok() && f.initialized() )
    {
	var mv=m.v;
	return meas(f(mv).dbl(), ::fabs(f.derivative(1)(mv).dbl()*m.e));
    }
    else
    {
	return meas(unset, unset);
    }
}
*/

std::ostream& operator<<(std::ostream &out, const meas &m)
{
//    out<<m.to_var(var("%.")&out.precision()&var("f"));
    out<<m.to_var(var("%.")&(int)out.precision()&var("f")); 
    return out;
}

std::istream& operator>>(std::istream &in, meas &m)
{
    std::string s;
    std::istringstream iss;
    static double zero=0.0;
    static double minusone=-1.0;
    if ( !(in>>s) ) return in;
    iss.str(s);
    if ( !(iss>>m.v) )
    {
	if ( s=="inf" ) m.v=1.0/zero;
	else if ( s=="-inf" ) m.v=-1.0/zero;
	else if ( s=="nan" ) m.v=::sqrt(minusone);
	else if ( s=="unset" ) m.v=unset;
	else { in.setstate(std::istream::failbit); return in; }
    }
    iss.clear();
    if ( !(in>>s) ) return in;
    iss.str(s);
    if ( s!="+/-" )
    {
	if ( !(iss>>m.e) )
	{
	    if ( s=="inf" ) m.e=1.0/zero;
	    else if ( s=="-inf" ) m.e=-1.0/zero;
	    else if ( s=="nan" ) m.e=::sqrt(minusone);
	    else if ( s=="unset" ) m.e=unset;
	    else { in.setstate(std::istream::failbit); return in; }
	}
	return in;
    }
    iss.clear();
    if ( !(in>>s) ) return in;
    iss.str(s);
    if ( !(iss>>m.e) )
    {
	if ( s=="inf" ) m.e=1.0/zero;
	else if ( s=="-inf" ) m.e=-1.0/zero;
	else if ( s=="nan" ) m.e=::sqrt(minusone);
	else if ( s=="unset" ) m.e=unset;
	else { in.setstate(std::istream::failbit); return in; }
    }
    return in;
}


}


