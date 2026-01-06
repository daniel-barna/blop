/*
  This file contains useful functions to plot complex impedances on
  the Smith-chart:

  ***     smith_chart();

          It plots a Smith-chart, and makes it permanent, so that
          subsequent plot(...) commands do not clear it from the
          screen

  ***     to_smith(complex);

          This function transforms a complex impedance to the
          Smith-chart, taking Z0=50 Ohm as reference

  This file is placed into my ~/.blop/ directory, so that whenever
  I need it, it can be simply included by #include "smith.h"

  Check out the file RLC-resonator.C, how this can be used!

  This little code here produced the output 'smith.pdf' in this same directory.

  #include "smith.h"
  int main()
  {
    smith_chart();
    pdf::print("smith.pdf");
  }

 */


#include <complex>
typedef complex<double> cmp;

complex<double> parallel(const complex<double> &a,
			 const complex<double> &b)
{
    const complex<double> ainv = 1.0/a;
    const complex<double> binv = 1.0/b;
    const complex<double> tmp  = ainv+binv;
    return 1.0/tmp;
}

complex<double> parallel(const complex<double> &a,
			 const complex<double> &b,
			 const complex<double> &c)
{
    const complex<double> ainv = 1.0/a;
    const complex<double> binv = 1.0/b;
    const complex<double> cinv = 1.0/c;
    const complex<double> tmp  = ainv+binv+cinv;
    return 1.0/tmp;
}

// Transform a complex impedance Z to the Smith-chart
complex<double> to_smith(const complex<double> &Z, double Z0 = 50)
{
    return (Z-Z0)/(Z+Z0);
}

// Transform a function (having two components: the real and imaginary
// parts - typically the complex impedance vs. omega) to the Smith-chart
function to_smith(const function &Z, double Z0 = 50)
{
    if(Z.components() != 2)
    {
	cerr<<"to_smith should be called on a function with 2 components"<<endl;
	return 0.0;
    }

    function a = Z[0]-Z0;  // real part of Z-Z0
    function b = Z[1];     // imag part of Z-Z0
    function c = Z[0]+Z0;  // real part of Z+Z0
    // function d = Z[1];  // imag part of Z-Z0 = imag part of Z+Z0

    function real = (a*c+b*b)/(c*c+b*b);
    function imag = b*(c-a)/(c*c+b*b);//(1-a/c);
    return function (real,imag);

/*
    // Z[0] is the real part, Z[1] is the imaginary part
    function A   = sqrt((pow(Z[0]-Z0,2.0)+pow(Z[1],2.0))/(pow(Z[0]+Z0,2.0)+pow(Z[1],2.0)));
    function phi = atan2(Z[1],Z[0]-Z0)-atan2(Z[1],Z[0]+Z0);
    return function(A*cos(phi),A*sin(phi));
*/
}

// Do the same on a complex-valued C-function
function to_smith(complex<double> (*Z)(double), double Z0 = 50)
{
    return to_smith(cfunc(Z));
}

const double PI = 3.14159265358979323846;

// return the center and radius of a constant-R circle
// on the Smith-chart (R is normalized already!)
void const_R_circle_params(double R, double &center, double &radius)
{
    const double cross = (::fabs(R)-1)/(::fabs(R)+1);
    center = 0.5*(cross+1);
    radius = center-cross;
}

// return the y0 coordinate of the center (along the vertical x=1 line)
// and the radius of a constant-X circle on the Smith-chart
// X is already normalized
void const_X_circle_params(double X, double &center, double &radius)
{
    center=1.0/X;
    radius=1.0/X;
}

line &fdraw_arc(double x0, double y0, double r, double start, double stop)
{
    line &a = line::draw(&frame::current());
    const int N = 40;
    for(int ia=0; ia<=N; ++ia)
    {
	double angle = start+ia*(stop-start)/N;
	a(x1len(x0+r*cos(angle)),y1len(y0+r*sin(angle)));
    }
    return a;
}

void smith_chart(var textsize="\\tiny", bool minor_lines=true)
{
    // set up the frame: do not draw axes, do not draw frame,
    // make ranges for both x and y [-1;1]
    // and make it equal size in x and y
    frame::current().clear();
    set::xrange(-1,1);
    set::yrange(-1,1);
    set::grid(false);
    frame::current().draw_frame(false);
    frame::current().draw_axes (false);

    // we want the circle to look like a circle, and not an ellipse.
    set::nodistort(true);

    // There will be circles around the r=1 circle of the Smith-chart,
    // which show the wavelength, angle, etc. The largest one has
    // r=1+6*height_0, so increase the frame's margins so that these
    // will still be visible

    length height_0 = height(textsize & " 0");
    set::lmargin(6*!height_0);
    set::bmargin(6*!height_0);
    set::rmargin(6*!height_0);
    set::tmargin(6*!height_0);


    // set linewidths and colors for the lines, labels
    length linewidth1 = 0.5*LW; // linewidth for minor lines
    length linewidth2 = 1  *LW; // linewidth for major lines
    color chart_color(0.5,0.6,0.4);
    double color_factor = 0.7;
    color label_color(chart_color.red()*color_factor,
		      chart_color.green()*color_factor,
		      chart_color.blue()*color_factor);
    color wavelength_scale_color(0.5,0.3,0.3);
    color angle_scale_color(0.3,0.3,0.5);


    // R=const circles
    // The first column contains the real resistances, at
    // which a constant-R circle of the Smith-chart should
    // be drawn. A negative value here means to make it a
    // major line (i.e. thicker line), and print the corresponding
    // label on it
    // 2nd and 3rd columns mean the ranges in the reactance X,
    // between which the circle should be drawn
    double R[] = {0.01,  -0.2,  0.2,
		  0.02,  -0.5,  0.5,
		  0.03,  -0.2,  0.2,
		  0.04,  -0.5,  0.5,
		  0.05,  -0.2,  0.2,
		  0.05,   0.5,  1,
		  0.05,  -0.5, -1,
		  0.06,  -0.5,  0.5,
		  0.07,  -0.2,  0.2,
		  0.08,  -0.5,  0.5,
		  0.09,  -0.2,  0.2,
                 -0.1,   -2  ,  2,
		  0.11,  -0.2,  0.2,
		  0.12,  -0.5,  0.5,
		  0.13,  -0.2,  0.2,
		  0.14,  -0.5,  0.5,
		  0.15,  -0.2,  0.2,
		  0.15,   0.5,  1,
		  0.15,  -0.5, -1,
		  0.16,  -0.5,  0.5,
		  0.17,  -0.2,  0.2,
		  0.18,  -0.5,  0.5,
		  0.19,  -0.2,  0.2,
		 -0.2,   -5  ,  5,
		  0.22,  -0.5,  0.5,
		  0.24,  -0.5,  0.5,
		  0.25,   0.5,  1,
		  0.25,  -0.5, -1,
		  0.26,  -0.5,  0.5,
		  0.28,  -0.5,  0.5,
		 -0.3,   -2  ,  2,
		  0.32,  -0.5,  0.5,
		  0.34,  -0.5,  0.5,
		  0.35,   0.5,  1,
		  0.35,  -0.5, -1,
		  0.36,  -0.5,  0.5,
		  0.38,  -0.5,  0.5,
		 -0.4,   -5  ,  5,
		  0.42,  -0.5,  0.5,
		  0.44,  -0.5,  0.5,
		  0.45,   0.5,  1,
		  0.45,  -0.5, -1,
		  0.46,  -0.5,  0.5,
		  0.48,  -0.5,  0.5,
		 -0.5,   -2  ,  2,
		  0.55,  -1  ,  1,
		 -0.6,   -5  ,  5,
		  0.65,  -1  ,  1,
		 -0.7,   -2  ,  2,
		  0.75,  -1  ,  1,
		 -0.8,   -5  ,  5,
		  0.85,  -1  ,  1,
		 -0.9,   -2  ,  2,
		  0.95,  -1  ,  1,
		 -1,    -10  , 10,
		  1.1,   -2  ,  2,
		  1.2,   -5  ,  5,
		  1.3,   -2  ,  2,
		  1.4,   -5  ,  5,
		  1.5,   -2  ,  2,
		  1.6,   -5  ,  5,
		  1.7,   -2  ,  2,
		  1.8,   -5  ,  5,
		  1.9,   -2  ,  2,
		 -2,    -20  , 20,
		  2.2,   -5  ,  5,
		  2.4,   -5  ,  5,
		  2.6,   -5  ,  5,
		  2.8,   -5  ,  5,
		 -3,    -10  , 10,
		  3.2,   -5  ,  5,
		  3.4,   -5  ,  5,
		  3.6,   -5  ,  5,
		  3.8,   -5  ,  5,
		 -4,    -20  , 20,
		  4.2,   -5  ,  5,
		  4.4,   -5  ,  5,
		  4.6,   -5  ,  5,
		  4.8,   -5  ,  5,
		 -5,    -10  , 10,
		  6,    -20  , 20,
		  7,    -10  , 10,
		  8,    -20  , 20,
		  9,    -10  , 10,
		 -10,   -50  , 50,
		  12,   -20  , 20,
		  14,   -20  , 20,
		  16,   -20  , 20,
		  18,   -20  , 20,
		 -20,   -50  , 50,
		 -50,  -100  ,100,
		  unset, unset};

    for(int i=0; R[i] != unset; i += 3)
    {
	if(!minor_lines && R[i]>0) continue;
	// calculate the center (along the real axis)
	// and the radius of the R=const circle
	double center, radius;
	const_R_circle_params(::fabs(R[i]),center,radius);

	// calculate the start/stop angles of this circle
	// from the limiting reactances: calculate the
	// Gamma (reflection factor) corresponding
	// to Z=(R,X), and calculate the angle from the
	// circle center
	const double start_X = max(R[i+1],R[i+2]);
	const double stop_X  = min(R[i+1],R[i+2]);
	cmp Gamma1 = to_smith(cmp(::fabs(R[i]),start_X),1);
	cmp Gamma2 = to_smith(cmp(::fabs(R[i]),stop_X ),1);
	double start_angle = atan2(Gamma1.imag(),Gamma1.real()-center);
	if(start_angle<0) start_angle += 2*PI;
	double stop_angle  = atan2(Gamma2.imag(),Gamma2.real()-center);
	if(stop_angle<0) stop_angle += 2*PI;
	length lw = linewidth1;

	// R[i]<0 means a thicker line, and a label on the circle
	if(R[i]<0)
	{
	    lw = linewidth2;
	    label::fdraw(textsize & var(" ") & ::fabs(R[i]),x1len(center-radius),y1len(0))
		.angle(90).textcolor(label_color).align(sym::center,sym::center);
	}
	arc::fdraw(x1len(center),y1len(0),x1len(0,radius),y1len(0,radius),start_angle*180/PI,stop_angle*180/PI)
	    .linecolor(chart_color).linewidth(lw);
    }


    // X=const circles. The first column is the reactance X. If this is negative, it means
    // that it should be a thicker line, but its absolute value is used. 
    double X[] = {0.01,  0,   0.2,
		  0.02,  0,   0.5,
		  0.03,  0,   0.2,
		  0.04,  0,   0.5,
		  0.05,  0,   0.2,
		  0.05,  0.5, 1,
		  0.06,  0,   0.5,
		  0.07,  0,   0.2,
		  0.08,  0,   0.5,
		  0.09,  0,   0.2,
		 -0.1,   0,   2,
		  0.11,  0,   0.2,
		  0.12,  0,   0.5,
		  0.13,  0,   0.2,
		  0.14,  0,   0.5,
		  0.15,  0,   0.2,
		  0.15,  0.5, 1,
		  0.16,  0,   0.5,
		  0.17,  0,   0.2,
		  0.18,  0,   0.5,
		  0.19,  0,   0.2,
		 -0.2,   0,   5,
		  0.22,  0,   0.5,
		  0.24,  0,   0.5,
		  0.26,  0,   0.5,
		  0.28,  0,   0.5,
		 -0.3,   0,   2,
		  0.32,  0,   0.5,
		  0.34,  0,   0.5,
		  0.36,  0,   0.5,
		  0.38,  0,   0.5,
		 -0.4,   0,   5,
		  0.42,  0,   0.5,
		  0.44,  0,   0.5,
		  0.46,  0,   0.5,
		  0.48,  0,   0.5,
		 -0.5,   0,   2,
		  0.55,  0,   1, 
		  0.6,   0,   5,
		  0.65,  0,   1,
		  0.7,   0,   2,
		  0.75,  0,   1,
		  0.8,   0,   5,
		  0.85,  0,   1,
		  0.9,   0,   2,
		  0.95,  0,   1,
		 -1,     0,  10,
		  1.1,   0,   2,
		  1.2,   0,   5,
		  1.3,   0,   2,
		  1.4,   0,   5,
		  1.5,   0,   2,
		  1.6,   0,   5,
		  1.7,   0,   2,
		  1.8,   0,   5,
		  1.9,   0,   2,
		 -2,     0,  20,
		  2.2,    0,  5,
		  2.4,    0,  5,
		  2.6,    0,  5,
		  2.8,    0,  5,
		 -3,      0, 10,
		  3.2,    0,  5,
		  3.4,    0,  5,
		  3.6,    0,  5,
		  3.8,    0,  5,
		 -4,      0, 20,
		  4.2,    0,  5,
		  4.4,    0,  5,
		  4.6,    0,  5,
		  4.8,    0,  5,
		 -5,      0, 10,
		  6,      0, 20,
		  7,      0, 10,
		  8,      0, 20,
		  9,      0, 10,
		 -10,     0, 50,
		  12,     0, 20,
		  14,     0, 20,
		  16,     0, 20,
		  18,     0, 20,
		 -20,     0, 50,
		  50,     0,100,
		  unset, unset, unset};


    for(int i=0; X[i] != unset; i += 3)
    {
	if(!minor_lines && X[i]>0) continue;
	double center, radius;
	const_X_circle_params(::fabs(X[i]),center,radius);
	cmp Gamma1 = to_smith(cmp(::fabs(X[i+1]),::fabs(X[i])),1);
	cmp Gamma2 = to_smith(cmp(::fabs(X[i+2]),::fabs(X[i])),1);
	double phi1 = ::atan2(Gamma1.imag()-center,Gamma1.real()-1.0);
	if(phi1<0) phi1 += 2*PI;
	double phi2 = ::atan2(Gamma2.imag()-center,Gamma2.real()-1.0);
	if(phi2<0) phi2 += 2*PI;

	length lw = linewidth1;

	if(X[i]<0)
	{
	    double a = atan2(Gamma1.imag(),Gamma1.real());
	    lw = linewidth2;
	    var ll = textsize & var(" ") & ::fabs(X[i]);
	    double label_angle = min(phi1,phi2)*180/PI+90;
	    if(::fabs(X[i])>1.1) label_angle -= 180;
	    label::fdraw(ll,
			 x1len(cos(a))-0.55*width(ll)*cos(a),
			 y1len(sin(a))-0.55*width(ll)*sin(a))
		.textcolor(label_color).angle(label_angle)
		.align(sym::center,sym::center);
	    label::fdraw(ll,
			 x1len(cos(a))-0.55*width(ll)*cos(a),
			 y1len(-sin(a))+0.55*width(ll)*sin(a))
		.textcolor(label_color).angle(-label_angle)
		.align(sym::center,sym::center);
	}
	double start = min(phi1,phi2);
	double stop  = max(phi1,phi2);

	if(::fabs(X[i])>0.30001)
	{
	    arc::fdraw(x1len(1),y1len(center),x1len(0,radius),y1len(0,radius),start*180/PI,stop*180/PI)
		.linecolor(chart_color);
	    arc::fdraw(x1len(1),y1len(-center),x1len(0,radius),y1len(0,radius),360-stop*180/PI,360-start*180/PI)
		.linecolor(chart_color);
	}
	else
	{
	    // otherwise we need to make a discrete sampling, the huge radius of these
	    // circles causes somehow that the start/stop angles are not precise enough,
	    // the arcs do not start/finish at the const-R circles
	    fdraw_arc(1,center,radius,start,stop).linecolor(chart_color);
	    fdraw_arc(1,-center,radius,2*PI-stop,2*PI-start).linecolor(chart_color);
	}
    }

    // plot the horizontal line through the origin
    line::fdraw(x1len(-1),y1len(0))(x1len(1),y1len(0)); //.linecolor(chart_color);  // why the hell does it not work?

    // mark the center with a point
    arc::fdraw(x1len(0),y1len(0),x1len(0,0.005),y1len(0,0.005)).fillcolor(chart_color).linecolor(chart_color).fill(true);

    double ticlength = 0.35;

    // draw the angle tics
    {
	for(int angle=-170; angle<=180; angle += 10)
	{
	    double a = angle/180.0*PI;
	    line::fdraw
		(x1len(cos(a)),
		 y1len(sin(a)))
		(x1len(cos(a))+ticlength*!height_0*cos(a),
		 y1len(sin(a))+ticlength*!height_0*sin(a)).linecolor(angle_scale_color);

	    var lab = textsize & (angle==180?"$\\pm$":" ") & var(angle);
	    label::fdraw(lab,
			 x1len(cos(a))+
			 !height_0*cos(a),
			 y1len(sin(a))+
			 !height_0*sin(a))
		.angle(angle-90).align(sym::center,sym::center).textcolor(angle_scale_color);

	}
    }

    // Now draw the circles around the unit circle (indicating
    // wavelengths, angles, etc....)
    arc::fdraw(x1len(0),y1len(0),x1len(0,1)+2*!height_0);
    arc::fdraw(x1len(0),y1len(0),x1len(0,1)+6*!height_0);
    arc::fdraw(x1len(0),y1len(0),x1len(0,1)+4*!height_0).linecolor(wavelength_scale_color);
    {
	for(double wl=0; wl<0.501; wl += 0.01)
	{
	    double a = PI-wl/0.5*2*PI;
	    line::fdraw
		(x1len(cos(a))+(4-ticlength)*!height_0*cos(a),
		 y1len(sin(a))+(4-ticlength)*!height_0*sin(a))
		(x1len(cos(a))+(4+ticlength)*!height_0*cos(a),
		 y1len(sin(a))+(4+ticlength)*!height_0*sin(a))
		.linecolor(wavelength_scale_color);
	    if(wl>0.045)
	    {
		label::fdraw(textsize & var(" ") & wl,
			     x1len(cos(a))+4.95*!height_0*cos(a),
			     y1len(sin(a))+4.95*!height_0*sin(a))
		    .angle(a*180/PI-90).align(sym::center,sym::center)
		    .textcolor(wavelength_scale_color);
	    }
	    if(wl<0.451)
	    {
		label::fdraw(textsize & var(" ") & (::fabs(0.5-wl)<0.0001?0:0.5-wl),
			     x1len(cos(a))+3.05*!height_0*cos(a),
			     y1len(sin(a))+3.05*!height_0*sin(a))
		    .angle(a*180/PI-90).align(sym::center,sym::center)
		    .textcolor(wavelength_scale_color);
	    }
	}

	// print it all capital letters. otherwise spacing will be wrong,
	// due to different heights of lowercase letters
	string wl_toward_generator = "> WAVELENGTH TOWARD GENERATOR>";
	for(int i=0; i<wl_toward_generator.size(); ++i)
	{
	    double deg = 180-3-i;
	    double rad = deg/180*PI;
	    var ll = wl_toward_generator[i];
	    if(wl_toward_generator[i] == '<') ll = "$<$";
	    if(wl_toward_generator[i] == '>') ll = "$>$";
	    label::fdraw(textsize & var("\\tt ") & ll,
			 x1len(cos(rad))+4.95*!height_0*cos(rad),
			 y1len(sin(rad))+4.95*!height_0*sin(rad)).
			 align(sym::center,sym::center).angle(deg-90)
			 .textcolor(wavelength_scale_color);
	}
	string wl_toward_load = "< WAVELENGTH TOWARD LOAD <";
	for(int i=0; i<wl_toward_load.size(); ++i)
	{
	    double deg = 180+3+wl_toward_load.size()-i;
	    double rad = deg/180*PI;
	    var ll = wl_toward_load[i];
	    if(wl_toward_load[i] == '<') ll = "$<$";
	    if(wl_toward_load[i] == '>') ll = "$>$";
	    label::fdraw(textsize & var("\\tt ") & ll,
			 x1len(cos(rad))+3.05*!height_0*cos(rad),
			 y1len(sin(rad))+3.05*!height_0*sin(rad)).
			 align(sym::center,sym::center).angle(deg-90)
			 .textcolor(wavelength_scale_color);
	}

    }

    // plot the R=0 (abs(Z)=1) circle
    arc::fdraw(x1len(0),y1len(0),x1len(0,1),y1len(0,1));
}


