#include "tic.h"
#include "warning.h"
#include <utility>
#include "constants.h"
#include <cmath>
#include <string.h>
#include <cstdio>
#include "global.h"
#include "units.h"
#include <limits>

using namespace std;

namespace blop
{
    tic::tic(const tic &o)
    {
	value_ = o.value_;
	label_ = o.label_;
    }

    bool tic::operator== (const tic &o) const
    {
	return value_ == o.value_ && label_ == o.label_;
    }


void calculate_tics(double &minimum, const bool minfixed, 
		    double &maximum, const bool maxfixed, 
		    double &step, const bool stepfixed, 
		    double tic_min, double tic_max,       // range for the tics, unset if not restricted
		    blop::tic &scale, 
		    double Unit,
		    const std::vector< std::pair<double,double> > &cuts, 
		    const bool logscale, 
		    const int normalform_tics, 
		    const bool normalform_scale, 
		    std::vector<blop::tic> &tics, 
		    const char *tic_format, 
                    const blop::function &tic_format_func,
		    const char *scale_format, 
		    const int nmintics,
                    bool symmetric_range)
{
    if(global::debug>0)
    {
	cout<<"[blop] [DEBUG] calculate_tics starts..."<<endl;
	cout<<"[blop] [DEBUG]   minimum  = "<<minimum<<endl;
	cout<<"[blop] [DEBUG]   minfixed = "<<minfixed<<endl;
	cout<<"[blop] [DEBUG]   maximum  = "<<maximum<<endl;
	cout<<"[blop] [DEBUG]   maxfixed = "<<maxfixed<<endl;
	cout<<"[blop] [DEBUG]   step     = "<<step<<endl;
	cout<<"[blop] [DEBUG]   stepfixed= "<<stepfixed<<endl;
	cout<<"[blop] [DEBUG]   tic_min  = "<<tic_min<<endl;
	cout<<"[blop] [DEBUG]   tic_max  = "<<tic_max<<endl;
	cout<<"[blop] [DEBUG]   ..."<<endl;
    }

    const bool scaleingisused = (scale.value() != unset);

    if(maximum!=unset && minimum!=unset)
    {
	if (maximum<minimum)
	{
	    warning::print("maximum<minimum","calculate_tics");
	    return;
	}
	if(maximum==minimum)
	{
	    warning::print("Zero range","calculate_tics");
            if(::fabs(minimum)>1)
            {
                const double tmp = minimum;
                minimum = tmp-::fabs(tmp)/10;
                maximum = tmp+::fabs(tmp)/10;
            }
            else
            {
                minimum = -1;
                maximum =  1;
            }
	}
    }
    
    if ( logscale==true && !(minimum>0.0) )
    {
	warning::print("minimum<=0 with logscale", "calculate_tics");
	return;
    }
    if ( logscale==true && !(maximum>0.0) )
    {
	warning::print("maximum<=0 with logscale", "calculate_tics");
	return;
    }
    if ( stepfixed==true && !(step>0.0) )
    {
	warning::print("ticstep<=0", "calculate_tics");
	return;
    }
    if ( scaleingisused==true && !(scale.value()>0) )
    {
	warning::print("scale<=0", "calculate_tics");
	return;
    }
    for ( unsigned int i=0 ; i<cuts.size() ; ++i )
    {
	if ( ! (cuts[i].first<cuts[i].second) )
	{
	    warning::print("The lower border of a cut domain should be"
			   " smaller than the higher border",
			   "calculate_tics");
	    return;
	}
	for ( unsigned int j=i+1 ; j<cuts.size() ; ++j )
	{
	    if ( ( cuts[i].first<=cuts[j].first && cuts[j].first<=cuts[i].second )
              || ( cuts[i].first<=cuts[j].second && cuts[j].second<=cuts[i].second ) )
	    {
		warning::print("There is overlap in cut domains", "calculate_tics(...)");
		return;
	    }
	}
    }
    const int Nmin=nmintics-1;
    const int Nmax=Nmin*2+1;
    double Scale=scale.value();
    if ( !scaleingisused ) Scale=1.0;

    tics.clear();

    if ( logscale==true )
    {
	double diff=::log10(maximum/minimum);
	for ( unsigned int i=0 ; i<cuts.size() ; ++i )
	{
	    if ( cuts[i].first<minimum && cuts[i].second<minimum ) continue;
	    else if ( cuts[i].first<minimum && minimum<cuts[i].second && cuts[i].second<maximum )
		diff-=(double)(::log10(cuts[i].second/minimum));
	    else if ( minimum<cuts[i].first && cuts[i].first<maximum && minimum<cuts[i].second && cuts[i].second<maximum )
		diff-=(double)(::log10(cuts[i].second/cuts[i].first));
	    else if ( minimum<cuts[i].first && cuts[i].first<maximum && maximum<cuts[i].second )
		diff-=(double)(::log10(maximum/cuts[i].first));
	    else continue;
	}

	// First calculate the stepsize (i.e. the factor tic[i+1]/tic[i])
	// This should be in normal cases 10^1 (i.e. tic[1]=1x10^1, tic[2]=1x10^2, etc)
	// but in those cases, where the axis range is very long, this would result in
	// too many tics, so the step should be increased to a higher power of 10, for example
	// 10^2.
	if ( stepfixed==false )
	{
	    step=::pow(10.0, ::floor(::log10(diff)));
	    int Ntics=(int)(diff/step);
	    while ( Ntics<Nmin || Ntics>Nmax )
	    { 
		if ( Ntics<Nmin ) step*=0.5;
		else if ( Ntics>Nmax ) step*=2;
		Ntics=(int)(diff/step);
	    }
	    double stepExp=step;
	    double stepMant=::pow(10.0, stepExp-::floor(stepExp));
	    step=(stepExp<1.0 ? 10.0 : 
		(stepMant<5.0 ? 1.0 : 10.0)*::pow(10.0, ::floor(stepExp)));
	    if ( !(stepExp<1.0) && (int)(diff/step)>Nmax ) step=::pow(10.0, ::floor(stepExp)+1.0);
	    else if ( !(stepExp<1.0) && (int)(diff/step)<Nmin ) step=::pow(10.0, ::floor(stepExp));
	}

	// If minimum is not fixed, extend it downwards to be 1x10^n (n=integer)
	double minimum_scaled=minimum/Scale/Unit;
	if ( minfixed==false )
	{
	    double minExp=::floor(::log10(minimum_scaled)/::log10(step))*::log10(step);
	    minimum_scaled=::pow(10.0, ::floor(minExp));
	    minimum=minimum_scaled*Scale*Unit;
	}

	// If maximum is not fixed, extend it upwards to be 1x10^n (n=integer)
	double maximum_scaled=maximum/Scale/Unit;
	if ( maxfixed==false )
	{
	    double maxExp=::ceil(::log10(maximum_scaled)/::log10(step))*::log10(step);
	    maximum_scaled=::pow(10.0, ::floor(maxExp));
	    maximum=maximum_scaled*Scale*Unit;
	}
	var tic_string;
	if ( scaleingisused==true )
	{
	    if ( normalform_scale==true )
	    {
		double scaleexp=::log10(Scale);
		double scalemant=::pow(10.0, scaleexp-::floor(scaleexp));
		tic_string = var::sprintf(scale_format, scalemant);
		if ( tic_string == "1" ) tic_string = var::sprintf("$10^{%g}$", ::floor(scaleexp));
		else                     tic_string = var::sprintf(((const var)"$"&scale_format&"\\cdot10^{%g}$").c_str(), scalemant, ::floor(scaleexp));
	    }
	    else tic_string = var::sprintf(scale_format, Scale);
	    scale.label(tic_string);
	}

	// Start from the smallest 1x10^n value >= than minimum
	double mytic = ::pow(10.0,::ceil(::log10(minimum_scaled*0.9999)));

	double epsilon=::pow(step, 0.1);

        double min_tic = numeric_limits<double>::max(), max_tic = numeric_limits<double>::min();
        for(double t = ::pow(10.0,::ceil(::log10(minimum_scaled*0.9999))); t<maximum_scaled*epsilon && tics.size()<50; t*=step)
        {
            if(t<min_tic) min_tic = t;
            if(t>max_tic) max_tic = t;
        }

	while ( mytic<maximum_scaled*epsilon && tics.size()<50 )
	{
	    bool cutflag=false;
	    for ( unsigned int i=0 ; i<cuts.size() ; ++i )
	    {
		if ( cuts[i].first<=mytic*Scale*Unit && mytic*Scale*Unit<=cuts[i].second )
		{
		    cutflag=true;
		    break;
		}
	    }
	    if ( !cutflag )
	    {
		if ( normalform_tics==1 || (normalform_tics==2 && ( min_tic<0.01 || 10<max_tic) )  )
		{
		    double myticexp=::log10(mytic);
                    if(::abs(myticexp)<1e-3) myticexp=0;
		    double myticmant=::pow(10.0, myticexp-::floor(myticexp));
                    if(tic_format_func.initialized())
                    {
                        tic_string = tic_format_func(mytic);
                    }
                    else
                    {
                        tic_string = var::sprintf(tic_format, myticmant);
                        if ( tic_string == "1" ) tic_string = var::sprintf("$10^{%g}$", ::floor(myticexp));
                        else                     tic_string = var::sprintf(((const var)"$"&tic_format&"\\cdot10^{%g}$").c_str(), myticmant, ::floor(myticexp));
                    }
		}
		else
                {
                    if(tic_format_func.initialized()) tic_string = tic_format_func(mytic);
                    else                              tic_string = var::sprintf(tic_format, mytic);
                }

		if(mytic*Scale*Unit > maximum)
		{
		    if(!maxfixed)
		    {
			maximum = mytic*Scale*Unit;
			tics.push_back(tic(mytic*Scale*Unit, tic_string));
		    }
		}
		else tics.push_back(tic(mytic*Scale*Unit, tic_string));
	    }
	    mytic*=step;
	}
    }
    else
    {
	double diff=maximum-minimum;
	for ( unsigned int i=0 ; i<cuts.size() ; ++i )
	{
	    if ( cuts[i].first<minimum && cuts[i].second<minimum ) continue;
	    else if ( cuts[i].first<minimum && minimum<cuts[i].second && cuts[i].second<maximum ) diff-=cuts[i].second-minimum;
	    else if ( minimum<cuts[i].first && cuts[i].first<maximum && minimum<cuts[i].second && cuts[i].second<maximum ) diff-=cuts[i].second-cuts[i].first;
	    else if ( minimum<cuts[i].first && cuts[i].first<maximum && maximum<cuts[i].second ) diff-=maximum-cuts[i].first;
	    else continue;
	}
	diff/=(Scale*Unit);
	double step_scaled=step/(Scale*Unit);
	double minimum_scaled=minimum/(Scale*Unit);
	double maximum_scaled=maximum/(Scale*Unit);

        // If the maximum and minimum are symmetric, try to make a symmetric range, and symmetric tics (i.e. one being at 0)
        if( (maximum*minimum<0 && ::fabs(::fabs(maximum)-::fabs(minimum))<0.01*(maximum-minimum)) || symmetric_range )
        {
            double sym_max = ::max(::fabs(maximum),::fabs(minimum))/(Scale*Unit);
            if(tic_min!=unset) sym_max = ::max(sym_max,tic_min);
            if(tic_max!=unset) sym_max = ::max(sym_max,tic_max);
            
            if(stepfixed==false)
            {
                step_scaled=::pow(10.0, ::floor(::log10(sym_max)));
                int Ntics=(int)(sym_max/step_scaled*2-1);
                while ( Ntics<Nmin || Ntics>Nmax )
                {
                    if ( Ntics<Nmin ) step_scaled*=0.5;
                    else if ( Ntics>Nmax ) step_scaled*=2;
                    Ntics=(int)(sym_max/step_scaled*2-1);
                }
                step=step_scaled*Scale*Unit;
            }

            sym_max = ::ceil(sym_max/step_scaled)*step_scaled;

            if (!minfixed)
            {
                minimum_scaled = -sym_max;
                minimum=minimum_scaled*Scale*Unit;
                if(tic_min!=unset && tic_min<minimum)
                {
                    minimum=tic_min;
                    minimum_scaled=minimum/(Scale*Unit);
                }
            }
            if (!maxfixed)
            {
                maximum_scaled = sym_max;
                maximum=maximum_scaled*Scale*Unit;
                if(tic_max!=unset && tic_max>maximum)
                {
                    maximum=tic_max;
                    maximum_scaled=maximum/(Scale*Unit);
                }
            }
            if(tic_min==unset)
            {
                const int i = ::ceil(minimum/step);
                if(::fabs((i-1)*step-minimum)<0.001*(maximum-minimum)) tic_min = (i-1)*step;
                else tic_min = i*step;
            }
            if(tic_max==unset) tic_max=maximum;
        }
        else
        {
            if ( stepfixed==false )
            {
                step_scaled=::pow(10.0, ::floor(::log10(diff)));
                int Ntics=(int)(diff/step_scaled);
                while ( Ntics<Nmin || Ntics>Nmax )
                {
                    if ( Ntics<Nmin ) step_scaled*=0.5;
                    else if ( Ntics>Nmax ) step_scaled*=2;
                    Ntics=(int)(diff/step_scaled);
                }
                step=step_scaled*Scale*Unit;
            }

            if (!minfixed)
            {
                minimum_scaled=::floor(minimum_scaled/step_scaled)*step_scaled;
                minimum=minimum_scaled*Scale*Unit;
                if(tic_min!=unset && tic_min<minimum)
                {
                    minimum=tic_min;
                    minimum_scaled=minimum/(Scale*Unit);
                }
            }

            if (!maxfixed)
            {
                maximum_scaled=::ceil(maximum_scaled/step_scaled)*step_scaled;
                maximum=maximum_scaled*Scale*Unit;
                if(tic_max!=unset && tic_max>maximum)
                {
                    maximum=tic_max;
                    maximum_scaled=maximum/(Scale*Unit);
                }
            }
            if(tic_min==unset)
            {
                const int i = ::ceil(minimum/step);
                if(::fabs((i-1)*step-minimum)<0.001*(maximum-minimum)) tic_min = (i-1)*step;
                else tic_min = i*step;
            }
            if(tic_max==unset) tic_max = maximum;

        }

        var tic_string;
	if ( scaleingisused==true )
	{
	    if ( normalform_scale==true )
	    {
                double scaleexp=::log10(Scale);
                double scalemant=::pow(10.0, scaleexp-::floor(scaleexp));
                tic_string = var::sprintf(scale_format, scalemant);
                if ( tic_string == "1" ) tic_string = var::sprintf("$10^{%g}$", ::floor(scaleexp));
                else                     tic_string = var::sprintf(((const var)"$"&scale_format&"\\cdot10^{%g}$").c_str(), scalemant, ::floor(scaleexp));
	    }
	    else tic_string = var::sprintf(scale_format, Scale);
	    scale.label(tic_string);
	}

	// set the starting value for the tics.
//	double mytic=minimum;
//	if(tic_min!=unset) mytic = ::max(tic_min,minimum);
        double mytic = tic_min;
	double epsilon=0.1*step;
	for (; mytic<(tic_max!=unset?::min(tic_max,maximum):maximum)+epsilon && tics.size()<50;
	     mytic+=step)
	{
	    bool cutflag=false;
	    for ( unsigned int i=0 ; i<cuts.size() ; ++i )
	    {
		if ( cuts[i].first<=mytic && mytic<=cuts[i].second )
		{
		    cutflag=true;
		    break;
		}
	    }
	    if ( cutflag==false )
	    {
		if ( normalform_tics )
		{
		    if ( mytic==0.0 )
                    {
                        if(tic_format_func.initialized())
                        {
                            tic_string = tic_format_func(0.0);
                        }
                        else
                        {
                            tic_string = var::sprintf((var("$")&tic_format&"$").c_str(), 0.0);
                        }
                    }
		    else
		    {
                        if(tic_format_func.initialized())
                        {
                            tic_string = tic_format_func(mytic/Scale/Unit);
                        }
                        else
                        {
                            double myticexp=::log10(::fabs(mytic/Scale/Unit));
                            double myticmant=(mytic<0.0 ? -1.0 : 1.0)* ::pow(10.0, myticexp-::floor(myticexp));
                            tic_string = var::sprintf(tic_format, myticmant);
                            
                            if      ( tic_string == "1" ) tic_string = var::sprintf("$10^{%g}$", ::floor(myticexp));
                            else if ( tic_string == "-1") tic_string = var::sprintf("$-10^{%g}$", ::floor(myticexp));
                            else                          tic_string = var::sprintf(((const var)"$"&tic_format&"\\cdot10^{%g}$").c_str(), myticmant, ::floor(myticexp));
                        }
		    }
		}
		else
		{
		    // The value 0 is quite often written as 1.734e-18 due to
		    // floating point precision. Check this case
                    if(tic_format_func.initialized())
                    {
                        tic_string = tic_format_func((::fabs(mytic/Scale/Unit)<step_scaled*1e-14?0:mytic/Scale/Unit) );
                    }
                    else
                    {
                        tic_string = var::sprintf(tic_format, (::fabs(mytic/Scale/Unit)<step_scaled*1e-14?0:mytic/Scale/Unit) );
                    }
		}

		// even if the tic position is strictly speaking out of the range
		// allow it if it is not 'too-much' out of range (floating-point
		// imprecisions)
		if(mytic > maximum + (maximum-minimum)*1e-5)
		{
		    if(!maxfixed)
		    {
			maximum = mytic;
			tics.push_back(tic(mytic, tic_string));
		    }
		}
		else tics.push_back(tic(mytic, tic_string));
	    }
	}
    }
    if(tics.size() >= 50)
    {
	warning::print("Number of tics exceeded 50","calculate_tics(...)");
    }
    if(global::debug>0) cout<<"[blop] [DEBUG] calculate_tics finished."<<endl;

}

}
