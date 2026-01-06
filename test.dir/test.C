#ifndef __BLOP__
#include "blop.h"
using namespace blop;
#endif
#include <iostream>

var setlabels(var a)
{
    if(a == 1) return "one";
    if(a == 2) return "two";
    if(a == 3) return "three";
    if(a == 4) return "four";
    return a;
}


const double R_bending = 500*unit::mm;
const double r0 = 20*unit::mm;


int main()
{
    blop::function f(cos(_1),cos(_1));
    std::vector<blop::var> args, results;
    const double a = 60*unit::deg;
    args.push_back(a);
//    f.meval(args,results);
    cerr<<"------------"<<endl;
    cerr<<f[0](a)<<endl; //<<"   "<<results[1]<<" "<<f[1](a)<<endl;
    return 0;



//    plot_if("data.dat",between_lines(_1=="most" && _2=="mehet",_1=="vege"));

//    plot(_1).p1range(0.02, 9);
//    set::logscale_y(true);

/*
    frame::default_framecolor(red);


    // create a pad in the lower right quarter of the canvas
    // and create a legendbox in it (to be shared by other frames)
    pad::mknew(0.5*PW,0,PW,0.5*PH); //.fillcolor(red).fill(true);


    legendbox &l = legendbox::mknew()
	.samplen(2.5*PS).bordersep(PS)   // increase these in order that the large
                                         // symbols also fit in
	.multi(true)
	.xcenter(0.5).ycenter(0.5);

    line &arrow = line::pdraw(CM,CM);
    arrow(2*CM,2*CM).arrowangle(30);


    // put a label to the lower left corner of this legendbox
    label::pdraw("\\rnode{A}{$\\bullet$}",!l.left(),!l.top()).xalign(sym::center).yalign(sym::center);


    // define a color
    color mycolor(0.6,0.8,0.5);

    // create a pad, from the lower left corner of the picture (0,0)
    // to the center of the picture (pw = picturewidth, ph = pictureheight)
    // and let it have a 3D look, with 1 mm border

    pad::mknew(0,0,0.5*PW,0.5*PH).border_3D(true).borderwidth(MM).fillcolor(mycolor);


    // let this frame share the legendbox in the lower
    // right corner
    frame::current().legend(l);

    plot(_1,15-_1,18-_1+2*_1*sin(5*_1)).ds(ybands()).fillcolor(color(0.2,0.2,0.5)).legend("Band style").p1range(0,5);

    function fff = cfunc(setlabels);
    mplot("data",_1,_3,_4,fff(_3))
	.allcolor(red).fillcolor(blue).drawstyle(xyticlabels(histo())).ls(sym::dashed).legend("Histogram style"); //.layer(2);

    function model = PAR(1)*_1*_1;
    fit<gauss_chi2>("data",model,fitopt().x(_1).y(_3));
    mplot(_1,model).ac(yellow).legend("Fitted function: " & model.param(1) & "$\\cdot x^2$");

    mplot("awk '{print $1,$3,4}' data|").ac(red).ds(syerrorbars().clip_errorbars(true)).ymin(0);
    mplot(2.5+cos(_1)+0.1*cos(10*_1),15+5*sin(_1)+0.5*cos(10*_1)).p1range(0,6.28).ds(dots()).legend("big formula");

    frame::current().foreground(true);


    // make another pad, in the upper right quarter of the picture
    pad::mknew(0.5*PW,0.5*PH,PW,PH).border_3D(true).fillcolor(color(0.7,0.7,0.6)).borderwidth(MM);

    // this frame also shares the same legendbox
    frame::current().legend(l);


    // set the ranges of the axes explicitely

    set::xrange(0,6);
    set::yrange(0,60);


    plot_if("data",_1<3.5,_1,_2,"{\\it\\small " & _4 & "}").drawstyle(labels()).legend("");


    mplot("data",_1,_2).drawstyle(lines()).allcolor(green).legend("$\\sum\\frac{1}{2} \\neq 0.4$");
    mplot("data",_1,_2).ds(points()).pt(star4()).ps(2*PS).ac(color(0.7,0.2,0.7)).legend("{\\it points} drawstyle");
    mplot("data",_1,_2*1.5).ds(points()).pt(ftriangle()).ps(PS).ac(color(0.7,0.2,0.7)).legend("{\\it points} drawstyle");
    
    mplot("<<1 20\n 2 30\n 3 55\n 4 15\n 5 0").ds(splines()).legend("spline style").ymax(50);
    mplot("<<
         0.5 30
         -0.5 70
         2 50").ds(lines()).fill(true).ac(color(0.3,0.3,0.1)).legend("");
    frame::current().y1axis()->pos(x1len(3));
    frame::current().draw_frame(false);
    label::fdraw("HELLO",x1len(3),y1len(30));

    text::fdraw("Elso sor\nMasodik sor").left(2*MM).top(1.0-2*MM);

    set::x1title("X1 Title");
    set::y1title("\\small Y1 Title");

    pad::mknew(0,0.5*PH,0.5*PW,PH);

    set::grid(true);
    set::title("cbox drawstyle");

//    set::xrange(-1.5,1.5);
    set::x1tics(20);

    set::y1title("HHHHH");
    set::yscale(100);
    //plot(_1,1.5).p1range(1,99);

    set::xunit("kHz");
    set::xtitle("X title");

    plot(_1,_2,_1*sin(_2/100))
	.p1range(1,99)
	.p2range(0,400)
	.nsamples(100).ds(cboxes(0,80).underflow(green)).legend("");

    mplot(_1,_2,_1*sin(_2/100))
	.p1range(1,99)
	.p2range(0,400)
	.nsamples(40).ds(isolines()).legend("");

    //frame::current().y1axis()->scale(1/1000.);
    frame::current().foreground(true);
*/

    blopeps::print("picture.beps");

    return 0;
}

