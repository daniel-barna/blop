#include "mpps.h"

#include "blop_bookkeeper.h"
#include "canvas.h"
#include "warning.h"
#include "bloputils.h"

namespace blop
{
    using namespace std;

    string &mpps::default_papersize_()
    {
	static string s = "a4";
	return s;
    }
    bool mpps::default_landscape_ = true;
    string mpps::default_dvips_options_ = "";
    var mpps::default_options_ = "width=\\linewidth,height=\\textheight";
    int mpps::default_verbose_ = 1;

    mpps &mpps::papersize(const var &s)
    {
	if(s != "a4" && s != "a3") warning::print("Allowed papersizes: a3, a4");
	else papersize_ = s.str();
	return *this;
    }

    void mpps::print(const var &options)
    {
	if(options.str() == "") opts_ = options_;
	else opts_ = options;
	canvas::current().print(this);
        needs_flush_ = true;
        ++pagecount_;
        if(pagecount_%flush_frequency_ == 0) flush();
    }

    void mpps::init()
    {
        pagecount_ = 0;
        flush_frequency_ = 1;
        needs_flush_ = false;
	texfilename_ = "";
	psfilename_ = "";
	beps_ = 0;
	opts_ = "";
	header_printed_ = false;
	landscape_ = default_landscape_;
	dvips_options_ = default_dvips_options_;
	options_ = default_options_;
	verbose_ = default_verbose_;
	papersize_ = default_papersize_();
    }

    mpps::mpps() 
    {
	init();
    }

    mpps::mpps(const var &filename)
    {
	init();
	mpps::open(filename);
    }


    mpps::~mpps()
    {
	flush();
    }

    void mpps::open(const var & filename)
    {
	psfilename_ = filename.str();
	texfilename_ = blop_bookkeeper::tmpfile("mpps_XXXXXX.tex");
	ofstream::open(texfilename_.c_str());
	beps_ = 0;
	header_printed_ = false;
    }

    void mpps::close()
    {
	if(psfilename_ != "" && texfilename_ != "") flush();
	ofstream::close();
	beps_ = 0;
	psfilename_ = "";
	texfilename_ = "";
	header_printed_ = false;
    }

    bool mpps::flush()
    {
        if(!needs_flush_) return false;
        needs_flush_ = false;

	if(!is_open()) return false;

	if(psfilename_ == "" || texfilename_ == "")
	{
	    warning::print("Use 'open' before you want to flush mpps","mpps::flush()");
	    return false;
	}

	if(!header_printed_)
	{
	    warning::print("Empty mpps file is being flushed","mpps::flush()");
	    return false;
	}

	int pos = ofstream::tellp();

	(*this)<<"\\end{document}"<<endl;
	ofstream::flush();

	var cmd;

	var tmpfilename(texfilename_);

	cmd = "cd `dirname " & tmpfilename & "`; ";
	cmd &= "echo Q | latex " & tmpfilename;

	int latex_rc = 0;
	var latex_output = readcmd(cmd & " 2>&1", &latex_rc);

	if(verbose_ == 0)
        {
	    if(latex_rc != 0) warning::print("LaTeX compilation failed for " & var(psfilename_));
	}
	else if(verbose_ == 1)
	{
	    if(latex_rc != 0)
	    {
		warning::print("LaTeX compilation failed for " & var(psfilename_));
		cerr<<latex_output<<endl;
	    }
	}
	else
	{
	    if(latex_rc != 0) warning::print("LaTeX compilation failed for " & var(psfilename_));
	    cerr<<latex_output<<endl;
	}
	
	if(latex_rc == 0)
	{
	    tmpfilename.replace(".tex",".dvi");
	    cmd = "dvips -o " & psfilename_ & " ";
	    cmd &= dvips_options_;
	    if(landscape_) cmd &= " -t landscape ";
	    cmd &= " ";
	    cmd &= tmpfilename;

	    int dvips_rc = 0;
	    var dvips_output = readcmd(cmd & " 2>&1",&dvips_rc);

	    if(verbose_ == 0)
	    {
		if(dvips_rc != 0) warning::print("dvips failed for " & var(psfilename_));
	    }
	    else if(verbose_ == 1)
	    {
		if(dvips_rc != 0)
		{
		    warning::print("dvips failed for " & var(psfilename_));
		    cerr<<dvips_output<<endl;
		}
	    }
	    else
	    {
		if(dvips_rc != 0) warning::print("dvips failed for " & var(psfilename_));
		cerr<<dvips_output<<endl;
	    }
	}

	ofstream::seekp(pos);
        return true;
    }

    mpps &mpps::print_header()
    {
        needs_flush_ = true;
	(*this)<<"\\documentclass["<<papersize_<<"paper";
	if(landscape_) (*this)<<",landscape";
	(*this)<<"]{article}"<<endl;
	if(landscape_) (*this)<<"\\special{! TeXDict begin /landplus90{true}store end }"<<endl;
	(*this)<<"\\usepackage[hscale=0.95,vscale=0.95]{geometry}"<<endl;
	(*this)<<"\\usepackage{blopeps}"<<endl;
	(*this)<<"\\usepackage{color}"<<endl;
	(*this)<<"\\parindent=0cm"<<endl;
	(*this)<<"\\pagestyle{empty}"<<endl;
	(*this)<<"\\begin{document}"<<endl;
	//(*this)<<"\\LARGE"<<endl;
        if(command_ != "") (*this)<<command_<<endl;
	header_printed_ = true;
	return *this;
    }

    void mpps::open_layer(const var &l)
    {
        needs_flush_ = true;
	if(beps_) beps_->open_layer(l);
    }
    void mpps::close_layer(const var &l)
    {
        needs_flush_ = true;
	if(beps_) beps_->close_layer(l);
    }

    void mpps::picture_begin()
    {
        needs_flush_ = true;
	if(header_printed_ == false) print_header();

	if(beps_) delete beps_;
	beps_ = new blopeps(blop_bookkeeper::tmpfile("mpps_BEPS_XXXXXX.beps"));
	beps_->picture_begin();
    }

    void mpps::picture_end()
    {
        needs_flush_ = true;
	beps_->picture_end();
	(*this)<<"\\blopeps";
	if(opts_.str() != "") (*this)<<"["<<opts_.str()<<"]";
	(*this)<<"{"<<beps_->filename()<<"}"<<endl;
    }
	    
    int mpps::newlength()
    {
        needs_flush_ = true;
        return beps_->newlength();
    }
    int mpps::overwrite(int i, double f1, const terminal::id &i1)
    {
        needs_flush_ = true;
	return beps_->overwrite(i,f1,i1);
    }
    int mpps::overwrite(int i,
			double f1, const terminal::id &i1,
			double f2, const terminal::id &i2)
    {
        needs_flush_ = true;
	return beps_->overwrite(i,f1,i1,f2,i2);
    }
    int mpps::lincombi(double_id f, const terminal::id &i)
    {
        needs_flush_ = true;
	return beps_->lincombi(f,i);
    }
    int mpps::lincombi(double_id f1, const terminal::id &i1,
		       double_id f2, const terminal::id &i2)
    {
        needs_flush_ = true;
	return beps_->lincombi(f1,i1,f2,i2);
    }
    int mpps::lincombi(const std::vector<double> &f, const std::vector<blop::terminal::id> &id)
    {
        needs_flush_ = true;
	return beps_->lincombi(f,id);
    }
    int mpps::maxlength(const std::vector<blop::terminal::id> &id)
    {
        needs_flush_ = true;
	return beps_->maxlength(id);
    }
    int mpps::minlength(const std::vector<blop::terminal::id> &id)
    {
        needs_flush_ = true;
	return beps_->minlength(id);
    }
    int mpps::textwidth(const string &s, double a)
    {
        needs_flush_ = true;
	return beps_->textwidth(s,a);
    }
    int mpps::textheight(const string &s, double a)
    {
        needs_flush_ = true;
	return beps_->textheight(s,a);
    }
    int mpps::textlheight(const string &s, double a)
    {
        needs_flush_ = true;
	return beps_->textlheight(s,a);
    }
    int mpps::textsize(const string &s, double a)
    {
        needs_flush_ = true;
	return beps_->textsize(s,a);
    }
    int mpps::max_textwidth(const std::vector<std::string> &s)
    {
        needs_flush_ = true;
	return beps_->max_textwidth(s);
    }
    int mpps::max_textheight(const std::vector<std::string> &s)
    {
        needs_flush_ = true;
	return beps_->max_textheight(s);
    }
    int mpps::max_textlheight(const std::vector<std::string> &s)
    {
        needs_flush_ = true;
	return beps_->max_textlheight(s);
    }

    void mpps::subpicture_begin(const terminal::coord &lower_left,
				const terminal::coord &upper_right)
    {
        needs_flush_ = true;
	beps_->subpicture_begin(lower_left, upper_right);
    }
    void mpps::subpicture_end()
    {
        needs_flush_ = true;
	beps_->subpicture_end();
    }

    void mpps::draw_dot(const terminal::coord &c)
    {
        needs_flush_ = true;
	beps_->draw_dot(c);
    }
    void mpps::draw_text(const terminal::coord &pos,
			 const string &s,
			 int xalign, int yalign,
			 const blop::terminal::id &angle_dx,
			 const blop::terminal::id &angle_dy,
			 bool rotatedsystem)
    {
        needs_flush_ = true;
	beps_->draw_text(pos,s,xalign,yalign,angle_dx,angle_dy,rotatedsystem);
    }
    void mpps::draw_arc(const terminal::coord &center,
			const id &r,
			double ang1, double ang2)
    {
        needs_flush_ = true;
	beps_->draw_arc(center,r,ang1,ang2);
    }
    void mpps::fill_arc(const terminal::coord &center,
			const id &r, double a1, double a2)
    {
        needs_flush_ = true;
	beps_->fill_arc(center,r,a1,a2);
    }
    void mpps::draw_line(const coord &c1, const coord &c2)
    {
        needs_flush_ = true;
	beps_->draw_line(c1,c2);
    }
    void mpps::draw_lines(const std::vector<blop::terminal::coord> &c)
    {
        needs_flush_ = true;
	beps_->draw_lines(c);
    }
    void mpps::draw_rlines(const terminal::coord &start,
			   const std::vector<blop::terminal::id> &dx,
			   const std::vector<blop::terminal::id> &dy)
    {
        needs_flush_ = true;
	beps_->draw_rlines(start,dx,dy);
    }
    void mpps::fill_polygon(const std::vector<blop::terminal::coord> &c)
    {
        needs_flush_ = true;
	beps_->fill_polygon(c);
    }

    void mpps::reset_transformation()
    {
        needs_flush_ = true;
	beps_->reset_transformation();
    }
    void mpps::translate(const id &x, const id &y)
    {
        needs_flush_ = true;
	beps_->translate(x,y);
    }
    void mpps::rotate(double angle)
    {
        needs_flush_ = true;
	beps_->rotate(angle);
    }
    void mpps::rotate(const id &x, const id &y)
    {
        needs_flush_ = true;
	beps_->rotate(x,y);
    }
    void mpps::scale(double s)
    {
        needs_flush_ = true;
	beps_->scale(s);
    }
    void mpps::scale(double xs, double ys)
    {
        needs_flush_ = true;
	beps_->scale(xs,ys);
    }
    void mpps::scale(const id &xfrom, const id &xto,
		     const id &yfrom, const id &yto)
    {
        needs_flush_ = true;
	beps_->scale(xfrom,xto,yfrom,yto);
    }



    void mpps::set_color(const color &c)
    {
        needs_flush_ = true;
	beps_->set_color(c);
    }
    void mpps::set_linewidth(const id &i)
    {
        needs_flush_ = true;
	beps_->set_linewidth(i);
    }
    void mpps::set_linestyle(sym::linestyle i)
    {
        needs_flush_ = true;
	beps_->set_linestyle(i);
    }

    int mpps::atan(const id &x, const id &y)
    {
        needs_flush_ = true;
	return beps_->atan(x,y);
    }
    int mpps::add_dbl(const double_id &a, const double_id &b)
    {
        needs_flush_ = true;
	return beps_->add_dbl(a,b);
    }
    int mpps::sin_dbl(int i)
    {
        needs_flush_ = true;
	return beps_->sin_dbl(i);
    }
    int mpps::cos_dbl(int i)
    {
        needs_flush_ = true;
	return beps_->cos_dbl(i);
    }
    int mpps::neg_dbl(int i)
    {
        needs_flush_ = true;
	return beps_->neg_dbl(i);
    }
    bool mpps::clip(const terminal::coord &lower_left, const terminal::coord &upper_right)
    {
        needs_flush_ = true;
	return beps_->clip(lower_left, upper_right);
    }
    void mpps::noclip()
    {
        needs_flush_ = true;
	beps_->noclip();
    }

}
