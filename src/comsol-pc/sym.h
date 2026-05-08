#ifndef __BLOP_SYM_H__
#define __BLOP_SYM_H__

#include <vector>


/*
All the symbolic constants are implemented as an enumerated type
within the blop::sym namespace, in order to avoid global
namespace pollution
 */

namespace blop
{
    namespace sym
    {
      typedef enum { begin=1, end=2, center=4, left=8, right=16, top=32, bottom=64, base=128, up=256, down=512 } position;
      //typedef enum { solid=100, dotted, dashed, dashdotted, dashddotted } linestyle;
      typedef enum { right_down, right_up, left_down, left_up, down_right, down_left, up_right, up_left } scan2d;
      typedef enum { horizontal=1, vertical=2 } horizontal_vertical;


      typedef std::vector<double> linestyle;

      extern linestyle &init_linestyle(int i); 

      namespace
      {
          linestyle &solid       = blop::sym::init_linestyle(0);
          linestyle &dotted      = blop::sym::init_linestyle(1);
          linestyle &dashed      = blop::sym::init_linestyle(2);
          linestyle &dashdotted  = blop::sym::init_linestyle(3);
          linestyle &dashddotted = blop::sym::init_linestyle(4);
      }
  }
}

#endif
