#ifndef __BLOP_GET_H__
#define __BLOP_GET_H__
#include "frame.h"

/*
Very similar file/class to set.h
Look there fore more details



*/

namespace blop
{
    class get
    {
    public:

	// ----------- get the most recently created plottable -------------
	// (plottable ==> function [fgraph], data [dgraph], histogram [hist]
	// Useful if one wants to update an already (recently) plotted
	// something 
	// 
	// plot(_1, sin(_1));   // plot a sine function
	//  // now, after seeing the result, realize that I want to have it in red:
	// get::last()->ac(red);
	
	static plottable *last() { return frame::current().last(); }

	// ----------- get the last plotted function -----------------------
	// this function may return 0, if no functions have been plotted yet
	// plot(_1, sin(_1));
	//   now, after seeing the result, realize that I wanted to plot
	//   cosine instead:
	// get::lastf()->set(_1,cos(_1));

	static fgraph *lastf() { return frame::current().lastf(); }

	// ----------- get the last plotted dataset (graph) ----------------
	// this funciton may return 0 if no graphs have been plotted yet

	static dgraph *lastd   () { return frame::current().lastd(); }


    };
}

#endif
