#ifndef __BLOP_COLOR_H__
#define __BLOP_COLOR_H__

#include "var.h"

namespace blop
{

class color
{
 private:
    double red_,green_,blue_;
    friend bool operator== (const color &c1, const color &c2);
    friend bool operator!= (const color &c1, const color &c2);
 public:
    color(double r,double g,double b) : red_(r), green_(g), blue_(b) {}
    color() : red_(0), green_(0), blue_(0) {}
    color(const color &o) : red_(o.red_), green_(o.green_), blue_(o.blue_) {}

    double red() const {return red_;}
    double green() const {return green_;}
    double blue() const {return blue_;}

    color &red(double d) {red_ = d; return *this;}
    color &green(double d) {green_ = d; return *this;}
    color &blue(double d) {blue_ = d; return *this;}

    void set(double r,double g,double b)
	{red_ = r; green_ = g; blue_ = b;}

    //static color get(const var &);

    class global_initializer
	{
	public:
	    global_initializer();
	};

    const color &operator+=(const color &);
    const color &operator-=(const color &);
    const color &operator*=(double f);
    const color &operator/=(double f);

    static color get(int i);
    static std::vector<blop::color> &default_sequence();
    static int sequence_index(const color &);
};

ostream &operator<<(ostream &out, const color &c);

inline bool operator== (const color &c1, const color &c2)
{
    return
    c1.red_ == c2.red_ &&
    c1.green_ == c2.green_ &&
    c1.blue_ == c2.blue_;
}

inline bool operator!= (const color &c1, const color &c2)
{
    return !(c1==c2);
}

inline bool operator< (const color &c1, const color &c2)
{
    if(c1.red() < c2.red()) return true;
    if(c1.red() > c2.red()) return false;
    if(c1.green() < c2.green()) return true;
    if(c1.green() > c2.green()) return false;
    if(c1.blue() < c2.blue()) return true;
    if(c1.blue() > c2.blue()) return false;
    return false;
}

color operator+ (const color &c1, const color &c2);
color operator- (const color &c1, const color &c2);
color operator* (const color &c1, double f);
color operator* (double f, const color &c2);
color operator/ (const color &c1, double f);


extern color black;
extern color red;
extern color green;
extern color blue;
extern color magenta;
extern color white;
extern color yellow;
extern color cyan;
extern color sienna;
extern color orange;
extern color pink;
extern color gray;
extern color purple;
extern color gold;
extern color transparent;
extern color autocolor;

}


#ifndef __MAKECINT__
namespace {
    blop::color::global_initializer blop_color_global_initializer;
}
#endif

#endif

