main()
{
    plot("<<
     1 1
     2 4
     3 14
     4 4").ds(lines()).legend("");
    set::xtics(1);
    set::xrange(0,5);
    blopeps::print("graphd-lines.beps");
}
