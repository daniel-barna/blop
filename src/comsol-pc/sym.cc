#include "sym.h"

namespace blop
{
    namespace sym
    {
        linestyle &init_linestyle(int i)
        {
            if(i==0) {static blop::sym::linestyle s; return s; }
            if(i==1) {static blop::sym::linestyle s = {1.5,3.0}; return s; }
            if(i==2) {static blop::sym::linestyle s = {7.0, 5.0}; return s; }
            if(i==3) {static blop::sym::linestyle s = {6.0, 2.5, 1.5, 2.5}; return s; }
            if(i==4) {static blop::sym::linestyle s = {6.0, 2.5, 1.5, 2.5, 1.5, 2.5}; return s; }

            // by default, return solid
            static blop::sym::linestyle s;
            return s;
        }
    }
}
