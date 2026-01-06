
main()
{
    canvas::current().fillcolor(color(0.9,0.9,0.9));
    label::pdraw("\\rotatebox{40}{\\Large Not correct! Better ask!}",0.1,0.1).tc(yellow);

    frame &f = frame::mknew()
	.loffset(1.2*width("loffset or left"))
	.bmargin(1.2*width("bmargin or cbottom"))
	.roffset(1.2*width("roffset"));

    f.mirror_y1(false);
    f.mirror_y2(false);
    set::x1title("X1 title");
    set::y1title("y1 title");
    plot(_1,_1*_1).linecolor(red).p1range(-2,2);
    mplot(_1,sin(_1)).p1range(-3,3).yaxis(axis::y2);

    line::default_linecolor(blue);
    label::default_textcolor(blue);
    label::default_xalign(sym::center);

    line &left_line = line::cdraw(!f.left(),0)(!f.left(),1);
    left_line.linestyle(sym::dashed);

    line &right_line = line::cdraw(!f.right(),0)(!f.right(),1);
    right_line.linestyle(sym::dashed);

    line &bottom_line = line::cdraw(0,!f.bottom())(1,!f.bottom());
    bottom_line.linestyle(sym::dashed);

    line &left_arrow = line::cdraw(0,PH-2*CM)(!f.left(),PH-2*CM);
    left_arrow.arrow(true).arrow_back(true);
    label::cdraw("loffset or left", 0.5*!f.left(),PH-18*MM).xalign(sym::center);

    line &cleft_arrow = line::cdraw(0,PH-4*CM)(!f.cleft(),PH-4*CM);
    cleft_arrow.arrow(true).arrow_back(true);
    label::cdraw("lmargin or cleft",0.5*!f.cleft(),PH-38*MM);

    line &cright_line = line::cdraw(!f.cright(),0)(!f.cright(),1);
    cright_line.linestyle(sym::dashed);
    line &cright_arrow = line::cdraw(0,12*MM)(!f.cright(),12*MM);
    cright_arrow.arrow(true).arrow_back(true);
    label::cdraw("cright",0.5*!f.left(),13*MM);

    line &rmargin_arrow = line::cdraw(!f.cright(),12*MM)(1,12*MM);
    rmargin_arrow.arrow(true).arrow_back(true);
    label::cdraw("rmargin",0.5*(!f.cright()+PW),14*MM);

    line &roffset_arrow = line::cdraw(!f.right(),PH-32*MM)(1,PH-32*MM);
    roffset_arrow.arrow(true).arrow_back(true);
    label::cdraw("roffset",0.5*(!f.right()+PW),PH-30*MM);

    line &bmargin_arrow = line::cdraw(!f.left()+0.5*!f.width()-CM,0)
	                             (!f.left()+0.5*!f.width()-CM,!f.cbottom());
    bmargin_arrow.arrow(true).arrow_back(true);
    label::cdraw("bmargin or cbottom",
		 !f.left()+0.5*!f.width()-8.5*MM,0.5*!f.cbottom())
	.angle(90)
	.align(sym::left,sym::center);

    line &boffset_arrow = line::cdraw(!f.left()+0.5*!f.width()+CM,0)
	                             (!f.left()+0.5*!f.width()+CM,!f.bottom());
    boffset_arrow.arrow(true).arrow_back(true);
    label::cdraw("boffset or bottom",
		 !f.left()+0.5*!f.width()+11.5*MM,0.5*!f.bottom())
	.angle(90)
	.align(sym::left,sym::center);



    label::fdraw("$\\bullet$",0,0).xalign(sym::center).yalign(sym::center).textcolor(blue);
    label::fdraw("coordsys. origin",MM,MM).textcolor(blue).xalign(sym::left);
    

    blopeps::print("frame.beps");
}
