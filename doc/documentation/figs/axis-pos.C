main()
{
    frame::current().draw_frame(false); // do not draw the rectangular frame
    frame::current().y1axis()->pos(x1len(0));  // put y axis at 0 position
    frame::current().x1axis()->pos(y1len(2));  // put y axis at 0 position
    set::xrange(-10,10);
    set::ytitle("Y value");
    set::xtitle("X value");
    plot(_1,_1*sin(_1));
    blopeps::print("axis-pos.beps");
}
