#ifndef __BLOP_FFT_H__
#define __BLOP_FFT_H__

#include "dgraph.h"

namespace blop
{
    // ------   FFT for high-level objects  ---------------------------
    bool fft(const dgraph &in, dgraph &out, int direction=1);
    bool fft(const function &f, int n, double x1, double x2, dgraph &out, int direction=1);
    bool fft(const var &filename, dgraph &out, int direction=1, 
	     const function &time_or_freq=unset, const function &real=unset, const function &imag=unset);

    // ------   IN-PLACE (!) fft of raw data  -------------------------

    // packed_data[2*i  ] = RE(data[i]);
    // packed_data[2*i+1] = IM(data[i]);
    // the same array contains the transformed data after the call

    bool fft(unsigned long int n, double packed_data[], int direction = +1);
}

#endif
