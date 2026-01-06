#ifndef __BLOP_BLOPEPS_H__
#define __BLOP_BLOPEPS_H__

#include "terminal.h"
#include "var.h"
#include "sym.h"
#include <fstream>
#include <stack>

namespace blop
{
    class blopeps : public terminal
	{
	private:
	    // to minimize output file size, write out some definitions only
	    // if they are used. These flags keep track if definitions were written
	    bool Ra_written_, Rt_written_, Sc_written_, Sr_written_;

	    int current_transformation_;
	    enum { rotation_=1, translation_=2, scaling_=4 };

	    int width_,height_;

	    sym::linestyle   current_linestyle_;
	    color current_color_;
	    int   current_linewidth_;

	    int  n_texts_;
	    void convertstr(const std::string &s,int n,std::string &);

	    int max_length_index_;
	    std::string i2length(int i);

	    string preferred_width_, preferred_height_;

	private:
	    std::stack<std::string> coordsys_x,coordsys_y,coordsys_w,coordsys_h;

	    void draw_arc_base(const terminal::coord &center,
			       const terminal::id &r,
			       double ang1, double ang2);

	    // write out a length into the file
	    void write(const terminal::id &, ostream *out = 0);
	    
	    // write out a coordinate, together with all its transformations
	    void write(const terminal::coord &);

	    blopeps(const blopeps &o);

	protected:
	    std::ofstream file_;
	    std::ofstream file_tmp_;
	    std::string tmpfilename_;
	    std::string filename_;

	public:

	    // -------------  Constructor, Dtor, assignment ---------------------------------
	    blopeps(const var &filename,int w=300,int h=200);
	    const blopeps &operator= (const blopeps &o);
	    ~blopeps();


	    // -------------  return the output filename ------------------------------------
	    
	    const string &filename() const { return filename_; }


	    blopeps &preferred_width(const var &v) { preferred_width_ = v.str(); return *this; }
	    blopeps &preferred_height(const var &v) { preferred_height_ = v.str(); return *this; }

	    // ---------------------  layers ---------------------------------------------

	    void open_layer(const var &);
	    void close_layer(const var &);

	    
	    // ------   Print current canvas to a file ------------------------------------
	    // The optional 2nd and 3rd args can be used to specify a default width/height
	    // (to be used by 'beps2eps', if not overwritten by command line arguments)

	    static void print(const var &filename, const var &pref_width="", const var &pref_height="");


	    // -------- Following functions are not supposed to be called by user ----------

	    int atan(const terminal::id &x, const terminal::id &y);
	    int add_dbl(const terminal::double_id &, const terminal::double_id  &);
	    int sin_dbl(int);
	    int cos_dbl(int);
	    int neg_dbl(int);

	    bool clip(const terminal::coord &lower_left,
		      const terminal::coord &upper_right);

	    void noclip();


	    void comment(const string &);

	    int newlength();
	    int overwrite(int id,double f1,const terminal::id &i1);
	    int overwrite(int id,
			  double f1,const terminal::id &i1,
			  double f2,const terminal::id &i2);

	    int lincombi(terminal::double_id f,const terminal::id &i);
	    int lincombi(terminal::double_id f1, const terminal::id &i1,
			 terminal::double_id f2, const terminal::id &i2);
	    int lincombi(const std::vector<double> &f,
			 const std::vector<blop::terminal::id> &ids);

	    int maxlength(const std::vector<blop::terminal::id> &ids);
	    int minlength(const std::vector<blop::terminal::id> &ids);

	    int textwidth(const string &,double angle=0);
	    int textheight(const string &,double angle=0);
	    int textlheight(const string &,double angle=0);
	    int textsize(const string &,double angle=0);
	    
	    int max_textwidth(const std::vector<std::string> &);
	    int max_textheight(const std::vector<std::string> &);
	    int max_textlheight(const std::vector<std::string> &);

	    void picture_begin();
	    void picture_end();

	    void subpicture_begin(const terminal::coord &lower_left,
				  const terminal::coord &upper_right);
	    void subpicture_end();

	    void draw_dot(const terminal::coord &pos);

	    void draw_text(const terminal::coord &pos,
			   const string &,
			   int xalign,int yalign,
			   const blop::terminal::id &dx,
			   const blop::terminal::id &dy,
			   bool rotatedsystem=true);

	    void draw_arc(const terminal::coord &center,
			  const terminal::id &r,
			  double ang1, double ang2);
	    void fill_arc(const terminal::coord &center,
			  const terminal::id &r,
			  double ang1, double ang2);

	    void draw_line(const terminal::coord &c1,
			   const terminal::coord &c2);
	    void draw_lines_base(const std::vector<blop::terminal::coord> &c);
	    void draw_lines     (const std::vector<blop::terminal::coord> &c);
	    void fill_polygon   (const std::vector<blop::terminal::coord> &c);
	    void draw_rlines(const blop::terminal::coord &start,
			     const std::vector<blop::terminal::id> &dx,
			     const std::vector<blop::terminal::id> &dy);

	    void reset_transformation();
	    void translate(const id &, const id &);
	    void rotate   (double angle);
	    void rotate   (const id &, const id &);
	    void scale    (double s);
	    void scale    (double xscale, double yscale);
	    void scale    (const id &xfrom, const id &xto,
			   const id &yfrom, const id &yto);

	    //void move_origin(const terminal::id &,const terminal::id &);

	    void set_color(const color &c);
	    void set_linewidth(const terminal::id &);
	    void set_linestyle(sym::linestyle);

            static std::string filename_extension() { return ".beps"; }
	};

    typedef blopeps beps;


}

#endif
