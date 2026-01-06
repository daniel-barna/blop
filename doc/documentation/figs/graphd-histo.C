main()
{
    plot("<< 1 1
             2 4
             3 9
             4 7 ").ds(histo()).legend("");
    set::x1tics(1);
    blopeps::print("graphd-histo.beps");
}
