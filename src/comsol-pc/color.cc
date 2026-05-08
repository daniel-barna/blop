#include "color.h"
#include <iostream>
#include "warning.h"

namespace blop
{
    color red(1,0,0);
    color green(0,0.7,0);
    color blue(0,0,1);
    color black(0,0,0);
    color magenta(0.5,0,0.5);
    color yellow(1,1,0);
    color white(1,1,1);
    color cyan(0.0, 0.7, 1.0);
    color sienna(0.6, 0.3, 0.2);
    color orange(1.0, 0.5, 0.0);
    color pink(1.0, 0.2, 1.0);
    color gold(1.0, 215.0/255.0, 0.0);
    color gray(0.55,0.55,0.55);
    color purple(160.0/256, 92.0/256, 240.0/256);
    color transparent(-1,-1,-1);
    color autocolor(-2,-2,-2);

    const color &color::operator+=(const color &c)
    {
	red_   += c.red_;
	green_ += c.green_;
	blue_  += c.blue_;
	return *this;
    }
    const color &color::operator-=(const color &c)
    {
	red_   -= c.red_;
	green_ -= c.green_;
	blue_  -= c.blue_;
	return *this;
    }
    const color &color::operator*=(double f)
    {
	red_   *= f;
	green_ *= f;
	blue_  *= f;
	return *this;
    }
    const color &color::operator/=(double f)
    {
	red_   /= f;
	green_ /= f;
	blue_  /= f;
	return *this;
    }

    color color::get(int i)
    {
        std::vector<blop::color> &seq = default_sequence();
        return seq[i%seq.size()];
    }

/*
    color color::get(const var &name)
    {
	if(name.str() == "red") return blop::red;
	if(name.str() == "green") return blop::green;
	if(name.str() == "blue") return blop::blue;
	if(name.str() == "black") return blop::black;
	if(name.str() == "magenta") return blop::magenta;
	if(name.str() == "yellow") return blop::yellow;
	if(name.str() == "white") return blop::white;
	if(name.str() == "cyan") return blop::cyan;
	if(name.str() == "sienna") return blop::sienna;
	if(name.str() == "orange") return blop::orange;
	if(name.str() == "pink") return blop::pink;
	warning::print("Can not interpret color name: " &name,
		       "color:;get(const var &)");
	return blop::black;
    }
*/


    std::vector<color> &color::default_sequence()
    {
	static std::vector<blop::color> s;
	if(s.empty())
	{
	    s.push_back(blop::black);  // 0
	    s.push_back(blop::red);    // 1
	    s.push_back(blop::green);  // 2
	    s.push_back(blop::blue);   // 3
	    s.push_back(blop::magenta);// 4
	    s.push_back(blop::cyan);   // 5
	    s.push_back(blop::orange); // 6
	    s.push_back(blop::purple); // 7
	    s.push_back(blop::sienna); // 8
	    s.push_back(blop::pink);   // 9
	    s.push_back(blop::gold);   // 10
	}
	return s;
    }

    int color::sequence_index(const color &c)
    {
	for(unsigned int i=0; i<default_sequence().size(); ++i)
	{
	    if(default_sequence()[i] == c) return (int)i;
	}
	return -1;
    }

    color::global_initializer::global_initializer()
    {
	new(&blop::red) color(1,0,0);
	new(&blop::green) color(0,0.7,0);
	new(&blop::blue) color(0,0,1);
	new(&black) color(0,0,0);
	new(&magenta) color(0.5,0,0.5);
	new(&yellow) color(1,1,0);
	new(&white) color(1,1,1);
	new(&cyan) color(0.0, 0.7, 1.0);
	new(&sienna) color(0.6, 0.3, 0.2);
	new(&orange) color(1.0, 0.5, 0.0);
	new(&pink) color(1.0, 0.2, 1.0);
	new(&transparent) color(-1,-1,-1);

    }

    color operator+ (const color &c1, const color &c2)
    {
	return color(c1.red()  +c2.red(),
		     c1.green()+c2.green(),
		     c1.blue() +c2.blue());
    }
    color operator- (const color &c1, const color &c2)
    {
	return color(c1.red()  -c2.red(),
		     c1.green()-c2.green(),
		     c1.blue() -c2.blue());
    }
    color operator* (const color &c1, double f)
    {
	return color(c1.red()  *f,
		     c1.green()*f,
		     c1.blue() *f);
    }
    color operator* (double f,const color &c1)
    {
	return color(c1.red()  *f,
		     c1.green()*f,
		     c1.blue() *f);
    }
    color operator/ (const color &c1, double f)
    {
	return color(c1.red()  /f,
		     c1.green()/f,
		     c1.blue() /f);
    }

    ostream &operator<<(ostream &out, const color &c)
    {
	out<<c.red()<<","<<c.green()<<","<<c.blue();
	return out;
    }

}
