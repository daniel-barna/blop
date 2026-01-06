main()
{
    plot("<<
     1 1 0.1
     2 4 0.4
     3 3 0.2
     4 4 0.3").ds(syerrorbars()).legend("").pt(fsquare()).ps(MM).ac(blue);
    blopeps::print("graphd-errorbars.beps");
}
