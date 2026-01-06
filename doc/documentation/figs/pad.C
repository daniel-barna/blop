void draw_horiz_arrow(const var &label,
		      const length &x1,
		      const length &x2,
		      const length &y)
{
    line &l = line::cdraw(x1,y)(x2,y);
    l.arrow(true).arrow_back(true).linecolor(blue);
    label::cdraw(label,0.5*(x1+x2),y+MM).textcolor(blue).xalign(sym::center);
}

void draw_vert_arrow(const var &label,
		      const length &x,
		      const length &y1,
		      const length &y2)
{
    line &l = line::cdraw(x,y1)(x,y2);
    l.arrow(true).arrow_back(true).linecolor(blue);
    label::cdraw(label,x+MM,0.5*(y1+y2)).textcolor(blue).yalign(sym::center).angle(90);
}


main()
{
    grob::default_unit(grob::use_parent);

    label::cdraw("Canvas",1,1).xalign(sym::right).yalign(sym::top);

    pad &pad1 = pad::mknew(0.2,0.4,0.6,0.8)
	.borderwidth(4*MM).border_3D(true).fillcolor(color(0.6,0.8,0.5));
    label::pdraw("Pad",0.5,0.5).xalign(sym::center).yalign(sym::center);

    line &l1 = line::cdraw(0,!pad1.cbottom())(1,!pad1.cbottom());
    l1.linestyle(sym::dashed).linecolor(blue).linewidth(PT);
    line &l2 = line::cdraw(0,!pad1.bottom())(1,!pad1.bottom());
    l2.linestyle(sym::dashed).linecolor(blue);

    line &l3 = line::cdraw(0,!pad1.ctop())(1,!pad1.ctop());
    l3.linestyle(sym::dashed).linecolor(blue).linewidth(PT);
    line &l4 = line::cdraw(0,!pad1.top())(1,!pad1.top());
    l4.linestyle(sym::dashed).linecolor(blue);

    line &l5 = line::cdraw(!pad1.left(),0)(!pad1.left(),1);
    l5.linestyle(sym::dashed).linecolor(blue);
    line &l6 = line::cdraw(!pad1.cleft(),0)(!pad1.cleft(),1);
    l6.linestyle(sym::dashed).linecolor(blue);

    line &l7 = line::cdraw(!pad1.right(),0)(!pad1.right(),1);
    l7.linestyle(sym::dashed).linecolor(blue);
    line &l8 = line::cdraw(!pad1.cright(),0)(!pad1.cright(),1);
    l8.linestyle(sym::dashed).linecolor(blue);

    length y = 3*MM;
    length dy = 5*MM;
    draw_horiz_arrow("left",0.0,!pad1.left(),y); y += dy;
    draw_horiz_arrow("cleft",0.0,!pad1.cleft(),y); y += dy;
    draw_horiz_arrow("cright",0.0,!pad1.cright(),y); y += dy;
    draw_horiz_arrow("right",0.0,!pad1.right(),y);
    draw_horiz_arrow("width",!pad1.left(),!pad1.right(),!pad1.top() + 2*MM);
    draw_horiz_arrow("cwidth",!pad1.cleft(),!pad1.cright(),!pad1.top() + 8*MM);

    length x = !pad1.right() + 8*MM;
    length dx = 8*MM;
    draw_vert_arrow("bottom",x,0.0,!pad1.bottom()); x += dx;
    draw_vert_arrow("cbottom",x,0.0,!pad1.cbottom()); x += dx;
    draw_vert_arrow("ctop",x,0.0,!pad1.ctop()); x += dx;
    draw_vert_arrow("top",x,0.0,!pad1.top()); x += dx;
//    draw_vert_arrow("borderwidth",x,!pad1.ctop(),!pad1.top()); x += dx;
    draw_vert_arrow("cheight",x,!pad1.cbottom(),!pad1.ctop()); x += dx;
    draw_vert_arrow("height",x,!pad1.bottom(),!pad1.top());




    blopeps::print("pad.beps");
}
