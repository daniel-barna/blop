main()
{
    dgraph::default_legend("");
    var data = "<< 0 1
             1 3
             2 5
             3 9
             4 7
             5 6";
    plot(data,_1,1.3*_2).ds(splines()).fill(true).fc(color(0.8,0.5,0.5));
    mplot(data).ds(splines());
    mplot(data).ds(points()).pt(fsquare()).ps(MM);
    frame::current().foreground(true);
    set::xtics(1);
    set::xrange(0,5);
    blopeps::print("graphd-spline.beps");
}
