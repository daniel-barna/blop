#ifndef __BLOP_TYPE_TRAITS_H__
#define __BLOP_TYPE_TRAITS_H__
#include "array.h"
#include "geometry.h"
#include "matrix.h"
#include <valarray>
#include <vector>

#ifndef __MAKECINT__

namespace blop
{

    template <typename T> inline int numeric_components(const T&) { return 1; }
    template <>           inline int numeric_components(const blop::geom::vec3 &) { return 3; }
    template <typename T> inline int numeric_components(const std::vector<T> &v) { return v.size(); }
    template <typename T> inline int numeric_components(const std::valarray<T> &v) { return v.size(); }
    template <typename T> inline int numeric_components(const blop::matrix<T>  &m) { return m.cols()*m.rows(); }


    template <typename T> inline int flatten_to_vector_var(const T &value, std::vector<blop::var> &a, int ind)
    {
        a[ind] = value;
        return 1;
    }
    template <typename T> inline int flatten_to_vector_dbl(const T &value, std::vector<blop::var> &a, int ind)
    {
        a[ind].dbl(value);
        return 1;
    }
    template <> inline int flatten_to_vector_var(const blop::geom::vec3 &value, std::vector<blop::var> &a, int ind)
    {
        a[ind++] = value.x;
        a[ind++] = value.y;
        a[ind++] = value.z;
        return 3;
    }
    template <> inline int flatten_to_vector_dbl(const blop::geom::vec3 &value, std::vector<blop::var> &a, int ind)
    {
        a[ind++].dbl(value.x);
        a[ind++].dbl(value.y);
        a[ind++].dbl(value.z);
        return 3;
    }
    template <typename T> inline int flatten_to_vector_var(const std::vector<T> &value, std::vector<blop::var> &a, int ind)
    {
        for(unsigned int i=0; i<value.size(); ++i) a[ind++] = value[i];
        return value.size();
    }
    template <typename T> inline int flatten_to_vector_dbl(const std::vector<T> &value, std::vector<blop::var> &a, int ind)
    {
        for(unsigned int i=0; i<value.size(); ++i) a[ind++].dbl(value[i]);
        return value.size();
    }
    template <typename T> inline int flatten_to_vector_var(const std::valarray<T> &value, std::vector<blop::var> &a, int ind)
    {
        for(unsigned int i=0; i<value.size(); ++i) a[ind++] = value[i];
        return value.size();
    }
    template <typename T> inline int flatten_to_vector_dbl(const std::valarray<T> &value, std::vector<blop::var> &a, int ind)
    {
        for(unsigned int i=0; i<value.size(); ++i) a[ind++].dbl(value[i]);
        return value.size();
    }
    template <typename T> inline int flatten_to_vector_var(const blop::matrix<T> &value, std::vector<blop::var> &a, int ind)
    {
        for(int row=value.row_base(); row<value.row_base()+value.rows(); ++row)
        {
            for(int col=value.col_base(); col<value.col_base()+value.cols(); ++col)
            {
                a[ind++] = value(row,col);
            }
        }
        return value.cols()*value.rows(); 
    }
    template <typename T> inline int flatten_to_vector_dbl(const blop::matrix<T> &value, std::vector<blop::var> &a, int ind)
    {
        for(int row=value.row_base(); row<value.row_base()+value.rows(); ++row)
        {
            for(int col=value.col_base(); col<value.col_base()+value.cols(); ++col)
            {
                a[ind++].dbl(value(row,col));
            }
        }
        return value.cols()*value.rows(); 
    }

}

#endif

#endif
