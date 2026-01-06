/*
This example demonstrates all drawstyles
*/

int main()
{
    pad::mknew(0,0,PW,0.33*PH);
    plot(_1,8-sin(2*_1),10+sin(3*_1)).p1range(0,5).ds(ybands()).legend("{\\bf yband} drawstyle").fc(color(0.3,0.5,0.3));
    mplot("data")           .ds(lines()) .linecolor(black) .legend("{\\bf lines} drawstyle");
    mplot("data").ds(histo()) .linecolor(blue)  .legend("{\\bf histo} drawstyle");
    mplot("data").ds(points()).pointcolor(red).pointtype(ftriangle()).legend("{\\bf points} drawstyle");
    frame::current().legendpos(sym::left,sym::top);

    pad::mknew(0,0.33*PH,PW,0.66*PH);
    frame::current().legendpos(sym::left,sym::top);

    // make 'multilegend': the samples of those graphs, which have the same legends,
    // are plotted in the same line
    frame::current().legend()->multi(true);

    set::xrange(0,6);

    // draw with 3mm wide bars
    plot("data",_1,_2).ds(bars(3*MM))  .fillcolor(color(0.5,0.5,0.5)).legend("{\\bf bars} drawstyle");

    // draw with 3mm wide bars, which are shifted also by 3mm
    // note that this graph will have no separate line in the legendbox
    mplot("data",_1,_2/2).ds(bars(3*MM,3*MM)). fillcolor(color(0.8,0.4,0.4)).legend("{\\bf bars} drawstyle");
    mplot("data",_1,sqrt(_2)).ds(bars(3*MM,-3*MM)). fillcolor(color(0.1,0.4,0.8)).legend("{\\bf bars} drawstyle");

    mplot("data",_1,_2,_3).ds(errorbars()).linecolor(red).legend("{\\bf errorbars} drawstyle");
    mplot("data",_1+0.3,_2,_3).ds(errorbars()).lc(blue).pt(circle())
	.pointcolor(red).legendcolor(blue).legend("{\\bf errorbars} drawstyle + points");

    // plot a label (the third specified column, _4) at the given points (the first two specified columns)
    // optionally, you can specify a horizontal and vertical offset, and horizontal and vertical
    // alignment of the labels, or you can simply leave it as the default: .ds(labels);
    mplot("data",_1,_2,"{\\Large\\it " & _4 & "}").ds(labels(sym::left,sym::bottom,10,MM,MM) );

    pad::mknew(0,0.66*PH,0.6*PW,PH);
    frame::current().legendpos(sym::left,sym::top);
    set::ytitle("Popularity");
    set::xrange(0,5);
    plot("data",_1,_2,_5).ds(xticlabels(bars(6*MM,3*MM))).legend("In 1984").ac(color(0.8,0.5,0));
    mplot("data",_1,sqrt(_2),_5).ds(xticlabels(bars(6*MM,-3*MM))).legend("In 2003").ac(color(0,0.5,0.7));

    pad::mknew(0.6*PW,0.66*PH,PW,PH);
    set::xrange(-2,2);
    set::yrange(-2,2);
    plot(_1,_2,exp(-_1*_1-2*_2*_2)).nsamples(40).ds(cboxes()).legend("");

    blopeps::print("example2.beps");

}
