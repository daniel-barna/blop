#include "blop_time.hh"
#include <cstdio>

using namespace std;

namespace blop
{

chrono::system_clock::time_point date2timepoint(int year, int mon, int day, int hour, int min, int sec)
{
   struct std::tm t;
   t.tm_sec = sec;        // second of minute (0 .. 59 and 60 for leap seconds)
   t.tm_min = min;        // minute of hour (0 .. 59)
   t.tm_hour = hour;      // hour of day (0 .. 23)
   t.tm_mday = day;       // day of month (0 .. 31)
   t.tm_mon = mon-1;      // month of year (0 .. 11)
   t.tm_year = year-1900; // year since 1900
   t.tm_isdst = 0;       // no daylight saving time
   std::time_t tt = std::mktime(&t);
   if (tt == -1) {
       throw "no valid system time";
   }
   return std::chrono::system_clock::from_time_t(tt);
}

chrono::system_clock::time_point date2timepoint(const var &datestr, const var &format)
{
    int values[7];
    for(int i=0; i<7; ++i) values[i] = 0;
    int year_index=6;
    int month_index=6;
    int day_index=6;
    int hour_index=6;
    int minute_index=6;
    int second_index=6;

    int index=0;
    string sscanf_format="";
    for(unsigned int i=0; i<format.size(); ++i)
    {
        if(format[i]=='%')
        {
            if(i+1<format.size())
            {
                if(format[i+1]=='y')      { year_index=index++;   sscanf_format += "%d"; ++i; }
                else if(format[i+1]=='m') { month_index=index++;  sscanf_format += "%d"; ++i; }
                else if(format[i+1]=='d') { day_index=index++;    sscanf_format += "%d"; ++i; }
                else if(format[i+1]=='H') { hour_index=index++;   sscanf_format += "%d"; ++i; }
                else if(format[i+1]=='M') { minute_index=index++; sscanf_format += "%d"; ++i; }
                else if(format[i+1]=='S') { second_index=index++; sscanf_format += "%d"; ++i; }
                else if(format[i+1]=='%') { sscanf_format += "%%"; ++i; }
                //else warning::print("Non-allowed specifier in format string");
            }
            //else warning::print("Trailing % in format");
        }
        else sscanf_format += format[i];
    }

    sscanf(datestr.c_str(),sscanf_format.c_str(),values+0, values+1, values+2, values+3, values+4, values+5);

    return date2timepoint(values[year_index], values[month_index], values[day_index], values[hour_index], values[minute_index], values[second_index]);
}

}
