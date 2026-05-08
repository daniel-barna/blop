#include "blopeps.h"
#include "var.h"
#include "exc.hh"
#include "canvas.h"
#include "blop_bookkeeper.h"
#include "units.h"
#include <cstdio>
#include <unistd.h>
#include <iomanip>

#define LAYERS

namespace blop
{
    using namespace std;

    bool blopeps::clip(const terminal::coord &lower_left,
		       const terminal::coord &upper_right)
    {
	write(lower_left);
	write(upper_right);
	file_tmp_<<"cl"<<endl;
	return true;
    }

    void blopeps::noclip()
    {
	file_tmp_<<"initclip"<<endl;
    }

    void blopeps::open_layer(const var & layer)
    {
#ifdef LAYERS
	var ll = layer;
	ll.replace(" ","");
	// check if the given layer is switched on
	file_tmp_<<"/BlopLayer@"<<ll<<" where {pop BlopLayer@"<<ll<<" 1 eq}{false} i"<<endl;

	// check if the parent layer is switched on (i.e. current layer == last switched-on
	// layer)
	file_tmp_<<"BlopLayerLevel BlopLayerOn eq"<<endl;

	// take their 'and'
	file_tmp_<<"and"<<endl;

	// increase the layer level by 1
	file_tmp_<<"/BlopLayerLevel BlopLayerLevel 1 a d"<<endl;

	// if the previous condition was true, set the last-switched-on to the current level
	file_tmp_<<"{/BlopLayerOn BlopLayerLevel d} if"<<endl;
#endif
    }
    void blopeps::close_layer(const var &)
    {
#ifdef LAYERS
	// Decrease BlopLayerLevel by 1
	file_tmp_<<"/BlopLayerLevel BlopLayerLevel -1 a d"<<endl;

	// BlopLayerOn
	file_tmp_<<"BlopLayerOn BlopLayerLevel gt {/BlopLayerOn BlopLayerLevel d} if"<<endl;
#endif
    }
	

    void blopeps::print(const var &name, const var &pref_width, const var &pref_height)
	TRY
    {
	blopeps t(name);
	t.preferred_width(pref_width).preferred_height(pref_height);
	canvas::current().print(&t);
    }
    CATCH("blopeps::print(const var &)")

    string blopeps::i2length(int i)
	TRY
    {
	if(i <= 0 || i > max_length_index_)
	{
	    char s[300];
	    sprintf(s,"Bad length id in blopeps::i2length: %i  [max_length_index_ = %i]",i,max_length_index_);
	    err(s);
	}
	if(i == ZERO) return "@Z0";
	if(i == PW  ) return "@PW";
	if(i == PH  ) return "@PH";
	if(i == PS  ) return "@PS";
	if(i == LW  ) return "@LW";
	if(i == CM  ) return "@CM";
	if(i == MM  ) return "@MM";
	if(i == PT  ) return "@PT";
	if(i == EM  ) return "@EM";
	if(i == EX  ) return "@EX";

	string s = "@";
	for(; i; i /= 26)
	{
	    s += 'a' + i%26;
	}
	return s;
    }
    CATCH("blopeps::i2length(int)")

    blopeps::blopeps(const blopeps &o)
	TRY
    {
	err("this should not be called");
    }
    CATCH("blopeps::blopeps(const blopeps &)")

    const blopeps &blopeps::operator= (const blopeps &)
	TRY
    {
	err("this should not be called");
	return *this;
    }
    CATCH("blopeps::operator= (const blopeps &)")

    blopeps::blopeps(const var &fname,int w,int h)
    {
	width_ = w;
	height_ = h;
	filename_ = fname.str();
	tmpfilename_ = blop_bookkeeper::tmpfile("beps-XXXXXX");

	current_linestyle_ = sym::solid;
	current_linewidth_ = terminal::LW;
	current_color_     = black;
	current_transformation_ = 0;

	Ra_written_ = Rt_written_ = Sc_written_ = Sr_written_ = false;
    }


    blopeps::~blopeps()
    {
    }

    void blopeps::comment(const string &s)
    {
	file_tmp_<<"%-->  "<<s<<endl;
    }

    int blopeps::newlength()
    {
	file_tmp_<<"/"<<i2length(++max_length_index_)<<" 0 d"<<endl;
	return max_length_index_;
    }

    void blopeps::write(const terminal::coord &c)
    {
	write(c.x);
	write(c.y);
	file_tmp_<<"K ";
    }

    void blopeps::write(const terminal::id &i, ostream *out)
    {
	if(!out) out = &file_tmp_;
	// immediate execution
#ifndef LAYERS
	if(i.length_id) (*out)<<"//"<<i2length(i.length_id);
#else
	if(i.length_id) (*out)<<i2length(i.length_id);
#endif	
	else
	{
	    if(i.direction == 1) (*out)<<" "<<i.relcoord<<" X";
	    else                 (*out)<<" "<<i.relcoord<<" Y";
	}
	(*out)<<" ";
    }

    int blopeps::overwrite(int id,double f1,const terminal::id &i1)
    {
	file_tmp_<<"/"<<i2length(id)<<" "<<f1<<" ";
	write(i1);
	file_tmp_<<"m d"<<endl;
	return 1;
    }

    int blopeps::overwrite(int id,double f1,const terminal::id &i1,double f2,const terminal::id &i2)
    {
	file_tmp_<<"/"<<i2length(id)<<" "<<f1<<" ";
	write(i1);
	file_tmp_<<"m "<<f2<<" ";
	write(i2);
	file_tmp_<<"m a d"<<endl;
	return 1;
    }

    int blopeps::lincombi(double_id f,const id &i)
    {
	file_tmp_<<"/"<<i2length(++max_length_index_);

	if(f.id > 0)
	{
	    file_tmp_<<i2length(f.id)<<" ";
	}
	else
	{
	    if(::fabs(f.value) < 0.000001) file_tmp_<<" 0 d"<<endl;
	    else if(::fabs(f.value-1) < 0.000001)
	    {
		file_tmp_<<" ";
		write(i);
		file_tmp_<<"d"<<endl;
	    }
	    else if(::fabs(f.value+1) < 0.000001)
	    {
		file_tmp_<<" ";
		write(i);
		file_tmp_<<"n d"<<endl;
	    }
	    else
	    {
		file_tmp_<<" "<<f.value<<" ";
		write(i);
		file_tmp_<<"m d"<<endl;
	    }
	}
	return max_length_index_;
    }
    
    int blopeps::lincombi(double_id f1,const id &i1,
			  double_id f2,const id &i2)
    {
	if(f1.id == 0 && ::fabs(f1.value) < 0.00001) return lincombi(f2,i2);
	if(f2.id == 0 && ::fabs(f2.value) < 0.00001) return lincombi(f1,i1);
	file_tmp_<<"/"<<i2length(++max_length_index_)<<" ";

	if(f1.id == 0 && ::fabs(f1.value+1) < 0.00001)
	{
	    write(i2);
	    if(f2.id > 0)
	    {
		file_tmp_<<i2length(f2.id)<<" ";
	    }
	    else
	    {
		if(::fabs(f2.value-1) < 0.00001) {}
		else if(::fabs(f2.value+1) < 0.00001) file_tmp_<<"n ";
		else file_tmp_<<f2.value<<" m ";
	    }
	    write(i1);
	    file_tmp_<<"s d"<<endl;
	    return max_length_index_;
	}

	if(f2.id == 0 && ::fabs(f2.value+1) < 0.00001)
	{
	    write(i1);
	    if(f1.id > 0)
	    {
		file_tmp_<<i2length(f1.id)<<" ";
	    }
	    else
	    {
		if(::fabs(f1.value-1) < 0.00001) {}
		else if(::fabs(f1.value+1) < 0.00001) file_tmp_<<"n ";
		else file_tmp_<<f1.value<<" m ";
	    }
	    write(i2);
	    file_tmp_<<"s d"<<endl;
	    return max_length_index_;
	}

	write(i1);

	if(f1.id > 0)
	{
	    file_tmp_<<i2length(f1.id)<<" m ";
	}
	else
	{
	    if(::fabs(f1.value-1) < 0.00001) {}
	    else file_tmp_<<f1.value<<" m ";
	}

	write(i2);

	if(f2.id > 0)
	{
	    file_tmp_<<i2length(f2.id)<<" m ";
	}
	else
	{
	    if(::fabs(f2.value-1) < 0.00001) {}
	    else file_tmp_<<f2.value<<" m ";
	}

	file_tmp_<<"a d"<<endl;

	return max_length_index_;
    }

    int blopeps::lincombi(const std::vector<double> &f,
			  const std::vector<id> &ids)
    {
	if(ids.size() == 1) return lincombi(f[0], ids[0]);
	if(ids.size() == 2) return lincombi(f[0], ids[0], f[1], ids[1]);

	unsigned int written = 0;
	file_tmp_<<"/"<<i2length(++max_length_index_)<<" ";
	for(unsigned int i=0; i<f.size(); ++i)
	{
	    if(::fabs(f[i]) < 0.000001) continue;

	    ++written;

	    write(ids[i]);
	    if(::fabs(f[i]-1) < 0.001) {}
	    else if(::fabs(f[i]+1) < 0.001) {file_tmp_<<"n ";}
	    else file_tmp_<<f[i]<<" m ";
	}

	if(written == 0)
	{
	    file_tmp_<<" 0 d"<<endl;
	}
	else if(written == 1)
	{
	    file_tmp_<<" d"<<endl;
	}
	else
	{
	    for(unsigned int i=0; i<written-1; ++i)
	    {
		file_tmp_<<"a ";
	    }
	    file_tmp_<<"d"<<endl;
	}
	return max_length_index_;
    }

    int blopeps::maxlength(const std::vector<id> &ids)
    {
	if(ids.empty()) return ZERO;

	file_tmp_<<"/"<<i2length(++max_length_index_)
		<<" 0 d";
	for(unsigned int i=0; i<ids.size(); ++i)
	{
	    file_tmp_<<" ";
	    write(ids[i]);
	}
	file_tmp_<<" "<<ids.size()
		<<" { dup "<<i2length(max_length_index_)
		<<" gt { /"<<i2length(max_length_index_)
		<<" exch d}{pop} i} repeat"
		<<endl;

	return max_length_index_;
    }

    int blopeps::minlength(const std::vector<id> &ids)
    {
	if(ids.empty()) return ZERO;

	file_tmp_<<"/"<<i2length(++max_length_index_)
		 <<" ";
	write(ids[0]);
	file_tmp_<<" d";

	if(ids.size() > 1)
	{
	    for(unsigned int i=1; i<ids.size(); ++i)
	    {
		file_tmp_<<" ";
		write(ids[i]);
	    }
	    file_tmp_<<" "<<ids.size()-1
		     <<" { dup "<<i2length(max_length_index_)
		     <<" lt { /"<<i2length(max_length_index_)
		     <<" exch d}{pop} i} repeat"
		     <<endl;
	}
	return max_length_index_;
    }


    int blopeps::max_textwidth(const std::vector<std::string> &s)
    {
	if(s.empty())
	{
	    file_tmp_<<"/"<<i2length(++max_length_index_)<<" 0 d"<<endl;
	    return max_length_index_;
	}

	file_<<"%\\setbox\\blop@tmp@box\\vbox{";
	unsigned int maxsize = 0;
	for(unsigned int i=0; i<s.size(); ++i)
	{
	    file_<<"\\hbox{"<<s[i]<<"}";
	    if(s[i].size() > maxsize) maxsize = s[i].size();
	}
	file_<<"}\\blopdimen{\\wd\\blop@tmp@box}{"<<i2length(++max_length_index_)
	     <<"}\\ignorespaces"<<endl;

	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)
		<<" "<<maxsize*10<<" d} i"<<endl;
	return max_length_index_;
    }

    int blopeps::max_textheight(const std::vector<std::string> &s)
    {
	if(s.empty())
	{
	    file_tmp_<<"/"<<i2length(++max_length_index_)<<" 0 d"<<endl;
	    return max_length_index_;
	}

	file_<<"%\\setbox\\blop@tmp@box\\hbox{";
	for(unsigned int i=0; i<s.size(); ++i)
	{
	    file_<<s[i];
	}
	file_<<"}\\blop@dimen=\\ht\\blop@tmp@box\\advance \\blop@dimen by \\dp\\blop@tmp@box\\blopdimen{\\blop@dimen}{"<<i2length(++max_length_index_)
	     <<"}\\ignorespaces"<<endl;
	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)
		<<" 12 d} i"<<endl;
	return max_length_index_;
    }

    int blopeps::max_textlheight(const std::vector<std::string> &s)
    {
	if(s.empty())
	{
	    file_tmp_<<"/"<<i2length(++max_length_index_)<<" 0 d"<<endl;
	    return max_length_index_;
	}

	file_<<"%\\setbox\\blop@tmp@box\\hbox{";
	for(unsigned int i=0; i<s.size(); ++i)
	{
	    file_<<s[i];
	}
	file_<<"}\\blopdimen{\\ht\\blop@tmp@box}{"<<i2length(++max_length_index_)
	     <<"}\\ignorespaces"<<endl;
	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)
		<<" 12 d} i"<<endl;
	return max_length_index_;
    }

    void blopeps::picture_begin()
    {

/*
  BLOP@CTM refers to the current transformation matrix
  [ a b c d tx ty ]
  Coordinates are calculated by the K operator (defined below) 
  as follows:
  x' = a*x + c*y + tx
  y' = b*x + d*y + ty
 */	

	n_texts_ = 0;

	file_.open(filename_.c_str());
	file_tmp_.open(tmpfilename_.c_str());
	file_tmp_<<setprecision(4);
	n_texts_ = 0;

	Ra_written_ = false;
	Rt_written_ = false;
	Sc_written_ = false;
	Sr_written_ = false;
	current_transformation_ = 0;
	current_color_ = black;
	current_linestyle_ = sym::solid;
	current_linewidth_ = terminal::LW;
	max_length_index_ = NOF_UNITS-1;


	file_<<"%!PS-Adobe-2.0 EPSF-2.0"<<endl;
	file_<<"%%Title: ...."<<endl;
	file_<<"%%Creator: blop (Author: D. Barna, http://blopplot.sourceforge.net)"<<endl;
	file_<<"%%BoundingBox: 0 0 "<<width_<<" "<<height_<<endl;
	file_<<"%%EndComments"<<endl;
	if(preferred_width_ != "") file_<<"%preferred_width "<<preferred_width_<<endl;
	if(preferred_height_ != "") file_<<"%preferred_height "<<preferred_height_<<endl;
	file_<<"%latexsection"<<endl;

	//file_tmp_<<"/blopdict 100 dict def"<<endl;
	//file_tmp_<<"blopdict begin"<<endl;
	file_tmp_<<"/d {def} def"<<endl;
	file_tmp_<<"/i {ifelse} d"<<endl;
	file_tmp_<<"/@PW where {pop}{/@PW "<<width_<<" d} i"<<endl;
	file_tmp_<<"/@PH where {pop}{/@PH "<<height_<<" d} i"<<endl;
	file_tmp_<<"/@PS where {pop}{/@PS 5 d} i"<<endl;
	file_tmp_<<"/@LW where {pop}{/@LW 0.5 d} i"<<endl;
	file_tmp_<<"/@CM 28.452756 d"<<endl;
	file_tmp_<<"/@MM 2.8452756 d"<<endl;
	file_tmp_<<"/@PT 1 d"<<endl;
	file_tmp_<<"/@Z0 0 d"<<endl;
	file_tmp_<<"/@EM where {pop}{/@EM 5 d} i"<<endl;
	file_tmp_<<"/@EX where {pop}{/@EX 5 d} i"<<endl;

	file_tmp_<<"/n {neg} d"<<endl;
	file_tmp_<<"/m {mul} d"<<endl;
	file_tmp_<<"/a {add} d"<<endl;
	file_tmp_<<"/s {sub} d"<<endl;
//	file_tmp_<<"/p {[ currentlinewidth 1.5 m currentlinewidth 3 m] 0 setdash} d"<<endl;  // dotted line
//	file_tmp_<<"/v {[ currentlinewidth 7 m currentlinewidth 5 m] 0 setdash} d"<<endl;    // dashed line
//	file_tmp_<<"/pv {[ currentlinewidth 6 m currentlinewidth 2.5 m currentlinewidth 1.5 m currentlinewidth 2.5 m] 0 setdash} d"<<endl;    // dash-dotted line
//	file_tmp_<<"/ppv {[ currentlinewidth 6 m currentlinewidth 2.5 m currentlinewidth 1.5 m currentlinewidth 2.5 m currentlinewidth 1.5 m currentlinewidth 2.5 m] 0 setdash} d"<<endl;    // dash-doubledotted line
//	file_tmp_<<"/f {[] 0 setdash} d"<<endl;  // solid
	file_tmp_<<"/w {setlinewidth} d"<<endl;
	file_tmp_<<"/c {setrgbcolor} d"<<endl;
	file_tmp_<<"/t {dup where {pop}{10 d} i} d"<<endl;

	// Clipping
	file_tmp_<<"/cl {4 2 roll 2 copy 6 2 roll exch 4 1 roll s 3 1 roll "
                   "exch s exch rectclip} d"<<endl;

	// change the CTM by this translation
	file_tmp_<<"/Tr { /BLOP@CTM BLOP@CTM 4 2 roll [ 1 0 0 1 0 0 ] translate "
                   "[ 1 0 0 1 0 0 ] concatmatrix d } d"<<endl;

	file_tmp_<<"/G {gsave} d"<<endl;
	file_tmp_<<"/E {grestore} d"<<endl;
	file_tmp_<<"/N {newpath} d"<<endl;
	file_tmp_<<"/M {moveto} d"<<endl;

	file_tmp_<<"/L {lineto} d"<<endl;
	file_tmp_<<"/R {rlineto} d"<<endl;
	file_tmp_<<"/Q {/Y2@ exch d /X2@ exch d /Y1@ exch d /X1@ exch d X1@ Y1@ M "
                   "X1@ Y2@ L X2@ Y2@ L X2@ Y1@ L C} d"<<endl;
	//file_tmp_<<"/S {stroke} d"<<endl;
	file_tmp_<<"/S {BlopLayerLevel BlopLayerOn eq {stroke}{newpath} i} d"<<endl;
	//file_tmp_<<"/F {fill} d"<<endl;
	file_tmp_<<"/F {BlopLayerLevel BlopLayerOn eq {fill}{newpath} i} d"<<endl;
	//file_tmp_<<"/T {show} d"<<endl;
	file_tmp_<<"/T {BlopLayerLevel BlopLayerOn eq {show}{pop} i} d"<<endl;

	file_tmp_<<"/C {closepath} d"<<endl;
	file_tmp_<<"/x{exch d}d"<<endl;
	file_tmp_<<"/max{2 copy gt {pop}{exch pop}ifelse}d"<<endl;
	file_tmp_<<"/min{2 copy gt {exch pop}{pop}ifelse}d"<<endl;
	file_tmp_<<"/Daa {G 0 setlinewidth 1 setlinecap [] 0 setdash M 0 0 "
                   "rlineto S E} d"<<endl;
	file_tmp_<<"/X {BLOP@W m} d"<<endl;
	file_tmp_<<"/Y {BLOP@H m} d"<<endl;
	file_tmp_<<"0.9962 0.9962 scale"<<endl;
	file_tmp_<<"@LW w"<<endl;
	file_tmp_<<"/Helvetica findfont 10 scalefont setfont"<<endl;
	file_tmp_<<"/BLOP@X 0 d"<<endl;
	file_tmp_<<"/BLOP@Y 0 d"<<endl;

	file_tmp_<<"/BLOP@CTM { [ 1 0 0 1 0 0 ] } d"<<endl;
	file_tmp_<<"/K { BLOP@CTM transform BLOP@Y a exch BLOP@X a exch } d"<<endl;
	file_tmp_<<"/BLOP@W @PW d"<<endl;
	file_tmp_<<"/BLOP@H @PH d"<<endl;

	// If BlopCalculateBBox is defined (i.e. when we call beps2eps), then
	// draw a line across the whole canvas, so that beps2eps does not crop
	// the empty spaces at the margins. This line is not visible otherwise,
	// because then BlopCalculateBBox is not defined
	file_tmp_<<"/BlopCalculateBBox where {pop 0 0 moveto @PW 0 lineto @PW @PH lineto 0 @PH lineto closepath stroke} if "<<endl;

#ifdef LAYERS
	// Switch on the default layers: frame, legend, .... ?
	// define these to be 1 (unless they have been defined already)
	file_tmp_<<"/BlopLayer@default where {pop}{/BlopLayer@default  1 d} ifelse"<<endl;
	file_tmp_<<"/BlopLayer@frame  where {pop}{/BlopLayer@frame  1 d} ifelse"<<endl;
	file_tmp_<<"/BlopLayer@legend where {pop}{/BlopLayer@legend 1 d} ifelse"<<endl;
	file_tmp_<<"/BlopLayer@grid where {pop}{/BlopLayer@grid 1 d} ifelse"<<endl;
	file_tmp_<<"/BlopLayerLevel 1 d"<<endl;
	file_tmp_<<"/BlopLayerOn 1 d"<<endl;
#endif
        file_tmp_<<"1 setlinejoin"<<endl; // round line joining

	file_tmp_<<"%endsetup"<<endl;
    }
    void blopeps::picture_end() 
    {
	// end blopdict dictionary
	//file_tmp_<<"end"<<endl;

	file_tmp_<<"showpage"<<endl;
	file_tmp_.close();
	file_<<"%endlatexsection"<<endl;
	ifstream in(tmpfilename_.c_str());
	string line;
	while(getline(in,line))
	{
	    file_<<line<<endl;
	}
	in.close();
	file_<<"%--EndOfFile--"<<endl;
	file_.close();
    }

    void blopeps::subpicture_end()
	TRY
    {
	if(coordsys_x.empty())
	    err("Unbalanced subpicture_begin -- subpicture_end");
	coordsys_x.pop();
	coordsys_y.pop();
	coordsys_w.pop();
	coordsys_h.pop();

	if(!coordsys_x.empty())
	{
	    // immediate execution
#ifndef LAYERS
	    file_tmp_<<"/BLOP@X  //"<<coordsys_x.top()<<" d"<<endl;
	    file_tmp_<<"/BLOP@Y  //"<<coordsys_y.top()<<" d"<<endl;
	    file_tmp_<<"/BLOP@W  //"<<coordsys_w.top()<<" d"<<endl;
	    file_tmp_<<"/BLOP@H  //"<<coordsys_h.top()<<" d"<<endl;
#else
	    file_tmp_<<"/BLOP@X  "<<coordsys_x.top()<<" d"<<endl;
	    file_tmp_<<"/BLOP@Y  "<<coordsys_y.top()<<" d"<<endl;
	    file_tmp_<<"/BLOP@W  "<<coordsys_w.top()<<" d"<<endl;
	    file_tmp_<<"/BLOP@H  "<<coordsys_h.top()<<" d"<<endl;
#endif
	}
	else
	{
	    // immediate execution
#ifndef LAYERS
	    file_tmp_<<"/BLOP@X 0 d"<<endl;
	    file_tmp_<<"/BLOP@Y 0 d"<<endl;
	    file_tmp_<<"/BLOP@W //@PW d"<<endl;
	    file_tmp_<<"/BLOP@H //@PH d"<<endl;
#else
	    file_tmp_<<"/BLOP@X 0 d"<<endl;
	    file_tmp_<<"/BLOP@Y 0 d"<<endl;
	    file_tmp_<<"/BLOP@W @PW d"<<endl;
	    file_tmp_<<"/BLOP@H @PH d"<<endl;
#endif
	}
    }
    CATCH("blopeps::subpicture_end()")

    void blopeps::subpicture_begin(const coord &lower_left,
				   const coord &upper_right)
	TRY
    {
	// first calculate the new width and origin
	// of the new coordinate system
	string new_x,new_y,new_w,new_h;
	new_x = i2length(++max_length_index_);
	new_y = i2length(++max_length_index_);
	new_w = i2length(++max_length_index_);
	new_h = i2length(++max_length_index_);

	file_tmp_<<"/"<<new_w<<" ";
	if(lower_left.x.length_id == 0 && upper_right.x.length_id == 0)
	{
	    file_tmp_<<" "<<(upper_right.x.relcoord - lower_left.x.relcoord)
		// immediate execution
#ifndef LAYERS
		    <<" //BLOP@W m d"<<endl;
#else
		    <<" BLOP@W m d"<<endl;
#endif	    
	}
	else if(lower_left.x.length_id == 0)
	{
	    // immediate execution
#ifndef LAYERS
	    file_tmp_<<" //"<<i2length(upper_right.x.length_id)
		    <<" "<<lower_left.x.relcoord<<" //BLOP@W m s d"<<endl;
#else
	    file_tmp_<<" "<<i2length(upper_right.x.length_id)
		    <<" "<<lower_left.x.relcoord<<" BLOP@W m s d"<<endl;
#endif
	}
	else if(upper_right.x.length_id == 0)
	{
	    // immediate execution
#ifndef LAYERS
	    file_tmp_<<" "<<upper_right.x.relcoord<<" //BLOP@W m //"
		    <<i2length(lower_left.x.length_id)<<" s d"<<endl;
#else
	    file_tmp_<<" "<<upper_right.x.relcoord<<" BLOP@W m "
		    <<i2length(lower_left.x.length_id)<<" s d"<<endl;
#endif
	}
	else
	{
	    // immediate execution
#ifndef LAYERS
	    file_tmp_<<" //"<<i2length(upper_right.x.length_id)<<" //"
		    <<i2length(lower_left.x.length_id)<<" s d"<<endl;
#else
	    file_tmp_<<" "<<i2length(upper_right.x.length_id)<<" "
		    <<i2length(lower_left.x.length_id)<<" s d"<<endl;
#endif
	}

	file_tmp_<<"/"<<new_h<<" ";
	if(lower_left.y.length_id == 0 && upper_right.y.length_id == 0)
	{
	    // immediate execution
#ifndef LAYERS
	    file_tmp_<<" "<<(upper_right.y.relcoord - lower_left.y.relcoord)
		    <<" //BLOP@H m d"<<endl;
#else
	    file_tmp_<<" "<<(upper_right.y.relcoord - lower_left.y.relcoord)
		    <<" BLOP@H m d"<<endl;
#endif
	}
	else if(lower_left.y.length_id == 0)
	{
	    // immediate execution
#ifndef LAYERS
	    file_tmp_<<" //"<<i2length(upper_right.y.length_id)
		    <<" "<<lower_left.y.relcoord<<" //BLOP@H m s d"<<endl;
#else
	    file_tmp_<<" "<<i2length(upper_right.y.length_id)
		    <<" "<<lower_left.y.relcoord<<" BLOP@H m s d"<<endl;
#endif
	}
	else if(upper_right.y.length_id == 0)
	{
	    // immediate execution
#ifndef LAYERS
	    file_tmp_<<" "<<upper_right.y.relcoord<<" //BLOP@H m //"
		    <<i2length(lower_left.y.length_id)<<" s d"<<endl;
#else
	    file_tmp_<<" "<<upper_right.y.relcoord<<" BLOP@H m "
		    <<i2length(lower_left.y.length_id)<<" s d"<<endl;
#endif
	}
	else
	{
	    // immediate execution
#ifndef LAYERS
	    file_tmp_<<" //"<<i2length(upper_right.y.length_id)<<" //"
		    <<i2length(lower_left.y.length_id)<<" s d"<<endl;
#else
	    file_tmp_<<" "<<i2length(upper_right.y.length_id)<<" "
		    <<i2length(lower_left.y.length_id)<<" s d"<<endl;
#endif
	}

	// immediate execution
#ifndef LAYERS
	file_tmp_<<"/"<<new_x<<" //BLOP@X ";
#else
	file_tmp_<<"/"<<new_x<<" BLOP@X ";
#endif
	write(lower_left.x);
	file_tmp_<<"a d"<<endl;

	// immediate execution
#ifndef LAYERS
	file_tmp_<<"/"<<new_y<<" //BLOP@Y ";
#else
	file_tmp_<<"/"<<new_y<<" BLOP@Y ";
#endif
	write(lower_left.y);
	file_tmp_<<"a d"<<endl;

	coordsys_x.push(new_x);
	coordsys_y.push(new_y);
	coordsys_w.push(new_w);
	coordsys_h.push(new_h);

	// and now redefine BLOP@X, etc
	// immediate execution
#ifndef LAYERS
	file_tmp_<<"/BLOP@X //"<<new_x<<" d"<<endl;
	file_tmp_<<"/BLOP@Y //"<<new_y<<" d"<<endl;
	file_tmp_<<"/BLOP@W //"<<new_w<<" d"<<endl;
	file_tmp_<<"/BLOP@H //"<<new_h<<" d"<<endl;
#else
	file_tmp_<<"/BLOP@X "<<new_x<<" d"<<endl;
	file_tmp_<<"/BLOP@Y "<<new_y<<" d"<<endl;
	file_tmp_<<"/BLOP@W "<<new_w<<" d"<<endl;
	file_tmp_<<"/BLOP@H "<<new_h<<" d"<<endl;
#endif

    }
    CATCH("blopeps::subpicture_begin(...)")


    /*
    int blopeps::textwidth(const string &s,double angle)
    {
	++max_length_index_;
	if(angle==0)
	{
	    file_<<"%\\setbox\\blop@tmp@box\\hbox{"<<s<<"}"
		 <<"\\blopdimen{\\wd\\blop@tmp@box}{"<<i2length(max_length_index_)<<"}"
		 <<"\\ignorespaces"<<endl;
	}
	else
	{
	    file_<<"%\\setbox\\blop@tmp@box\\hbox{\\rotatebox{"
		<<angle<<"}{"<<s
		<<"}}\\blopdimen{\\wd\\blop@tmp@box}{"<<i2length(max_length_index_)
		<<"}\\ignorespaces"<<endl;
	}
	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)<<" "
		<<10*s.size()<<" d} i"<<endl;
	return max_length_index_;
    }
    */

    int blopeps::textwidth(const string &s,double angle)
    {
	++max_length_index_;
	file_<<"%\\blop@textsize{"<<s<<"}{";
	if(angle == 0) file_<<"@";
//kaki
//	else file_<<angle;
	else file_<<angle/unit::deg;
	file_<<"}{"<<i2length(max_length_index_)<<"}{0}{0}\\ignorespaces"<<endl;
	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)<<" "
		<<10*s.size()<<" d} i"<<endl;
	return max_length_index_;
    }

/*
    int blopeps::textheight(const string &s,double angle)
    {
	++max_length_index_;
	if(angle==0)
	{
	    file_<<"%\\setbox\\blop@tmp@box\\hbox{"<<s<<"}"
		<<"\\blop@dimen=\\ht\\blop@tmp@box\\advance \\blop@dimen by \\dp\\blop@tmp@box\\blopdimen{\\blop@dimen}{"
		<<i2length(max_length_index_)<<"}\\ignorespaces"<<endl;
	}
	else
	{
	    file_<<"%\\setbox\\blop@tmp@box\\hbox{\\rotatebox{"
		<<angle<<"}{"<<s
		<<"}}\\blop@dimen=\\ht\\blop@tmp@box\\advance\\blop@dimen by \\dp\\blop@tmp@box\\blopdimen{\\blop@dimen}{"<<i2length(max_length_index_)
		<<"}\\ignorespaces"<<endl;
	}
	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)
		<<" 10 d} i"<<endl;
	return max_length_index_;
    }
*/

    int blopeps::textheight(const string &s,double angle)
    {
	++max_length_index_;
	file_<<"%\\blop@textsize{"<<s<<"}{";
	if(angle == 0) file_<<"@";
//kaki
//	else file_<<angle;
	else file_<<angle/unit::deg;
	file_<<"}{0}{"<<i2length(max_length_index_)<<"}{0}\\ignorespaces"<<endl;
	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)<<" "
		<<10*s.size()<<" d} i"<<endl;
	return max_length_index_;
    }

    int blopeps::textlheight(const string &s,double angle)
    {
	++max_length_index_;
	file_<<"%\\blop@textsize{"<<s<<"}{";
	if(angle == 0) file_<<"@";
//kaki
//	else file_<<angle;
	else file_<<angle/unit::deg;
	file_<<"}{0}{0}{"<<i2length(max_length_index_)<<"}\\ignorespaces"<<endl;

	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)
		<<" 10 d} i"<<endl;
	return max_length_index_;
    }

/*
    int blopeps::textsize(const string &s,double angle)
    {
	if(angle==0)
	{
	    file_<<"%\\setbox\\blop@tmp@box\\hbox{"<<s<<"}"
		<<"\\blopdimen{\\wd\\blop@tmp@box}{"<<i2length(++max_length_index_)
		<<"}"
		<<"\\blopdimen{\\ht\\blop@tmp@box}{"<<i2length(++max_length_index_)
		<<"}\\ignorespaces"<<endl;
	}
	else
	{
	    file_<<"%\\setbox\\blop@tmp@box\\hbox{\\rotatebox{"<<angle<<"}{"<<s<<"}}"
		<<"\\blopdimen{\\wd\\blop@tmp@box}{"<<i2length(++max_length_index_)
		<<"}"
		<<"\\blopdimen{\\ht\\blop@tmp@box}{"<<i2length(++max_length_index_)
		<<"}\\ignorespaces"<<endl;
	}
	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)
		<<" 50 d} i"<<endl;
	file_tmp_<<"/"<<i2length(max_length_index_-1)
		<<" where {pop}{/"<<i2length(max_length_index_-1)
		<<" 50 d} i"<<endl;
	return max_length_index_;
    }
*/

    int blopeps::textsize(const string &s,double angle)
    {
	file_<<"%\\blop@textsize{"<<s<<"}{";
	if(angle==0) file_<<"@";
//kaki
//	else file_<<angle;
	else file_<<angle/unit::deg;
	file_<<"}{"<<i2length(max_length_index_+1)<<"}{"<<i2length(max_length_index_+2)<<"}{0}\\ignorespaces"<<endl;
	max_length_index_ += 2;
	file_tmp_<<"/"<<i2length(max_length_index_)
		<<" where {pop}{/"<<i2length(max_length_index_)
		<<" 50 d} i"<<endl;
	file_tmp_<<"/"<<i2length(max_length_index_-1)
		<<" where {pop}{/"<<i2length(max_length_index_-1)
		<<" 50 d} i"<<endl;
	return max_length_index_;
    }


    void blopeps::convertstr(const std::string &s,int n,
			     std::string &ps)
    {
	char sss[100];
	sprintf(sss,"<%i>:",n);
	ps     = "";
	for(unsigned int i=0; i<s.size(); ++i)
	{
	    if(s[i] == ')')      { ps += ']';   }
	    else if(s[i] == '(') { ps += '[';   }
	    else if(s[i] == '{') { ps += '[';   }
	    else if(s[i] == '}') { ps += ']';   }
	    else if(s[i] == '\\'){ ps += '|';   }
	    else if(s[i] == '#' ){ ps += '@';   }
	    else if(s[i] == ' ') { ps += ' ';   }
	    else if(s[i] == '~') { ps += '^';   }
	    else {ps += s[i]; }
	}
    }

    void blopeps::draw_dot(const coord &c)
    {
	write(c);
	file_tmp_<<"Daa"<<endl;
    }


    void blopeps::draw_text(const coord &pos,
			    const string &s,
			    int xalign,int yalign,
			    const blop::terminal::id &angle_dx,
			    const blop::terminal::id &angle_dy,
			    bool rotatedsystem)
    {
	if(s == "") return;

	++n_texts_;

	string psmacro = "bloptextmacro";
	{
	    int nchars = (int)'z' - (int)'a';
	    for(int i=n_texts_; i>0; i/=nchars) psmacro += (char)((int)'a' + i%nchars);
	}

	string ps;
	convertstr(s,n_texts_,ps);

	write(pos);
	file_tmp_<<"M ";
	if(current_transformation_&rotation_)
	{
	    file_tmp_<<"G BLOP@CTM 1 get BLOP@CTM 0 get atan rotate"<<endl;
	}
	//file_tmp_<<"("<<ps<<") T"<<endl;
	file_tmp_<<"blopdict /"<<psmacro<<" known {"<<psmacro<<"}{("<<ps<<")T}i"<<endl;
	if(current_transformation_&rotation_)
	{
	    file_tmp_<<"E"<<endl;
	}
	file_<<"%\\blop@text{";
	file_<<s;
	file_<<"}{"<<psmacro<<"}{";
	if(xalign == sym::left) file_<<"l";
	else if(xalign == sym::right) file_<<"r";
	else file_<<"c";
	file_<<"}{";
	if(yalign == sym::base) file_<<"a";
	else if(yalign == sym::bottom) file_<<"b";
	else if(yalign == sym::top) file_<<"t";
	else file_<<"c";
	file_<<"}";
	if(rotatedsystem) file_<<"{R}";
	else file_<<"{O}";
	file_<<"{";
	write(angle_dy,&file_);
	write(angle_dx,&file_);
	file_<<" atan";
	file_<<"}{"<<current_color_.red()<<","<<current_color_.green()<<","<<current_color_.blue()<<"}\\ignorespaces"<<endl;
    }

    void blopeps::reset_transformation()
    {
	current_transformation_ = 0;
	file_tmp_<<"/BLOP@CTM { [ 1 0 0 1 0 0 ] } def"<<endl;
    }

    void blopeps::translate(const id &x, const id &y)
    {
	current_transformation_ |= translation_;
	write(x);
	write(y);
	file_tmp_<<"Tr"<<endl;
    }

    void blopeps::rotate(double angle)
    {
	current_transformation_ |= rotation_;
	if(!Ra_written_)
	{
	    Ra_written_ = true;
	    // rotate by angle the CTM
	    file_tmp_<<"/Ra { /BLOP@CTM BLOP@CTM 3 2 roll [ 1 0 0 1 0 0 ] rotate "
		       "[ 1 0 0 1 0 0 ] concatmatrix def } def"<<endl;
	}
//kaki
//	file_tmp_<<angle<<" Ra"<<endl;
	file_tmp_<<angle/unit::deg<<" Ra"<<endl;
    }

    void blopeps::rotate(const id &x, const id &y)
    {
	current_transformation_ |= rotation_;

	if(!Rt_written_)
	{
	    Rt_written_ = true;
	    // rotate the CTM by tangent
	    file_tmp_<<"/Rt { /BLOP@CTM BLOP@CTM 4 2 roll atan [ 1 0 0 1 0 0 ] rotate "
		       "[ 1 0 0 1 0 0 ] concatmatrix def } def"<<endl;
	}
	write(y);
	write(x);
	file_tmp_<<"Rt"<<endl;
    }

    void blopeps::scale(double s)
    {
	current_transformation_ |= scaling_;

	if(!Sc_written_)
	{
	    Sc_written_ = true;
	    // scale the CTM
	    file_tmp_<<"/Sc { /BLOP@CTM BLOP@CTM 4 2 roll [ 1 0 0 1 0 0 ] scale "
		       "[ 1 0 0 1 0 0 ] concatmatrix def } def"<<endl;
	}
	file_tmp_<<s<<" "<<s<<" Sc"<<endl;
    }
    void blopeps::scale(double xs,double ys)
    {
	current_transformation_ |= scaling_;
	if(!Sc_written_)
	{
	    Sc_written_ = true;
	    // scale the CTM
	    file_tmp_<<"/Sc { /BLOP@CTM BLOP@CTM 4 2 roll [ 1 0 0 1 0 0 ] scale "
		       "[ 1 0 0 1 0 0 ] concatmatrix def } def"<<endl;
	}
	file_tmp_<<xs<<" "<<ys<<" Sc"<<endl;
    }
    void blopeps::scale(const id &xfrom, const id &xto,
			const id &yfrom, const id &yto)
    {
	current_transformation_ |= scaling_;
	if(!Sr_written_)
	{
	    Sr_written_ = true;
	    file_tmp_<<"/Sr { /BLOP@CTM BLOP@CTM 6 2 roll div 3 1 roll div exch "
                       "[ 1 0 0 1 0 0 ] scale [ 1 0 0 1 0 0 ] concatmatrix def } def"<<endl;
	}
	write(xfrom);
	write(xto);
	write(yfrom);
	write(yto);
	file_tmp_<<"Sr"<<endl;
    }

/*
    void blopeps::move_origin(const id &x,const id &y)
    {
	file_tmp_<<"/BLOP@X@TMP //BLOP@X ";
	if(x.length_id == 0) file_tmp_<<x.relcoord<<" //BLOP@W m ";
	else file_tmp_<<"//"<<i2length(x.length_id)<<" ";
	file_tmp_<<" a d"<<endl;

	file_tmp_<<"/BLOP@Y@TMP //BLOP@Y ";
	if(y.length_id == 0) file_tmp_<<y.relcoord<<" //BLOP@H m ";
	else file_tmp_<<"//"<<i2length(y.length_id)<<" ";
	file_tmp_<<" a d"<<endl;
    }
*/

    void blopeps::draw_arc_base(const coord &center, const id &r, double ang1, double ang2)
    {
	file_tmp_<<"N"<<endl;
	write(center);
	write(r);
	file_tmp_<<ang1/unit::deg<<" "<<ang2/unit::deg<<" arc"<<endl;

/*
	file_tmp_<<"BLOP@X@TMP ";
	if(x.length_id) file_tmp_<<i2length(x.length_id)<<" ";
	else file_tmp_<<x.relcoord<<" BLOP@W m ";
	file_tmp_<<"a ";

	file_tmp_<<"BLOP@Y@TMP ";
	if(y.length_id) file_tmp_<<i2length(y.length_id)<<" ";
	else file_tmp_<<y.relcoord<<" BLOP@H m ";
	file_tmp_<<"a ";

	if(r.length_id) file_tmp_<<i2length(r.length_id)<<" ";
	else file_tmp_<<r.relcoord<<" blopdef m ";
	file_tmp_<<ang1<<" "<<ang2<<" arc"<<endl;
*/
    }
    
    void blopeps::draw_arc(const terminal::coord &center,
			   const id &r,
			   double ang1, double ang2)
    {
	draw_arc_base(center,r,ang1,ang2);
	file_tmp_<<"S"<<endl;
    }

    void blopeps::fill_arc(const terminal::coord &center,
			   const id &r,
			   double ang1, double ang2)
    {
	draw_arc_base(center,r,ang1,ang2);
	file_tmp_<<"F"<<endl;
    }

    void blopeps::draw_line(const terminal::coord &c1,
			    const terminal::coord &c2)
    {
	file_tmp_<<"N"<<endl;
	write(c1);
	file_tmp_<<"M"<<endl;
	write(c2);
	file_tmp_<<"L S"<<endl;
    }

/*
    void blopeps::draw_lines_base(const std::vector<blop::terminal::id> &x_id,
				  const std::vector<blop::terminal::id> &y_id,
				  bool rlines)
    {

	if(x_id.empty() || y_id.empty()) return;
	int n = min(x_id.size(), y_id.size());

	if(x_id.size() == 5 && rlines == false)
	{
	    if( ( x_id[0] == x_id[1] && x_id[2] == x_id[3] && x_id[4] == x_id[0] &&
		  y_id[1] == y_id[2] && y_id[3] == y_id[4] && y_id[3] == y_id[0]) ||
		( x_id[0] == x_id[3] && x_id[0] == x_id[4] && x_id[1] == x_id[2] &&
		  y_id[0] == y_id[1] && y_id[2] == y_id[3] && y_id[0] == y_id[4]) )

	    {
		if(x_id[0].length_id == 0)
		{
		    if(::fabs(x_id[0].relcoord) < 0.0001) file_tmp_<<"0 ";
		    else if(::fabs(x_id[0].relcoord-1) < 0.0001) file_tmp_<<"BLOP@W ";
		    else if(::fabs(x_id[0].relcoord+1) < 0.0001) file_tmp_<<"BLOP@W n ";
		    else file_tmp_<<x_id[0].relcoord<<" X ";
		}
		else file_tmp_<<i2length(x_id[0].length_id)<<" ";
		if(y_id[0].length_id == 0)
		{
		    if(::fabs(y_id[0].relcoord) < 0.0001) file_tmp_<<"0 ";
		    else if(::fabs(y_id[0].relcoord-1) < 0.0001) file_tmp_<<"BLOP@H ";
		    else if(::fabs(y_id[0].relcoord+1) < 0.0001) file_tmp_<<"BLOP@H n ";
		    else file_tmp_<<y_id[0].relcoord<<" Y ";
		}
		else file_tmp_<<i2length(y_id[0].length_id)<<" ";

		if(x_id[2].length_id == 0)
		{
		    if(::fabs(x_id[2].relcoord) < 0.0001) file_tmp_<<"0 ";
		    else if(::fabs(x_id[2].relcoord-1) < 0.0001) file_tmp_<<"BLOP@W ";
		    else if(::fabs(x_id[2].relcoord+1) < 0.0001) file_tmp_<<"BLOP@W n ";
		    else file_tmp_<<x_id[2].relcoord<<" X ";
		}
		else file_tmp_<<i2length(x_id[2].length_id)<<" ";
		if(y_id[2].length_id == 0)
		{
		    if(::fabs(y_id[2].relcoord) < 0.0001) file_tmp_<<"0 ";
		    else if(::fabs(y_id[2].relcoord-1) < 0.0001) file_tmp_<<"BLOP@H ";
		    else if(::fabs(y_id[2].relcoord+1) < 0.0001) file_tmp_<<"BLOP@H n ";
		    else file_tmp_<<y_id[2].relcoord<<" Y ";
		}
		else file_tmp_<<i2length(y_id[2].length_id)<<" ";
		file_tmp_<<" Q ";
		return;
	    }
	}

	file_tmp_<<"N"<<endl;
	bool closepath = false;
	if(x_id[n-1] == x_id[0] && y_id[n-1] == y_id[0] && rlines == false)
	{
	    --n;
	    closepath = true;
	}

	for(int i=0; i<n; ++i)
	{
	    if(x_id[i].length_id == 0) file_tmp_<<x_id[i].relcoord<<" X ";
	    else file_tmp_<<i2length(x_id[i].length_id)<<" ";
	    if(y_id[i].length_id == 0) file_tmp_<<y_id[i].relcoord<<" Y ";
	    else file_tmp_<<i2length(y_id[i].length_id)<<" ";
	    if(i == 0) file_tmp_<<"M"<<endl;
	    else
	    {
		if(rlines == false) file_tmp_<<"L"<<endl;
		else file_tmp_<<"R"<<endl;
	    }
	}
	if(closepath) file_tmp_<<"C"<<endl;
    }
*/

    void blopeps::draw_lines_base(const std::vector<blop::terminal::coord> &c)
    {
	file_tmp_<<"N"<<endl;
	bool closepath = false;
	int n = (int)c.size();
	if(c[n-1].x == c[0].x && c[n-1].y == c[0].y)
	{
	    --n;
	    closepath = true;
	}
	for(int i=0; i<n; ++i)
	{
	    write(c[i]);
	    if(i == 0) file_tmp_<<"M"<<endl;
	    else file_tmp_<<"L"<<endl;
	}
	if(closepath) file_tmp_<<"C ";
    }

    void blopeps::draw_lines(const std::vector<blop::terminal::coord> &c)
    {
	draw_lines_base(c);
	file_tmp_<<"S"<<endl;
    }

    void blopeps::fill_polygon(const std::vector<blop::terminal::coord> &c)
    {
	draw_lines_base(c);
	file_tmp_<<"F"<<endl;
    }


    void blopeps::draw_rlines(const blop::terminal::coord &start,
			      const std::vector<blop::terminal::id> &dx,
			      const std::vector<blop::terminal::id> &dy)
    {
	file_tmp_<<"N"<<endl;
	write(start);
	file_tmp_<<" M"<<endl;
	for(unsigned int i=0; i<dx.size(); ++i)
	{
	    write(dx[i]);
	    write(dy[i]);
	    file_tmp_<<"R"<<endl;
	}
	file_tmp_<<"S"<<endl;
    }


    void blopeps::set_color(const color &c)
    {
	if(current_color_ == c) return;
	current_color_ = c;
	file_tmp_<<c.red()<<" "<<c.green()<<" "<<c.blue()<<" c"<<endl;
    }

    void blopeps::set_linewidth(const id &i)
    {
	if(i.length_id)
	{
	    if(current_linewidth_ == i.length_id) return;
	    current_linewidth_ = i.length_id;
	}
	else current_linewidth_ = 0;
	write(i);
	file_tmp_<<"w";

        file_tmp_<<"[";
        for(unsigned int i=0; i<current_linestyle_.size(); ++i)
        {
            if(i>0) file_tmp_<<" ";
            file_tmp_<<"currentlinewidth "<<current_linestyle_[i]<<" m";
        }
        file_tmp_<<"] 0 setdash"<<endl;

/*        
	if(current_linestyle_ == sym::dotted) file_tmp_<<" p";
	else if(current_linestyle_ == sym::dashed) file_tmp_<<" v";
	else if(current_linestyle_ == sym::dashdotted) file_tmp_<<" pv";
	else if(current_linestyle_ == sym::dashddotted) file_tmp_<<" ppv";
	file_tmp_<<endl;
*/
    }

    void blopeps::set_linestyle(sym::linestyle ls)
    {
        if(current_linestyle_.size() == ls.size())
        {
            bool same = true;
            for(unsigned int i=0; i<ls.size(); ++i)
            {
                if(current_linestyle_[i] != ls[i])
                {
                    same = false; 
                    break;
                }
            }
            if(same) return;
        }

	current_linestyle_ = ls;

        file_tmp_<<"[";
        for(unsigned int i=0; i<current_linestyle_.size(); ++i)
        {
            if(i>0) file_tmp_<<" ";
            file_tmp_<<"currentlinewidth "<<current_linestyle_[i]<<" m";
        }
        file_tmp_<<"] 0 setdash"<<endl;

/*
	if(i == sym::dotted) file_tmp_<<" p"<<endl;
	else if(i == sym::dashed) file_tmp_<<" v"<<endl;
	else if(i == sym::dashdotted) file_tmp_<<" pv"<<endl;
	else if(i == sym::dashddotted) file_tmp_<<" ppv"<<endl;
	else file_tmp_<<" f"<<endl;
*/
        
    }
	

    int blopeps::atan(const id &x,const id &y)
    {
	file_tmp_<<"/"<<i2length(++max_length_index_)<<" ";
	write(y);
	write(x);
	file_tmp_<<"atan d"<<endl;
	return max_length_index_;
    }

    int blopeps::add_dbl(const double_id &a,const double_id &b)
    {
	file_tmp_<<"/"<<i2length(++max_length_index_)<<" ";
	if(a.id > 0) file_tmp_<<i2length(a.id)<<" ";
	else file_tmp_<<a.value<<" ";
	if(b.id > 0) file_tmp_<<i2length(b.id)<<" ";
	else file_tmp_<<b.value<<" a d"<<endl;
	return max_length_index_;
    }

    int blopeps::sin_dbl(int i)
    {
	file_tmp_<<"/"<<i2length(++max_length_index_)<<" "<<i2length(i)<<" sin d"<<endl;
	return max_length_index_;
    }

    int blopeps::cos_dbl(int i)
    {
	file_tmp_<<"/"<<i2length(++max_length_index_)<<" "<<i2length(i)<<" cos d"<<endl;
	return max_length_index_;
    }

    int blopeps::neg_dbl(int i)
    {
	file_tmp_<<"/"<<i2length(++max_length_index_)<<" "<<i2length(i)<<" neg d"<<endl;
	return max_length_index_;
    }


}
