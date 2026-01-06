main()
{
    dgraph::default_legend("");
    dgraph::default_drawstyle(points());
    set::yrange(0,9);
    set::xrange(0,11);
    set::y1tics(0,"");
    set::x1tics(0,"");
    mplot("<<1 8").pt(square());
    mplot("<<2 8").pt(fsquare());
    label::fdraw("square,fsquare",x1len(3),y1len(8)).yalign(sym::center);
    mplot("<<1 7").pt(diamond());
    mplot("<<2 7").pt(fdiamond());
    label::fdraw("diamond,fdiamond",x1len(3),y1len(7)).yalign(sym::center);
    mplot("<<1 6").pt(triangledown());
    mplot("<<2 6").pt(ftriangledown());
    label::fdraw("triangledown,ftriangledown",x1len(3),y1len(6)).yalign(sym::center);
    mplot("<<1 5").pt(ftriangle());
    label::fdraw("Sorry, triangle is not accepted in CINT",x1len(1),y1len(5.1));
    mplot("<<2 5").pt(ftriangle());
    label::fdraw("triangle,ftriangle",x1len(3),y1len(5)).yalign(sym::center);
    mplot("<<1 4").pt(circle());
    mplot("<<2 4").pt(fcircle());
    label::fdraw("circle,fcircle",x1len(3),y1len(4)).yalign(sym::center);
    mplot("<<1 3").pt(plus());
    label::fdraw("plus",x1len(3),y1len(3)).yalign(sym::center);
    mplot("<<1 2").pt(cross());
    label::fdraw("cross",x1len(3),y1len(2)).yalign(sym::center);
    mplot("<<1 1").pt(star4());
    mplot("<<2 1").pt(fstar4());
    label::fdraw("star4,fstar4",x1len(3),y1len(1)).yalign(sym::center);

    blopeps::print("point-types.beps");
}

