#ifndef __BLOP_MATRIX_H__
#define __BLOP_MATRIX_H__
#include "warning.h"
#include <vector>
#include <iostream>
#include <iomanip>

#ifndef __MAKECINT__
#include <algorithm>
#endif


namespace blop
{
    template <typename T=double>
    class matrix
    {
    private:

	std::vector<std::vector<T> > data_;
	int nrows_, ncols_;
	int row_base_, col_base_;
	static int &default_nrows_() { static int dummy=0; return dummy; }
	static int &default_ncols_() { static int dummy=0; return dummy; }

	void init(const T &t)
        {
            data_.resize(nrows_);
            for(int i=0; i<nrows_; ++i) data_[i].resize(ncols_,t);
        }
    public:
        matrix(const T &t = T())
            : nrows_(default_nrows_()), ncols_(default_ncols_()), row_base_(0), col_base_(0) { init(t); }
        matrix(int nrows, int ncols, const T &t = T())
            : nrows_(nrows), ncols_(ncols), row_base_(0), col_base_(0) { init(t); }
        matrix(const matrix<T> &o)
        {
            nrows_ = o.nrows_;
            ncols_ = o.ncols_;
            col_base_ = o.col_base_;
            row_base_ = o.row_base_;
            data_ = o.data_;
        }
	~matrix() {}
	const matrix<T> &operator= (const matrix<T> &o)
	{
            nrows_ = o.nrows_;
            ncols_ = o.ncols_;
            col_base_ = o.col_base_;
            row_base_ = o.row_base_;
            data_ = o.data_;
            return *this;
        }

	static void default_nrows(int n) { default_nrows_() = n; }
	static void default_ncols(int n) { default_ncols_() = n; }

	int cols() const { return ncols_; }
	int rows() const { return nrows_; }

	void resize(int new_nrows, int new_ncols, const T &val = T())
        {
            data_.resize(new_nrows);
            for(int i=0; i<new_nrows; ++i) data_[i].resize(new_ncols, val);
            nrows_ = new_nrows;
            ncols_ = new_ncols;
        }

	int col_base() const { return col_base_; }
	int row_base() const { return row_base_; }

	T &operator()       (int row, int col)           { return data_[row-row_base_][col-col_base_]; }
	const T &operator() (int row, int col) const     { return data_[row-row_base_][col-col_base_]; }
	std::vector<T> &row(int rownumber)               { return data_[rownumber-row_base_]; }
	const std::vector<T> &row(int rownumber)   const { return data_[rownumber-row_base_]; }
	void index_base(int rb, int cb)                  { row_base_ = rb; col_base_ = cb; }


// for some reason the produced code G__cpp_blop.cxx does not want to compile:
// ambiguous overload of operator*(T,T) [with T=blop::var], etc
// although there IS an operator* for blop::var. I don't understand why. 
// so I just remove this problematic code here. template classes are anyway
// not available in the interpreter...       
#ifndef __MAKECINT__

        matrix<T> inverse() const;
        T matrix_minor   (int ind1, int ind2) const;
        T cofactor(int ind1, int ind2) const;
        T det() const;

        const matrix<T> &operator*=(const T &);
        const matrix<T> &operator/=(const T &);

        const matrix<T> &operator+=(const matrix<T> &);
        const matrix<T> &operator-=(const matrix<T> &);

#endif

    };

    inline int parity(const int P[], int N)
    {
        int G = 0;
        for(int i=0; i<N; ++i)
        {
            for(int j=i+1; j<N; ++j)
            {
                if(P[i] > P[j]) ++G;
            }
        }
        return 1-2*(G%2);
    }

#ifndef __MAKECINT__
    template <typename T>
    T matrix<T>::matrix_minor(int ind1, int ind2) const
    {
        if(cols() != rows())
        {
            warning::print("Matrix minor can only be called for square matrices","matrix<T>::matrix_minor(int,int)");
            return T();
        }

        // make both indices 0-based
        ind1 -= row_base();
        ind2 -= col_base();
        
        const int N = cols();
        int *I = new int[N-1];
        int *J = new int[N-1];
        {
            int iout = 0;
            for(int i=0; i<N; ++i) if(i!=ind1) I[iout++] = i;
        }
        {
            int iout = 0;
            for(int i=0; i<N; ++i) if(i!=ind2) J[iout++] = i;
        }

        T result = 0;
        do
        {
            T subresult = 1;
            for(int i=0; i<N-1; ++i)
            {
                subresult *= operator()(I[i]+row_base(),J[i]+col_base());
            }
            result += parity(J,N-1)*subresult;
        }
        while(next_permutation(J,J+(N-1)));

        delete [] I;
        delete [] J;

        return result;
    }

    template <typename T>
    T matrix<T>::cofactor(int ind1, int ind2) const
    {
        const int P = 1-((ind1-row_base())+(ind2-col_base()))%2*2;
        return P * matrix_minor(ind1,ind2);
    }

    template <typename T>
    matrix<T> matrix<T>::inverse() const
    {
        if(cols() != rows())
        {
            warning::print("Can not invert a non-square matrix","matrix<T>::inverse()");
            return *this;
        }
        const int N = cols();
        matrix<T> result(N,N);
        if(N == 2)
        {
            T a = operator()(0+row_base(), 0+col_base());
            T b = operator()(0+row_base(), 1+col_base());
            T c = operator()(1+row_base(), 0+col_base());
            T d = operator()(1+row_base(), 1+col_base());
            T det = a*d-b*c;
            result(0,0) =  d/det;
            result(0,1) = -b/det;
            result(1,0) = -c/det;
            result(1,1) =  a/det;
            result.index_base(row_base(), col_base());
        }
        else if(N == 3)
        {
            const T a = operator()(0+row_base(), 0+col_base());
            const T b = operator()(0+row_base(), 1+col_base());
            const T c = operator()(0+row_base(), 2+col_base());
            const T d = operator()(1+row_base(), 0+col_base());
            const T e = operator()(1+row_base(), 1+col_base());
            const T f = operator()(1+row_base(), 2+col_base());
            const T g = operator()(2+row_base(), 0+col_base());
            const T h = operator()(2+row_base(), 1+col_base());
            const T k = operator()(2+row_base(), 2+col_base());
            const T det = a*(e*k-f*h) + b*(f*g-k*d) + c*(d*h-e*g);
            result(0,0) = (e*k-f*h)/det;
            result(0,1) = (c*h-b*k)/det;
            result(0,2) = (b*f-c*e)/det;
            result(1,0) = (f*g-d*k)/det;
            result(1,1) = (a*k-c*g)/det;
            result(1,2) = (c*d-a*f)/det;
            result(2,0) = (d*h-e*g)/det;
            result(2,1) = (g*b-a*h)/det;
            result(2,2) = (a*e-b*d)/det;
            result.index_base(row_base(),col_base());
        }
        else
        {
            const T D = det();
            for(int i=0; i<N; ++i)
            {
                for(int j=0; j<N; ++j)
                {
                    result(i,j) = cofactor(j+row_base(), i+col_base())/D;
                }
            }
        }
        return result;
    }

    template <typename T>
    T matrix<T>::det() const
    {
        if(cols() != rows())
        {
            warning::print("Determinant can only be calculated for a square matrix","matrix<T>::det()");
            return T();
        }
        const int N = cols();
        T result = 0;
        int *ind = new int[N];
        for(int i=0; i<N; ++i) ind[i] = i; // create the first permutation
        
        for(bool ok=true; ok; ok=next_permutation(ind, ind+N))
        {
            T tmp = 1.0;
            for(int i=0; i<N; ++i)
            {
                tmp *= operator()(i+row_base(), ind[i]+col_base()) * parity(ind,N);
            }
            result += tmp;
        }
        delete [] ind;
        return result;
    }


    template <typename T>
    const matrix<T> &matrix<T>::operator*=(const T &t)
    {
        for(unsigned int i=0; i<data_.size(); ++i)
        {
            for(unsigned int j=0; j<data_[i].size(); ++j)
            {
                data_[i][j] *= t;
            }
        }
        return *this;
    }
    template <typename T>
    const matrix<T> &matrix<T>::operator/=(const T &t)
    {
        for(unsigned int i=0; i<data_.size(); ++i)
        {
            for(unsigned int j=0; j<data_[i].size(); ++j)
            {
                data_[i][j] /= t;
            }
        }
        return *this;
    }

    template <typename T>
    const matrix<T> &matrix<T>::operator+=(const matrix<T> &M)
    {
        if(M.cols() != cols() || M.rows() != rows())
        {
            warning::print("Row or column mismatch","matrix<T>::operator+=(const matrix<T>&)");
            return *this;
        }
        for(int row=0; row<rows(); ++row)
        {
            for(int col=0; col<cols(); ++col)
            {
                (*this)(row_base()+row,col_base()+col) += M(M.row_base()+row,M.col_base()+col);
            }
        }
        return *this;
    }
    template <typename T>
    const matrix<T> &matrix<T>::operator-=(const matrix<T> &M)
    {
        if(M.cols() != cols() || M.rows() != rows())
        {
            warning::print("Row or column mismatch","matrix<T>::operator-=(const matrix<T>&)");
            return *this;
        }
        for(int row=0; row<rows(); ++row)
        {
            for(int col=0; col<cols(); ++col)
            {
                (*this)(row_base()+row,col_base()+col) -= M(M.row_base()+row,M.col_base()+col);
            }
        }
        return *this;
    }


#endif // __MAKECINT__

    // Product of two matrices. The resulting matrix will have 0-based indexes!
    template <typename T>
    matrix<T> operator*(const matrix<T> &a, const matrix<T> &b)
    {
	matrix<T> result(a.rows(),b.cols());
	if(a.cols() != b.rows())
	{
	    warning::print("a.cols() != b.rows()","operator*(const matrix &a, const matrix &b)");
	    return result;
	}
	for(int row=0; row<a.rows(); ++row)
	{
	    for(int col=0; col<b.cols(); ++col)
	    {
		T r = 0;
		for(int i=0; i<a.cols(); ++i)
		{
		    r += a(a.row_base()+row,a.col_base()+i)*b(b.row_base()+i,b.col_base()+col);
		}
		result(row,col) = r;
	    }
	}
	return result;
    }

    template <class T>
    matrix<T> operator+(const matrix<T> &a, const matrix<T> &b)
    {
	matrix<T> result(a);
        result += b;
        return result;
    }
   
    template <class T>
    matrix<T> operator-(const matrix<T> &a, const matrix<T> &b)
    {
	matrix<T> result(a);
        result -= b;
	return result;
    }


    template <class T>
    matrix<T> operator*(const matrix<T> &a, const T &factor)
    {
        matrix<T> result(a.rows(),a.cols());
	for(int row=0; row<a.rows(); ++row)
	{
	    for(int col=0; col<a.cols(); ++col)
	    {
		result(row,col) = a(a.row_base()+row,a.col_base()+col)*factor;
	    }
	}
	return result;
    }
    template <class T>
    matrix<T> operator*(const T &factor,const matrix<T> &a)
    {
        matrix<T> result(a.rows(),a.cols());
	for(int row=0; row<a.rows(); ++row)
	{
	    for(int col=0; col<a.cols(); ++col)
	    {
		result(row,col) = a(a.row_base()+row,a.col_base()+col)*factor;
	    }
	}
	return result;
    }

    template <class T>
    matrix<T> operator/(const matrix<T> &a, const T &factor)
    {
        matrix<T> result(a.rows(),a.cols());
	for(int row=0; row<a.rows(); ++row)
	{
	    for(int col=0; col<a.cols(); ++col)
	    {
		result(row,col) = a(a.row_base()+row,a.col_base()+col)/factor;
	    }
	}
	return result;
    }

    // The transpose of a matrix. The resulting matrix will keep the index-bases of the
    // original matrix, i.e. its base for the col-index will be the same as that of the row-index
    // of the original matrix, and similarly for the other direction.
    template <class T>
        matrix<T> transpose(const matrix<T> &a)
    {
	matrix<T> result(a.cols(),a.rows());
	for(int row=0; row<a.rows(); ++row)
	{
	    for(int col=0; col<a.cols(); ++col)
	    {
		result(col,row) = a(a.row_base()+row,a.col_base()+col);
	    }
	}
	result.index_base(a.col_base(),a.row_base());
	return result;
    }

    template <typename T>
        ostream &operator<<(ostream &out, const matrix<T> &m)
    {
        for(int row=m.row_base(); row<m.row_base()+m.rows(); ++row)
        {
            for(int col=m.col_base(); col<m.col_base()+m.cols(); ++col)
            {
                out<<setw(10)<<m(row,col)<<" ";
            }
            out<<endl;
        }
        return out;
    }

    template <typename T>
    matrix<T> identity_matrix(int dim)
    {
        matrix<T> result(dim,dim);
        for(int i=0; i<dim; ++i) result(i,i) = 1;
        return result;
    }
    
}


#endif
