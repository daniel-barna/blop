int main()
{

    canvas::current().fillcolor(color(0.95,0.95,0.95));

    label *t1 = &label::cdraw("\\rnode{canvasA}{$\\bullet$}",0.05,0.05);

    pad::mknew(0.4,0,0.9,0.48).bordercolor(red);
    label::pdraw("\\rnode{padA2}{$\\bullet$}",0.95,0.9);
    set::y1title("Y1 Title");
    set::x1title("X1 Title");
    frame::current().rmargin(CM);
    set::ymax(1.7);
    plot(_1,sin(_1)).p1range(0,10).legend("Sin(x)");

    label::fdraw("\\rnode{legendA}{$\\bullet$}",
		 frame::current().legend()->left(),
		 frame::current().legend()->bottom());

    var x[1]={1.8*3.1415};
    var y[1]={sin(x[0])};
    var z[1]={"\\rnode{graphA1}{$\\bullet$}"};
    mplot(1,x,y,z).ds(labels()).legend("");

    pad::mknew(0.05,0.5,0.7,1).border_3D(true).fillcolor(color(0.7,0.7,0.4)).borderwidth(1.5*MM);
    label::pdraw("\\rnode{padA1}{$\\bullet$}",0.8,0.9);
    label::pdraw("\\textcolor{blue}{\\small Label in the pad}",0.2,0.95).yalign(sym::top);
    frame::current().tmargin(CM);
    label::fdraw("\\rnode{frameA}{$\\bullet$}",0.005,0.995).yalign(sym::top);
    label::fdraw("\\rnode{padlabelA}{\\small A label inside a frame}",0.1,0.6);
    plot(_1,_1*_1).p1range(0,10).ac(blue).legend("$x^2$");

    x[0]=4;
    y[0]=16;
    z[0]="\\rnode{graphA2}{$\\bullet$}";
    mplot(1,x,y,z).ds(labels()).legend("");

    blopeps::print("object-hierarchy.beps");
}
