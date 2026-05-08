#ifndef __BLOP_MPPS_H__
#define __BLOP_MPPS_H__

#include "blopeps.h"
#include "var.h"
#include <fstream>
#include <string>

namespace blop
{
    class mpps : public blop::terminal, public std::ofstream
    {
    private:
        int flush_frequency_;
        int pagecount_;
        bool needs_flush_;

	std::string texfilename_, psfilename_;
	blopeps *beps_;
	bool header_printed_;

	static string &default_papersize_();
	string papersize_;

	static bool default_landscape_;
	bool landscape_;

	static int default_verbose_;
	int verbose_;

	static std::string default_dvips_options_;
	std::string dvips_options_;

	static var default_options_;
	var options_, opts_;

        std::string command_;

	void init();

    public:
	mpps();
	mpps(const var &filename);
	virtual ~mpps();

	virtual void open(const var &filename);
	virtual void close();
	virtual bool flush();

        //SEP
        // Return the output filename
        virtual string filename() const { return psfilename_; }

	//SEP
	// The mpps terminal is derived from ofstream. It means, you can
	// write to it using the << operator, for example. 
	// The things you write with this operator go to the LaTeX source
	// file (which then will be compiled to a PostScript file).
	// You can either rely on the default header written by blop
	// (which uses the 'article' documentclass, etc). In this case
	// you do not have to do anything. Just print your figures
	// sequentially to this terminal. Otherwise, if you print your
	// header, do not forget to call this 'header_printed(true)'
	// function to inform the terminal, that it does not have
	// to write the header.

	mpps &header_printed(bool f) { header_printed_ = f; return *this; }


	// If you use the default LaTeX header (i.e. you do not make anything
	// extra...), then you can control the orientation of the page
	// 

	mpps &landscape(bool f) { landscape_ = f; return *this; }
	bool  landscape() const { return landscape_; }
	static void default_landscape(bool f) { default_landscape_ = f; }

	mpps &papersize(const var &s);
	var   papersize() const { return papersize_; }
	static void default_papersize(const var &s) { default_papersize_() = s.str(); }

        //SEP
        // You can specify a LaTeX command, which will be put into the LaTeX file before including
        // any plots
        mpps &command(const var &c) { command_ = c.str(); return *this; }
        var   command() const       { return command_; }

	//SEP
	// You can specify extra dvips options to be used when converting
	// the .dvi file to .ps

	mpps &dvips_options(const var &f) { dvips_options_ = f.str(); return *this; }
	const std::string &dvips_options() const { return dvips_options_; }
	static void default_dvips_options(const var &f) { default_dvips_options_ = f.str(); }


	//SEP
	// Verbose levels: 0 = no LaTeX/dvips messages printed
	//                 1 = LaTeX/dvips output printed, if they fail
	//                 2 = LaTeX/dvips output is printed 
	static void default_verbose(int f) { default_verbose_ = f; }
	mpps &verbose(int f) { verbose_ = f; return *this; }
	int   verbose() const { return verbose_; }


	//SEP
	// with these functions you can 

	static void default_options(const var &f) { default_options_ = f; }
	mpps &options(const var &f) { options_ = f; return *this; }
	const var &options() const { return options_; }

	mpps &print_header();

	void print(const var &opts="");

	void picture_begin();
	void picture_end();

	int newlength();
	int overwrite(int i, double f1, const terminal::id &i1);
	int overwrite(int i,
		      double f1, const terminal::id &i1,
		      double f2, const terminal::id &i2);
	int lincombi(terminal::double_id f, const terminal::id &i);
	int lincombi(terminal::double_id f1, const terminal::id &i1,
		     terminal::double_id f2, const terminal::id &i2);
	int lincombi(const std::vector<double> &f,
		     const std::vector<blop::terminal::id> &id);
	int maxlength(const std::vector<blop::terminal::id> &id);
	int minlength(const std::vector<blop::terminal::id> &id);
	int textwidth(const string &, double angle=0);
	int textheight(const string &, double angle=0);
	int textlheight(const string &, double angle=0);
	int textsize(const string &, double angle=0);
	int max_textwidth(const std::vector<std::string> &);
	int max_textheight(const std::vector<std::string> &);
	int max_textlheight(const std::vector<std::string> &);

	void subpicture_begin(const terminal::coord &lower_left,
			      const terminal::coord &upper_right);
	void subpicture_end();

	void draw_dot(const terminal::coord &c);

	void draw_text(const terminal::coord &pos,
		       const string &s,
		       int xalign,
		       int yalign,
		       const blop::terminal::id &dx,
		       const blop::terminal::id &dy,
		       bool rotatedsystem=true);
	void draw_arc(const terminal::coord &center,
		      const terminal::id &r,
		      double ang1 = 0,
		      double ang2 = 360);
	void fill_arc(const terminal::coord &center,
		      const terminal::id &r,
		      double ang1 = 0,
		      double ang2 = 360);
	
	void draw_line (const terminal::coord &c1,
			const terminal::coord &c2);
	void draw_lines (const std::vector<blop::terminal::coord> &c);
	void draw_rlines(const terminal::coord &start,
			 const std::vector<blop::terminal::id> &dx,
			 const std::vector<blop::terminal::id> &dy);
	void fill_polygon(const std::vector<blop::terminal::coord> &c);
	

	void reset_transformation();
	void translate(const id &, const id &);
	void rotate   (double angle);
	void rotate   (const id &, const id &);
	void scale    (double s);
	void scale    (double xs, double ys);
	void scale    (const id &xfrom, const id &xto,
		       const id &yfrom, const id &yto);

	void set_color(const color &c);
	void set_linewidth(const terminal::id &);
	void set_linestyle(sym::linestyle);
	
	
	int  atan(const terminal::id &x,const terminal::id &y);
	int  add_dbl(const terminal::double_id &, const terminal::double_id &);
	int  sin_dbl(int);
	int  cos_dbl(int);
	int  neg_dbl(int);
	
	bool clip(const terminal::coord &lower_left,
		  const terminal::coord &upper_right);
	
	void noclip();

	void open_layer(const var &);
	void close_layer(const var &);
	
	
    };

}

#endif
