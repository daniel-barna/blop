int main()
{
    // set axis titles
    set::xtitle("X1-axis");
    set::ytitle("Y1 axis $\\int \\sum \\frac{1}{2}$");

    // plot the first two columns of the datafile, with 'lines' style
    plot("data").drawstyle(lines()).linecolor(red).legend("{\\it lines} style");

    // plot with errorbars. if you don't specify .pointtype(...), no points are drawn
    mplot("data",_1    , _2+2, _3).drawstyle(errorbars()).legend("Errorbars only");

    // if you specify .pointtype(...) as well, points are also drawn
    mplot("data",_1+0.2, _2  , _3)
	.drawstyle(errorbars()).pointtype(ftriangle())
	.linecolor(red).pointcolor(blue).legend("Errorbars + points");

    // put the legend to the upper left corner
    frame::current().legendpos(sym::left,sym::top);

    // and print to a terminal
    blopeps::print("example1.beps");

}
