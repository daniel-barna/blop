#ifndef __BLOP_SET_H__
#define __BLOP_SET_H__

/*
  Since one has access to all public member functions of all classes in
  blop, one can do everything, which has been implemented. However, this
  would be sometimes too much typing, for example: 

  frame::current().x1axis()->title("This is the x axis");

  Therefore wrapper functions for these most common methods of classes are
  provided. In order to logically group them, they are implemented within
  the 'set' namespace, for example:

  set::xtitle("This is the x axis");

*/

#include "frame.h"
#include "mframe.h"
#include "canvas.h"
#include "global.h"
#include "line.h"
#include "length.h"
#include "bloputils.h"
#include "sym.h"

namespace blop
{
    // dummy class to be returned, upon which G__ateval will replot the canvas
    class replot_request {};

    namespace set
    {
	inline replot_request  clear()
	    {
		canvas::current().clear();
		return replot_request();
	    }

	//html <a name='margins'>
	// --------------- Set the current frame's margins ---------------
	inline replot_request lmargin(const length &l)
	{
	    frame::current().lmargin(l);
	    return replot_request();
	}
	inline replot_request rmargin(const length &l)
	{
	    frame::current().rmargin(l);
	    return replot_request();
	}
	inline replot_request bmargin(const length &l)
	{
	    frame::current().bmargin(l);
	    return replot_request();
	}
	inline replot_request tmargin(const length &l)
	{
	    frame::current().tmargin(l);
	    return replot_request();
	}

	//html </a>


	//html <a name='clear'> 
	// --------- Clear the current canvas ----------------------------
	// (delete every content, for which the 'autodel' flag is set)
	//html </a>

	// Clear all graphs from the current frame. If the flag 'all' is set
	// to true, also the permanent graphs are deleted
	inline replot_request clear_graphs(bool all=false)
	    {
		frame::current().clear_graphs(all);
		return replot_request();
	    }

	//html <a name='axiscolor'> </a>
	// ------ set the color of a given axis of the current frame -----

	inline replot_request x1color(const color &c)
	  { frame::current().x1axis()->axiscolor(c); return replot_request(); }
	inline replot_request y1color(const color &c)
	  { frame::current().y1axis()->axiscolor(c); return replot_request(); }
	inline replot_request x2color(const color &c)
	  { frame::current().x2axis()->axiscolor(c); return replot_request(); }
	inline replot_request y2color(const color &c)
	  { frame::current().y2axis()->axiscolor(c); return replot_request(); }


	//html <a name='axiscut'> </a>
	// ---------- Set cuts on the axes of the current frame ----------
	inline replot_request x1cut(double v1, double v2)
	{
	    frame::current().x1axis()->cut(v1,v2);
	    return replot_request();
	}
	inline replot_request xcut(double v1, double v2)
	{
	    frame::current().x1axis()->cut(v1,v2);
	    return replot_request();
	}
	inline replot_request y1cut(double v1, double v2)
	{
	    frame::current().y1axis()->cut(v1,v2);
	    return replot_request();
	}
	inline replot_request ycut(double v1, double v2)
	{
	    frame::current().y1axis()->cut(v1,v2);
	    return replot_request();
	}
	inline replot_request x2cut(double v1, double v2)
	{
	    frame::current().x2axis()->cut(v1,v2);
	    return replot_request();
	}
	inline replot_request y2cut(double v1, double v2)
	{
	    frame::current().y2axis()->cut(v1,v2);
	    return replot_request();
	}



	//html <a name='axistitle'> </a>
	// ----- Set the title of the axes of the current frame  ---------

	inline replot_request  x1title(const var &t)
	    {
		frame::current().x1axis()->title(t);
		return replot_request();
	    }
	inline replot_request  y1title(const var &t)
	    {
		frame::current().y1axis()->title(t);
		return replot_request();
	    }
	inline replot_request  x2title(const var &t)
	    {
		frame::current().x2axis()->title(t);
		return replot_request();
	    }
	inline replot_request  y2title(const var &t)
	    {
		frame::current().y2axis()->title(t);
		return replot_request();
	    }
	inline replot_request  xtitle(const var &t)
	    {
		x1title(t);
		return replot_request();
	    }
        inline replot_request x1title(const var &t, const var &u, const var &format=unit::default_format())
        {
            frame::current().x1axis()->title(t);
            frame::current().x1axis()->unit(u,format);
            return replot_request();
        }
        inline replot_request x2title(const var &t, const var &u, const var &format=unit::default_format())
        {
            frame::current().x2axis()->title(t);
            frame::current().x2axis()->unit(u,format);
            return replot_request();
        }
        inline replot_request y1title(const var &t, const var &u, const var &format=unit::default_format())
        {
            frame::current().y1axis()->title(t);
            frame::current().y1axis()->unit(u,format);
            return replot_request();
        }
        inline replot_request y2title(const var &t, const var &u, const var &format=unit::default_format())
        {
            frame::current().y2axis()->title(t);
            frame::current().y2axis()->unit(u,format);
            return replot_request();
        }
        inline replot_request xtitle(const var &t, const var &u, const var &format=unit::default_format())
        {
            frame::current().x1axis()->title(t);
            frame::current().x1axis()->unit(u,format);
            return replot_request();
        }
        inline replot_request ytitle(const var &t, const var &u, const var &format=unit::default_format())
        {
            frame::current().y1axis()->title(t);
            frame::current().y1axis()->unit(u,format);
            return replot_request();
        }


	inline replot_request  ytitle(const var &t)
	    {
		y1title(t);
		return replot_request();
	    }

        inline replot_request x1title(const var &t, sym::position pos, sym::position align)
        {
            frame::current().x1axis()->title(t);
            frame::current().x1axis()->title_pos(pos,align);
            return replot_request();
        }
        inline replot_request x2title(const var &t, sym::position pos, sym::position align)
        {
            frame::current().x2axis()->title(t);
            frame::current().x2axis()->title_pos(pos,align);
            return replot_request();
        }
        inline replot_request y1title(const var &t, sym::position pos, sym::position align)
        {
            frame::current().y1axis()->title(t);
            frame::current().y1axis()->title_pos(pos,align);
            return replot_request();
        }
        inline replot_request y2title(const var &t, sym::position pos, sym::position align)
        {
            frame::current().y2axis()->title(t);
            frame::current().y2axis()->title_pos(pos,align);
            return replot_request();
        }
        inline replot_request xtitle(const var &t, sym::position pos, sym::position align)
        {
            x1title(t,pos,align);
            return replot_request();
        }
        inline replot_request ytitle(const var &t, sym::position pos, sym::position align)
        {
            y1title(t,pos,align);
            return replot_request();
        }


	inline replot_request x1label(const var &t) { return x1title(t); }  // These are provided to have more closely
	inline replot_request x2label(const var &t) { return x2title(t); }  // mimic Gnuplot's naming scheme
	inline replot_request y1label(const var &t) { return y1title(t); }
	inline replot_request y2label(const var &t) { return y2title(t); }
	inline replot_request xlabel (const var &t) { return x1title(t); }
	inline replot_request ylabel (const var &t) { return y1title(t); }
	

        //html <a name='mirror'>
        inline replot_request mirror_x1(bool f) { frame::current().mirror_x1(f); return replot_request(); }
        inline replot_request mirror_x2(bool f) { frame::current().mirror_x2(f); return replot_request(); }
        inline replot_request mirror_y1(bool f) { frame::current().mirror_y1(f); return replot_request(); }
        inline replot_request mirror_y2(bool f) { frame::current().mirror_y2(f); return replot_request(); }
        //html </a>

	//html <a name='range'> </a>
	// ---- Set the range of the axes of the current frame  ----------------------

	inline replot_request x1range(double a=unset,double b=unset)
	    {
		frame::current().x1axis()->range(a,b);
		return replot_request();
	    }
	inline replot_request x2range(double a=unset,double b=unset)
	    {
		frame::current().x2axis()->range(a,b);
		return replot_request();
	    }
	inline replot_request y1range(double a=unset,double b=unset)
	    {
		frame::current().y1axis()->range(a,b);
		return replot_request();
	    }
	inline replot_request y2range(double a=unset,double b=unset)
	    {
		frame::current().y2axis()->range(a,b);
		return replot_request();
	    }
	inline replot_request xrange(double a=unset,double b=unset)
	    {
		x1range(a,b);
		return replot_request();
	    }
	inline replot_request yrange(double a=unset,double b=unset)
	    {
		y1range(a,b);
		return replot_request();
	    }

	inline void x1min(double a=unset) { frame::current().x1axis()->min(a); }
	inline void x2min(double a=unset) { frame::current().x2axis()->min(a); }
	inline void y1min(double a=unset) { frame::current().y1axis()->min(a); }
	inline void y2min(double a=unset) { frame::current().y2axis()->min(a); }
	inline void xmin(double a=unset)  { x1min(a); }
	inline void ymin(double a=unset)  { y1min(a); }

	inline void x1max(double a=unset) { frame::current().x1axis()->max(a); }
	inline void x2max(double a=unset) { frame::current().x2axis()->max(a); }
	inline void y1max(double a=unset) { frame::current().y1axis()->max(a); }
	inline void y2max(double a=unset) { frame::current().y2axis()->max(a); }
	inline void xmax(double a=unset)  { x1max(a); }
	inline void ymax(double a=unset)  { y1max(a); }

	//html <a name='nodistort'>
	inline replot_request nodistort(bool f=true)
	    {
		frame::current().nodistort(f);
		return replot_request();
	    }
	//html </a>

        //html <a name="symmetric_range">
        // Enforce that the range of the given axis is symmetric around 0
        inline replot_request symmetric_range_x1(bool s)
        {
            frame::current().x1axis()->symmetric_range(s);
            return replot_request();
        }
        inline replot_request symmetric_range_y1(bool s)
        {
            frame::current().y1axis()->symmetric_range(s);
            return replot_request();
        }
        inline replot_request symmetric_range_x2(bool s)
        {
            frame::current().x2axis()->symmetric_range(s);
            return replot_request();
        }
        inline replot_request symmetric_range_y2(bool s)
        {
            frame::current().y2axis()->symmetric_range(s);
            return replot_request();
        }
        inline replot_request symmetric_range_x(bool s)
        {
            return symmetric_range_x1(s);
        }
        inline replot_request symmetric_range_y(bool s)
        {
            return symmetric_range_y1(s);
        }
        //html </a>

	//html <a name='unit'> </a>
	// --------------  set axis units ------------------------------------
	inline replot_request xunit(const var &u, const var &format=unit::default_format())
	{
	    frame::current().x1axis()->unit(u,format);
	    return replot_request();
	}
	inline replot_request yunit(const var &u, const var &format=unit::default_format())
	{
	    frame::current().y1axis()->unit(u,format);
	    return replot_request();
	}
        // Set both x and y units to the same
        inline replot_request xyunit(const var &u, const var &format=unit::default_format())
        {
	    frame::current().x1axis()->unit(u,format);
	    frame::current().y1axis()->unit(u,format);
            return replot_request();
        }
	inline replot_request x1unit(const var &u, const var &format=unit::default_format())
	{
	    frame::current().x1axis()->unit(u,format);
	    return replot_request();
	}
	inline replot_request y1unit(const var &u, const var &format=unit::default_format())
	{
	    frame::current().y1axis()->unit(u,format);
	    return replot_request();
	}
	inline replot_request x2unit(const var &u, const var &format=unit::default_format())
	{
	    frame::current().x2axis()->unit(u,format);
	    return replot_request();
	}
	inline replot_request y2unit(const var &u, const var &format=unit::default_format())
	{
	    frame::current().y2axis()->unit(u,format);
	    return replot_request();
	}

	//html <a name='scale'> </a>
	// ------- set axis scales --------------------------------------------

	inline replot_request xscale(const var &scale)
	    {
		frame::current().x1axis()->scale(scale);
		return replot_request();
	    }
	inline replot_request yscale(const var &scale)
	    {
		frame::current().y1axis()->scale(scale);
		return replot_request();
	    }
	inline replot_request x1scale(const var &scale)
	    {
		frame::current().x1axis()->scale(scale);
		return replot_request();
	    }
	inline replot_request y1scale(const var &scale)
	    {
		frame::current().y1axis()->scale(scale);
		return replot_request();
	    }
	inline replot_request x2scale(const var &scale)
	    {
		frame::current().x2axis()->scale(scale);
		return replot_request();
	    }
	inline replot_request y2scale(const var &scale)
	    {
		frame::current().y2axis()->scale(scale);
		return replot_request();
	    }


	//html <a name='grid'> </a>
	// ------- Grid control -----------------------------------------------
	// Switch on-off grid drawing for the current frame (grid), or permanently, for any
	// future frames (Grid)

	inline replot_request  grid(int which_directions)
	    {
                if(frame *f = frame::check_current()) f->grid(which_directions);
		else frame::default_grid(which_directions);
		return replot_request();
	    }
	inline replot_request grid(bool g=true)
	    {
                if(frame *f = frame::check_current())
                {
                    if(g) f->grid(axis::x1|axis::y1);
                    else  f->grid(0);
                }
                else
                {
                    if(g) frame::default_grid(axis::x1|axis::y1);
                    else  frame::default_grid(0);
                }
		return replot_request();
	    }
        inline replot_request grid_at_minor_tics(int which_directions)
            {
                if(frame *f = frame::check_current()) f->grid_at_minor_tics(which_directions);
                else frame::default_grid_at_minor_tics(which_directions);
                return replot_request();
            }
        inline replot_request grid_at_minor_tics(bool g)
            {
                if(frame *f = frame::check_current())
                {
                    if(g) f->grid_at_minor_tics(axis::x1|axis::y1);
                    else  f->grid_at_minor_tics(0);
                }
                else
                {
                    if(g) frame::default_grid_at_minor_tics(axis::x1|axis::y1);
                    else  frame::default_grid_at_minor_tics(0);
                }
                return replot_request();
            }
	inline replot_request default_grid(int which_directions)
	    {
		frame::default_grid(which_directions);
		return replot_request();
	    }
	inline replot_request default_grid(bool f)
	    {
		if(f)
		{
		    frame::default_grid(axis::x1|axis::y1);
		}
		else
		{
		    frame::default_grid(0);
		}
		return replot_request();
	    }

        // --------------  set the width of major/minor gridlines -------------

        inline replot_request gridwidth(const length &l)
        {
            if(frame *f = frame::check_current()) f->gridwidth(l);
            else frame::default_gridwidth(l);
            return replot_request();
        }
        inline replot_request minor_gridwidth(const length &l)
        {
            if(frame *f = frame::check_current()) f->minor_gridwidth(l);
            else frame::default_minor_gridwidth(l);
            return replot_request();
        }

        // ----------------  set the color of the major/minor gridlines -------

        inline replot_request gridcolor(const color &c)
        {
            if(frame *f = frame::check_current()) f->gridcolor(c);
            else frame::default_gridcolor(c);
            return replot_request();
        }
        inline replot_request minor_gridcolor(const color &c)
        {
            if(frame *f = frame::check_current()) f->minor_gridcolor(c);
            else frame::default_minor_gridcolor(c);
            return replot_request();
        }

        inline void default_gridcolor(const color &c)
        {
            frame::default_gridcolor(c);
        }
        inline void default_minor_gridcolor(const color &c)
        {
            frame::default_minor_gridcolor(c);
        }


        // -----------  set the linestyle of minor/major gridlines -----------

        inline replot_request gridstyle(sym::linestyle s)
        {
            if(frame *f = frame::check_current()) f->gridstyle(s);
            else frame::default_gridstyle(s);
            return replot_request();
        }
        inline replot_request minor_gridstyle(sym::linestyle s)
        {
            if(frame *f = frame::check_current()) f->minor_gridstyle(s);
            else frame::default_minor_gridstyle(s);
            return replot_request();
        }
        inline void default_gridstyle(sym::linestyle s)
        {
            frame::default_gridstyle(s);
        }
        inline void default_minor_gridstyle(sym::linestyle s)
        {
            frame::default_minor_gridstyle(s);
        }

	//html <a name='title'> </a>
	// -------  Set title  -------------------------------------------
        // If called within a subframe of an mframe, sets the title of the mframe
        // If called within a standalone frame, sets the title of the frame
        // Otherwise sets the title of the current pad (or canvas)

	inline replot_request title(const var &t)
	    {
                // If there is a current frame...
                if(frame *f = frame::check_current())
                {
                    // check if it is part of an mframe. If yes, set the title of the mframe
                    if(mframe *mf = dynamic_cast<mframe*>(f->parent()))
                    {
                        mf->title(t);
                    }
                    else // otherwise set the title of the frame
                    {
                        f->title(t);
                    }
                }
                else
                {
                    // If there is no current frame, 
                    pad::current().title(t);
                }
		return replot_request();
	    }

        inline replot_request canvastitle(const var &t)
        {
            canvas::current().title(t);
            return replot_request();
        }
        inline replot_request canvas_title(const var &t)
        {
            canvas::current().title(t);
            return replot_request();
        }

        inline replot_request padtitle(const var &t)
        {
            pad::current().title(t);
            return replot_request();
        }
        inline replot_request pad_title(const var &t)
        {
            pad::current().title(t);
            return replot_request();
        }

        //html <a name='legend_sample_length'>
        inline replot_request legend_sample_length(const length &len)
        {
            if(legendbox *l = frame::current().legend()) l->sample_length(len);
            return replot_request();
        }


	//html <a name='multilegend'> </a>
	// -------  Set multilegends on/off  ----------------------------------

	inline void multilegend(bool t) { frame::current().legend()->multi(t); }

	//html <a name='legendpos'> </a>
	// -------  Set legend position of current frame ----------------------

	inline replot_request legendpos(sym::position xpos, sym::position ypos)
	    {
		frame::current().legendpos(xpos,ypos);
		return replot_request();
	    }
        
        //html <a name="fill_legend">
        // -------------  Switch on-off filling of the legendbox --------------
        // If we are within an active frame, the legendbox of that frame is affected.
        // Otherwise this flag is set globally, and all subsequent frames/legendboxes
        // will be affected.

        inline replot_request fill_legend(bool f)
        {
            if(frame *current_frame = frame::check_current())
            {
                if(legendbox *l = current_frame->legend()) l->fill(f);
            }
            else legendbox::default_fill(f);
            return replot_request();
        }
        //html </a>

	// ------ Switch to logscale/nologscale -------------------------------

	//html <a name='logscale'> </a>
	inline replot_request logscale_x1(bool t)
	    {
		frame::current().x1axis()->logscale(t);
		return replot_request();
	    }
	inline replot_request logscale_x2(bool t)
	    {
		frame::current().x2axis()->logscale(t);
		return replot_request();
		
	    }
	inline replot_request logscale_y1(bool t)
	    {
		frame::current().y1axis()->logscale(t);
		return replot_request();
	    }
	inline replot_request logscale_y2(bool t)
	    {
		frame::current().y2axis()->logscale(t);
		return replot_request();
	    }
	inline replot_request logscale_x (bool t)
	    {
		frame::current().x1axis()->logscale(t);
		return replot_request();
	    }
	inline replot_request logscale_y (bool t)
	    {
		frame::current().y1axis()->logscale(t);
		return replot_request();
	    }


	// ------  Set the axis tics  -----------------------------------------

	extern tic_setter xtics, ytics, x1tics, x2tics, y1tics, y2tics;

        inline replot_request ticlength(const length &l)
        {
            frame::current().ticlength(l);
            return replot_request();
        }
        inline replot_request minor_ticlength(const length &l)
        {
            frame::current().minor_ticlength(l);
            return replot_request();
        }

        // ---------------------------------------------------------------
        // These functions specify the number of intervals between major tics

        inline replot_request n_minor_tics_x1(int n)
        {
            frame::current().x1axis()->n_minor_tics(n);
            frame::current().x1axis()->draw_minor_tics(true);
            return replot_request();
        }
        inline replot_request n_minor_tics_y1(int n)
        {
            frame::current().y1axis()->n_minor_tics(n);
            frame::current().y1axis()->draw_minor_tics(true);
            return replot_request();
        }
        inline replot_request n_minor_tics_x(int n)
        {
            n_minor_tics_x1(n);
            return replot_request();
        }
        inline replot_request n_minor_tics_y(int n)
        {
            n_minor_tics_y1(n);
            return replot_request();
        }
        inline replot_request n_minor_tics_x2(int n)
        {
            frame::current().x2axis()->n_minor_tics(n);
            frame::current().x2axis()->draw_minor_tics(true);
            return replot_request();
        }
        inline replot_request n_minor_tics_y2(int n)
        {
            frame::current().y2axis()->n_minor_tics(n);
            frame::current().y2axis()->draw_minor_tics(true);
            return replot_request();
        }
        inline replot_request n_minor_tics(int n)
        {
            n_minor_tics_x1(n);
            n_minor_tics_y1(n);
            return replot_request();
        }

	// -------- ticformat  ------------------------------------------------

        inline replot_request ticformat(const var &f)
        {
            frame::current().x1axis()->ticformat(f);
            frame::current().x2axis()->ticformat(f);
            frame::current().y1axis()->ticformat(f);
            frame::current().y2axis()->ticformat(f);
            return replot_request();
        }



	inline replot_request x1ticformat(const var &f)
	{
	    frame::current().x1axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request xticformat(const var &f)
	{
	    frame::current().x1axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request y1ticformat(const var &f)
	{
	    frame::current().y1axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request yticformat(const var &f)
	{
	    frame::current().y1axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request x2ticformat(const var &f)
	{
	    frame::current().x2axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request y2ticformat(const var &f)
	{
	    frame::current().y2axis()->ticformat(f);
	    return replot_request();
	}

	inline replot_request x1ticformat(const char *f) {return x1ticformat(var(f));}
	inline replot_request xticformat(const char *f)  {return xticformat(var(f));}
	inline replot_request y1ticformat(const char *f) {return y1ticformat(var(f));}
	inline replot_request yticformat(const char *f)  {return yticformat(var(f));}
	inline replot_request x2ticformat(const char *f) {return x2ticformat(var(f));}
	inline replot_request y2ticformat(const char *f) {return y2ticformat(var(f));}

	inline replot_request x1ticformat(const std::string &f) {return x1ticformat(var(f));}
	inline replot_request xticformat(const std::string &f) {return xticformat(var(f));}
	inline replot_request y1ticformat(const std::string &f) {return y1ticformat(var(f));}
	inline replot_request yticformat(const std::string &f) {return yticformat(var(f));}
	inline replot_request x2ticformat(const std::string &f) {return x2ticformat(var(f));}
	inline replot_request y2ticformat(const std::string &f) {return y2ticformat(var(f));}
        
	inline replot_request x1ticformat(const blop::function &f)
	{
	    frame::current().x1axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request xticformat(const blop::function &f)
	{
	    frame::current().x1axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request y1ticformat(const blop::function &f)
	{
	    frame::current().y1axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request yticformat(const blop::function &f)
	{
	    frame::current().y1axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request x2ticformat(const blop::function &f)
	{
	    frame::current().x2axis()->ticformat(f);
	    return replot_request();
	}
	inline replot_request y2ticformat(const blop::function &f)
	{
	    frame::current().y2axis()->ticformat(f);
	    return replot_request();
	}

        
	//html <a name='frame_foreground'> </a>
	//html <a name='grid_foreground'> </a>
	// -----  Draw the grid/frame in the foreground (on top of graphs) --------
	inline replot_request grid_foreground(bool f)
	    { frame::current().grid_foreground(f); return replot_request(); }
	inline replot_request frame_foreground(bool f)
	    { frame::current().foreground(f); return replot_request(); }

	// ------  transform axis  --------------------------------------------
	// y2transform, for example, sets the y2 axis of the current frame to
	// be a transformed version of the y1 axis

	replot_request y2transform(const function &to, const function &back,bool own_tics=false);
	replot_request x2transform(const function &to, const function &back,bool own_tics=false);
	replot_request y1transform(const function &to, const function &back,bool own_tics=false);
	replot_request x1transform(const function &to, const function &back,bool own_tics=false);

	inline void debug(int d) { blop::global::debug = d; }

        // Draw a horizontal or vertical marker line at a given value of the specified axis
        inline line &aline(double d, int ax)
        {
            if(ax==axis::x1) return line::fdraw(x1len(d),0.0)(x1len(d),1.0);
            if(ax==axis::x2) return line::fdraw(x2len(d),0.0)(x2len(d),1.0);
            if(ax==axis::y1) return line::fdraw(0.0,y1len(d))(1.0,y1len(d));
            if(ax==axis::y2) return line::fdraw(0.0,y2len(d))(1.0,y2len(d));
            static line the_line;
            return the_line;
        }
#ifndef __MAKECINT__
        auto aline(double d,int ax, var label, const length &x=-0.5*EX, const length &y=0.05, sym::position halign=sym::right, sym::position valign=sym::bottom, double angle=unset);
#endif

        inline line &vline(double d) {return line::fdraw(x1len(d),0.0)(x1len(d),1.0);}
        inline line &v1line(double d) {return line::fdraw(x1len(d),0.0)(x1len(d),1.0);}
        inline line &v2line(double d) {return line::fdraw(x2len(d),0.0)(x2len(d),1.0);}
#ifndef __MAKECINT__
        inline auto vline(double d,var label, const length &dx=-0.5*EX, const length &y=0.05, sym::position halign=sym::right, sym::position valign=sym::bottom, double a=90*unit::deg)
        {
            struct _ { blop::line &the_line; blop::label &the_label; };
            return _ {line::fdraw(x1len(d),0.0)(x1len(d),1.0),
                      label::fdraw(label,x1len(d)+dx,y).align(halign,valign).angle(a)};
        }
#endif

        inline grobs vlines(const blop::array &values)
        {
            grobs result;
            for(unsigned int i=0; i<values.size(); ++i)
            {
                result.add(&line::fdraw(x1len(values[i].dbl()),0.0)(x1len(values[i].dbl()),1.0));
            }
            return result;
        }
        inline grobs vlines(const std::vector<double> &values)
        {
            grobs result;
            for(unsigned int i=0; i<values.size(); ++i)
            {
                result.add(&line::fdraw(x1len(values[i]),0.0)(x1len(values[i]),1.0));
            }
            return result;
        }
        inline grobs vlines(const std::valarray<double> &values)
        {
            grobs result;
            for(unsigned int i=0; i<values.size(); ++i)
            {
                result.add(&line::fdraw(x1len(values[i]),0.0)(x1len(values[i]),1.0));
            }
            return result;
        }
#ifndef __MAKECINT__
        inline grobs vlines(const std::initializer_list<double> values)
        {
            std::vector<double> v = values;
            return vlines(v);
        }
#endif

        inline line &hline(double d) {return line::fdraw(0.0,y1len(d))(1.0,y1len(d));}
        inline line &h1line(double d) {return line::fdraw(0.0,y1len(d))(1.0,y1len(d));}
        inline line &h2line(double d) {return line::fdraw(0.0,y2len(d))(1.0,y2len(d));}
#ifndef __MAKECINT__
        inline auto hline(double d,var label, const length &x=0.05, const length &dy=0.5*EX, sym::position halign=sym::left, sym::position valign=sym::bottom, double a=0)
        {
            struct _ { blop::line &the_line; blop::label &the_label; };
            return _ {line::fdraw(0.0,y1len(d))(1.0,y1len(d)),
                      label::fdraw(label,x,y1len(d)+dy).align(halign,valign).angle(a)};
        }
#endif

        inline grobs hlines(const blop::array &values)
        {
            grobs result;
            for(unsigned int i=0; i<values.size(); ++i) 
            {
                result.add(&line::fdraw(0.0,y1len(values[i].dbl()))(1.0,y1len(values[i].dbl())));
            }
            return result;
        }
        inline grobs hlines(const std::vector<double> &values)
        {
            grobs result;
            for(unsigned int i=0; i<values.size(); ++i)
            {
                result.add(&line::fdraw(0.0,y1len(values[i]))(1.0,y1len(values[i])));
            }
            return result;
        }
        inline grobs hlines(const std::valarray<double> &values)
        {
            grobs result;
            for(unsigned int i=0; i<values.size(); ++i)
            {
                result.add(&line::fdraw(0.0,y1len(values[i]))(1.0,y1len(values[i])));
            }
            return result;
        }
#ifndef __MAKECINT__
        inline grobs hlines(const std::initializer_list<double> values)
        {
            std::vector<double> v = values;
            return hlines(v);
        }
#endif
    }
}

#endif
