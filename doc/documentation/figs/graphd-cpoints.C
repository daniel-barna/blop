main()
{
    plot("<<
     1 1 1 0 0 
     2 2 0 1 0
     3 3 0 0 1
     4 4 0.5 0 0.5").ds(cpoints()).pt(ftriangle()).legend("");
    set::x1tics(1);
    set::xrange(0,5);
    set::yrange(0,5);
    blopeps::print("graphd-cpoints.beps");
}
