main()
{
    mplot("<<25 22.5
                              27 22
                              31 21.5
                              32 22
                              36 20
                              38 18.5
                              40 18.6").legend("Northern hemisphere")
	.ds(points()).pt(fsquare()).ac(red);

    mplot("<<19 24
             20 24.1
             23 23.5
             24.5 22.5
             26 22.5
             31.5 22
             32.7 22
             35 20.4
    37 18.8
    40.5 18").legend("Northern hemisphere")
	.ds(points()).pt(ftriangle()).ac(red);

    mplot("<<16 28
 17 27.3
 20 26
 22.5 25
 23 24.5
 25.5 23
 27.7 22
 30 21.4
 32 20
 35.5 19").legend("Southern hemisphere")
	.ds(points()).pt(ftriangle()).ac(blue);


    set::xtitle("Distance from the Equator");
    set::ytitle("Average temperature");

    set::multilegend(true);
    blopeps::print("legendbox-multi.beps");
}
