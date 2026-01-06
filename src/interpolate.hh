#ifndef __BLOP_INTERPOLATE_H__
#define __BLOP_INTERPOLATE_H__


#include <typeinfo>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <valarray>
#include <cmath>
#include "var.h"
#include "matrix.h"
#include "constants.h"
#include "warning.h"
#include "blop_gts.h"
#include "geometry.h"
#include "type_traits.h"
#include "function_core.hh"


//#define DEBUG_BLOP_INTERPOLATE

template <typename T> inline void print_debug(const T &) {}
template <> inline void print_debug(const std::valarray<double> &v) { for(unsigned int i=0; i<v.size(); ++i) std::cerr<<v[i]<<" ";  }

using namespace std;


/*

  These classes provide the framework for one- and multi-dimensional interpolation. 
  There are different algorithms for interpolating in one-dimension (linear, spline, and
  shape-preserving piecewise cubic hermite polynomial), but only one algorithm (linear)
  in multidimensions.

  These classes are implemented as templates, so any type Y for which the addition, subtraction,
  multiplication/division by a number is implemented, can be used.

  These classes have been converted from a non-template version, so there are surely many places
  where instead of the correct template types there is a 'double' variable. Please report such cases
  (i.e. if you use these interpolator classes with some non-double types, and it doesn't compile)

  The 'dimension' of an interpolator means the number of the independent variables
  it is expecting. For example a 1-dimensional iterator expects one single x-value, and returns
  a y-value. A 2-dimensional iterator expects x1 and x2 and returns a y-value.
  
 */


namespace blop
{

    template <class Ytype=double> class interpolator : public function::core
    {
    public:
        // typedef for the y-value. It can be any type for which additional & subtraction
        // is defined, and in addition the multiplication and division by a number.
        typedef Ytype y_type;

    protected:
        // the 'zero' value of the given y_type. 
        mutable y_type zero_;
        mutable bool  zero_isset_;

        // The value that will be returned if the x-variable is outside of the interpolation range
        mutable y_type out_of_range_value_;
        mutable bool out_of_range_value_set_;

        mutable bool points_modified_;

    protected:
	mutable std::string name_;

        // The vector of the  y-values. 
	std::vector<y_type> y_;

	// return the index in the given array 'xvalues', which is "just below"
	// the provided value 'x'
/*
	template <class Xtype>
        int find_index_(const std::vector<Xtype> &xvalues, Xtype x) const
	{
	    if(xvalues.empty()) return -1;
	    if(x<xvalues[0] || xvalues[xvalues.size()-1]<x) return -1;
	    typedef const typename std::vector<Xtype>::const_iterator iterator_type;
	    iterator_type pos = std::lower_bound(xvalues.begin(), xvalues.end(), x);
	    int result = pos - xvalues.begin();
	    if(result > 0) --result;
	    return result;
	}
*/

        interpolator() : zero_(), zero_isset_(false), out_of_range_value_(), out_of_range_value_set_(false), points_modified_(false) {}

        // Initialize with the vector of the y-values
        interpolator(const std::vector<y_type> &y)
          : zero_(), zero_isset_(false), out_of_range_value_(), out_of_range_value_set_(false), y_(y), points_modified_(true) {}

	virtual ~interpolator() {}

    public:

        virtual void clear() = 0;

        // return the number of arguments this interpolator expects (i.e. the dimensionality)
        virtual int dim() const = 0;

        // set the 'zero' value of the given type y_type. If you use 'normal' values (like double or float),
        // this is automatically set to the default value of that type. Otherwise (like for valarray<double>....)
        // you must set this value. If you use valarray, make sure the 'zero' has the same number of components
        // as your y-values. 
        interpolator &zero(const y_type &z)
        {
	    zero_ = z;
	    zero_isset_ = true;
	    return *this;
	}

        interpolator &out_of_range_value(const y_type &z)
        {
            out_of_range_value_ = z;
            out_of_range_value_set_ = true;
            return *this;
        }

        // return the zero-value of the y_type 
        const y_type &zero() const;

        // return the out-of-range value
        const y_type &out_of_range_value() const;

        // Return the interpolated value. A general function intended to work in 1D and multi-D
	virtual y_type interpolate(const std::vector<blop::var> &args) const = 0;

	const string &name() const { return name_; }
	void name(const std::string &n) const { name_ = n; }

        // The y-values can be any type which can be added/subtracted, and multiplied/divided
        // by a number. In the simplest case it's just a number (double), but it can be (the
        // second most common case) a valarray<double> as well, or anything else.
        // This function returns the number of (double)components of the y-values.
        int components() const
        {
            if(zero_isset_) return numeric_components(zero_);
            if(!y_.empty()) return numeric_components(y_[0]);
            return 0;
        }


        // --------------------- function::core overloads -----------------------------
        int nargs() const { return dim(); }
        int npars() const { return 0; }
        bool uses_arg(int i) const
        {
            cerr<<"interpolator::uses_arg("<<i<<"), dim = "<<dim()<<endl;
            return i<=dim();
        }
        bool uses_par(int i) const { return false; }
        var sprint(const std::vector<blop::var> &pars, bool parvalue, std::map<int,blop::var> variable_names={}, std::map<int,blop::var> param_names={}) const { return "Interpolatedfunc"; }
        bool equals(const function::core *) const { return false; }
        int n_out() const { return components(); }

        function::core *create_derivative(int i) const
        {
            cerr<<"This interpolator can not create a derivative"<<endl;
            return new function_core::constant(0.0);
        }

        void eval(const std::vector<blop::var> &args,
                  const std::vector<blop::var> &def_args,
                  const std::vector<blop::var> &params,
                  std::vector<blop::var> &result, int *ind) const
        {
            flatten_to_vector_var(interpolate(args),result,*ind);
            *ind += n_out();
        }
    
        void eval_dbl(const std::vector<blop::var> &args,
                      const std::vector<blop::var> &def_args,
                      const std::vector<blop::var> &params,
                      std::vector<blop::var> &result, int *ind) const
        {
            flatten_to_vector_dbl(interpolate(args),result,*ind);
            *ind += n_out();
        }
    };


/*
    // Specialization of 'components' for floating-point precision numbers and valarray
    template <> inline int interpolator<double>::components()      const { return 1; }
    template <> inline int interpolator<float>::components()       const { return 1; }
    template <> inline int interpolator<long double>::components() const { return 1; }
    template <> inline int interpolator<blop::geom::vec3>::components() const { return 3; }
    template <> inline int interpolator<std::valarray<double> >::components() const
    {
	if(y_.empty()) return 0;
	return y_[0].size();
    }
    template <> inline int interpolator<std::valarray<float> >::components() const
    {
	if(y_.empty()) return 0;
	return y_[0].size();
    }
    template <> inline int interpolator<std::valarray<long double> >::components() const
    {
	if(y_.empty()) return 0;
	return y_[0].size();
    }
*/

    // specializations of the 'zero' function
    // By default, return zero_, whether or not it has been set. zero_ is initialized by the default constructur,
    // which for 'normal' types (double, float, etc) is 0 anyway
    template<class Ytype>
    const Ytype &interpolator<Ytype>::zero() const { return zero_; }
    
    template<> inline const std::valarray<double>  &interpolator<std::valarray<double> >::zero() const
    {
	if(!zero_isset_ && !y_.empty())
	{
	    zero_.resize(y_[0].size());
	    for(unsigned int i=0; i<y_[0].size(); ++i) zero_[i] = 0;
	    zero_isset_ = true;
	}
	if(!zero_isset_) warning::print("The zero value of interpolator has not been set");
	return zero_;
    }
    template<> inline const std::valarray<long double>  &interpolator<std::valarray<long double> >::zero() const
    {
	if(!zero_isset_ && !y_.empty())
	{
	    zero_.resize(y_[0].size());
	    for(unsigned int i=0; i<y_[0].size(); ++i) zero_[i] = 0;
	    zero_isset_ = true;
	}
	if(!zero_isset_) warning::print("The zero value of interpolator has not been set");
	return zero_;
    }
    template<> inline const std::valarray<float>  &interpolator<std::valarray<float> >::zero() const
    {
	if(!zero_isset_ && !y_.empty())
	{
	    zero_.resize(y_[0].size());
	    for(unsigned int i=0; i<y_[0].size(); ++i) zero_[i] = 0;
	    zero_isset_ = true;
	}
	if(!zero_isset_) warning::print("The zero value of interpolator has not been set");
	return zero_;
    }
    template<> inline const blop::geom::vec3  &interpolator<blop::geom::vec3>::zero() const
    {
        if(!zero_isset_)
        {
            zero_.x = zero_.y = zero_.z = 0;
            zero_isset_ = true;
        }
	return zero_;
    }

    // specializations of the 'out_of_range_value' function
    // In the default version, if the out-of-range-value has not been set, it is set now to 'unset'.
    // In constrast to zero_, it could not be initialized in the constructor, because for
    // types like valarray<double> for example, initialization by 'unset' makes no sense. So here in this
    // default version of the function (meant for double, float, etc - i.e. 'normal' types),
    // the out_of_range_value_ is initialized to 'unset' if it has not been yet.
    template<class Ytype>
    const Ytype &interpolator<Ytype>::out_of_range_value() const
    {
        if(!out_of_range_value_set_) out_of_range_value_ = unset;
        return out_of_range_value_;
    }
    template<> inline const std::valarray<double>  &interpolator<std::valarray<double> >::out_of_range_value() const
    {
	if(!out_of_range_value_set_ && !y_.empty())
	{
	    out_of_range_value_.resize(y_[0].size());
	    for(unsigned int i=0; i<y_[0].size(); ++i) out_of_range_value_[i] = unset;
	    out_of_range_value_set_ = true;
	}
	if(!out_of_range_value_set_) warning::print("The out-of-range value of interpolator has not been set");
	return out_of_range_value_;
    }
    template<> inline const std::valarray<long double>  &interpolator<std::valarray<long double> >::out_of_range_value() const
    {
	if(!out_of_range_value_set_ && !y_.empty())
	{
	    out_of_range_value_.resize(y_[0].size());
	    for(unsigned int i=0; i<y_[0].size(); ++i) out_of_range_value_[i] = unset;
	    out_of_range_value_set_ = true;
	}
	if(!out_of_range_value_set_) warning::print("The out-of-range value of interpolator has not been set");
	return out_of_range_value_;
    }
    template<> inline const std::valarray<float>  &interpolator<std::valarray<float> >::out_of_range_value() const
    {
	if(!out_of_range_value_set_ && !y_.empty())
	{
	    out_of_range_value_.resize(y_[0].size());
	    for(unsigned int i=0; i<y_[0].size(); ++i) out_of_range_value_[i] = unset;
	    out_of_range_value_set_ = true;
	}
	if(!out_of_range_value_set_) warning::print("The out-of-range value of interpolator has not been set");
	return out_of_range_value_;
    }
    template<> inline const blop::geom::vec3  &interpolator<blop::geom::vec3>::out_of_range_value() const
    {
	if(!out_of_range_value_set_)
	{
            out_of_range_value_.x = out_of_range_value_.y = out_of_range_value_.z = unset;
        }
	return out_of_range_value_;
    }


    /*
      1D interpolation class. 
     */

    template <class Xtype=double, class Ytype=double>
    class interpolator_1d : public interpolator<Ytype>
    {
    public:
        // typedef for the type of the x values
	typedef Xtype                       x_type;
        typedef typename interpolator<Ytype>::y_type y_type;

    protected:
        // Ordered array of the x-values
        std::vector<x_type> x_;

    public:
        void clear() { x_.clear(); interpolator<y_type>::y_.clear(); interpolator<y_type>::points_modified_=true;}
        void resize(int s) { x_.resize(s); interpolator<y_type>::y_.resize(s); interpolator<y_type>::points_modified_=true;}

        // This is a 1D interpolator
        int dim() const { return 1; }

        // Set the points used for interpolation, for any 'arrays' (must be indexable by the [] operator)
        // The x- and  y-arrays must be ordered
	template <class Xarray, class Yarray, class INT>
        void points(Xarray x, Yarray y, INT n)
	{
            interpolator<y_type>::points_modified_ = true;
	    x_.resize(n);
	    interpolator<y_type>::y_.resize(n);
            bool must_sort = false;
	    for(INT i=0; i<n; ++i)
	    {
		if(i>0 && x[i]<=x[i-1]) must_sort = true;
		x_[i] = x[i];
		interpolator<y_type>::y_ [i] = y[i];
	    }
            if(must_sort)
            {
                //warning::print("Sorting the (x,y) array","template<X,Y> interpolator_1d::points(X x, Y y, int n)");
                parallel_sort(x_,interpolator<y_type>::y_);
            }
            // Remove unset values
            while(!x_.empty() && x_.back()==unset)
            {
                x_.pop_back();
                interpolator<y_type>::y_.pop_back();
            }
            // Remove occasional points with identical x-values
            for(unsigned int i=1; i<x_.size(); ++i)
            {
                if(x_[i]==x[i-1])
                {
                    if(interpolator<y_type>::y_[i] != interpolator<y_type>::y_[i-1])
                    {
                        warning::print("Identical x-values with different y-values","template<X,Y> interpolator_1d::points(X x, Y y, int n)");
                    }
                    x_.erase(x_.begin()+i);
                    interpolator<y_type>::y_.erase(interpolator<y_type>::y_.begin()+i);
                    --i;
                }
            }
	}

        // Set the poitns used for interpolation by vectors
        void points(const std::vector<x_type> &x, const std::vector<y_type> &y)
	{
	    points(x.begin(), y.begin(), std::min(x.size(), y.size()));
	}

        const std::vector<x_type> &x() const { return x_; }
        const std::vector<y_type> &y() const { return interpolator<y_type>::y_; }

        // Add a point (x,y) at the correct position, so that the x-array remains ordered
        // (use this function to add points one-by-one, if they are coming in an unordered way)
        interpolator_1d<x_type,y_type> &add_point(x_type x, y_type y)
	{
            interpolator<y_type>::points_modified_ = true;
	    if(x_.size() != interpolator<y_type>::y_.size())
	    {
		cerr<<"Mismatch in x/y sizes of interpolator"<<endl;
		return *this;
	    }

	    if(x_.empty() || x>x_.back())
	    {
		x_.push_back(x);
		interpolator<y_type>::y_ .push_back(y);
	    }
	    else
	    {
		typename std::vector<x_type>::iterator iter = lower_bound(x_.begin(),x_.end(),x);
		typename std::vector<x_type>::size_type pos = iter - x_.begin();
		x_.resize(x_.size()+1);
		interpolator<y_type>::y_.resize(interpolator<y_type>::y_.size()+1);
		for(typename std::vector<x_type>::size_type i=x_.size()-1; i>pos; --i)
		{
		    x_[i] = x_[i-1];
		    interpolator<y_type>::y_[i] = interpolator<y_type>::y_[i-1];
		}
		x_[pos] = x;
		interpolator<y_type>::y_[pos] = y;

		// this obvious solution worked for a while, but then started to produce a segfault... mystery
		//x_.insert(iter,x);
		//interpolator<y_type>::y_.insert(interpolator<y_type>::y_.begin()+(iter-x_.begin()),y);
	    }
	    return *this;
	}

	// ----------------  Constructors -----------------------
	interpolator_1d() {}
	template <class Xarray, class Yarray, class INT> interpolator_1d(Xarray x, Yarray y, INT n)
	{
	    points(x,y,n);
	}
	interpolator_1d(const std::vector<x_type> &x, const std::vector<y_type> &y)
	{
	    points(x.begin(), y.begin(), ::min(x.size(), y.size()));
	}


	// ----------  evaluate/interpolate --------------------
	virtual y_type operator()(x_type x) const = 0;
	y_type interpolate(const std::vector<blop::var> &args) const
	{
	    return operator()(!args.empty()?args[0].dbl():0);
	}

	// ------------------  number of points -----------------------------
	int n()      const { return x_.size();  }
        int size()   const { return n();        }
        bool empty() const { return x_.empty(); }

	// ------------------  get/set the ith x,y value ---------------------
	x_type x(int i) const { return (0<=i && i<(int)x_.size()?x_[i]:0); }
        y_type y(int i) const { return (0<=i && i<(int)interpolator<y_type>::y_.size() ? interpolator<y_type>::y_[i] : interpolator<y_type>::out_of_range_value()); }

	interpolator_1d &x(int i, const x_type &xval)
	{
	    if(i<0) return *this;
            interpolator<y_type>::points_modified_ = true;
	    if(i>=(int)x_.size())
	    {
		x_.resize(i+1,0);
		interpolator<y_type>::y_.resize(i+1,0);
	    }
	    x_[i] = xval;
	    return *this;
	}
	interpolator_1d &y(int i, const y_type &yval)
	{
	    if(i<0) return *this;
            interpolator<y_type>::points_modified_ = true;
	    if(i>=(int)x_.size())
	    {
		x_.resize(i+1,0);
		interpolator<y_type>::y_.resize(i+1,0);
	    }
	    interpolator<y_type>::y_[i] = yval;
	    return *this;
	}
    };


    template <class Xtype, class Ytype>
    void polynomial_interpolator_1d_spline_derivs(const vector<Xtype> &x,
						  const vector<Ytype> &y,
						  const Ytype &yp1,
						  const Ytype &ypn,
						  vector<Ytype> &y2)
    {
	const int n = (int)std::min(x.size(), y.size());
	int i,k;
	Xtype sig;
	double qn;
	Ytype un, *u = new Ytype[n+2];

	y2.resize(n);

	if(yp1 > 0.99e30) y2[0] = u[0] = 0.0;
	else
	{
	    y2[0] = -0.5;
	    u[0] = (3/(x[1]-x[0])) * ((y[1]-y[0])/(x[1]-x[0])-yp1);
	}
	
	for(i=1; i<n-1; ++i)
	{
	    sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
	    const Ytype p = sig*y2[i-1]+2.0;
	    y2[i] = (sig-1.0)/p;
	    u[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
	    u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if(ypn > 0.99e30) qn = un = 0;
	else
	{
	    qn = 0.5;
	    un = (3/(x[n-1]-x[n-2]))*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
	}
	y2[n-1] = (un-qn*u[n-2])/(qn*y2[n-2]+1);

	for(k=n-2; k>=0; --k)
	{
	    y2[k] = y2[k]*y2[k+1]+u[k];
	}
	delete [] u;
    }

    // partially specialize for Ytype=valarray<double>
    template <class Xtype, class valarray_value_type>
    void polynomial_interpolator_1d_spline_derivs(const vector<Xtype> &x,
						  const vector<valarray<valarray_value_type> > &y,
						  const valarray<valarray_value_type> &yp1,
						  const valarray<valarray_value_type> &ypn,
						  vector<valarray<valarray_value_type> > &y2)
    {
	const int n = (int)std::min(x.size(), y.size());
	if(n == 0) return;

	int i,k;
	Xtype sig;
	valarray<valarray_value_type> qn(y[0].size()),p(y[0].size()), un(y[0].size()), *u = new valarray<valarray_value_type>[n+2];
	y2.resize(n);

	for(int i=0; i<n+2; ++i) u [i].resize(y[0].size());
	for(int i=0; i<n;   ++i) y2[i].resize(y[0].size());

	for(unsigned int i=0; i<y[0].size(); ++i)
	{
	    if(yp1[i] > 0.99e30) y2[0][i] = u[0][i] = 0.0;
	    else
	    {
		y2[0][i] = -0.5;
		u[0][i] = (3.0/(x[1]-x[0])) * ((y[1][i]-y[0][i])/(x[1]-x[0])-yp1[i]);
	    }
	}
	
	for(i=1; i<n-1; ++i)
	{
	    sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
	    p = sig*y2[i-1]+2.0;
	    y2[i] = (sig-1.0)/p;
	    u[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
	    u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}

	for(unsigned int i=0; i<y[0].size(); ++i)
	{
	    if(ypn[i] > 0.99e30) qn[i] = un[i] = 0;
	    else
	    {
		qn[i] = 0.5;
		un[i] = (3/(x[n-1]-x[n-2]))*(ypn[i]-(y[n-1][i]-y[n-2][i])/(x[n-1]-x[n-2]));
	    }
	    y2[n-1][i] = (un[i]-qn[i]*u[n-2][i])/(qn[i]*y2[n-2][i]+1.0);
	}

	for(k=n-2; k>=0; --k)
	{
	    y2[k] = y2[k]*y2[k+1]+u[k];
	}
	delete [] u;
    }

    // partially specialize for Ytype=vec3
    template <class Xtype>
    void polynomial_interpolator_1d_spline_derivs(const vector<Xtype> &x,
						  const vector<blop::geom::vec3> &y,
						  const blop::geom::vec3 &yp1,
						  const blop::geom::vec3 &ypn,
						  vector<blop::geom::vec3> &y2)
    {
	const int n = (int)std::min(x.size(), y.size());
	if(n == 0) return;

	int i,k;
	Xtype sig;
        blop::geom::vec3 qn, un, *u = new blop::geom::vec3[n+2];
	y2.resize(n);

        if(yp1.x > 0.99e30) y2[0].x = u[0].x = 0.0;
        else
        {
            y2[0].x = -0.5;
            u[0].x = (3.0/(x[1]-x[0])) * ((y[1].x-y[0].x)/(x[1]-x[0])-yp1.x);
        }
        if(yp1.y > 0.99e30) y2[0].y = u[0].y = 0.0;
        else
        {
            y2[0].y = -0.5;
            u[0].y = (3.0/(x[1]-x[0])) * ((y[1].y-y[0].y)/(x[1]-x[0])-yp1.y);
        }
        if(yp1.z > 0.99e30) y2[0].z = u[0].z = 0.0;
        else
        {
            y2[0].z = -0.5;
            u[0].z = (3.0/(x[1]-x[0])) * ((y[1].z-y[0].z)/(x[1]-x[0])-yp1.z);
        }
        
	for(i=1; i<n-1; ++i)
	{
	    sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
	    const double p = sig*y2[i-1]+2.0;
	    y2[i].x = (sig-1.0)/p;
	    y2[i].y = (sig-1.0)/p;
	    y2[i].z = (sig-1.0)/p;
	    u[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
	    u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}

        if(ypn.x > 0.99e30) qn.x = un.x = 0;
        else
        {
            qn.x = 0.5;
            un.x = (3/(x[n-1]-x[n-2]))*(ypn.x-(y[n-1].x-y[n-2].x)/(x[n-1]-x[n-2]));
        }
        y2[n-1].x = (un.x-qn.x*u[n-2].x)/(qn.x*y2[n-2].x+1.0);

        if(ypn.y > 0.99e30) qn.y = un.y = 0;
        else
        {
            qn.y = 0.5;
            un.y = (3/(x[n-1]-x[n-2]))*(ypn.y-(y[n-1].y-y[n-2].y)/(x[n-1]-x[n-2]));
        }
        y2[n-1].y = (un.y-qn.y*u[n-2].y)/(qn.y*y2[n-2].y+1.0);

        if(ypn.z > 0.99e30) qn.z = un.z = 0;
        else
        {
            qn.z = 0.5;
            un.z = (3/(x[n-1]-x[n-2]))*(ypn.z-(y[n-1].z-y[n-2].z)/(x[n-1]-x[n-2]));
        }
        y2[n-1].z = (un.z-qn.z*u[n-2].z)/(qn.z*y2[n-2].z+1.0);

	for(k=n-2; k>=0; --k)
	{
	    y2[k].x = y2[k].x*y2[k+1].x+u[k].x;
	    y2[k].y = y2[k].y*y2[k+1].y+u[k].y;
	    y2[k].z = y2[k].z*y2[k+1].z+u[k].z;
	}
	delete [] u;
    }

    template <typename x_type, typename y_type>
    y_type pchipend(x_type h1,
                    x_type h2,
                    const y_type &del1,
                    const y_type &del2,
                    const y_type &zero)
    {
        const y_type d = ((2.0*h1+h2)*del1-h1*del2)/(h1+h2);
        if(d*del1 <= 0.0) return 0;
        if(del1*del2<=0.0 && std::fabs(d)>std::fabs(3.0*del1)) return 3.0*del1;
        return d;
    }
    template <typename x_type, typename valarray_value_type>
    valarray<valarray_value_type> pchipend(x_type h1,
                                           x_type h2,
                                           const valarray<valarray_value_type> &del1,
                                           const valarray<valarray_value_type> &del2,
                                           const valarray<valarray_value_type> &zero)
    {
        valarray<valarray_value_type> d;
        for(unsigned int i=0; i<del1.size(); ++i)
        {
            d[i] = ((2.0*h1+h2)*del1[i]-h1*del2[i])/(h1+h2);
            if(d[i]*del1[i] <= 0.0) d[i] = 0.0;
            if(del1[i]*del2[i]<=0.0 && std::fabs(d[i])>std::fabs(3.0*del1[i])) d[i] = 3.0*del1[i];
        }
        return d;
    }
    template <typename x_type>
    blop::geom::vec3 pchipend(x_type h1,
                        x_type h2,
                        const blop::geom::vec3 &del1,
                        const blop::geom::vec3 &del2,
                        const blop::geom::vec3 &zero)
    {
        blop::geom::vec3 d;

        d.x = ((2.0*h1+h2)*del1.x-h1*del2.x)/(h1+h2);
        if(d.x*del1.x <= 0.0) d.x = 0.0;
        if(del1.x*del2.x<=0.0 && std::fabs(d.x)>std::fabs(3.0*del1.x)) d.x = 3.0*del1.x;

        d.y = ((2.0*h1+h2)*del1.y-h1*del2.y)/(h1+h2);
        if(d.y*del1.y <= 0.0) d.y = 0.0;
        if(del1.y*del2.y<=0.0 && std::fabs(d.y)>std::fabs(3.0*del1.y)) d.y = 3.0*del1.y;

        d.z = ((2.0*h1+h2)*del1.z-h1*del2.z)/(h1+h2);
        if(d.z*del1.z <= 0.0) d.z = 0.0;
        if(del1.z*del2.z<=0.0 && std::fabs(d.z)>std::fabs(3.0*del1.z)) d.z = 3.0*del1.z;

        return d;
    }


    template <typename x_type, typename y_type>
    void sppchip_derivs(const vector<x_type> &x,
                        const vector<y_type> &y,
                        vector<y_type> &d,
                        const y_type &zero)
    {
        const int n=std::min(x.size(), y.size());
        for(int k=1; k+1<n; ++k)
        {
            if((y[k+1]-y[k])*(y[k]-y[k-1])<=0.0) d[k] = zero;
            else
            {
                const x_type hk  =x[k+1]-x[k];
                const x_type hkm1=x[k]-x[k-1];
                const x_type w1 = 2.0*hk+hkm1;
                const x_type w2 = hk+2.0*hkm1;
                const y_type deltak  = (y[k+1]-y[k])/hk;
                const y_type deltakm1= (y[k]-y[k-1])/hkm1;
                d[k] = (w1+w2)/(w1/deltakm1+w2/deltak);
            }
        }
        d[0] = pchipend<x_type,y_type>(x[1]-x[0],
                                       x[2]-x[1],
                                       (y[1]-y[0])/(x[1]-x[0]),
                                       (y[2]-y[1])/(x[2]-x[1]),
                                       zero);
        d[n-1] = pchipend<x_type,y_type>(x[n-1]-x[n-2],
                                         x[n-2]-x[n-3],
                                         (y[n-1]-y[n-2])/(x[n-1]-x[n-2]),
                                         (y[n-2]-y[n-3])/(x[n-2]-x[n-3]),
                                         zero);
        
    }


    template <typename x_type, typename valarray_value_type>
    void sppchip_derivs(const vector<x_type> &x,
                        const vector<valarray<valarray_value_type> > &y,
                        vector<valarray<valarray_value_type> > &d,
                        const valarray<valarray_value_type> &zero)
    {
        const int n=std::min(x.size(), y.size());
        for(int k=1; k+1<n; ++k)
        {
            d[k].resize(y[k].size());
            
            for(unsigned int valarray_index=0; valarray_index<d[k].size(); ++valarray_index)
            {
                if((y[k+1][valarray_index]-y[k][valarray_index])*(y[k][valarray_index]-y[k-1][valarray_index])<=0.0) d[k][valarray_index] = 0.0;
                else
                {
                    const x_type hk  =x[k+1]-x[k];
                    const x_type hkm1=x[k]-x[k-1];
                    const x_type w1 = 2.0*hk+hkm1;
                    const x_type w2 = hk+2.0*hkm1;
                    const valarray_value_type deltak  = (y[k+1][valarray_index]-y[k][valarray_index])/hk;
                    const valarray_value_type deltakm1= (y[k][valarray_index]-y[k-1][valarray_index])/hkm1;
                    d[k][valarray_index] = (w1+w2)/(w1/deltakm1+w2/deltak);
                }
            }
        }
        {
            const valarray<valarray_value_type> d1 = (y[1]-y[0])/(x[1]-x[0]);
            const valarray<valarray_value_type> d2 = (y[2]-y[1])/(x[2]-x[1]);
            d[0] = pchipend(x[1]-x[0], x[2]-x[1], d1, d2, zero);
        }
        {
            const valarray<valarray_value_type> d1 = (y[n-1]-y[n-2])/(x[n-1]-x[n-2]);
            const valarray<valarray_value_type> d2 = (y[n-2]-y[n-3])/(x[n-2]-x[n-3]);
            d[n-1] = pchipend(x[n-1]-x[n-2], x[n-2]-x[n-3], d1, d2, zero);
        }
    }
    template <typename x_type>
    void sppchip_derivs(const vector<x_type> &x,
                        const vector<blop::geom::vec3 > &y,
                        vector<blop::geom::vec3> &d,
                        const blop::geom::vec3 &zero)
    {
        const int n=std::min(x.size(), y.size());
        for(int k=1; k+1<n; ++k)
        {
            if((y[k+1].x-y[k].x)*(y[k].x-y[k-1].x)<=0.0) d[k].x = 0.0;
            else
            {
                const x_type hk  =x[k+1]-x[k];
                const x_type hkm1=x[k]-x[k-1];
                const x_type w1 = 2.0*hk+hkm1;
                const x_type w2 = hk+2.0*hkm1;
                const double deltak  = (y[k+1].x-y[k].x)/hk;
                const double deltakm1= (y[k].x-y[k-1].x)/hkm1;
                d[k].x = (w1+w2)/(w1/deltakm1+w2/deltak);
            }
            if((y[k+1].y-y[k].y)*(y[k].y-y[k-1].y)<=0.0) d[k].y = 0.0;
            else
            {
                const x_type hk  =x[k+1]-x[k];
                const x_type hkm1=x[k]-x[k-1];
                const x_type w1 = 2.0*hk+hkm1;
                const x_type w2 = hk+2.0*hkm1;
                const double deltak  = (y[k+1].y-y[k].y)/hk;
                const double deltakm1= (y[k].y-y[k-1].y)/hkm1;
                d[k].y = (w1+w2)/(w1/deltakm1+w2/deltak);
            }
            if((y[k+1].z-y[k].z)*(y[k].z-y[k-1].z)<=0.0) d[k].z = 0.0;
            else
            {
                const x_type hk  =x[k+1]-x[k];
                const x_type hkm1=x[k]-x[k-1];
                const x_type w1 = 2.0*hk+hkm1;
                const x_type w2 = hk+2.0*hkm1;
                const double deltak  = (y[k+1].z-y[k].z)/hk;
                const double deltakm1= (y[k].z-y[k-1].z)/hkm1;
                d[k].z = (w1+w2)/(w1/deltakm1+w2/deltak);
            }
        }
        {
            const blop::geom::vec3 d1 = (y[1]-y[0])/(x[1]-x[0]);
            const blop::geom::vec3 d2 = (y[2]-y[1])/(x[2]-x[1]);
            d[0] = pchipend(x[1]-x[0], x[2]-x[1], d1, d2, zero);
        }
        {
            const blop::geom::vec3 d1 = (y[n-1]-y[n-2])/(x[n-1]-x[n-2]);
            const blop::geom::vec3 d2 = (y[n-2]-y[n-3])/(x[n-2]-x[n-3]);
            d[n-1] = pchipend(x[n-1]-x[n-2], x[n-2]-x[n-3], d1, d2, zero);
        }
    }


    template <class Xtype=double, class Ytype=double>
    class polynomial_interpolator_1d : public interpolator_1d<Xtype,Ytype>
    {
    public:
        typedef typename interpolator_1d<Xtype,Ytype>::x_type x_type;
        typedef typename interpolator_1d<Xtype,Ytype>::y_type y_type;

    protected:
        // The matrix of the coefficients. 
        mutable blop::matrix<y_type> coeff_;
        
    public:
	// No initialization of the points
	polynomial_interpolator_1d()
	{
	    coeff_.resize(0,0);
	}

	// initialize the set of points from two 'pointers'
	// (indexable whatever)
	template <class Xarray, class Yarray>
	polynomial_interpolator_1d(Xarray x, Yarray y, int n):interpolator_1d<Xtype,Ytype>(x,y,n) {coeff_.resize(0,0);}

	// initialize from vectors
        polynomial_interpolator_1d(const std::vector<Xtype> &x, const std::vector<Ytype> &y) : interpolator_1d<Xtype,Ytype>(x,y)
	{
	    coeff_.resize(0,0);
	}

        // Return the order of the polynomials used for interpolation. 
	int order() const
	{
	    return (coeff_.cols()>0?coeff_.cols()-1:0);
	}

        // This function should calculate the matrix of the coefficients. 
        virtual void calculate_coefficients() const
        {
            warning::print("Don't know how to calculate interpolation coefficients for this general polynomial interpolator!");
        }

	y_type operator()(x_type x) const
        {
	    if(coeff_.cols() == 0 || interpolator<y_type>::points_modified_) calculate_coefficients();

            return function_core::piecewise_polynomial_function<x_type,y_type>::evaluate(x,interpolator_1d<x_type,y_type>::x_,
                                                                                         coeff_,
                                                                                         interpolator<y_type>::zero_,
                                                                                         interpolator<y_type>::out_of_range_value_);
	}


	// ------- calculate coeffs according to different interp. scheme ------------

        // Calculate coefficients for linear interpolation
	bool linear() const
	{
            interpolator<y_type>::points_modified_ = false;
	    interpolator<Ytype>::name("LININTERP");

	    if(interpolator_1d<Xtype,Ytype>::x_.size()<2)
	    {
		coeff_.resize(interpolator_1d<Xtype,Ytype>::x_.size(),0);
		return false;
	    }
	    coeff_.resize(interpolator_1d<Xtype,Ytype>::x_.size(), 2);

	    for(int i=0; i+1<(int)interpolator_1d<Xtype,Ytype>::x_.size(); ++i)
	    {
		coeff_(i,0) = interpolator<Ytype>::y_[i];
		const Ytype coeff1 =
		    (interpolator<Ytype>::y_[i+1]-interpolator<Ytype>::y_[i])/(interpolator_1d<Xtype,Ytype>::x_[i+1]-interpolator_1d<Xtype,Ytype>::x_[i]); 
		coeff_(i,1) = coeff1;
		// When not using the temporary coeff1 variable, but equating coeff_(i,1) directly to this expression, it produced sometimes
		// unpredictable result... strange. 
	    }
	    return true;
	}

        // Calculate coefficients for spline interpolation
	bool spline() const
	{
            interpolator<y_type>::points_modified_ = false;
	    interpolator<Ytype>::name("SPLINE");

	    const int n = (int)std::min(interpolator_1d<Xtype,Ytype>::x_.size(), interpolator<Ytype>::y_.size());

	    if(n < 3)
	    {
		warning::print("Less than 3 points: no spline is prepared",
			       "polynomial_interpolator_1d::spline()");
		coeff_.resize(0,0);
		return false;
	    }

	    vector<Ytype> d(n);

	    const Ytype yp1 = (interpolator<Ytype>::y_[1]-interpolator<Ytype>::y_[0])  /(interpolator_1d<Xtype,Ytype>::x_[1]-interpolator_1d<Xtype,Ytype>::x_[0]);
	    const Ytype ypn = (interpolator<Ytype>::y_[n-1]-interpolator<Ytype>::y_[n-2])/(interpolator_1d<Xtype,Ytype>::x_[n-1]-interpolator_1d<Xtype,Ytype>::x_[n-2]);
	    polynomial_interpolator_1d_spline_derivs(interpolator_1d<Xtype,Ytype>::x_,interpolator<Ytype>::y_,yp1,ypn,d);

	    coeff_.resize(n,4);

	    for(int k=0; k+1<n; ++k)
	    {
		const Xtype h     =  interpolator_1d<Xtype,Ytype>::x_[k+1]-interpolator_1d<Xtype,Ytype>::x_[k];
		const Ytype delta = (interpolator<Ytype>::y_[k+1]-interpolator<Ytype>::y_[k])/h;

		coeff_(k,0) = interpolator<Ytype>::y_[k];
		const Ytype coeff1 = delta - h*d[k]/3. - h*d[k+1]/6.; 
		coeff_(k,1) = coeff1;
		const Ytype coeff2 = d[k]/2.0;
		coeff_(k,2) = coeff2;
		const Ytype coeff3 = d[k+1]/6.0/h - d[k]/6.0/h;
		coeff_(k,3) = coeff3;
	    }

	    return true;
	}

	// --------- shape-preserving piecewise cubic hermite polynomial --------
	// http://www.mathworks.com/moler/interp.pdf
	bool sppchip() const
	{
            interpolator<y_type>::points_modified_ = false;
	    interpolator<y_type>::name("SPPCHIP");
	    const vector<x_type> &x = interpolator_1d<x_type,y_type>::x_;
	    const vector<y_type> &y = interpolator<y_type>::y_;
            
	    const int n=(int)(std::min(x.size(), y.size()));
	    if(n<3) return false;
	    vector<y_type> d(n);
	    sppchip_derivs(interpolator_1d<x_type,y_type>::x_,interpolator<y_type>::y_,d,interpolator<y_type>::zero());
            
	    coeff_.resize(n,4);
	    for(int k=0; k+1<n; ++k)
	    {
		const x_type  h     =  x[k+1]-x[k];
		const y_type  delta = (y[k+1]-y[k])/h;
		coeff_(k,0) = y[k];
		coeff_(k,1) = d[k];
		coeff_(k,2) = (3.0*delta-2.0*d[k]-d[k+1])/h;
		coeff_(k,3) = (d[k]-2.0*delta+d[k+1])/(h*h);
	    }
	    return true;
	}

        void operator*= (const y_type &factor)
        {
            for(unsigned int i=0; i<interpolator<y_type>::y_.size(); ++i)
            {
                interpolator<y_type>::y_[i] *= factor;
            }
            coeff_ *= factor;
        }
        void operator/= (const y_type &factor)
        {
            for(unsigned int i=0; i<interpolator<y_type>::y_.size(); ++i)
            {
                interpolator<y_type>::y_[i] /= factor;
            }
            coeff_ /= factor;
        }

        function::core *create_derivative(int i) const
        {
            if(i!=0 || coeff_.cols()<2)
            {
                const int N = interpolator<y_type>::components();
                vector<var> a(N);
                flatten_to_vector_var(interpolator<y_type>::zero_,a,0);
                if(N==1) return new function_core::constant(a[0]);
                function_core::multiple *m = new function_core::multiple;
                for(int k=0; k<N; ++k) m->append(new function_core::constant(a[k]));
                return m;
            }
            function_core::piecewise_polynomial_function<x_type,y_type> *result =
                 new function_core::piecewise_polynomial_function<x_type,y_type>(interpolator_1d<x_type,y_type>::x_,
                                                                                 coeff_,
                                                                                 interpolator<y_type>::zero_,
                                                                                 interpolator<y_type>::out_of_range_value_);
            result->derivate();
            return result;
        }

        // When cloning (for function), do not copy the whole class with the interpolated points,
        // just a piecewise polynomial
        function_core::piecewise_polynomial_function<x_type,y_type> *clone() const
        {
            if(coeff_.cols() == 0 || interpolator<y_type>::points_modified_) calculate_coefficients();
            return new function_core::piecewise_polynomial_function<x_type,y_type>(interpolator_1d<x_type,y_type>::x_,coeff_,
                                                                                   interpolator<y_type>::zero_,
                                                                                   interpolator<y_type>::out_of_range_value_);
        }
    };

    template <class Xtype=double, class Ytype=double>
    class linear_interpolator_1d : public polynomial_interpolator_1d<Xtype,Ytype>
    {
    public:
        typedef typename polynomial_interpolator_1d<Xtype,Ytype>::x_type x_type;
        typedef typename polynomial_interpolator_1d<Xtype,Ytype>::y_type y_type;

        linear_interpolator_1d() : polynomial_interpolator_1d<Xtype,Ytype>() {}
	template <class Xarray, class Yarray> linear_interpolator_1d(Xarray x, Yarray y, int n) : polynomial_interpolator_1d<Xtype,Ytype>(x,y,n) {}
        linear_interpolator_1d(const std::vector<Xtype> &x, const std::vector<Ytype> &y) : polynomial_interpolator_1d<Xtype,Ytype>(x,y) {}

        void calculate_coefficients() const { polynomial_interpolator_1d<x_type,y_type>::linear(); }

    };

    template <class Xtype=double, class Ytype=double>
    class spline_interpolator_1d : public polynomial_interpolator_1d<Xtype,Ytype>
    {
    public:
        typedef typename polynomial_interpolator_1d<Xtype,Ytype>::x_type x_type;
        typedef typename polynomial_interpolator_1d<Xtype,Ytype>::y_type y_type;

        spline_interpolator_1d() : polynomial_interpolator_1d<Xtype,Ytype>() {}
	template <class Xarray, class Yarray> spline_interpolator_1d(Xarray x, Yarray y, int n) : polynomial_interpolator_1d<Xtype,Ytype>(x,y,n) {}
        spline_interpolator_1d(const std::vector<Xtype> &x, const std::vector<Ytype> &y) : polynomial_interpolator_1d<Xtype,Ytype>(x,y) {}

        void calculate_coefficients() const { polynomial_interpolator_1d<x_type,y_type>::spline(); }
    };

    template <class Xtype=double, class Ytype=double>
    class sppchip_interpolator_1d : public polynomial_interpolator_1d<Xtype,Ytype>
    {
    public:
        typedef typename polynomial_interpolator_1d<Xtype,Ytype>::x_type x_type;
        typedef typename polynomial_interpolator_1d<Xtype,Ytype>::y_type y_type;

        sppchip_interpolator_1d() : polynomial_interpolator_1d<Xtype,Ytype>() {}
	template <class Xarray, class Yarray> sppchip_interpolator_1d(Xarray x, Yarray y, int n) : polynomial_interpolator_1d<Xtype,Ytype>(x,y,n) {}
        sppchip_interpolator_1d(const std::vector<Xtype> &x, const std::vector<Ytype> &y) : polynomial_interpolator_1d<Xtype,Ytype>(x,y) {}

        void calculate_coefficients() const { polynomial_interpolator_1d<x_type,y_type>::sppchip(); }
    };
        


    // multidimensional interpolator
    template <class Xtype=double, class Ytype=double>
    class interpolator_md : public interpolator<Ytype>
    {
    public:
        typedef Ytype y_type;

    protected:
	std::vector<std::vector<Xtype> > x_;

	// create a 'global' index from the individual indexes along the
	// dimensions, to be used for indexing the y_ array of values
	unsigned int make_index_(const std::vector<unsigned int> &indexes) const
	{
	    unsigned int result = 0;
	    for(unsigned int i=0; i<x_.size(); ++i)
	    {
		unsigned int n = 1;
		for(unsigned int j=i+1; j<x_.size(); ++j) n *= x_[j].size();
		result += indexes[i]*n;
	    }
	    return result;
	}

    public:

	// ----------------  Constructors -----------------------

	// dimension not specified, has to be specified later
	// (setting the dimension later needs to preceed any
	// setting of the data points)
	interpolator_md() {}

	// Initialize by setting the dimension as well.
	interpolator_md(int ndim) {x_.resize(ndim);}

        void clear() { x_.clear(); interpolator<y_type>::y_.clear(); interpolator<y_type>::points_modified_ = true;}

	// ----------  evaluate/interpolate ---------------------
	// overloading interpolator::interpolate
	Ytype interpolate(const std::vector<blop::var> &args) const
	{
	    std::vector<Xtype> x(args.size());
	    for(unsigned int i=0; i<args.size(); ++i) x[i] = args[i].dbl();
	    return interpolate(x);
	}

	virtual Ytype interpolate(const std::vector<Xtype> &x) const = 0;
	virtual Ytype operator()(Xtype x1, Xtype x2) const
	{
	    std::vector<Xtype> x;
	    x.push_back(x1);
	    x.push_back(x2);
	    return interpolate(x);
	}
	virtual Ytype operator()(Xtype x1, Xtype x2, Xtype x3) const
	{
	    std::vector<Xtype> x;
	    x.push_back(x1);
	    x.push_back(x2);
	    x.push_back(x3);
	    return interpolate(x);
	}
	virtual Ytype operator()(Xtype x1, Xtype x2, Xtype x3, Xtype x4) const
	{
	    std::vector<double> x;
	    x.push_back(x1);
	    x.push_back(x2);
	    x.push_back(x3);
	    x.push_back(x4);
	    return interpolate(x);
	}

	// ------------  fill the points ------------------------

	// provide the list of x-values along dimension 'dim' (1-based)
	// The vector does not need to be ordered, and it can contain
	// values multiple times, they are automatically removed and ordered
	interpolator_md& x(int dim, const std::vector<Xtype> &values)
	{
            interpolator<y_type>::points_modified_ = true;
	    --dim;
	    if(dim<0 || x_.size()<=(unsigned int)dim)
	    {
		warning::print("Dimension is out of range",
			       var("interpolator_md::x(") & (dim+1) & var(",...values...)"));
		return *this;
	    }

	    // store the values
	    x_[dim] = values;
	    sort(x_[dim].begin(), x_[dim].end());
	    vector<double>::iterator it = unique(x_[dim].begin(), x_[dim].end());
	    x_[dim].resize(it-x_[dim].begin());

	    // check if all the x-values (along all dimension) have been set. If not,
	    // simply return
	    for(unsigned int i=0; i<x_.size(); ++i) if(x_[i].empty()) return *this;;

	    // otherwise resize the y-values
	    int ny = 1;
	    for(unsigned int i=0; i<x_.size(); ++i) ny*=x_[i].size();
	    interpolator<Ytype>::y_.resize(ny);
	    return *this;
	}

	interpolator_md& y(const std::vector<Xtype> &x, Ytype y)     // set y value at given x
	{
            interpolator<y_type>::points_modified_ = true;
	    if(x.size() != x_.size())
	    {
		warning::print("Dimension mismatch","interpolator_md::y(vector<x_type>,y_type)");
		return *this;
	    }
	    vector<unsigned int> indexes(x_.size());
	    for(unsigned int i=0; i<x_.size(); ++i)
	    {
		vector<double>::iterator pos = std::find(x_[i].begin(), x_[i].end(), x[i]);
		if(pos == x_[i].end())
		{
		    warning::print(var("This value ") & x[i] &
				   var(" is not found in the x-values of dim[") & var(i) & var("]"),
				   "interpolator_md::y(vector<x_type>,double)");
		    return *this;
		}
		indexes[i] = pos-x_[i].begin();
	    }
	
	    const unsigned int index = make_index_(indexes);
	    interpolator<Ytype>::y_[index] = y;
	    return *this; 
	}

	interpolator_md& y(const std::vector<unsigned int> &indexes, Ytype y)     // set y value at given x
	{
            interpolator<y_type>::points_modified_ = true;
	    if(indexes.size() != x_.size())
	    {
		warning::print("Dimension mismatch","interpolator_md::y(vector<int> &indexes,y_type)");
		return *this;
	    }
	    const unsigned int index = make_index_(indexes);
	    interpolator<Ytype>::y_[index] = y;
	    return *this; 
	}

	// ----------- dimension number -------------------------
        // return the number of dimensions (i.e. the number of arguments this interpolator expects)
	int  dim() const { return x_.size(); }

	// Specify the dimension. Invalidates any previous setting, clears data points
	void dim(int ndim)
        {
            interpolator<y_type>::points_modified_ = true;
            x_.clear();
            interpolator<Ytype>::y_.clear();
            x_.resize(ndim);
        }

	// return the number of x-values of the given dimension
	int n  (int dim) const { --dim; return (0<=dim&&(unsigned int)dim<x_.size()?x_[dim].size():0); }

        // return the array of x points along dimension 'dim' (1-based!)
        const std::vector<Xtype> &x(int dim) const { return x_[dim-1]; }

        // Return an y-value based on the grid indexes
        const y_type &y(const std::vector<unsigned int> &indexes) const
        {
            unsigned int index = make_index_(indexes);
            return interpolator<Ytype>::y_[index];
        }
        y_type &y(const std::vector<unsigned int> &indexes)
        {
            unsigned int index = make_index_(indexes);
            return interpolator<Ytype>::y_[index];
        }

        Xtype xmax(int dim) const { return x_[dim-1].back(); }
        Xtype xmin(int dim) const { return x_[dim-1].front(); }
    };


    template <class Xtype=double, class Ytype=double>
    class linear_interpolator_md : public interpolator_md<Xtype,Ytype>
    {
    public:
        linear_interpolator_md() {}
        linear_interpolator_md(int dim) : interpolator_md<Xtype,Ytype>(dim) {}
	Ytype interpolate(const std::vector<Xtype> &x) const
	{
#ifdef DEBUG_BLOP_INTERPOLATE
            cerr<<"linear_interpolator_md::interpolate(...) called"<<endl;
#endif            
	    // loop over all dimensions, and find the index along that dimension
	    // corresponding to the provided x[i] value ('just below')
	    vector<unsigned int> indexes(interpolator_md<Xtype,Ytype>::x_.size());
	    for(unsigned int i=0; i<interpolator_md<Xtype,Ytype>::x_.size(); ++i)
	    {
		// -1 means out of range, so return 0;
		if(x[i]<interpolator_md<Xtype,Ytype>::x_[i].front() || interpolator_md<Xtype,Ytype>::x_[i].back()<x[i])
                {
                    return interpolator<Ytype>::out_of_range_value();
                }
                typedef const typename std::vector<Xtype>::const_iterator iterator_type;
                iterator_type pos = std::lower_bound(interpolator_md<Xtype,Ytype>::x_[i].begin(),
                                                     interpolator_md<Xtype,Ytype>::x_[i].end(),
                                                     x[i]);
                indexes[i] = pos - interpolator_md<Xtype,Ytype>::x_[i].begin();
                if(indexes[i] > 0) --indexes[i];
	    }

	    // the index-array 'indexes' stores now the indexes (in the x_ arrays)
	    // of the left-lower corners of the box, which surrounds the given x value

	    Ytype result = interpolator<Ytype>::zero();

	    // now loop over all combinations of indexes (0 or +1) to go to the corners
	    // of the box, which surrounds the given x point
	    // the 'indexloop' variable is a clever way to do this: if it has the bit
	    // pattern for example 00101 this means that take +1 index (i.e. higher
	    // side of the box) along dimensions 1 and 3 (bits are from the right)
	    // looping of indexloop from 0 to pow(2,dim) makes all combinations, all
	    // corners of the box
	    for(unsigned int indexloop = 0; indexloop< ::pow(2,interpolator_md<Xtype,Ytype>::x_.size()); ++indexloop)
	    {
		// this array of indexes is the one which moves around all corner
		// of the box. It is derived from the lower-left corner 'indexes'
		// by adding +1 along some dimensions
		vector<unsigned int> indexes2(indexes.size());

		double weight = 1;

                bool corner_out_of_range = false;

		// loop over all dimension
		for(unsigned int i=0; i<indexes.size(); ++i)
		{
		    unsigned int mask = (1<<i);

		    // a flag indicating wether we use now the upper x-value
		    // along this dimension
		    bool upper = ((indexloop&mask)!=0);

		    // prepare the index array: take +1 for the actual dimensions (bits),
		    // taken from the 'indexloop' variable
		    indexes2[i] = indexes[i] + upper;


                    // If in this dimension the index is out of range, i.e. the corner of the hypercube
                    // is out of the grid, set the appropriate flag and break the loop. The y-value at this
                    // corner will not be included in the weighted sum.
                    if(indexes2[i] >= interpolator_md<Xtype,Ytype>::x_[i].size())
                    {
                        corner_out_of_range = true;
                        break;
                    }


                    // Accumulate the weight only if along this dimension the next point is still in the grid.
                    // If not, it means that we are on the edge, i.e. the weight along this dimension will be
                    // anyway 1, so do not accumulate
                    if(indexes[i]+1<interpolator_md<Xtype,Ytype>::x_[i].size())
                    {
                        const double w = 
                            std::fabs(x[i]-interpolator_md<Xtype,Ytype>::x_[i][indexes[i]+(!upper)])/
                            (interpolator_md<Xtype,Ytype>::x_[i][indexes[i]+1]-interpolator_md<Xtype,Ytype>::x_[i][indexes[i]]);
                        weight *= w;
                    }
		}

#ifdef DEBUG_BLOP_INTERPOLATE
                cerr<<"Processing corner: ";
                for(unsigned int i=0; i<indexes2.size(); ++i) cerr<<interpolator_md<Xtype,Ytype>::x_[i][indexes2[i]]<<" ";
                cerr<<endl;
#endif
                if(!corner_out_of_range)
                {
                    Ytype value_at_this_corner = interpolator<Ytype>::y_[interpolator_md<Xtype,Ytype>::make_index_(indexes2)];
#ifdef DEBUG_BLOP_INTERPOLATE
                    cerr<<"Adding "<<weight<<"*(";
                    print_debug(value_at_this_corner);
                    cerr<<")"<<endl;
#endif                    
                    result += value_at_this_corner*weight;
#ifdef DEBUG_BLOP_INTERPOLATE
                    cerr<<"Current value of result: ";
                    print_debug(result);
                    cerr<<endl;
#endif
                    //result += interpolator<Ytype>::y_[interpolator_md<Xtype,Ytype>::make_index_(indexes2)]*weight;
                }
#ifdef DEBUG_BLOP_INTERPOLATE
                else
                {
                    cerr<<"This corner is out of range"<<endl;
                }
#endif

	    }
#ifdef DEBUG_BLOP_INTERPOLATE
            cerr<<"linear_interpolator_md::interpolate(...) returning: ";
            print_debug(result);
            cerr<<endl;
#endif
            
	    return result;
	}

	linear_interpolator_md<Xtype,Ytype> *clone() const
	{
	    return new linear_interpolator_md<Xtype,Ytype>(*this);
	}
    };


#ifdef HAVE_GTS_H
    //html <a name="delaunay">
    // The delaunay-interpolator uses a delaunay-triangulated surface to interpolate
    // a 2-dimensional function z(x,y) given at (x,y) values which do not need to be
    // regular
    //html </a>
    class delaunay_interpolator : public interpolator<double>
    {
    private:
	delaunay_surface surface_;
	mutable bool initialized_;
	mutable int nx_, ny_;
	mutable double xmin_, xmax_, ymin_, ymax_, dx_, dy_;
	mutable std::vector<std::vector<const GtsTriangle*> > triangles_;
	void initialize_() const;
	int index_(int ix, int iy) const { return ix*ny_+iy; }

    public:
	// Constructors
        delaunay_interpolator() : initialized_(false), nx_(0), ny_(0) { name_ = "delaunay interpolator"; }
	delaunay_interpolator(const delaunay_surface &);

        // clear
        void clear();

	// Add a data point z(x,y)
	delaunay_interpolator &add_point(double x, double y, double z);

	// Return the dimensionality (i.e. the number of arguments it expects)
	int dim() const { return 2; }

	// Evaluate (interpolate) at the given points (x,y)
	double operator()(double x, double y) const;
	double interpolate(const std::vector<blop::var> &args) const;

	// Get the triangulated surface
	const delaunay_surface &surface() const { return surface_; }

	delaunay_interpolator *clone() const { return new delaunay_interpolator(*this); }

	int components() const { return 1; }

	
    };
#endif


}

#ifdef __MAKECINT__
#pragma link off function operator==(std::valarray<double>&,std::valarray<double>&);
#pragma link off function operator!=(std::valarray<double>&,std::valarray<double>&);
#pragma link off function operator<(std::valarray<double>&,std::valarray<double>&);
#pragma link off function operator<=(std::valarray<double>&,std::valarray<double>&);
#pragma link off function operator>(std::valarray<double>&,std::valarray<double>&);
#pragma link off function operator>=(std::valarray<double>&,std::valarray<double>&);

#pragma link off function operator==(std::vector<std::valarray<double> >&,std::vector<std::valarray<double> >&);
#pragma link off function operator!=(std::vector<std::valarray<double> >&,std::vector<std::valarray<double> >&);
#pragma link off function operator<(std::vector<std::valarray<double> >&,std::vector<std::valarray<double> >&);
#pragma link off function operator<=(std::vector<std::valarray<double> >&,std::vector<std::valarray<double> >&);
#pragma link off function operator>(std::vector<std::valarray<double> >&,std::vector<std::valarray<double> >&);
#pragma link off function operator>=(std::vector<std::valarray<double> >&,std::vector<std::valarray<double> >&);
#endif

#endif
