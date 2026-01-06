#include "fft.h"
#include "warning.h"
#include "config.h"

#ifdef HAVE_GSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft.h>
#include <gsl/gsl_fft_complex.h>
#endif

namespace blop
{

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

#ifdef HAVE_GSL
bool fft(unsigned long int nn, double data[], int isign)
{
    gsl_fft_direction direction =
#ifdef HAVE_GSL_FFT_FORWARD
	(isign==1?gsl_fft_forward:gsl_fft_backward);
#else
	(isign==1?forward:backward);
#endif    

    unsigned long int mask = 1;
    while(mask < nn) mask<<=1;

    // power of 2
    if(mask == nn)
    {
	gsl_fft_complex_radix2_transform(data,1,nn,direction);
    }
    // not a power of 2, use general algorithm
    else
    {
       gsl_fft_complex_wavetable * wavetable = gsl_fft_complex_wavetable_alloc(nn);
       gsl_fft_complex_workspace * workspace = gsl_fft_complex_workspace_alloc (nn);
       gsl_fft_complex_transform (data, 1, nn, wavetable, workspace, direction);
       gsl_fft_complex_wavetable_free (wavetable);
       gsl_fft_complex_workspace_free (workspace);
    }


    const double norm = 1.0/::sqrt((double)nn);
    for(unsigned int i=0; i<2*nn; ++i) data[i] *= norm;

    return true;
}

#else


// 1D FFT algorithm
// Replaces data[0..2*nn-1] by its discrete Fourier transform
// if isign is 1, or replaces by the inverse discrete Fourier transform,
// if isign is -1.
// data[] is interpreted as follows: data[0] = real part of point #1,
// data[1] = imag part of point #1, data[2] = real part of point #2,
// data[3] = imag part of point #2, etc
// nn must be an integer power of 2 (this is not checked!)
// only change in the code below compared to the nr original
// code: indexes are decremented by 1 to have 0-based indexes
// (nr uses 1-based indexes)
bool fft(unsigned long int nn, double data[], int isign)
{
     isign *= -1;

    // check if nn = power of 2
    {
	unsigned long int mask = 1;
	while(mask < nn) mask<<=1;
	if(nn != mask)
	{
	    warning::print("Number of points must be 2^n for the Numerical Recipes "
			   "FFT algorithm. Install the GNU Scientific Library development "
			   "package (libgsl-dev, or libgsl0-dev, etc) and recompile "
			   "blop (./configure; make; make doc; make install) to be able "
			   "to use an arbitrary-length FFT");
	    return false;
	}
    }

    unsigned long int n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;

    n = nn<<1;
    j=1;
    for(i=1; i<n; i+=2)
    {
	if(j>i)
	{
	    swap(data[j-1],data[i-1]);
	    swap(data[j+1-1], data[i+1-1]);
	}
	m=nn;
	while(m>=2 && j>m)
	{
	    j -= m;
	    m >>= 1;
	}
	j += m;
    }
    mmax=2;
    while(n>mmax)
    {
	istep=mmax<<1;
	theta=isign*(6.28318530717959/mmax);
	wtemp=::sin(0.5*theta);
	wpr=-2.0*wtemp*wtemp;
	wpi=::sin(theta);
	wr=1.0;
	wi=0.0;
	for(m=1; m<mmax; m+=2)
	{
	    for(i=m; i<=n; i+=istep)
	    {
		j=i+mmax;
		tempr=wr*data[j-1]-wi*data[j+1-1];
		tempi=wr*data[j+1-1]+wi*data[j-1];
		data[j-1]=data[i-1]-tempr;
		data[j+1-1]=data[i+1-1]-tempi;
		data[i-1] += tempr;
		data[i+1-1] += tempi;
	    }
	    wr=(wtemp=wr)*wpr-wi*wpi+wr;
	    wi=wi*wpr+wtemp*wpi+wi;
	}
	mmax=istep;
    }

    const double norm = 1.0/::sqrt((double)nn);
    for(unsigned int i=0; i<2*nn; ++i) data[i] *= norm;

    return true;
}
#endif

//html <a name='fft_from_file'></a>
bool fft(const var &filename, dgraph &out, int direction,
	 const function &col1, const function &col2, const function  &col3)
{
    dgraph in;
    in.read(filename,col1,col2,col3);
    return fft(in,out,direction);
}

//html <a name='fft_from_dgraph'></a>
bool fft(const dgraph &in, dgraph &out, int direction)
{
    const unsigned long int n = in.size();

    double *data = new double[2*n];

    for(unsigned int i=0; i<n; ++i)
    {
//	data[2*i]   = (*in.get(i))[1].dbl();
//	data[2*i+1] = (in.columns()>2 ? (*in.get(i))[2].dbl() : 0);
	REAL(data,i)   = (*in.get(i))[1].dbl();
	IMAG(data,i) = (in.columns()>2 ? (*in.get(i))[2].dbl() : 0);
    }
    const double dx = ((*in.get(n-1))[0].dbl() - (*in.get(0))[0].dbl())/(n-1);
    const double dx_out = 1.0/(dx*n);

    fft(n,data,direction);

    const double tf0 = in[0][0].dbl();

    if(out.columns() != 3) out.columns(3);
    if(out.size() != n) out.resize(n);

    // if input is not shifted
    if(tf0==0.0)
    {
	for(unsigned int i=0; i<n; ++i)
	{
	    out[i][0] = i*dx_out;
	    out[i][1] = REAL(data,i);
	    out[i][2] = IMAG(data,i);
	}
    }
    else
    {
	for(unsigned int i=0; i<n; ++i)
	{
	    const double x = i*dx_out;
	    out[i][0] = x;

	    const double re_shift = ::cos(-2*cons::pi*direction*x*tf0);
	    const double im_shift = ::sin(-2*cons::pi*direction*x*tf0);

//	    const double re = data[2*i];
//	    const double im = data[2*i+1];
	    const double re = REAL(data,i);
	    const double im = IMAG(data,i);

	    out[i][1] = re*re_shift - im*im_shift;
	    out[i][2] = re*im_shift + im*re_shift;

	}
    }

    delete [] data;
    return true;
}

//html <a name='fft_from_function'></a>
bool fft(const function &f, int n, double x1, double x2, dgraph &out, int direction)
{
    dgraph in;
    in.columns(f.components()+1);
    in.resize(n);
    const double dx = (x2-x1)/n;
    std::vector<blop::var> args(1);
    const function ff(_1,f);
    for(int i=0; i<n; ++i)
    {
        args[0] = x1+i*dx;
        ff.meval(args,in[i]);
    }

    return fft(in,out,direction);
}


}
