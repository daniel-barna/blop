main()
{

    dgraph &g = plot("<<1 1\n 2 4\n 3 14\n 4 4").ds(points()).legend("");
    set::xtics(1);
    set::xrange(0,5);
    blopeps::print("graphd-points.beps");
}
