main()
{
    plot("<<
     1 1 1
     2 2 2
     3 3 3
     4 4 4").ds(spoints()).pt(ftriangle()).legend("");
    set::xtics(1);
    set::xrange(0,5);
    set::yrange(0,5);
    blopeps::print("graphd-spoints.beps");
}
