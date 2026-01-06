main()
{
    set::xrange(0,6);
    var data = "<<
      1 1 3
      2 2 2
      3 3 1
      4 4 -1
      5 5 -2";
    plot(data ,_1,_2).ds(bars(2*MM, -MM)).ac(red) .legend("");
    mplot(data,_1,_3).ds(bars(2*MM,  MM)).ac(blue).legend("");
    blopeps::print("graphd-bars.beps");
}
