#include "blop_time.hh"
#include "warning.h"
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <cstring>

using namespace std;

namespace blop
{
    namespace time
    {
        double date2epoch(const var &datetime, const var &format)
        {
/*
            std::tm tm{};
            
            std::istringstream is(datetime.str());

            is >> std::get_time(&tm, format.c_str());
            
            if (is.fail()) warning::print(var("Failed to parse date '") & datetime & "'");
            
            std::chrono::year_month_day ymd{
                std::chrono::year{tm.tm_year + 1900},
                std::chrono::month{static_cast<unsigned>(tm.tm_mon + 1)},
                std::chrono::day{static_cast<unsigned>(tm.tm_mday)}
            };
            
            std::chrono::sys_days days{ymd};
            
            auto tp =
                days
                + std::chrono::hours{tm.tm_hour}
                + std::chrono::minutes{tm.tm_min}
                + std::chrono::seconds{tm.tm_sec};
            
            return duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
*/

            int year, month, day, hour, minute;
            double second;

            void *ptr[6] = {0,0,0,0,0,0};

            int index=0;
            string sscanf_format="";
            for(unsigned int i=0; i<format.size(); ++i)
            {
                if(format[i]=='%')
                {
                    if(i+1<format.size())
                    {
                        if(format[i+1]=='Y')      { if(index<6) { ptr[index++] = &year;   sscanf_format += "%d"; } ++i; }
                        else if(format[i+1]=='m') { if(index<6) { ptr[index++] = &month;  sscanf_format += "%d"; } ++i; }
                        else if(format[i+1]=='d') { if(index<6) { ptr[index++] = &day;    sscanf_format += "%d"; } ++i; }
                        else if(format[i+1]=='H') { if(index<6) { ptr[index++] = &hour;   sscanf_format += "%d"; } ++i; }
                        else if(format[i+1]=='M') { if(index<6) { ptr[index++] = &minute; sscanf_format += "%d"; } ++i; }
                        else if(format[i+1]=='S') { if(index<6) { ptr[index++] = &second; sscanf_format += "%lf"; } ++i; }
                        else if(format[i+1]=='%') { sscanf_format += "%%"; ++i; }
                        //else warning::print("Non-allowed specifier in format string");
                    }
                    //else warning::print("Trailing % in format");
                }
                else sscanf_format += format[i];
            }
            sscanf(datetime.c_str(),sscanf_format.c_str(),ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5]);

            int ns = round((second-std::floor(second))*1e9);

            std::chrono::year_month_day ymd{
                std::chrono::year{year},
                std::chrono::month{month},
                std::chrono::day{day}
            };
            
            std::chrono::sys_days days{ymd};

            auto tp =
                days
                + std::chrono::hours{hour}
                + std::chrono::minutes{minute}
                + std::chrono::seconds{(int)floor(second)}
                + std::chrono::nanoseconds{ns};

            return std::chrono::duration<double>(tp.time_since_epoch()).count();
          
//            return duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();

//            return date2timepoint(values[year_index], values[month_index], values[day_index], values[hour_index], values[minute_index], values[second_index]);            


        }

        std::string epoch2date(double epoch_seconds, const var &format)
        {
            const std::int64_t  seconds_int  = std::floor(epoch_seconds);
            const double        seconds_frac = epoch_seconds - seconds_int;


            // This does not support fractional seconds
            std::chrono::sys_seconds tp{std::chrono::seconds{seconds_int}};
            auto days = floor<chrono::days>(tp);
            std::chrono::year_month_day ymd{days};
            std::chrono::hh_mm_ss tod{tp - days};

            std::tm tm{};

            const int year  = int(ymd.year());
            const int month = unsigned(ymd.month());
            const int day   = unsigned(ymd.day());
            const int hour = int(tod.hours().count());
            const int min  = int(tod.minutes().count());
            const double sec = int(tod.seconds().count()) + seconds_frac;

            /*
            tm.tm_year = int(ymd.year()) - 1900;
            tm.tm_mon  = unsigned(ymd.month()) - 1;
            tm.tm_mday = unsigned(ymd.day());
            
            tm.tm_hour = int(tod.hours().count());
            tm.tm_min  = int(tod.minutes().count());
            tm.tm_sec  = int(tod.seconds().count());
            */

            std::string result;
            char tmp[30];
            for(unsigned int i=0; i<format.size(); ++i)
            {
                if(format[i]=='%')
                {
                    if(i+1<format.size())
                    {
                        if(format[i+1]=='Y')      {sprintf(tmp,"%4d",year);   result += tmp; ++i;}
                        else if(format[i+1]=='m') {sprintf(tmp,"%02d",month); result += tmp; ++i;}
                        else if(format[i+1]=='d') {sprintf(tmp,"%02d",day);   result += tmp; ++i;}
                        else if(format[i+1]=='H') {sprintf(tmp,"%02d",hour);  result += tmp; ++i;}
                        else if(format[i+1]=='M') {sprintf(tmp,"%02d",min);   result += tmp; ++i;}
                        else if(format[i+1]=='S') 
                        {
                            if(seconds_frac>=0.9999e-9) 
                            {
                                sprintf(tmp,"%.9f",sec);

                                // Check if there is a decimal point
                                auto dot = strchr(tmp,'.');

                                if(dot)
                                {
                                    // discard trailing zeros
                                    for(int j=strlen(tmp)-1; j>=0; --j)
                                    {
                                        if(tmp[j]!='0') break;
                                        tmp[j]='\0';
                                    }
                                    for(int j=0; j<2-(dot-tmp); ++j) result += "0";
                                }
                                else
                                {
                                    for(int j=0; j<2-strlen(tmp); ++j) result += "0";
                                }
                            }
                            else sprintf(tmp,"%02d",(int)floor(sec));
                            result += tmp; 
                            ++i;
                        }
                        else if(format[i+1]=='%') {result += "%"; }
                    }
                }
                else result += format[i];
            }

            return result;
        }

    
    }
}
