main()
{
    set::xrange(0,6);
    set::yrange(0);
    set::xtics(1);
    set::ytics(1);
    plot("<<
1 1 \"one\"
2 2 \"second label\"
3 3 \"third label\"").ds(labels());
    blopeps::print("graphd-labels.beps");
}
