main()
{
    var data = "<< 1 2 3
                   2 6 4
                   3 8.2 4.5
                   4 8.0 5
                   5 7.5 5.5
                   6 2 4
                   7 1 3
                   8 0 1";  

    frame::current().x1axis()->cut(9,100);

    set::grid(true);
    plot(data,_1,_2).ds(xticlabels(points())).pt(square()).legend("2nd column");
    mplot(data,_1+100,_3,_1).ds(xticlabels(points())).pt(star4()).legend("3rd column");

    blopeps::print("axis-cut.beps");
}
