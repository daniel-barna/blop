#ifndef __BLOP_TERMINAL_H__
#define __BLOP_TERMINAL_H__

#include <string>
#include <vector>
#include "color.h"
#include "sym.h"

#ifdef __CINT__
#pragma define G__NOALGORITHM
#endif


namespace blop
{
    using namespace std;

//    enum {left=1,right=2,bottom=4,top=8,center=16};

    class length;
    class terminal;



    class terminal
	{
	    // base class for all terminals (to which a canvas can be printed)
	    // since this is not meant for use by users, this is somewhat
	    // underdocumented (check if there are any comments at all below...)

	public:



	    class double_id
	    {
	        public:
		    int    id;
		    double value;
                    double_id(double v,int i=0) : id(i), value(v) {}
	    };

	    class id
	    {
		public:
		    int    length_id;
		    double relcoord;
		    int    direction;

	            id() : length_id(0), relcoord(0), direction(0) {}

		    // specify a length from a termspecific_id of length.
	            id(int i) : length_id(i), relcoord(0), direction(0) {}

		    // specify a length as 'd' times the width (if dir==1) or height (if dir==2)
		    // of the current subpicture environment. 
                    id(double d, int dir) : length_id(0), relcoord(d), direction(dir) {} 

		    bool operator== (const blop::terminal::id &o) const;
		    bool operator< (const blop::terminal::id &) const {return false;}
		    bool operator> (const blop::terminal::id &) const {return false;}
	    };

	    class coord
	    {
                public:
		id x,y;

                coord() : x(0), y(0) {}
	        coord(id xx, id yy) : x(xx), y(yy) {}
		bool operator== (const blop::terminal::coord &o) const
		{
		    return x == o.x && y == o.y;
		}
		bool operator< (const blop::terminal::coord &) const { return false; }
		bool operator> (const blop::terminal::coord &) const { return false; }
	    };


            enum {ZERO=1,PW,PH,PS,LW,CM,MM,PT,EM,EX,NOF_UNITS };

	          // NOF_UNITS *must* be the last one !!!

	    // --------------  write a comment into the terminal ----------------
	    // (debugging purposes)

	    virtual void comment(const string &) {}


	    // ---------------  open/close a layer ------------------------------
	    // (conditional drawing: layers can be switched on/off later, when
	    // including into the latex document, etc

	    virtual void open_layer(const var &) = 0;
	    virtual void close_layer(const var &) = 0;


	    virtual int  newlength() = 0;

	    virtual int  overwrite(int i,
				   double f1,const terminal::id &i1) = 0;
	    virtual int  overwrite(int id,
				   double f1,const terminal::id &i1,
				   double f2,const terminal::id &i2) = 0;

	    // --------------  linear combination ------------------------------
	    //  create the linear combination of lengths. the
	    // factors are contained in the array 'f', and 'id'
	    // contains the lengths IDs of the lengths
	    
	    virtual int  lincombi(double_id f,const terminal::id &i) = 0;
	    virtual int  lincombi(double_id f1,const terminal::id &i1,
				  double_id f2,const terminal::id &i2) = 0;
	    virtual int  lincombi(const std::vector<double> &f,
				  const std::vector<blop::terminal::id> &id)  = 0;


	    // --------------- minimum or maximum of listed lengths --------------
	    virtual int  maxlength(const std::vector<blop::terminal::id> &id) = 0;
	    virtual int  minlength(const std::vector<blop::terminal::id> &id) = 0;


	    virtual int  textwidth (const string &,double angle=0) = 0;
	    virtual int  textheight(const string &,double angle=0) = 0;
	    virtual int  textlheight(const string &,double angle=0) = 0;

                  // create a new length, which holds the width or height
                  // of the specified string

	    virtual int  textsize  (const string &,double angle=0) = 0;

                  // create two new lengths, holding the width and height
	          // of the string. the returned length ID corresponds to
	          // the length holding the width, ID+1 corresponds to
	          // the length holding the height

	    virtual int max_textwidth (const std::vector<std::string>&)=0;
	    virtual int max_textheight(const std::vector<std::string>&)=0;
	    virtual int max_textlheight(const std::vector<std::string>&)=0;

	          // create a new length whic is the maximum of the
	          // widths or heights or the provided strings

	    virtual void picture_begin() = 0;
	    virtual void picture_end()   = 0;

	    virtual void subpicture_begin(const coord &lower_left,
					  const coord &upper_right) = 0;
	    virtual void subpicture_end()  = 0;

	    virtual void draw_dot(const coord &pos) = 0;
	    virtual void draw_text(const coord &position,
				   const string &s,
				   int xalign,
				   int yalign,
				   const blop::terminal::id &dx,
				   const blop::terminal::id &dy,
				   bool rotatedsystem = true) = 0;
	    virtual void draw_text(const coord &position,
				   const string &s,
				   int xalign,
				   int yalign,
				   double angle,
				   bool rotatedsystem = true)
	    {
		draw_text(position,s,xalign,yalign,terminal::id(1.0,1),terminal::id(::tan(angle/180.0*3.14159265358979323846),1));
	    }
	    virtual void draw_arc(const coord &center, const id &r,
				  double ang1 = 0,
				  double ang2 = 360) = 0;
	    virtual void fill_arc(const coord &center, const id &r,
				  double ang1 = 0,
				  double ang2 = 360) = 0;

	    virtual void draw_line   (const coord &c1,const coord &c2) = 0;
	    virtual void draw_lines  (const std::vector<blop::terminal::coord> &c) = 0;
	    virtual void draw_rlines (const blop::terminal::coord &start,
				      const std::vector<blop::terminal::id> &x,
				      const std::vector<blop::terminal::id> &y) = 0;
	    virtual void fill_polygon(const std::vector<blop::terminal::coord> &c) = 0;


	    virtual void reset_transformation() = 0;
	    virtual void translate(const id &, const id &) = 0;
	    virtual void rotate   (double angle) = 0;
	    virtual void rotate   (const id &, const id &) = 0;
	    virtual void scale    (double s) = 0;
	    virtual void scale    (double xscale, double yscale) = 0;
	    virtual void scale    (const id &xfrom, const id &xto,
				   const id &yfrom, const id &yto) = 0;

	    //virtual void move_origin(const id &,const id &) = 0;

	    virtual void set_color(const color &c) = 0;
	    virtual void set_linewidth(const id &) = 0;
	    virtual void set_linestyle(sym::linestyle) = 0;


	    virtual int  atan(const id &x,const id &y) = 0;
	    virtual int  add_dbl(const double_id &, const double_id &) = 0;
	    virtual int  sin_dbl(int) = 0;
	    virtual int  cos_dbl(int) = 0;
	    virtual int  neg_dbl(int) = 0;

//	    virtual bool clip(const id &x1, const id &y1,
//			      const id &x2, const id &y2) { return false; }
	    virtual bool clip(const coord &lower_left,
			      const coord &upper_right) { return false; }

	    virtual void noclip() {};

	    virtual ~terminal() {}

	};



}

#endif
