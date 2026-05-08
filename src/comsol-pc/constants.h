#ifndef __BLOP_CONSTANTS_H__
#define __BLOP_CONSTANTS_H__
#include <float.h>
#include <cmath>
#include <cmath>
namespace blop
{
    /*
      For the value that is defined as 'unset', the following should hold:
      - if two values are 'unset', the operator == should return 1 for them
      - unset should not be equal to any other number
      The first does not hold for nan (i.e. sqrt(-1.0)). Inf(=1.0/0.0) would
      be ok, but cint complains about things like this:
      Error: Symbol inf is not defined in current scope  function.h:272:
      (        void copy_arg_ranges_(const function &f1,
                              const function &f2=blop::unset,
                              const function &f3=blop::unset,....)

      Therefore some rarely used value is adopted for the moment                              

     */

    namespace { const double unset = DBL_MAX; }
//    namespace { const double unset = 1.0/0.0; }
}
#endif
