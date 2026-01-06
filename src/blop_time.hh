#ifndef __BLOP_TIME_H__
#define __BLOP_TIME_H__

#ifndef __MAKECINT__
#include <chrono>
#include "var.h"

namespace blop
{
    std::chrono::system_clock::time_point date2timepoint(int year, int mon, int day, int hour=0, int min=0, int sec=0);

    // Convert a date represented by a string to a timepoint. In the format string %y refers to year, %m to month
    // %d to day, %H to hour, %M to minutes and %S to seconds
    std::chrono::system_clock::time_point date2timepoint(const var &datestr, const var &format="%y/%m/%d %H:%M:%S");

}
#endif


#endif


