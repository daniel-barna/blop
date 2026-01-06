/*
  This example shows how to plot a complex function. It plots
  the complex impedance of an RLC resonator on the Smith-chart.
  See the file smith.h in this same directory also!
 */

#include "smith.h"

const double C = 10e-6;
const double R = 50;
const double L = 10e-9;
const double w0 = 1/sqrt(L*C);
const double f0 = 2*3.1415*w0;
const double X  = sqrt(L/C);
const double Q  = R/X;
const double bandwidth_w = w0/Q;
const double bandwidth_f = f0/Q;

complex<double> Z_smith(double w)
{
    const complex<double> i(0,1);
    return to_smith(1/(1.0/(i*w*L)+i*w*C+1.0/R));
}

int main()
{
    smith_chart();
    plot(cfunc(Z_smith)).p1range(w0-3*bandwidth_w, w0+3*bandwidth_w)
	.legend("RLC resonator");
    eps::print("RLC-resonator.eps","-w 14cm -h 14cm");
    return 0;
}
