main()
{
    // define a color
    color mycolor(0.6,0.8,0.5);

    // create a pad, from the lower left corner of the picture (0,0)
    // to the center of the picture (pw = picturewidth, ph = pictureheight)
    // and let it have a 3D look, with 1 mm border

    pad::mknew(0,0,0.5*pw,0.5*ph)->border_3D(true).borderwidth(mm).bgcolor(mycolor);

    // put a label into the frame (flabel), positioned to the lower left
    // edge of the legendbox

    legendbox *l1 = frame::current()->legend();
    flabel("\\rnode{A}{}")->x(!l1->left()).y(!l1->bottom());

    // and draw the datafile with the histogram drawstyle

    plot("data",_1,_3).allcolor(red).drawstyle(histo).legend("Histogram style");

    // make another pad, in the upper right quarter of the picture

    pad::mknew(0.5*pw,0.5*ph,pw,ph);

    // set the ranges of the axes explicitely

    x1range(0,6);
    y1range(0,60);
    

    legendbox *l2 = frame::current()->legend();
    flabel("\\rnode{B}{}")->x(!l2->left() + 0.5*!l2->width()).y(!l2->bottom());

    plot("data",_1,_2,_4).drawstyle(labels).legend("");
    mplot("data",_1,_2).drawstyle(lines).allcolor(red).legend("$\\sum\\frac{1}{2} \\neq 0.4$");
    mplot("data",_1,_2).drawstyle(points).allcolor(blue).legend("");

    blopeps term("picture.eps");

    print(&term);
    return 0;
}

