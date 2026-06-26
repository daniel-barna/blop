#ifndef __BLOP_TIME_H__
#define __BLOP_TIME_H__

#ifndef __MAKECINT__
#include <chrono>
#include "var.h"
#include <string>

namespace blop
{
    namespace time
    {
        double       date2epoch(const var &datetime, const var &format);
        std::string  epoch2date(double epoch_seconds, const var &format);

        /*
        // Convert a date/time to std::chrono::system_clock::time_point
        std::chrono::system_clock::time_point date2timepoint(int year, int mon, int day, int hour=0, int min=0, int sec=0);
        
        // Convert a date represented by a string to a timepoint. In the format string %Y refers to year, %m to month
        // %d to day, %H to hour, %M to minutes and %S to seconds
        std::chrono::system_clock::time_point date2timepoint(const var &datestr, const var &format="%Y-%m-%d %H:%M:%S");

        std::int64_t date2epoch(const var &datestr, const var &format="%Y-%m-%d %H:%M:%S");
        std::int64_t date2epoch(int year, int mon, int day, int hour=0, int min=0, int sec=0);
        
        std::string timepoint2date(const std::chrono::system_clock::time_point &tp, var format="%Y-%m-%d %H:%M:%S");
        std::string epoch2date(double seconds_since_epoch, var format="%Y-%m-%d %H:%M:%S", bool integer_seconds=true);
        std::string epoch2date(std::int64_t seconds_since_epoch, var format="%Y-%m-%d %H:%M:%S");
        std::string epoch2date(int seconds_since_epoch, var format="%Y-%m-%d %H:%M:%S");
        std::string epoch2date(unsigned int seconds_since_epoch, var format="%Y-%m-%d %H:%M:%S");
        */
    }
}
#endif


#endif


